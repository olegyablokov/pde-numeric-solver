#include "mainwindow.h"
#include <cmath>
#include <memory>

#include <QtCore/qmath.h>
#include <QScriptEngine>
#include <QSlider>

using namespace QtDataVisualization;

Q_DECLARE_METATYPE(PdeSolver::GraphDataSlice_t);
Q_DECLARE_METATYPE(PdeSolver::GraphData_t);
Q_DECLARE_METATYPE(PdeSolver::GraphSolution_t);
Q_DECLARE_METATYPE(PdeSettings);
Q_DECLARE_METATYPE(PdeSolver::SolutionMethod_t);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_timer(new QTimer()), m_series(new QSurface3DSeries())
{
    ui.setupUi(this);

	m_pde_settings_filename = "pde_settings.json";
    m_PdeSettings = init_pde_settings(m_pde_settings_filename);
    m_PdeSettings->set_coords_type(PdeSettings::CoordsType::Polar, 2);

    init_EquationComboBox();
    init_graph();
    set_PdeSettingsTableWidget(*m_PdeSettings);

    ui.EvaluatePushButton->setDisabled(true);
    ui.MethodsComboBox->setDisabled(true);
    ui.EquationComboBox->setDisabled(true);
}

MainWindow::~MainWindow()
{
    m_GraphThread.quit();
    m_GraphThread.wait();

    delete m_GraphTimeSpeedSlider;
    delete m_GraphTimeSpeedLabel;
    delete m_GraphTimeSpeedLayout;

    delete m_GraphCurrentTimeLabel;
    delete m_GraphCurrentTimeSlider;
    delete m_GraphCurrentTimeLayout;

    delete m_PlayStopPushButton;
    delete m_NextSlidePushButton;
    delete m_PrevSlidePushButton;
    delete m_FirstSlidePushButton;
    delete m_LastSlidePushButton;

    delete m_timer;
    delete m_series;

    delete m_graph;
}

void MainWindow::start()
{
    connect(ui.EvaluatePushButton, SIGNAL(clicked()), this, SLOT(EvaluatePushButton_clicked()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update_TimeSlice()));

    m_GraphThread.start();
    change_pde_solver("Wave equation");

    m_PdeSolver->solve(*m_PdeSettings, ui.MethodsComboBox->currentData().value<PdeSolver::SolutionMethod_t>());
}

