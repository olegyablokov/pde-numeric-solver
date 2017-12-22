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

#include "pde_solver_wave_equation.h"
#include "../math_module/math_module.h"

using namespace QtDataVisualization;
using namespace PdeSolver;

PdeSolverWaveEquation::PdeSolverWaveEquation() : PdeSolverBase()
{

}

PdeSolverWaveEquation::~PdeSolverWaveEquation()
{

}

QVector<SolutionMethod_t> PdeSolverWaveEquation::get_implemented_methods()
{
    QVector<SolutionMethod_t> methods;
    methods.push_back(PdeSolver::SolutionMethod_t("Crank-Nicolson", "Polar"));
    return methods;
}

void PdeSolverWaveEquation::get_solution(const PdeSettings& set, SolutionMethod_t method)
{
    if (method.coord_system != "Polar") throw("This method can be used only in polar coords");

    const PdeSettings::CoordGridSet_t& coordT = *set.get_coord_by_label("T");

    GraphSolution_t solution;
    solution.set = set;
    solution.graph_data.u_list.reserve(coordT.count);
    solution.graph_data.u_t_list.reserve(coordT.count);

    GraphDataSlice_t init_slice = get_initial_conditions_in_polar_coords(set);
    solution.graph_data.u_list.push_back(init_slice.u);
    solution.graph_data.u_t_list.push_back(init_slice.u_t);

    GraphDataSlice_t new_graph_data_slice;
    int t_count = 1;
    for (float t_val = coordT.min + coordT.step; t_val < coordT.max; t_val += coordT.step)
    {
        new_graph_data_slice = crank_nicolson_method(set, solution.graph_data, t_count);

        solution.graph_data.u_list.push_back(new_graph_data_slice.u);
        solution.graph_data.u_t_list.push_back(new_graph_data_slice.u_t);

        emit solution_progress_update("Computing the equation...", int(float(t_count * 100) / coordT.count));
        ++t_count;
    }

    emit solution_progress_update("", 100);
    qDebug() << "PdeSolverWaveEquation: Data generated";
    emit solution_generated(solution);
}

GraphDataSlice_t PdeSolverWaveEquation::crank_nicolson_method(const PdeSettings& set, const GraphData_t& graph_data, int t_count)
{
    const PdeSettings::CoordGridSet_t& coordF = *set.get_coord_by_label("F1");
    const PdeSettings::CoordGridSet_t& coordR = *set.get_coord_by_label("R");
    const PdeSettings::CoordGridSet_t& coordT = *set.get_coord_by_label("T");

    GraphDataSlice_t cur_graph_data_slice_reversed;
    cur_graph_data_slice_reversed.u = new QSurfaceDataArray();
    cur_graph_data_slice_reversed.u_t = new QSurfaceDataArray();
    cur_graph_data_slice_reversed.u->reserve(coordF.count);
    cur_graph_data_slice_reversed.u_t->reserve(coordF.count);

    std::vector<float> a(coordR.count, -qPow(set.c, 2) / qPow(coordR.step, 2));
    std::vector<float> b(coordR.count);
    std::vector<float> c(coordR.count);
    std::vector<float> d;

    int prev_i = 0, next_i = 0, prev_t_count = (t_count > 0) ? t_count - 1 : 0;
    float u_prev_t = 0.0f;

    float z_val = 0.0f, z_val_t = 0.0f, next_R_val = 0, u1, u2, u3, u4;
    for (int j = 0; j < coordF.count; ++j)
    {
        QSurfaceDataRow *row = new QSurfaceDataRow();
        QSurfaceDataRow *row_t = new QSurfaceDataRow();
        row->reserve(coordR.count);
        row_t->reserve(coordR.count);

        d.clear();
        d.reserve(coordR.count);
        next_R_val = 0;
        for (int i = 0; i < coordR.count; ++i)
        {
            if (i == 0) prev_i = i;
            else prev_i = i - 1;
            if (i >= coordR.count - 1) next_i = i;
            else next_i = i + 1;

            u_prev_t = (prev_t_count > 0) ? graph_data.u_list[prev_t_count - 1]->at(i)->at(j).y() :
                        (graph_data.u_list[0]->at(i)->at(j).y() - coordT.step * graph_data.u_t_list[0]->at(i)->at(j).y());

            next_R_val += coordR.step;
            b[i] = 1 / qPow(coordT.step, 2) + 2 * qPow(set.c, 2) / qPow(coordR.step, 2) + qPow(set.c, 2) / (coordR.step * next_R_val);
            c[i] = -(qPow(set.c, 2) / qPow(coordR.step, 2) + qPow(set.c, 2) / (coordR.step * next_R_val));

            u1 = qPow(set.c, 2) / qPow(coordR.step, 2) * graph_data.u_list[prev_t_count]->at(prev_i)->at(j).y();
            u2 = ((-2 * qPow(set.c, 2) / qPow(coordR.step, 2)) + 2 / qPow(coordT.step, 2) - qPow(set.c, 2) / (coordR.step * next_R_val)) *
                    graph_data.u_list[prev_t_count]->at(i)->at(j).y();
            u3 = (qPow(set.c, 2) / qPow(coordR.step, 2) + qPow(set.c, 2) / (coordR.step * next_R_val)) * graph_data.u_list[prev_t_count]->at(next_i)->at(j).y();
            u4 = -(1 / qPow(coordT.step, 2)) * u_prev_t;

            d.push_back(u1 + u2 + u3 + u4);
        }

        MathModule::solve_tridiagonal_equation(a, b, c, d, coordR.count);
        for (int i = 0; i < coordR.count; ++i)
        {
            auto& prev_vector = graph_data.u_list[prev_t_count]->at(i)->at(j);
            z_val = d[i];
            row->push_back(QVector3D(prev_vector.x(), z_val, prev_vector.z()));

            z_val_t = (z_val - prev_vector.y()) / (coordT.step / 2);
            row_t->push_back(QVector3D(prev_vector.x(), z_val_t, prev_vector.z()));
        }
        cur_graph_data_slice_reversed.u->push_back(row);
        cur_graph_data_slice_reversed.u_t->push_back(row_t);
    }

    //reversing the slice
    GraphDataSlice_t cur_graph_data_slice;
    cur_graph_data_slice.u = new QSurfaceDataArray();
    cur_graph_data_slice.u_t = new QSurfaceDataArray();
    cur_graph_data_slice.u->reserve(coordR.count);
    cur_graph_data_slice.u_t->reserve(coordR.count);

    for (int i = 0; i < coordR.count; ++i)
    {
        QSurfaceDataRow *row = new QSurfaceDataRow();
        QSurfaceDataRow *row_t = new QSurfaceDataRow();
        row->reserve(coordF.count);
        row_t->reserve(coordF.count);

        for (int j = 0; j < coordF.count; ++j)
        {
            auto& reversed_vector = cur_graph_data_slice_reversed.u->at(j)->at(i);
            row->push_back(QVector3D(reversed_vector.x(), reversed_vector.y(), reversed_vector.z()));
        }
        cur_graph_data_slice.u->push_back(row);
        cur_graph_data_slice.u_t->push_back(row_t);
    }
    clear_graph_data_slice(cur_graph_data_slice_reversed);

    return cur_graph_data_slice;
}
