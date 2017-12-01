#include "mainwindow.h"
#include <cmath>
#include <memory>

#include <QtCore/qmath.h>
#include <QScriptEngine>
#include <QSlider>

using namespace QtDataVisualization;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_timer(new QTimer()), m_series(new QSurface3DSeries())
{
    ui.setupUi(this);
    //setStyleSheet("QGroupBox{padding-top:15px; margin-top:-15px}");

	m_pde_settings_filename = "pde_settings.json";
	m_PdeSettings = init_pde_settings(m_pde_settings_filename);

    init_graph();
	init_PdeSettingsTableWidget(m_PdeSettings->toQVariantMap());
    init_EquationComboBox();
}

MainWindow::~MainWindow()
{
    delete m_GraphSlider;
    delete m_GraphSliderLabel;
    delete m_GraphSliderLayout;
}

void MainWindow::start()
{
    //now calculate pde with default settings
    m_PdeSolver = new PdeSolverHeatEquation();  // std::make_shared<PdeSolverHeatEquation>
    m_graph_data = m_PdeSolver->solve(*m_PdeSettings);

    //connect(ui.PdeSettingsTableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(PdeSettingsTableWidgetCellClickedSlot(int, int)));
    connect(ui.EvaluatePushButton, SIGNAL(clicked()), this, SLOT(EvaluatePushButton_clicked()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update_TimeSlice()));
    m_timer->start(m_graph_update_frequency);
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

    m_GraphSliderLabel = new QLabel("Update frequency: " + QString::number(m_graph_update_frequency));
    m_GraphSliderLabel->setMinimumWidth(200);
    m_GraphSlider = new QSlider(Qt::Horizontal);
    m_GraphSlider->setMinimum(10);
    m_GraphSlider->setMaximum(1000);
    m_GraphSlider->setSingleStep(10);
    m_GraphSlider->setValue(m_graph_update_frequency);

    m_GraphSliderLayout = new QHBoxLayout();
    m_GraphSliderLayout->addWidget(m_GraphSliderLabel);
    m_GraphSliderLayout->addWidget(m_GraphSlider);

    m_GraphWidget = QWidget::createWindowContainer(m_graph);

    ui.GraphLayout->addWidget(m_GraphWidget);
    ui.GraphLayout->addLayout(m_GraphSliderLayout);

    connect(m_GraphSlider, SIGNAL(actionTriggered(int)), this, SLOT(GraphSlider_changed(int)));
}

void MainWindow::GraphSlider_changed(int action)
{
    if (action == QAbstractSlider::SliderMove)
    {
        m_graph_update_frequency = m_GraphSlider->value();
        m_timer->stop();
        m_timer->start(m_graph_update_frequency);
        m_GraphSliderLabel->setText("Update frequency: " + QString::number(m_graph_update_frequency));
    }
}

void MainWindow::init_PdeSettingsTableWidget(QVariantMap pde_settings_map)
{
	ui.PdeSettingsTableWidget->verticalHeader()->setVisible(false);
    ui.PdeSettingsTableWidget->setColumnWidth(0, 90);
    //ui.PdeSettingsTableWidget->setColumnWidth(1, 165);
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

	ui.PdeSettingsTableWidget->repaint();
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
    ui.EquationComboBox->addItem("Heat equation");
    ui.EquationComboBox->addItem("Wave equation");
    connect(ui.EquationComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(EquationComboBoxCurrentIndex_changed(QString)));
}

void MainWindow::EquationComboBoxCurrentIndex_changed(QString value)
{
    if (value == "Heat equation")
    {
        delete m_PdeSolver;
        m_PdeSolver = new PdeSolverHeatEquation();
    }
    else if (value == "Wave equation")
    {
        delete m_PdeSolver;
        m_PdeSolver = new PdeSolverWaveEquation();
    }
}

void clearSurfaceDataArray(QSurfaceDataArray& array)
{
  for (int j(0); j < array.size(); j++) delete array[j];
  array.clear();
}

void MainWindow::clearData()
{
    for (int i(0); i < m_graph_data.first.size(); i++)
    {
        QSurfaceDataArray* array = m_graph_data.first.at(i);
        clearSurfaceDataArray(*array);
        delete array;
    }
    m_graph_data.first.erase(m_graph_data.first.begin(), m_graph_data.first.end());
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
	if (m_current_time >= m_PdeSettings->countT)
		m_current_time = 0;
    auto qsurface_data_array = m_graph_data.first.at(m_current_time);
	auto modifier = [](QSurfaceDataItem item) -> void { item.position(); };
	
    m_series->dataProxy()->resetArray(newSurfaceDataArrayFromSource(*qsurface_data_array, modifier));

	++m_current_time;
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
	m_timer->stop();
    clearData();

	//reading from ui.PdeSettingsTableWidget
	QVariantMap map;
	int rowCount = ui.PdeSettingsTableWidget->rowCount();
	for (int i = 0; i < rowCount; ++i)
		map.insert(ui.PdeSettingsTableWidget->item(i, 0)->text(), ui.PdeSettingsTableWidget->item(i, 1)->text());
	m_PdeSettings->reset(map);
	
	m_graph_data = m_PdeSolver->solve(*m_PdeSettings);
	
	m_current_time = 0;
	m_timer->start(m_graph_update_frequency);
}
