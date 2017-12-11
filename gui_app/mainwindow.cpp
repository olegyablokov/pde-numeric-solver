#include "mainwindow.h"
#include <cmath>
#include <memory>

#include <QtCore/qmath.h>
#include <QScriptEngine>
#include <QSlider>

using namespace QtDataVisualization;

Q_DECLARE_METATYPE(PdeSolverBase::GraphDataSlice_t);
Q_DECLARE_METATYPE(PdeSolverBase::GraphData_t);
Q_DECLARE_METATYPE(PdeSolverBase::GraphSolution_t);
Q_DECLARE_METATYPE(PdeSettings);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_timer(new QTimer()), m_series(new QSurface3DSeries())
{
    ui.setupUi(this);

	m_pde_settings_filename = "pde_settings.json";
	m_PdeSettings = init_pde_settings(m_pde_settings_filename);

    init_graph();
	init_PdeSettingsTableWidget(m_PdeSettings->toQVariantMap());
    init_EquationComboBox();
}

MainWindow::~MainWindow()
{
    m_GraphThread.quit();
    m_GraphThread.wait();
    delete m_GraphTimeSpeedSlider;
    delete m_GraphTimeSpeedLabel;
    delete m_GraphTimeSpeedLayout;
}

void MainWindow::start()
{
    connect(ui.EvaluatePushButton, SIGNAL(clicked()), this, SLOT(EvaluatePushButton_clicked()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update_TimeSlice()));

    m_GraphThread.start();
    change_pde_solver("Wave equation");

    m_PdeSolver->solve(*m_PdeSettings);

    //connect(ui.PdeSettingsTableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(PdeSettingsTableWidgetCellClickedSlot(int, int)));
}

//void MainWindow::PdeSettingsTableWidgetCellClickedSlot(int row, int column)
//{
//    if(!((row == 0) && (column == 1))) return;

//    QString program = ui.PdeSettingsTableWidget->itemAt(row, column)->text();
//    QScriptEngine expression;

//    QColor color;
//    if (expression.canEvaluate(program))
//    {
//        ui.EvaluatePushButton->setDisabled(false);
//        color.setRgb(0, 255, 0);
//    }
//    else
//    {
//        ui.EvaluatePushButton->setDisabled(true);
//        color.setRgb(255, 0, 0);
//    }
//    ui.PdeSettingsTableWidget->itemAt(row, 0)->setBackgroundColor(color);
//    ui.PdeSettingsTableWidget->itemAt(row, 1)->setBackgroundColor(color);
//}

