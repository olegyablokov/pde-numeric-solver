#include "pde_solver_wave_equation.h"
#include "../math_module/math_module.h"

using namespace QtDataVisualization;

PdeSolverWaveEquation::PdeSolverWaveEquation() : PdeSolverBase()
{

}

PdeSolverWaveEquation::~PdeSolverWaveEquation()
{

}

void PdeSolverWaveEquation::get_solution(const PdeSettings& set)
{
    GraphSolution_t solution;
    solution.set = set;

    GraphDataSlice_t init_slice = get_initial_conditions(set);
    solution.graph_data.first.push_back(init_slice.first);
    solution.graph_data.second.push_back(init_slice.second);

//    //explicit solution
//    double val = MathModule::solve_heat_equation_explicitly(QVector2D(1, 1), set.minT, set);
//    solution.occuracy.push_back(val);

    GraphDataSlice_t cur_graph_data_slice;
    GraphDataSlice_t half_new_graph_data_slice;
    GraphDataSlice_t new_graph_data_slice;
    int t_count = 1;
    for (float t_val = set.minT + set.stepT; t_val < set.maxT; t_val += set.stepT)
    {
        cur_graph_data_slice.first = solution.graph_data.first.at(t_count - 1);
        cur_graph_data_slice.second = solution.graph_data.second.at(t_count - 1);

        half_new_graph_data_slice = alternating_direction_method(set, cur_graph_data_slice, 'x');
        new_graph_data_slice = alternating_direction_method(set, half_new_graph_data_slice, 'y');

        solution.graph_data.first.push_back(new_graph_data_slice.first);
        solution.graph_data.second.push_back(new_graph_data_slice.second);

//        //explicit solution
//        double val = MathModule::solve_heat_equation_explicitly(QVector2D(1, 1), t_val, set);
//        solution.occuracy.push_back(val);

        emit solution_progress_update("Computing the equation...", int(float(t_count * 100) / set.countT));
        ++t_count;
    }

    emit solution_progress_update("", 100);
    qDebug() << "PdeSolverWaveEquation: Data generated";
    emit solution_generated(solution);
}

PdeSolverBase::GraphDataSlice_t PdeSolverWaveEquation::alternating_direction_method(const PdeSettings& set,
                                                                                    const GraphDataSlice_t& prev_graph_data_slice, char stencil)
{
    int max_index1, max_index2;
    float step1, step2;
    if (stencil == 'x')
    {
        max_index1 = set.countY;
        max_index2 = set.countX;
        step1 = set.stepY;
        step2 = set.stepX;
    }
    else if (stencil == 'y')
    {
        max_index1 = set.countX;
        max_index2 = set.countY;
        step1 = set.stepX;
        step2 = set.stepY;
    }
    else throw("Wrong stencil");

    GraphDataSlice_t cur_graph_data_slice(new QSurfaceDataArray(), new QSurfaceDataArray());

    std::vector<float> a(max_index1, -set.c * set.c / step1 / step1);
    std::vector<float> b(max_index1, 4 / set.stepT / set.stepT + 2 * set.c * set.c / step1 / step1);
    std::vector<float> c(max_index1, -set.c * set.c / step1 / step1);
    float z_val = 0.0f, z_val_t = 0.0f, u1, u2, u3, u4;
    for (int index1 = 0; index1 < max_index1; ++index1)
    {
        std::vector<float> d;
        QSurfaceDataRow *row = new QSurfaceDataRow();
        QSurfaceDataRow *row_t = new QSurfaceDataRow();
        row->reserve(max_index1);

        for (int index2 = 0; index2 < max_index2; ++index2)
        {
//            if ((index1 == 0) || (index2 == 0) || (index1 == max_index1 - 1) || (index2 == max_index2 - 1))
//            {
//                u1 = 0;
//                u2 = 0;
//                u3 = 0;
//                u4 = 0;
//            }
            if (stencil == 'x')
            {
                u1 = index2 > 0 ? set.c * set.c / step2 / step2 * prev_graph_data_slice.first->at(index1)->at(index2 - 1).y() : 0;
                u2 = (4 / set.stepT / set.stepT - 2 * set.c * set.c / step2 / step2) * prev_graph_data_slice.first->at(index1)->at(index2).y();
                u3 = index2 < max_index2 - 1 ? set.c * set.c / step2 / step2 * prev_graph_data_slice.first->at(index1)->at(index2 + 1).y() : 0;
                u4 = (2 / set.stepT) * prev_graph_data_slice.second->at(index1)->at(index2).y();
            }
            else if (stencil == 'y')
            {
                u1 = index1 > 0 ? set.c * set.c / step2 / step2 * prev_graph_data_slice.first->at(index1 - 1)->at(index2).y() : 0;
                u2 = (4 / set.stepT / set.stepT - 2 * set.c * set.c / step2 / step2) * prev_graph_data_slice.first->at(index1)->at(index2).y();
                u3 = index1 < max_index1 - 1 ? set.c * set.c / step2 / step2 * prev_graph_data_slice.first->at(index1 + 1)->at(index2).y() : 0;
                u4 = (2 / set.stepT) * prev_graph_data_slice.second->at(index1)->at(index2).y();
            }

            d.push_back(u1 + u2 + u3 + u4);
        }

        MathModule::solve_tridiagonal_equation(a, b, c, d, max_index1);
        for (int index2 = 0; index2 < max_index2; ++index2)
        {
            auto& prev_vector = prev_graph_data_slice.first->at(index1)->at(index2);

            z_val = d[index2];
            row->push_back(QVector3D(prev_vector.x(), z_val, prev_vector.z()));

            z_val_t = + (z_val - prev_vector.y()) / (set.stepT / 2);
            row_t->push_back(QVector3D(prev_vector.x(), z_val_t, prev_vector.z()));
        }
        cur_graph_data_slice.first->push_back(row);
        cur_graph_data_slice.second->push_back(row_t);
    }

    return cur_graph_data_slice;
}
