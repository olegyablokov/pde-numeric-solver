#ifndef PDE_SOLVER_WAVE_EQUATION_H
#define PDE_SOLVER_WAVE_EQUATION_H

#include "pde_solver_base.h"

class PdeSolverWaveEquation : public PdeSolverBase
{
public:
    PdeSolverWaveEquation();
    virtual ~PdeSolverWaveEquation();

    virtual GraphData_t solve(const PdeSettings& set);

protected:
    PdeSolverBase::GraphDataSlice_t alternating_direction_method(const PdeSettings& set,
                                                                 const PdeSolverBase::GraphDataSlice_t& prev_graph_data_slice, char stencil);
};

#endif // PDE_SOLVER_WAVE_EQUATION_H
