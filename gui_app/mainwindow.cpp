#include "mainwindow.h"
#include <cmath>
#include <memory>
#include <QtCore/qmath.h>

using namespace QtDataVisualization;


float V1(QVector2D x)
{
	float R = qSqrt(x[0] * x[0] + x[1] * x[1]);

	//return (15 * qExp(-R * R / 100)) * (qSin(R * 2));
	return (15 * qExp(-R * R / 100)) * (qSin(R * 2)) * (qCos(x[1] / 10));

	//return 20 * qExp(-R * R / 20);
	//return (qPow(1 - x[0], 2) + 100 * qPow(x[1] - x[0] * x[0], 2)) / 1000;
}

QVector2D V2(QVector2D x)
{
	float R = qSqrt(x[0] * x[0] + x[1] * x[1]);
	return QVector2D(-2.0f * x[0] * qExp(-R * R), -2.0f * x[1] * qExp(-R * R));
}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), m_timer(new QTimer())
{
	ui.setupUi(this);

	m_pde_settings_filename = "pde_settings.json";
	m_PdeSettings = init_pde_settings(m_pde_settings_filename);
	init_graph();
	init_PdeSettingsTableWidget(m_PdeSettings->toQVariantMap());

	m_graph_data = m_PdeSolver->solve(*m_PdeSettings);

	m_series = new QSurface3DSeries();

	m_graph->addSeries(m_series);

	connect(ui.EvaluatePushButton, SIGNAL(clicked()), this, SLOT(EvaluatePushButtonClicked()));
	connect(m_timer, SIGNAL(timeout()), this, SLOT(updateTimeSlice()));
	m_timer->start(m_graph_update_frequency);

	ui.GraphWidget->show();
}

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
	//m_graph->axisZ()->setRange(0.0f, 1.0f);

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


	ui.GraphWidget = QWidget::createWindowContainer(m_graph, this);
	ui.GraphWidget->resize(1000, 600);
	ui.GraphWidget->move(0, 10);

}

void MainWindow::init_PdeSettingsTableWidget(QVariantMap pde_settings_map)
{
	ui.PdeSettingsTableWidget->verticalHeader()->setVisible(false);
	ui.PdeSettingsTableWidget->setColumnWidth(0, 80);
	ui.PdeSettingsTableWidget->setColumnWidth(1, 80);
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
		set.V1 = V1;
		set.V2 = V2;

		return std::make_shared<PdeSettings>(set);
	}
	else
	{
		pde_file.open(QIODevice::ReadWrite);

		PdeSettings set;
		set.V1 = V1;
		set.V2 = V2;

		QJsonObject object = QJsonObject::fromVariantMap(set.toQVariantMap());

		QJsonDocument document;
		document.setObject(object);

		pde_file.write(document.toJson());
		pde_file.close();
		return std::make_shared<PdeSettings>(set);
	}
}


QSurfaceDataArray* newSurfaceDataArrayFromSource(
	QSurfaceDataArray& source_surface_data_array,
	std::function<void(QSurfaceDataItem&)> modifier) {
	int sampleCount = source_surface_data_array.size();
	auto newArray = new QSurfaceDataArray();
	newArray->reserve(sampleCount);
	for (int i(0); i < sampleCount; i++)
		newArray->append(new QSurfaceDataRow(sampleCount));
	for (int i(0); i < sampleCount; i++) {
		const QSurfaceDataRow& sourceRow = *(source_surface_data_array.at(i));
		QSurfaceDataRow& row = *(*newArray)[i];
		for (int j(0); j < sampleCount; j++) {
			row[j].setPosition(sourceRow.at(j).position());
			modifier(row[j]);
		}
	}
	return newArray;
};

void MainWindow::updateTimeSlice()
{
	if (m_current_time >= m_PdeSettings->countT)
		m_current_time = 0;
	auto qsurface_data_array = m_graph_data->at(m_current_time);
	auto modifier = [](QSurfaceDataItem item) -> void { item.position(); };
	
	m_series->dataProxy()->resetArray(newSurfaceDataArrayFromSource(*qsurface_data_array, modifier));

	//m_series->dataProxy()->resetArray(m_graph_data->at(current_time));
	++m_current_time;
}

void clear_data(std::shared_ptr<QVector<QtDataVisualization::QSurfaceDataArray*>> vector)
{
	//TODO: implent delete_data
}

void MainWindow::EvaluatePushButtonClicked()
{
	m_timer->stop();
	clear_data(m_graph_data);

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
