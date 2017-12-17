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
#include <QLabel>
#include <QThread>

#include <QJsonValue>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariant>
#include <QToolTip>
#include <iostream>
#include <fstream>
#include <QFile>
#include <QString>

#include <sys/types.h>
#include <functional>
#include <memory>
#include <string>

#include "../pde_solver/pde_settings.h"
#include "ui_mainwindow.h"

#include "../pde_solver/pde_solver_base.h"
#include "../pde_solver/pde_solver_heat_equation.h"
#include "../pde_solver/pde_solver_wave_equation.h"
#include "../pde_solver/pde_solver_structs.h"

/**
 * @brief The main controlling GUI class.
 */
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
    virtual ~MainWindow();

    void start();

    QWidget* m_GraphWidget;

public slots:
    void update_TimeSlice();
    void EvaluatePushButton_clicked();

    void PlayStopPushButton_clicked();
    void NextSlidePushButton_clicked();
    void PrevSlidePushButton_clicked();
    void FirstSlidePushButton_clicked();
    void LastSlidePushButton_clicked();

    void toggle_graph_playing(bool play);
    void change_pde_solver(QString new_solver);
    void change_pde_settings_coord_system(QString new_coord_system);

    void GraphTimeSpeedSlider_changed(int);

    void graph_solution_generated(PdeSolver::GraphSolution_t);
    void solution_progress_updated(QString, int);

private:
    void init_graph();
    void set_PdeSettingsTableWidget(const PdeSettings& set);
	std::shared_ptr<PdeSettings> init_pde_settings(QString pde_settings_filename);
    void init_EquationComboBox();

    PdeSettings get_pde_settings_from_TableWidget();

    void set_TimeSlice(int new_time_slice);

	Ui::MainWindowClass ui;

    void clear_graph_data(PdeSolver::GraphData_t& graph_data);

    std::shared_ptr<PdeSettings> m_PdeSettings;
	QString m_pde_settings_filename;

    QtDataVisualization::QSurface3DSeries *m_series;
    QtDataVisualization::Q3DSurface *m_graph;
    std::shared_ptr<PdeSolverBase> m_PdeSolver;  // std::shared_ptr<PdeSolver>
    PdeSolver::GraphData_t m_graph_data;

    bool m_graph_is_valid = false;

    int m_current_time_slice = 0;
    int m_graph_update_time_step = 40;  // in ms

    QThread m_GraphThread;

    QTimer* m_timer;

    QLabel* m_GraphOccuracyLabel;

    QHBoxLayout* m_GraphTimeSpeedLayout;
    QSlider* m_GraphTimeSpeedSlider;
    QLabel* m_GraphTimeSpeedLabel;

    QHBoxLayout* m_GraphCurrentTimeLayout;
    QSlider* m_GraphCurrentTimeSlider;
    QLabel* m_GraphCurrentTimeLabel;

    QPushButton* m_PlayStopPushButton;
    QPushButton* m_NextSlidePushButton;
    QPushButton* m_PrevSlidePushButton;
    QPushButton* m_LastSlidePushButton;
    QPushButton* m_FirstSlidePushButton;
};

#endif //MAIN_WINDOW_H
