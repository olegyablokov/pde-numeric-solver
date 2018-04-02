/*
    Copyright (c) 2017 Oleg Yablokov

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

**/

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
    PdeSolver::GraphDataSlice_t alternating_direction_method(const PdeSettings& set, const PdeSolver::GraphDataSlice_t& prev_graph_data_slice, char stencil, double t_count);
};

#endif // PDE_SOLVER_HEAT_EQUATION_H
