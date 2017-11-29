#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/QValue3DAxis>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QTimer>
#include <QTableWidgetItem>

#include <QJsonValue>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariant>
#include <iostream>
#include <fstream>
#include <QFile>
#include <QString>

#include <sys/types.h>
#include <functional>
#include <memory>
#include <string>

#include "pde_settings.h"
#include "pde_solver.h"
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);

    QWidget* m_GraphWidget;

public slots:
	void updateTimeSlice();
	void EvaluatePushButtonClicked();
    //void PdeSettingsTableWidgetCellClickedSlot(int, int);

private:
    QtDataVisualization::Q3DSurface* init_graph(QtDataVisualization::QSurface3DSeries* series);
    void init_PdeSettingsTableWidget(QVariantMap pde_settings_map);
	std::shared_ptr<PdeSettings> init_pde_settings(QString pde_settings_filename);

	Ui::MainWindowClass ui;

    void clearData();

	//
	QList<std::shared_ptr<QVector3D>>* m_vector;
	//

	std::shared_ptr<PdeSettings> m_PdeSettings;
	QString m_pde_settings_filename;
	int m_current_time = 0;
	//QtDataVisualization::QSurfaceDataProxy m_SurfaceDataProxy;
	std::shared_ptr<PdeSolver> m_PdeSolver;
	std::shared_ptr<QVector<QtDataVisualization::QSurfaceDataArray*>> m_graph_data;
	int m_graph_update_frequency = 40;  // in ms

	QtDataVisualization::QSurface3DSeries *m_series;
	QtDataVisualization::Q3DSurface *m_graph;
	QTimer *m_timer;
};

#endif //MAIN_WINDOW_H
