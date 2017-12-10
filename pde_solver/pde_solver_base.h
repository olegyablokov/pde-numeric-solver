#ifndef PDE_SOLVER_H
#define PDE_SOLVER_H

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/QValue3DAxis>
#include <QVector>
#include <QList>
#include <QThread>
#include <QObject>

#include <memory>
#include <functional>
#include <sys/types.h>

#include "pde_settings.h"

class PdeSolverBase : public QObject
{
    Q_OBJECT

public:
    typedef std::pair<QtDataVisualization::QSurfaceDataArray*, QtDataVisualization::QSurfaceDataArray*> GraphDataSlice_t;  // first - u(x,t), second - partial du/dt(x,t) (here t is fixed)
    typedef std::pair<QList<QtDataVisualization::QSurfaceDataArray*>, QList<QtDataVisualization::QSurfaceDataArray*>> GraphData_t;  // first - u(x,t), second - partial du/dt(x,t) (here t is fixed)
    typedef struct
    {
        GraphData_t graph_data;
        PdeSettings set;
        //QVector<float> occuracy;
    } GraphSolution_t;

    PdeSolverBase(QObject *parent = NULL);
    virtual ~PdeSolverBase();

public slots:
    void solve(const PdeSettings& set);

private slots:
    virtual void get_solution(const PdeSettings& set);

signals:
    void solution_generated(PdeSolverBase::GraphSolution_t);
    void solve_invoked(const PdeSettings&);
    void solution_progress_update(QString, int);

protected:
    GraphDataSlice_t get_initial_conditions(const PdeSettings& set);
    void clear_graph_data_slice(GraphDataSlice_t& data_slice);
};

#endif //PDE_SOLVER_H
