#ifndef PDE_SOLVER_HEAT_EQUATION_H
#define PDE_SOLVER_HEAT_EQUATION_H

#include "pde_solver_base.h"

class PdeSolverHeatEquation : public PdeSolverBase
{
    Q_OBJECT

public:
    PdeSolverHeatEquation();
    virtual ~PdeSolverHeatEquation();

public slots:
    virtual void get_solution(const PdeSettings& set);

protected:
    GraphDataSlice_t alternating_direction_method(const PdeSettings& set, const GraphDataSlice_t& prev_graph_data_slice, char stencil);
};

#endif // PDE_SOLVER_HEAT_EQUATION_H