void MainWindow::init_graph()
{
	//QStringList labels;
	//labels.push_back(QString("LABEL"));
	//QValue3DAxis axis_y, axis_z;
	//axis_x.setLabels(labels);

    m_graph = new Q3DSurface();
    m_graph->setAxisX(new QValue3DAxis);
    m_graph->setAxisY(new QValue3DAxis);
    m_graph->setAxisZ(new QValue3DAxis);

    m_graph->axisX()->setRange(m_PdeSettings->minX, m_PdeSettings->maxX);
    m_graph->axisY()->setRange(m_PdeSettings->minY, m_PdeSettings->maxY);
    //new_graph->axisZ()->setRange(0.0f, 1.0f);

    m_graph->axisX()->setLabelFormat("%.2f");
    m_graph->axisY()->setLabelFormat("%.2f");
    m_graph->axisZ()->setLabelFormat("%.2f");

    m_graph->axisX()->setTitle("Y");
    m_graph->axisX()->setTitleVisible(true);
    m_graph->axisY()->setTitle("Z");
    m_graph->axisY()->setTitleVisible(true);
    m_graph->axisZ()->setTitle("X");
    m_graph->axisZ()->setTitleVisible(true);
	//m_graph->axisZ()->setReversed(true);

    m_graph->addSeries(m_series);

    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::black);
    gr.setColorAt(0.4, Qt::blue);
    gr.setColorAt(0.8, Qt::red);
    gr.setColorAt(1.0, Qt::yellow);

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);

    //setting graph time speed layout
    m_GraphTimeSpeedLabel = new QLabel("Update frequency (ms/frame): " + QString::number(m_graph_update_time_step));
    m_GraphTimeSpeedSlider = new QSlider(Qt::Horizontal);
    m_GraphTimeSpeedLayout = new QHBoxLayout();

    m_GraphTimeSpeedLabel->setMinimumWidth(200);

    m_GraphTimeSpeedSlider->setMinimum(10);
    m_GraphTimeSpeedSlider->setMaximum(1000);
    m_GraphTimeSpeedSlider->setSingleStep(1);
    m_GraphTimeSpeedSlider->setValue(m_graph_update_time_step);

    m_GraphTimeSpeedLayout->addWidget(m_GraphTimeSpeedLabel);
    m_GraphTimeSpeedLayout->addWidget(m_GraphTimeSpeedSlider);

    //setting graph current time layout
    m_GraphCurrentTimeLabel = new QLabel("Current time slice: " + QString::number(m_current_time_slice));
    m_GraphCurrentTimeSlider = new QSlider(Qt::Horizontal);
    m_GraphCurrentTimeLayout = new QHBoxLayout();

    m_GraphCurrentTimeLabel->setMinimumWidth(160);

    m_GraphCurrentTimeSlider->setMinimum(0);
    m_GraphCurrentTimeSlider->setMaximum(m_PdeSettings->countT);
    m_GraphCurrentTimeSlider->setSingleStep(1);
    m_GraphCurrentTimeSlider->setValue(0);

    m_PlayStopPushButton = new QPushButton();
    m_PlayStopPushButton->setMinimumWidth(34);
    m_PlayStopPushButton->setIcon(QIcon(":/mainwindow/icons/stop"));
    connect(m_PlayStopPushButton, SIGNAL(clicked()), this, SLOT(PlayStopPushButton_clicked()));

    m_GraphCurrentTimeLayout->addWidget(m_GraphCurrentTimeLabel);
    m_GraphCurrentTimeLayout->addWidget(m_PlayStopPushButton);
    m_GraphCurrentTimeLayout->addWidget(m_GraphCurrentTimeSlider);

    m_GraphWidget = QWidget::createWindowContainer(m_graph);

    //m_GraphOccuracyLabel = new QLabel("Occuracy: " + QString::number(m_graph_update_time_step));
    //m_GraphOccuracyLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    ui.GraphLayout->addWidget(m_GraphWidget);
    ui.GraphLayout->addLayout(m_GraphTimeSpeedLayout);
    ui.GraphLayout->addLayout(m_GraphCurrentTimeLayout);
    //ui.GraphLayout->addWidget(m_GraphOccuracyLabel);

    connect(m_GraphTimeSpeedSlider, SIGNAL(actionTriggered(int)), this, SLOT(GraphTimeSpeedSlider_changed(int)));
}

void MainWindow::toggle_graph_playing(bool play)
{
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
    toggle_graph_playing(!m_timer->isActive());
}

void MainWindow::GraphTimeSpeedSlider_changed(int action)
{
    if (action == QAbstractSlider::SliderMove)
    {
        m_graph_update_time_step = m_GraphTimeSpeedSlider->value();
        toggle_graph_playing(m_timer->isActive());

        m_timer->start(m_graph_update_time_step);

        m_GraphTimeSpeedLabel->setText("Update frequency (ms/frame): " + QString::number(m_graph_update_time_step));
    }
}

