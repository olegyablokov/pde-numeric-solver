#ifndef PDE_SOLVER_H
#define PDE_SOLVER_H

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/QValue3DAxis>
#include <QVector>

#include <memory>
#include <functional>
#include <sys/types.h>

#include "pde_settings.h"
//struct PdeSettings;

class PdeSolver
{
public:
	PdeSolver();
	~PdeSolver();

	std::shared_ptr<QVector<QtDataVisualization::QSurfaceDataArray*>> solve(const PdeSettings& pde_settings);
private:

    QtDataVisualization::QSurfaceDataArray* get_initial_array(const PdeSettings& pde_settings);

	QtDataVisualization::QSurfaceDataArray* alternating_direction_method(const PdeSettings& pde_settings, const QtDataVisualization::QSurfaceDataArray& prev_array, char stencil);
};

#endif //PDE_SOLVER_H
