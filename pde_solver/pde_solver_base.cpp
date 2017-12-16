#include "pde_solver_base.h"
#include <unistd.h>

using namespace QtDataVisualization;

Q_DECLARE_METATYPE(PdeSolverBase::GraphDataSlice_t);
Q_DECLARE_METATYPE(PdeSolverBase::GraphData_t);
Q_DECLARE_METATYPE(PdeSolverBase::GraphSolution_t);
Q_DECLARE_METATYPE(PdeSettings);

PdeSolverBase::PdeSolverBase(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<GraphDataSlice_t>();
    qRegisterMetaType<GraphData_t>();
    qRegisterMetaType<GraphSolution_t>();
    qRegisterMetaType<PdeSettings>();

    connect(this, SIGNAL(solve_invoked(PdeSettings)), this, SLOT(get_solution(PdeSettings)));
}


PdeSolverBase::~PdeSolverBase()
{

}

void PdeSolverBase::solve(const PdeSettings& set)
{
    emit solve_invoked(set);
}

void PdeSolverBase::clear_graph_data_slice(PdeSolverBase::GraphDataSlice_t& data_slice)
{
    for (auto& row_ptr : *data_slice.u) delete row_ptr;
    delete data_slice.u;

    for (auto& row_ptr : *data_slice.u_t) delete row_ptr;
    delete data_slice.u_t;
}

PdeSolverBase::GraphDataSlice_t PdeSolverBase::get_initial_conditions(const PdeSettings& set)
{
    qDebug() << "PdeSolverBase::get_initial_conditions invoked";
    GraphDataSlice_t graph_data_slice;
    graph_data_slice.u = new QSurfaceDataArray();
    graph_data_slice.u_t = new QSurfaceDataArray();  // partial 𝛿u/𝛿t

    graph_data_slice.u->reserve(set.countX);
    graph_data_slice.u_t->reserve(set.countX);

	int x_count = 0, y_count = 0;
    for (float x_val = set.minX; x_val < set.maxX; x_val += set.stepX)
	{
        auto row = new QSurfaceDataRow();
        auto row_t = new QSurfaceDataRow();

        row->reserve(set.countY);
        row_t->reserve(set.countY);

		y_count = 0;
        for (float y_val = set.minY; y_val < set.maxY; y_val += set.stepY)
		{
            row->push_back(QVector3D(y_val, set.V1(QVector2D(x_val, y_val)), x_val));
            ++y_count;
            row_t->push_back(QVector3D(y_val, set.V2(QVector2D(x_val, y_val)), x_val));

            emit solution_progress_update("Computing initial conditions...", int(float(x_count * 100) / set.countX));
        }
        graph_data_slice.u->push_back(row);
        graph_data_slice.u_t->push_back(row_t);
		++x_count;
	}
    qDebug() << "PdeSolverBase::get_initial_conditions returned";

    return graph_data_slice;
}

void PdeSolverBase::get_solution(const PdeSettings& set)
{
    throw("Error: calling PdeSolverBase::get_solution method (it is a base class)");
}

//PdeSolverBase::GraphDataSlice_t::GraphDataSlice_t(QtDataVisualization::QSurfaceDataArray* u_, QtDataVisualization::QSurfaceDataArray* u_t) { u = u_; u_t = u_t_; }