void MainWindow::init_PdeSettingsTableWidget(QVariantMap pde_settings_map)
{
	ui.PdeSettingsTableWidget->verticalHeader()->setVisible(false);
	ui.PdeSettingsTableWidget->setRowCount(0);
	int n_row = 0;
	for (QVariantMap::const_iterator iter = pde_settings_map.begin(); iter != pde_settings_map.end(); ++iter)
	{
		ui.PdeSettingsTableWidget->insertRow(n_row);

		QTableWidgetItem *key = new QTableWidgetItem(0);
		QTableWidgetItem *value = new QTableWidgetItem(0);

		key->setText(iter.key());
		value->setText(iter.value().toString());

		ui.PdeSettingsTableWidget->setItem(n_row, 0, key);
		ui.PdeSettingsTableWidget->setItem(n_row, 1, value);

		++n_row;
	}

    //ui.PdeSettingsTableWidget->resizeRowsToContents();
    ui.PdeSettingsTableWidget->resizeColumnsToContents();
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

void MainWindow::graph_solution_generated(PdeSolverBase::GraphSolution_t solution)
{
    qDebug() << "MainWindow::graph_solution_generated invoked";

    toggle_graph_playing(false);

    clear_graph_data(m_graph_data);

    m_graph_data = solution.graph_data;
    *m_PdeSettings = solution.set;

    qDebug() << "Update timer started";
    m_current_time_slice = 0;

    toggle_graph_playing(true);

    ui.EvaluatePushButton->setDisabled(false);
    ui.EquationComboBox->setDisabled(false);

    m_GraphCurrentTimeSlider->setMinimum(0);
    m_GraphCurrentTimeSlider->setMaximum(m_PdeSettings->countT);
}

void MainWindow::solution_progress_updated(QString msg, int value)
{
    ui.GraphSolutionProgressBar->setValue(value);
    ui.GraphSolutionProgressLabel->setText(msg);
}

void MainWindow::change_pde_solver(QString value)
{
    if (value == "Heat equation")
    {
        m_PdeSolver.reset(new PdeSolverHeatEquation());
    }
    else if (value == "Wave equation")
    {
        m_PdeSolver.reset(new PdeSolverWaveEquation());
    }
    else throw("Wrong value. Must be \"Heat equation\" or \"Wave equation\"");

    m_PdeSolver->moveToThread(&m_GraphThread);

    connect(m_PdeSolver.get(), SIGNAL(solution_progress_update(QString, int)), this, SLOT(solution_progress_updated(QString, int)), Qt::QueuedConnection);
    connect(m_PdeSolver.get(), SIGNAL(solution_generated(PdeSolverBase::GraphSolution_t)), this, SLOT(graph_solution_generated(PdeSolverBase::GraphSolution_t)), Qt::QueuedConnection);
}

void MainWindow::clear_graph_data(PdeSolverBase::GraphData_t& graph_data)
{
    for (auto& array_ptr : graph_data.first)
    {
        for (auto& row_ptr : *array_ptr)
        {
            delete row_ptr;
        }
        delete array_ptr;
    }
    graph_data.first.clear();

    for (auto& array_ptr : graph_data.second)
    {
        for (auto& row_ptr : *array_ptr)
        {
            delete row_ptr;
        }
        delete array_ptr;
    }
    graph_data.second.clear();
}

QSurfaceDataArray* newSurfaceDataArrayFromSource(QSurfaceDataArray& source_surface_data_array,
                                                 std::function<void(QSurfaceDataItem&)> modifier)
{
	int sampleCount = source_surface_data_array.size();
	auto newArray = new QSurfaceDataArray();
	newArray->reserve(sampleCount);
	for (int i(0); i < sampleCount; i++)
		newArray->append(new QSurfaceDataRow(sampleCount));
    for (int i(0); i < sampleCount; i++)
    {
		const QSurfaceDataRow& sourceRow = *(source_surface_data_array.at(i));
		QSurfaceDataRow& row = *(*newArray)[i];
        for (int j(0); j < sampleCount; j++)
        {
			row[j].setPosition(sourceRow.at(j).position());
			modifier(row[j]);
		}
	}
	return newArray;
}

void MainWindow::update_TimeSlice()
{
    ++m_current_time_slice;
    if (m_current_time_slice >= m_PdeSettings->countT)
        m_current_time_slice = 0;
    auto qsurface_data_array = m_graph_data.first.at(m_current_time_slice);
	auto modifier = [](QSurfaceDataItem item) -> void { item.position(); };
	
    m_series->dataProxy()->resetArray(newSurfaceDataArrayFromSource(*qsurface_data_array, modifier));
    //m_GraphOccuracyLabel->setText("Occuracy : " + QString::number(m_graph_solution.occuracy[m_current_time]));

    m_GraphCurrentTimeSlider->setValue(m_current_time_slice);
    m_GraphCurrentTimeLabel->setText("Current time slice: " + QString::number(m_current_time_slice));
}

//void clear_data(std::shared_ptr<QVector<QtDataVisualization::QSurfaceDataArray*>> vector)
//{
//	//TODO: implent delete_data
//    if (!vector) return;

//    for (auto data_array : *vector)
//    {
//        if (!data_array) continue;
//        for (auto row : *data_array)
//        {
//            if (!row) continue;
//            row->clear();
////            for (auto& item : *row)
////            {
////                item.reset();
////            }
//        }
//    }
//}

void MainWindow::EvaluatePushButton_clicked()
{
    ui.EvaluatePushButton->setDisabled(true);
    ui.EquationComboBox->setDisabled(true);

    //reading from ui.PdeSettingsTableWidget
    QVariantMap map;
    int rowCount = ui.PdeSettingsTableWidget->rowCount();
    for (int i = 0; i < rowCount; ++i)
        map.insert(ui.PdeSettingsTableWidget->item(i, 0)->text(), ui.PdeSettingsTableWidget->item(i, 1)->text());

    m_PdeSolver->solve(PdeSettings(map));
}