void MainWindow::init_graph()
{
    m_graph = new Q3DSurface();
    m_graph->setAxisX(new QValue3DAxis);
    m_graph->setAxisY(new QValue3DAxis);
    m_graph->setAxisZ(new QValue3DAxis);

    m_PdeSettings->set_coords_type(PdeSettings::CoordsType::Polar, 2);
    m_graph->axisZ()->setRange(m_PdeSettings->get_coord_by_label("R")->min,
                               m_PdeSettings->get_coord_by_label("R")->max);
    m_graph->axisY()->setRange(-10.0f, 10.0f);

    m_graph->addSeries(m_series);

    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::black);
    gr.setColorAt(0.4, Qt::blue);
    gr.setColorAt(0.7, Qt::red);
    gr.setColorAt(1.0, Qt::yellow);

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);

    //setting graph time speed layout
    m_GraphTimeSpeedLabel = new QLabel("Update frequency (ms/frame): " + QString::number(m_graph_update_time_step));
    m_GraphTimeSpeedSlider = new QSlider(Qt::Horizontal);
    m_GraphTimeSpeedLayout = new QHBoxLayout();

    m_GraphTimeSpeedLabel->setMinimumWidth(200);

    m_GraphTimeSpeedSlider->setMinimum(10);
    m_GraphTimeSpeedSlider->setMaximum(200);
    m_GraphTimeSpeedSlider->setSingleStep(1);
    m_GraphTimeSpeedSlider->setValue(m_graph_update_time_step);

    m_GraphTimeSpeedLayout->addWidget(m_GraphTimeSpeedLabel);
    m_GraphTimeSpeedLayout->addWidget(m_GraphTimeSpeedSlider);

    //setting graph current time layout
    m_GraphCurrentTimeLabel = new QLabel("Current time slice: " + QString::number(m_current_time_slice));
    m_GraphCurrentTimeSlider = new QSlider(Qt::Horizontal);
    m_GraphCurrentTimeLayout = new QHBoxLayout();

    m_GraphCurrentTimeLabel->setMinimumWidth(200);

    m_GraphCurrentTimeSlider->setMinimum(0);
    m_GraphCurrentTimeSlider->setMaximum(m_PdeSettings->get_coord_by_label("T")->count);
    m_GraphCurrentTimeSlider->setSingleStep(1);
    m_GraphCurrentTimeSlider->setValue(0);

    //play/stop/etc. buttons:
    m_PlayStopPushButton = new QPushButton();
    m_PlayStopPushButton->setIcon(QIcon(":/mainwindow/icons/stop"));
    connect(m_PlayStopPushButton, SIGNAL(clicked()), this, SLOT(PlayStopPushButton_clicked()));

    m_NextSlidePushButton = new QPushButton();
    m_NextSlidePushButton->setIcon(QIcon(":/mainwindow/icons/next_slide"));

    connect(m_NextSlidePushButton, SIGNAL(clicked()), this, SLOT(NextSlidePushButton_clicked()));

    m_PrevSlidePushButton = new QPushButton();
    m_PrevSlidePushButton->setIcon(QIcon(":/mainwindow/icons/prev_slide"));
    connect(m_PrevSlidePushButton, SIGNAL(clicked()), this, SLOT(PrevSlidePushButton_clicked()));

    m_FirstSlidePushButton = new QPushButton();
    m_FirstSlidePushButton->setIcon(QIcon(":/mainwindow/icons/first_slide"));
    connect(m_FirstSlidePushButton, SIGNAL(clicked()), this, SLOT(FirstSlidePushButton_clicked()));

    m_LastSlidePushButton = new QPushButton();
    m_LastSlidePushButton->setIcon(QIcon(":/mainwindow/icons/last_slide"));
    connect(m_LastSlidePushButton, SIGNAL(clicked()), this, SLOT(LastSlidePushButton_clicked()));
    //

    m_GraphCurrentTimeLayout->addWidget(m_GraphCurrentTimeLabel);
    m_GraphCurrentTimeLayout->addWidget(m_FirstSlidePushButton);
    m_GraphCurrentTimeLayout->addWidget(m_PrevSlidePushButton);
    m_GraphCurrentTimeLayout->addWidget(m_PlayStopPushButton);
    m_GraphCurrentTimeLayout->addWidget(m_NextSlidePushButton);
    m_GraphCurrentTimeLayout->addWidget(m_LastSlidePushButton);
    m_GraphCurrentTimeLayout->addWidget(m_GraphCurrentTimeSlider);

    m_GraphWidget = QWidget::createWindowContainer(m_graph);

    ui.GraphLayout->addWidget(m_GraphWidget);
    ui.GraphLayout->addLayout(m_GraphTimeSpeedLayout);
    ui.GraphLayout->addLayout(m_GraphCurrentTimeLayout);

    connect(m_GraphTimeSpeedSlider, SIGNAL(actionTriggered(int)), this, SLOT(GraphTimeSpeedSlider_changed(int)));

    //set the current value of EquationComboBox
    int i;
    for (i = 0; i < ui.EquationComboBox->count(); ++i)
    {
        if (ui.EquationComboBox->itemText(i) == "Wave equation")
        {
            ui.EquationComboBox->setCurrentIndex(i);
            break;
        }
    }
    if (i == ui.EquationComboBox->count()) throw("Error: \"Wave equation\" not found in ui.EquationComboBox");
}

