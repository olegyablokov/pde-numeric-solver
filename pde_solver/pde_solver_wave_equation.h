#ifndef PDE_SOLVER_WAVE_EQUATION_H
#define PDE_SOLVER_WAVE_EQUATION_H

#include "pde_solver_base.h"

/**
 * @brief A class for solving the 2d wave equation.
 */
class PdeSolverWaveEquation : public PdeSolverBase
{
    Q_OBJECT

public:
    PdeSolverWaveEquation();
    virtual ~PdeSolverWaveEquation();

    virtual QVector<PdeSolver::SolutionMethod_t> get_implemented_methods();

public slots:
    virtual void get_solution(const PdeSettings& set, PdeSolver::SolutionMethod_t method);

protected:
    PdeSolver::GraphDataSlice_t crank_nicolson_method(const PdeSettings& set, const PdeSolver::GraphData_t& graph_data, int t_count);
};

#endif // PDE_SOLVER_WAVE_EQUATION_H
