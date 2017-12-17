#include "pde_solver_heat_equation.h"
#include "../math_module/math_module.h"

using namespace QtDataVisualization;
using namespace PdeSolver;

PdeSolverHeatEquation::PdeSolverHeatEquation() : PdeSolverBase()
{

}

PdeSolverHeatEquation::~PdeSolverHeatEquation()
{

}

QVector<SolutionMethod_t> PdeSolverHeatEquation::get_implemented_methods()
{
    QVector<SolutionMethod_t> methods;
    methods.push_back(PdeSolver::SolutionMethod_t("Alternating direction implicit", "Cartesian"));
    return methods;
}

void PdeSolverHeatEquation::get_solution(const PdeSettings& set, SolutionMethod_t method)
{
    if (method.coord_system != "Cartesian") throw("This method can be used only in Cartesian coords");

    const PdeSettings::CoordGridSet_t& coordT = *set.get_coord_by_label("T");

    GraphSolution_t solution;
    solution.set = set;
    solution.graph_data.u_list.reserve(coordT.count);
    solution.graph_data.u_t_list.reserve(coordT.count);

    GraphDataSlice_t init_slice = get_initial_conditions_in_cartesian_coords(set);
    solution.graph_data.u_list.push_back(init_slice.u);
    solution.graph_data.u_t_list.push_back(init_slice.u_t);

    GraphDataSlice_t cur_graph_data_slice;
    GraphDataSlice_t half_new_graph_data_slice;
    GraphDataSlice_t new_graph_data_slice;
    int t_count = 1;
    for (float t_val = coordT.min + coordT.step; t_val < coordT.max; t_val += coordT.step)
    {
        cur_graph_data_slice.u = solution.graph_data.u_list.at(t_count - 1);
        cur_graph_data_slice.u_t = solution.graph_data.u_t_list.at(t_count - 1);

        half_new_graph_data_slice = alternating_direction_method(set, cur_graph_data_slice, 'x');
        new_graph_data_slice = alternating_direction_method(set, half_new_graph_data_slice, 'y');

        solution.graph_data.u_list.push_back(new_graph_data_slice.u);
        solution.graph_data.u_t_list.push_back(new_graph_data_slice.u_t);

        clear_graph_data_slice(half_new_graph_data_slice);

        emit solution_progress_update("Computing the equation...", int(float(t_count * 100) / coordT.count));
        ++t_count;
    }

    emit solution_progress_update("", 100);
    qDebug() << "PdeSolverHeatEquation: Data generated";
    emit solution_generated(solution);
}

GraphDataSlice_t PdeSolverHeatEquation::alternating_direction_method(const PdeSettings& set,
                                                                     const GraphDataSlice_t& prev_graph_data_slice, char stencil)
{
    const PdeSettings::CoordGridSet_t& coordX1 = *set.get_coord_by_label("X1");
    const PdeSettings::CoordGridSet_t& coordX2 = *set.get_coord_by_label("X2");
    const PdeSettings::CoordGridSet_t& coordT = *set.get_coord_by_label("T");

    int max_index1, max_index2;
    float step1, step2;
    if (stencil == 'x')
    {
        max_index1 = coordX2.count;
        max_index2 = coordX1.count;
        step1 = coordX1.step;
        step2 = coordX2.step;
    }
    else if (stencil == 'y')
    {
        max_index1 = coordX1.count;
        max_index2 = coordX2.count;
        step1 = coordX2.step;
        step2 = coordX1.step;
    }
    else throw("Wrong stencil");

    GraphDataSlice_t cur_graph_data_slice;
    cur_graph_data_slice.u = new QSurfaceDataArray();
    cur_graph_data_slice.u_t = new QSurfaceDataArray();
    cur_graph_data_slice.u->reserve(max_index1);

    std::vector<float> a(max_index1, -set.c * set.c / step1 / step1);
    std::vector<float> b(max_index1, 2 / coordT.step + 2 * set.c * set.c / step1 / step1);
    std::vector<float> c(max_index1, -set.c * set.c / step1 / step1);
    std::vector<float> d;

    int prev_ind1 = 0, next_ind1 = 0, prev_ind2 = 0, next_ind2 = 0;

    float z_val = 0.0f, u1, u2, u3;
    for (int index1 = 0; index1 < max_index1; ++index1)
    {
        QSurfaceDataRow *row = new QSurfaceDataRow();
        row->reserve(max_index2);

        d.clear();
        for (int index2 = 0; index2 < max_index2; ++index2)
        {
            if (index1 == 0) prev_ind1 = index1;
            else prev_ind1 = index1 - 1;
            if (index2 == 0) prev_ind2 = index2;
            else prev_ind2 = index2 - 1;

            if (index1 >= max_index1 - 1) next_ind1 = index1;
            else next_ind1 = index1 + 1;
            if (index2 >= max_index2 - 1) next_ind2 = index2;
            else next_ind2 = index2 + 1;

            if ((index1 == 0) || (index2 == 0) || (index1 == max_index1 - 1) || (index2 == max_index2 - 1))
            {
                u1 = 0;
                u2 = (2 / coordT.step - 2 * set.c * set.c / step2 / step2) * prev_graph_data_slice.u->at(index1)->at(index2).y();
                u3 = 0;
            }
            else if (stencil == 'x')
            {
                u1 = set.c * set.c / step2 / step2 * prev_graph_data_slice.u->at(index1)->at(prev_ind2).y();
                u2 = (2 / coordT.step - 2 * set.c * set.c / step2 / step2) * prev_graph_data_slice.u->at(index1)->at(index2).y();
                u3 = set.c * set.c / step2 / step2 * prev_graph_data_slice.u->at(index1)->at(next_ind2).y();
            }
            else if (stencil == 'y')
            {
                u1 = set.c * set.c / step2 / step2 * prev_graph_data_slice.u->at(prev_ind1)->at(index2).y();
                u2 = (2 / coordT.step - 2 * set.c * set.c / step2 / step2) * prev_graph_data_slice.u->at(index1)->at(index2).y();
                u3 = set.c * set.c / step2 / step2 * prev_graph_data_slice.u->at(next_ind1)->at(index2).y();
            }

            d.push_back(u1 + u2 + u3);
        }

        MathModule::solve_tridiagonal_equation(a, b, c, d, max_index1);
        for (int index2 = 0; index2 < max_index2; ++index2)
        {
            auto& prev_vector = prev_graph_data_slice.u->at(index2)->at(index1);
            z_val = d[index2];
            row->push_back(QVector3D(prev_vector.x(), z_val, prev_vector.z()));
        }
        cur_graph_data_slice.u->push_back(row);
    }

    return cur_graph_data_slice;
}
