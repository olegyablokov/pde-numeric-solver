#include "pde_solver_base.h"
#include <unistd.h>

using namespace QtDataVisualization;
using namespace PdeSolver;

Q_DECLARE_METATYPE(PdeSolver::GraphDataSlice_t);
Q_DECLARE_METATYPE(PdeSolver::GraphData_t);
Q_DECLARE_METATYPE(PdeSolver::GraphSolution_t);
Q_DECLARE_METATYPE(PdeSolver::SolutionMethod_t);
Q_DECLARE_METATYPE(PdeSettings);

PdeSolverBase::PdeSolverBase(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<GraphDataSlice_t>();
    qRegisterMetaType<GraphData_t>();
    qRegisterMetaType<GraphSolution_t>();
    qRegisterMetaType<SolutionMethod_t>();
    qRegisterMetaType<PdeSettings>();

    connect(this, SIGNAL(solve_invoked(PdeSettings, PdeSolver::SolutionMethod_t)), this, SLOT(get_solution(PdeSettings, PdeSolver::SolutionMethod_t)));
}


PdeSolverBase::~PdeSolverBase()
{

}

QVector<SolutionMethod_t> PdeSolverBase::get_implemented_methods()
{
    throw("Error: calling PdeSolverBase::get_implemented_methods method (it is a base class)");
}

void PdeSolverBase::solve(const PdeSettings& set, SolutionMethod_t method)
{
    emit solve_invoked(set, method);
}

void PdeSolverBase::clear_graph_data_slice(GraphDataSlice_t& data_slice)
{
    for (auto& row_ptr : *data_slice.u) delete row_ptr;
    delete data_slice.u;

    for (auto& row_ptr : *data_slice.u_t) delete row_ptr;
    delete data_slice.u_t;
}

GraphDataSlice_t PdeSolverBase::get_initial_conditions_in_cartesian_coords(const PdeSettings& set)
{
    const PdeSettings::CoordGridSet_t& coordX1 = *set.get_coord_by_label("X1");
    const PdeSettings::CoordGridSet_t& coordX2 = *set.get_coord_by_label("X2");

    GraphDataSlice_t graph_data_slice;
    graph_data_slice.u = new QSurfaceDataArray();
    graph_data_slice.u_t = new QSurfaceDataArray();  // partial 𝛿u/𝛿t

    graph_data_slice.u->reserve(coordX1.count);
    graph_data_slice.u_t->reserve(coordX1.count);

    int x1_count = 0;
    for (float x1_val = coordX1.min; x1_val < coordX1.max; x1_val += coordX1.step)
    {
        emit solution_progress_update("Computing initial conditions...", int(float(x1_count * 100) / coordX1.count));

        auto row = new QSurfaceDataRow();
        auto row_t = new QSurfaceDataRow();

        row->reserve(coordX2.count);
        row_t->reserve(coordX2.count);

        for (float x2_val = coordX2.min; x2_val < coordX2.max; x2_val += coordX2.step)
        {
            row->push_back(QVector3D(x2_val, set.V1(QVector2D(x1_val, x2_val)), x1_val));
            row_t->push_back(QVector3D(x2_val, set.V2(QVector2D(x1_val, x2_val)), x1_val));
        }

        graph_data_slice.u->push_back(row);
        graph_data_slice.u_t->push_back(row_t);
        ++x1_count;
    }
    qDebug() << "PdeSolverBase::get_initial_conditions returned";

    return graph_data_slice;
}

GraphDataSlice_t PdeSolverBase::get_initial_conditions_in_polar_coords(const PdeSettings& set)
{
    qDebug() << "PdeSolverBase::get_initial_conditions invoked";

    const PdeSettings::CoordGridSet_t& coordF = *set.get_coord_by_label("F1");
    const PdeSettings::CoordGridSet_t& coordR = *set.get_coord_by_label("R");

    GraphDataSlice_t graph_data_slice;
    graph_data_slice.u = new QSurfaceDataArray();
    graph_data_slice.u_t = new QSurfaceDataArray();  // partial 𝛿u/𝛿t

    graph_data_slice.u->reserve(coordR.count);
    graph_data_slice.u_t->reserve(coordR.count);

    int R_count = 0, F_count = 0;
    for (float R_val = coordR.min; R_val < coordR.max; R_val += coordR.step)
    {
        emit solution_progress_update("Computing initial conditions...", int(float(R_count * 100) / coordR.count));

        auto row = new QSurfaceDataRow();
        auto row_t = new QSurfaceDataRow();

        row->reserve(coordR.count);
        row_t->reserve(coordR.count);

        F_count = 0;
        for (float F_val = coordF.min; F_val < coordF.max; F_val += coordF.step)
        {
            row->push_back(QVector3D(F_val, set.V1(QVector2D(R_val, F_val)), R_val));
            row_t->push_back(QVector3D(F_val, set.V2(QVector2D(R_val, F_val)), R_val));
            ++F_count;
        }
        graph_data_slice.u->push_back(row);
        graph_data_slice.u_t->push_back(row_t);
        ++R_count;
    }
    qDebug() << "PdeSolverBase::get_initial_conditions returned";

    return graph_data_slice;
}

void PdeSolverBase::get_solution(const PdeSettings& set, SolutionMethod_t method)
{
    throw("Error: calling PdeSolverBase::get_solution method (it is a base class)");
}
