#ifndef PDE_SOLVER_H
#define PDE_SOLVER_H

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/QValue3DAxis>
#include <QVector>
#include <QList>

#include <memory>
#include <functional>
#include <sys/types.h>

#include "pde_settings.h"

class PdeSolverBase
{
public:
    typedef std::pair<QtDataVisualization::QSurfaceDataArray*, QtDataVisualization::QSurfaceDataArray*> GraphDataSlice_t;  // first - u(x,t), second - partial du/dt(x,t) (here t is fixed)
    typedef std::pair<QList<QtDataVisualization::QSurfaceDataArray*>, QList<QtDataVisualization::QSurfaceDataArray*>> GraphData_t;  // first - u(x,t), second - partial du/dt(x,t) (here t is fixed)

    PdeSolverBase();
    virtual ~PdeSolverBase();

    virtual GraphData_t solve(const PdeSettings& set) = 0;

protected:
    GraphDataSlice_t get_initial_conditions(const PdeSettings& set);
};

#endif //PDE_SOLVER_H