void MainWindow::toggle_graph_playing(bool play)
{
    if (!m_graph_is_valid) return;
    if (play)
    {
        m_PlayStopPushButton->setIcon(QIcon(":/mainwindow/icons/stop"));
        m_timer->start(m_graph_update_time_step);
    }
    else
    {
        m_PlayStopPushButton->setIcon(QIcon(":/mainwindow/icons/play"));
        m_timer->stop();
    }
}

void MainWindow::PlayStopPushButton_clicked()
{
    if (!m_graph_is_valid) return;
    toggle_graph_playing(!m_timer->isActive());
}

void MainWindow::NextSlidePushButton_clicked()
{
    if (!m_graph_is_valid) return;
    set_TimeSlice((m_PdeSettings->get_coord_by_label("T")->count - 1) ? m_current_time_slice + 1 : m_current_time_slice);
}

void MainWindow::PrevSlidePushButton_clicked()
{
    if (!m_graph_is_valid) return;
    set_TimeSlice((m_current_time_slice > 0) ? m_current_time_slice - 1 : 0);
}

void MainWindow::FirstSlidePushButton_clicked()
{
    if (!m_graph_is_valid) return;
    set_TimeSlice(0);
}

void MainWindow::LastSlidePushButton_clicked()
{
    if (!m_graph_is_valid) return;
    set_TimeSlice(m_PdeSettings->get_coord_by_label("T")->count - 1);
}

void MainWindow::GraphTimeSpeedSlider_changed(int action)
{
    if (action == QAbstractSlider::SliderMove)
    {
        bool was_playing = m_timer->isActive();
        if (was_playing) toggle_graph_playing(false);
        m_graph_update_time_step = m_GraphTimeSpeedSlider->value();
        if (was_playing) toggle_graph_playing(true);

        m_GraphTimeSpeedLabel->setText("Update frequency (ms/frame): " + QString::number(m_graph_update_time_step));
    }
}

void MainWindow::set_PdeSettingsTableWidget(const PdeSettings& set)
{
	ui.PdeSettingsTableWidget->verticalHeader()->setVisible(false);
	ui.PdeSettingsTableWidget->setRowCount(0);
	int n_row = 0;

    QVariantMap values_map = set.toQVariantMap();
    QVariantMap tooltip_map = set.getQVariantMapToolTips();
    for (QVariantMap::const_iterator iter = values_map.begin(); iter != values_map.end(); ++iter)
	{
		ui.PdeSettingsTableWidget->insertRow(n_row);

        QTableWidgetItem *key = new QTableWidgetItem(0);
        QTableWidgetItem *value = new QTableWidgetItem(0);

        key->setText(iter.key());
        key->setToolTip(tooltip_map[iter.key()].value<QString>());
        value->setText(iter.value().toString());

        ui.PdeSettingsTableWidget->setItem(n_row, 0, key);
        ui.PdeSettingsTableWidget->setItem(n_row, 1, value);

		++n_row;
	}

    ui.PdeSettingsTableWidget->resizeRowsToContents();
    ui.PdeSettingsTableWidget->resizeColumnsToContents();
    ui.PdeSettingsTableWidget->setColumnWidth(1, 205);
}

std::shared_ptr<PdeSettings> MainWindow::init_pde_settings(QString pde_settings_filename)
{
	QFile pde_file(pde_settings_filename);

	if (pde_file.exists())
	{
		pde_file.open(QIODevice::ReadOnly | QIODevice::Text);
		QString val = pde_file.readAll();
		pde_file.close();

		QJsonDocument document = QJsonDocument::fromJson(val.toUtf8());
		QJsonObject json_obj = document.object();
		QVariantMap map = json_obj.toVariantMap();

		PdeSettings set(map);

		return std::make_shared<PdeSettings>(set);
	}
	else
	{
		pde_file.open(QIODevice::ReadWrite);

		PdeSettings set;

		QJsonObject object = QJsonObject::fromVariantMap(set.toQVariantMap());

		QJsonDocument document;
		document.setObject(object);

		pde_file.write(document.toJson());
		pde_file.close();
		return std::make_shared<PdeSettings>(set);
	}
}

