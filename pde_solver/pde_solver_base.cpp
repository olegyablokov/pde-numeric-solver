//#include "pde_settings.h"
#include "pde_solver_base.h"

using namespace QtDataVisualization;


PdeSolverBase::PdeSolverBase()
{
}


PdeSolverBase::~PdeSolverBase()
{
}


PdeSolverBase::GraphDataSlice_t PdeSolverBase::get_initial_conditions(const PdeSettings& set)
{
    auto cur_array = new QSurfaceDataArray();
    auto cur_array_t = new QSurfaceDataArray();  // partial du/dt

	int x_count = 0, y_count = 0;
    for (float x_val = set.minX; x_val < set.maxX; x_val += set.stepX)
	{
        auto row = new QSurfaceDataRow();
        auto row_t = new QSurfaceDataRow();
        row->reserve(set.countY);
		y_count = 0;
        for (float y_val = set.minY; y_val < set.maxY; y_val += set.stepY)
		{
			//row->push_back(QVector3D(x_val, y_val, pde_settings.V1(QVector2D(x_val, y_val))));
            row->push_back(QVector3D(y_val, set.V1(QVector2D(x_val, y_val)), x_val));
			++y_count;

            row_t->push_back(QVector3D(y_val, set.V2(QVector2D(x_val, y_val)), x_val));
        }
		cur_array->push_back(row);
        cur_array_t->push_back(row_t);
		++x_count;
	}

    return std::make_pair(cur_array, cur_array_t);
}
