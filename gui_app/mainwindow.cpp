#include "mainwindow.h"
#include <cmath>
#include <memory>

#include <QtCore/qmath.h>
#include <QScriptEngine>

using namespace QtDataVisualization;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_timer(new QTimer()), m_series(new QSurface3DSeries())
{
    ui.setupUi(this);

	m_pde_settings_filename = "pde_settings.json";
	m_PdeSettings = init_pde_settings(m_pde_settings_filename);

    //initialize graph and its widget:
    m_graph = init_graph(m_series);
    m_GraphWidget = QWidget::createWindowContainer(m_graph);
    ui.horizontalLayout->addWidget(m_GraphWidget, Qt::AlignLeft);  // but still aligns to the right. why?..

	init_PdeSettingsTableWidget(m_PdeSettings->toQVariantMap());

	m_graph_data = m_PdeSolver->solve(*m_PdeSettings);

    //connect(ui.PdeSettingsTableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(PdeSettingsTableWidgetCellClickedSlot(int, int)));

	connect(ui.EvaluatePushButton, SIGNAL(clicked()), this, SLOT(EvaluatePushButtonClicked()));
	connect(m_timer, SIGNAL(timeout()), this, SLOT(updateTimeSlice()));
	m_timer->start(m_graph_update_frequency);

    //ui.GraphWidget->show();
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

Q3DSurface* MainWindow::init_graph(QSurface3DSeries* series)
{
	//QStringList labels;
	//labels.push_back(QString("LABEL"));
	//QValue3DAxis axis_y, axis_z;
	//axis_x.setLabels(labels);

    auto new_graph = new Q3DSurface();
    new_graph->setAxisX(new QValue3DAxis);
    new_graph->setAxisY(new QValue3DAxis);
    new_graph->setAxisZ(new QValue3DAxis);

    new_graph->axisX()->setRange(m_PdeSettings->minX, m_PdeSettings->maxX);
    new_graph->axisY()->setRange(m_PdeSettings->minY, m_PdeSettings->maxY);
    //new_graph->axisZ()->setRange(0.0f, 1.0f);

    new_graph->axisX()->setLabelFormat("%.2f");
    new_graph->axisY()->setLabelFormat("%.2f");
    new_graph->axisZ()->setLabelFormat("%.2f");

    new_graph->axisX()->setTitle("Y");
    new_graph->axisX()->setTitleVisible(true);
    new_graph->axisY()->setTitle("Z");
    new_graph->axisY()->setTitleVisible(true);
    new_graph->axisZ()->setTitle("X");
    new_graph->axisZ()->setTitleVisible(true);
	//m_graph->axisZ()->setReversed(true);

    new_graph->addSeries(series);

    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::black);
    gr.setColorAt(0.2, Qt::blue);
    gr.setColorAt(0.8, Qt::red);
    gr.setColorAt(1.0, Qt::yellow);

    new_graph->seriesList().at(0)->setBaseGradient(gr);
    new_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);

    return new_graph;
}

void MainWindow::init_PdeSettingsTableWidget(QVariantMap pde_settings_map)
{
	ui.PdeSettingsTableWidget->verticalHeader()->setVisible(false);
	ui.PdeSettingsTableWidget->setColumnWidth(0, 80);
    ui.PdeSettingsTableWidget->setColumnWidth(1, 165);
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

void clearSurfaceDataArray(QSurfaceDataArray& array)
{
  for (int j(0); j < array.size(); j++) delete array[j];
  array.clear();
}

void MainWindow::clearData()
{
    for (int i(0); i < m_graph_data->size(); i++)
    {
        QSurfaceDataArray* array = m_graph_data->at(i);
        clearSurfaceDataArray(*array);
    }
    m_graph_data->erase(m_graph_data->begin(), m_graph_data->end());
    m_graph_data.reset();
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

void MainWindow::updateTimeSlice()
{
	if (m_current_time >= m_PdeSettings->countT)
		m_current_time = 0;
	auto qsurface_data_array = m_graph_data->at(m_current_time);
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

void MainWindow::EvaluatePushButtonClicked()
{
	m_timer->stop();
    clearData();

	//reading from ui.PdeSettingsTableWidget
	QVariantMap map;
	int rowCount = ui.PdeSettingsTableWidget->rowCount();
	for (int i = 0; i < rowCount; ++i)
		map.insert(ui.PdeSettingsTableWidget->item(i, 0)->text(), ui.PdeSettingsTableWidget->item(i, 1)->text());
	m_PdeSettings->reset(map);
	
	m_graph_data.reset();
	m_graph_data = m_PdeSolver->solve(*m_PdeSettings);
	
	m_current_time = 0;
	m_timer->start(m_graph_update_frequency);
}