void MainWindow::init_EquationComboBox()
{
    ui.EquationComboBox->addItem("Wave equation");
    ui.EquationComboBox->addItem("Heat equation");

    connect(ui.EquationComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(change_pde_solver(QString)));
}

void MainWindow::graph_solution_generated(PdeSolver::GraphSolution_t solution)
{
    qDebug() << "MainWindow::graph_solution_generated invoked";

    toggle_graph_playing(false);

    clear_graph_data(m_graph_data);

    m_graph_data = solution.graph_data;
    *m_PdeSettings = solution.set;

    //setting graph ranges:
    if (solution.set.m_CoordsType == PdeSettings::CoordsType::Polar)
    {
        m_graph->setPolar(true);
        m_graph->axisZ()->setRange(m_PdeSettings->get_coord_by_label("R")->min,
                                   m_PdeSettings->get_coord_by_label("R")->max);
        m_graph->setAxisX(new QValue3DAxis);
    }
    else if (solution.set.m_CoordsType == PdeSettings::CoordsType::Cartesian)
    {
        m_graph->setPolar(false);
        m_graph->axisZ()->setRange(m_PdeSettings->get_coord_by_label("X1")->min,
                                   m_PdeSettings->get_coord_by_label("X1")->max);
        m_graph->axisX()->setRange(m_PdeSettings->get_coord_by_label("X2")->min,
                                   m_PdeSettings->get_coord_by_label("X2")->max);
    }

    qDebug() << "Update timer started";
    m_current_time_slice = 0;

    toggle_graph_playing(true);

    ui.EvaluatePushButton->setDisabled(false);
    ui.MethodsComboBox->setDisabled(false);
    ui.EquationComboBox->setDisabled(false);
    if (!m_graph_is_valid)
    {
        m_graph_is_valid = true;
        toggle_graph_playing(true);
    }

    m_GraphCurrentTimeSlider->setMinimum(0);
    m_GraphCurrentTimeSlider->setMaximum(m_PdeSettings->get_coord_by_label("T")->count);
}

void MainWindow::solution_progress_updated(QString msg, int value)
{
    ui.GraphSolutionProgressBar->setValue(value);
    ui.GraphSolutionProgressLabel->setText(msg);
}

void MainWindow::change_pde_solver(QString new_solver)
{
    if (new_solver == "Heat equation")
    {
        m_PdeSolver.reset(new PdeSolverHeatEquation());
    }
    else if (new_solver == "Wave equation")
    {
        m_PdeSolver.reset(new PdeSolverWaveEquation());
    }
    else throw("Wrong value. Must be \"Heat equation\" or \"Wave equation\"");

    m_PdeSolver->moveToThread(&m_GraphThread);

    connect(m_PdeSolver.get(), SIGNAL(solution_progress_update(QString, int)), this, SLOT(solution_progress_updated(QString, int)), Qt::QueuedConnection);
    connect(m_PdeSolver.get(), SIGNAL(solution_generated(PdeSolver::GraphSolution_t)), this, SLOT(graph_solution_generated(PdeSolver::GraphSolution_t)), Qt::QueuedConnection);

    //set methods combo box:
    QVector<PdeSolver::SolutionMethod_t> methods = m_PdeSolver->get_implemented_methods();
    ui.MethodsComboBox->clear();
    QVariant qvar;
    for (auto& method : methods)
    {
        qvar.setValue(method);
        ui.MethodsComboBox->addItem(method.name, qvar);
    }
    change_pde_settings_coord_system(ui.MethodsComboBox->currentData().value<PdeSolver::SolutionMethod_t>().coord_system);
}

void MainWindow::change_pde_settings_coord_system(QString new_coords_system)
{
    if (new_coords_system == "Cartesian") set_PdeSettingsTableWidget(PdeSettings(PdeSettings::CoordsType::Cartesian, 2));
    else if (new_coords_system == "Polar") set_PdeSettingsTableWidget(PdeSettings(PdeSettings::CoordsType::Polar, 2));
    else throw("Wrong coords system");
}

