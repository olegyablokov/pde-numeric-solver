#ifndef PDE_SOLVER_HEAT_EQUATION_H
#define PDE_SOLVER_HEAT_EQUATION_H

#include "pde_solver_base.h"

/**
 * @brief A class for solving the 2d heat equation.
 */
class PdeSolverHeatEquation : public PdeSolverBase
{
    Q_OBJECT

public:
    PdeSolverHeatEquation();
    virtual ~PdeSolverHeatEquation();

    virtual QVector<PdeSolver::SolutionMethod_t> get_implemented_methods();

public slots:
    virtual void get_solution(const PdeSettings& set, PdeSolver::SolutionMethod_t method);

protected:
    PdeSolver::GraphDataSlice_t alternating_direction_method(const PdeSettings& set, const PdeSolver::GraphDataSlice_t& prev_graph_data_slice, char stencil);
};

#endif // PDE_SOLVER_HEAT_EQUATION_H
