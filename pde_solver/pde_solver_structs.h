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

#ifndef PDE_SOLVER_STRUCTS_H
#define PDE_SOLVER_STRUCTS_H

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/QValue3DAxis>
#include <QString>

#include "pde_settings.h"

namespace PdeSolver
{
    /**
     * @brief A slice of graph data.
     */
    struct GraphDataSlice_t
    {
        QtDataVisualization::QSurfaceDataArray* u;      /**< The pointer to the u(x, t) data slice (with a fixed t) */
        QtDataVisualization::QSurfaceDataArray* u_t;    /**< The pointer to the partial derivative 𝛿u/𝛿t(x, t) data slice (with a fixed t) */
    };

    /**
     * @brief Slices of graph data.
     */
    struct GraphData_t
    {
        QList<QtDataVisualization::QSurfaceDataArray*> u_list;      /**< A list of slices. Here the index of Qlist is time and the array pointers are time slices of the u(x, t) function */
        QList<QtDataVisualization::QSurfaceDataArray*> u_t_list;    /**< A list of slices. Here the index of Qlist is time and the array pointers are time slices of the partial 𝛿u/𝛿t(x, t) function */
    };

    /**
     * @brief The output type of a solution.
     * @see get_solution(const PdeSettings& set)
     */
    struct GraphSolution_t
    {
        GraphData_t graph_data;         /**< main graph data */
        PdeSettings set;                /**< settings used when solving pde */
    };

    struct SolutionMethod_t
    {
        QString name;
        QString coord_system;

        SolutionMethod_t() { name = "<method_name>"; coord_system = "<coord_system>"; }
//        ~SolutionMethod_t() {}
        SolutionMethod_t(QString name_, QString coord_system_) { name = name_; coord_system = coord_system_; }
    };
}

#endif // PDE_SOLVER_STRUCTS_H