void MainWindow::clear_graph_data(PdeSolver::GraphData_t& graph_data)
{
    for (auto& array_ptr : graph_data.u_list)
    {
        for (auto& row_ptr : *array_ptr)
        {
            delete row_ptr;
        }
        delete array_ptr;
    }
    graph_data.u_list.clear();

    for (auto& array_ptr : graph_data.u_t_list)
    {
        for (auto& row_ptr : *array_ptr)
        {
            delete row_ptr;
        }
        delete array_ptr;
    }
    graph_data.u_t_list.clear();
}

QSurfaceDataArray* newSurfaceDataArrayFromSource(QSurfaceDataArray& source_surface_data_array,
                                                 std::function<void(QSurfaceDataItem&)> modifier)
{
    if (source_surface_data_array.empty()) return new QSurfaceDataArray();

	int sampleCount = source_surface_data_array.size();
    int countX = source_surface_data_array[0]->size();

	auto newArray = new QSurfaceDataArray();
	newArray->reserve(sampleCount);

    for (int i = 0; i < sampleCount; i++)
    {
        newArray->append(new QSurfaceDataRow(countX));
		const QSurfaceDataRow& sourceRow = *(source_surface_data_array.at(i));
		QSurfaceDataRow& row = *(*newArray)[i];
        for (int j = 0; j < countX; j++)
        {
			row[j].setPosition(sourceRow.at(j).position());
			modifier(row[j]);
		}
	}
	return newArray;
}

void MainWindow::set_TimeSlice(int new_time_slice)
{
    m_current_time_slice = new_time_slice;

    auto qsurface_data_array = m_graph_data.u_list.at(m_current_time_slice);
    auto modifier = [](QSurfaceDataItem item) -> void { item.position(); };

    m_series->dataProxy()->resetArray(newSurfaceDataArrayFromSource(*qsurface_data_array, modifier));
    //m_GraphOccuracyLabel->setText("Occuracy : " + QString::number(m_graph_solution.occuracy[m_current_time]));

    m_GraphCurrentTimeSlider->setValue(m_current_time_slice);
    m_GraphCurrentTimeLabel->setText("Current time slice: " + QString::number(m_current_time_slice));
}

void MainWindow::update_TimeSlice()
{
    set_TimeSlice((m_current_time_slice < m_PdeSettings->get_coord_by_label("T")->count - 1) ? m_current_time_slice + 1 : 0);
}

PdeSettings MainWindow::get_pde_settings_from_TableWidget()
{
    QVariantMap map;
    QString key, value;
    int rowCount = ui.PdeSettingsTableWidget->rowCount();
    for (int i = 0; i < rowCount; ++i)
    {
        key = ui.PdeSettingsTableWidget->item(i, 0)->text();
        value = ui.PdeSettingsTableWidget->item(i, 1)->text();
        map.insert(key, value);
    }

    PdeSettings set(*m_PdeSettings);

    QString coords_type = ui.MethodsComboBox->currentData().value<PdeSolver::SolutionMethod_t>().coord_system;
    if (coords_type == "Cartesian") set.set_coords_type(PdeSettings::CoordsType::Cartesian);
    else if (coords_type == "Polar") set.set_coords_type(PdeSettings::CoordsType::Polar);
    else throw("Wrong coords type");
    set.reset(map);

    return set;
}

void MainWindow::EvaluatePushButton_clicked()
{
    ui.EvaluatePushButton->setDisabled(true);
    ui.MethodsComboBox->setDisabled(true);
    ui.EquationComboBox->setDisabled(true);

    m_PdeSolver->solve(get_pde_settings_from_TableWidget(), ui.MethodsComboBox->currentData().value<PdeSolver::SolutionMethod_t>());
}
