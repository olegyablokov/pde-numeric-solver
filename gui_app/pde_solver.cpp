//#include "pde_settings.h"
#include "pde_solver.h"

using namespace QtDataVisualization;


PdeSolver::PdeSolver()
{
}


PdeSolver::~PdeSolver()
{
}

void solve_tridiagonal_equation(std::vector<float>& a, std::vector<float>& b, std::vector<float>& c, std::vector<float>& d, int n) {
	/*
	// n is the number of unknowns

	|b0 c0 0 ||x0| |d0|
	|a1 b1 c1||x1|=|d1|
	|0  a2 b2||x2| |d2|

	1st iteration: b0x0 + c0x1 = d0 -> x0 + (c0/b0)x1 = d0/b0 ->

	x0 + g0x1 = r0               where g0 = c0/b0        , r0 = d0/b0

	2nd iteration:     | a1x0 + b1x1   + c1x2 = d1
	from 1st it.: -| a1x0 + a1g0x1        = a1r0
	-----------------------------
	(b1 - a1g0)x1 + c1x2 = d1 - a1r0

	x1 + g1x2 = r1               where g1=c1/(b1 - a1g0) , r1 = (d1 - a1r0)/(b1 - a1g0)

	3rd iteration:      | a2x1 + b2x2   = d2
	from 2nd it. : -| a2x1 + a2g1x2 = a2r2
	-----------------------
	(b2 - a2g1)x2 = d2 - a2r2
	x2 = r2                      where                     r2 = (d2 - a2r2)/(b2 - a2g1)
	Finally we have a triangular matrix:
	|1  g0 0 ||x0| |r0|
	|0  1  g1||x1|=|r1|
	|0  0  1 ||x2| |r2|

	Condition: ||bi|| > ||ai|| + ||ci||

	in this version the c matrix reused instead of g
	and             the d matrix reused instead of r and x matrices to report results
	Written by Keivan Moradi, 2014
	*/
	n--; // since we start from x0 (not x1)
	c[0] /= b[0];
	d[0] /= b[0];
	
	for (int i = 1; i < n; i++) {
		c[i] /= b[i] - a[i] * c[i - 1];
		d[i] = (d[i] - a[i] * d[i - 1]) / (b[i] - a[i] * c[i - 1]);
	}

	d[n] = (d[n] - a[n] * d[n - 1]) / (b[n] - a[n] * c[n - 1]);

	for (int i = n; i-- > 0;) {
		d[i] -= c[i] * d[i + 1];
	}
}

QSurfaceDataArray* PdeSolver::get_initial_array(const PdeSettings& pde_settings)
{
	QSurfaceDataArray* cur_array = new QSurfaceDataArray();

	int x_count = 0, y_count = 0;
	for (float x_val = pde_settings.minX; x_val < pde_settings.maxX; x_val += pde_settings.stepX)
	{
		QSurfaceDataRow *row = new QSurfaceDataRow();
		row->reserve(pde_settings.countY);
		y_count = 0;
		for (float y_val = pde_settings.minY; y_val < pde_settings.maxY; y_val += pde_settings.stepY)
		{
			//row->push_back(QVector3D(x_val, y_val, pde_settings.V1(QVector2D(x_val, y_val))));
			row->push_back(QVector3D(y_val, pde_settings.V1(QVector2D(x_val, y_val)), x_val));
			++y_count;
		}
		cur_array->push_back(row);
		++x_count;
	}

	return cur_array;
}

QSurfaceDataArray* PdeSolver::alternating_direction_method(const PdeSettings& pde_settings, const QSurfaceDataArray& prev_array, char stencil)
{
	int max_index1, max_index2;
	float step1, step2;
	if (stencil == 'x')
	{
		max_index1 = pde_settings.countY;
		max_index2 = pde_settings.countX;
		step1 = pde_settings.stepY;
		step2 = pde_settings.stepX;
	}
	else if (stencil == 'y')
	{
		max_index1 = pde_settings.countX;
		max_index2 = pde_settings.countY;
		step1 = pde_settings.stepX;
		step2 = pde_settings.stepY;
	}
	else throw("Wrong stencil");

	QSurfaceDataArray* cur_array = new QSurfaceDataArray;

	float z_val = 0.0f, u1, u2, u3;
	for (int index1 = 0; index1 < max_index1; ++index1)
	{
		std::vector<float> a(max_index1, -pde_settings.c * pde_settings.c / step1 / step1);
		std::vector<float> b(max_index1, 2 / pde_settings.stepT + 2 * pde_settings.c * pde_settings.c / step1 / step1);
		std::vector<float> c(max_index1, -pde_settings.c * pde_settings.c / step1 / step1);
		std::vector<float> d;

		QSurfaceDataRow *row = new QSurfaceDataRow();
		row->reserve(max_index1);

		for (int index2 = 0; index2 < max_index2; ++index2)
		{
			if (stencil == 'x')
			{
				u1 = index1 > 0 ? pde_settings.c * pde_settings.c / step2 / step2 * prev_array.at(index2)->at(index1 - 1).y() : 0;
				u2 = (2 / pde_settings.stepT - 2 * pde_settings.c * pde_settings.c / step2 / step2) * prev_array.at(index2)->at(index1).y();
				u3 = index1 < max_index1 - 1 ? pde_settings.c * pde_settings.c / step2 / step2 * prev_array.at(index2)->at(index1 + 1).y() : 0;
			}
			else if (stencil == 'y')
			{
				u1 = index1 > 0 ? pde_settings.c * pde_settings.c / step2 / step2 * prev_array.at(index1 - 1)->at(index2).y() : 0;
				u2 = (2 / pde_settings.stepT - 2 * pde_settings.c * pde_settings.c / step2 / step2) * prev_array.at(index1)->at(index2).y();
				u3 = index1 < max_index1 - 1 ? pde_settings.c * pde_settings.c / step2 / step2 * prev_array.at(index1 + 1)->at(index2).y() : 0;
			}

			d.push_back(u1 + u2 + u3);
		}

		solve_tridiagonal_equation(a, b, c, d, max_index1);
		for (int index2 = 0; index2 < max_index2; ++index2)
		{
			auto& prev_vector = prev_array.at(index2)->at(index1);
			z_val = d[index2];
			row->push_back(QVector3D(prev_vector.x(), z_val, prev_vector.z()));
		}
		cur_array->push_back(row);
	}
	return cur_array;
}

std::shared_ptr<QVector<QSurfaceDataArray*>> PdeSolver::solve(const PdeSettings& pde_settings)
{
	QVector<QSurfaceDataArray*> vector;
	vector.reserve(pde_settings.countT);

	vector.push_back(get_initial_array(pde_settings));

	int t_count = 1;
	for (float t_val = pde_settings.minT + pde_settings.stepT; t_val < pde_settings.maxT; t_val += pde_settings.stepT)
	{
		QSurfaceDataArray* half_new_array = alternating_direction_method(pde_settings, *vector[t_count - 1], 'x');
		QSurfaceDataArray* new_array = alternating_direction_method(pde_settings, *half_new_array, 'y');
		vector.push_back(new_array);

		++t_count;
	}

	return std::make_shared<QVector<QSurfaceDataArray*>>(vector);
}


/*
#include "pde_solver.h"

using namespace QtDataVisualization;


PdeSolver::PdeSolver()
{
}


PdeSolver::~PdeSolver()
{
}

void solve_tridiagonal_equation(std::vector<float>& a, std::vector<float>& b, std::vector<float>& c, std::vector<float>& d, int n) {

n--; // since we start from x0 (not x1)
c[0] /= b[0];
d[0] /= b[0];

for (int i = 1; i < n; i++) {
	c[i] /= b[i] - a[i] * c[i - 1];
	d[i] = (d[i] - a[i] * d[i - 1]) / (b[i] - a[i] * c[i - 1]);
}

d[n] = (d[n] - a[n] * d[n - 1]) / (b[n] - a[n] * c[n - 1]);

for (int i = n; i-- > 0;) {
	d[i] -= c[i] * d[i + 1];
}
}

QSurfaceDataArray* PdeSolver::get_initial_array(const PdeSettings& pde_settings)
{
	QSurfaceDataArray* cur_array = new QSurfaceDataArray();

	int x_count = 0, y_count = 0;
	for (float x_val = pde_settings.minX; x_val < pde_settings.maxX; x_val += pde_settings.stepX)
	{
		QSurfaceDataRow *row = new QSurfaceDataRow();
		row->reserve(pde_settings.countY);
		y_count = 0;
		for (float y_val = pde_settings.minY; y_val < pde_settings.maxY; y_val += pde_settings.stepY)
		{
			//row->push_back(QVector3D(x_val, y_val, pde_settings.V1(QVector2D(x_val, y_val))));
			row->push_back(QVector3D(y_val, pde_settings.V1(QVector2D(x_val, y_val)), x_val));
			++y_count;
		}
		cur_array->push_back(row);
		++x_count;
	}

	return cur_array;
}


QSurfaceDataArray* PdeSolver::alternating_direction_method_Y(const PdeSettings& pde_settings, const QSurfaceDataArray& prev_array)
{
	QSurfaceDataArray* new_array = new QSurfaceDataArray;

	float z_val = 0.0f;
	for (int x_count = 0; x_count < pde_settings.countX; ++x_count)
	{
		std::vector<float> a(pde_settings.countX, pde_settings.c * pde_settings.c / pde_settings.stepX / pde_settings.stepX);
		std::vector<float> b(pde_settings.countX, 2 / pde_settings.stepT + 2 * pde_settings.c * pde_settings.c / pde_settings.stepX / pde_settings.stepX);
		std::vector<float> c(pde_settings.countX, -pde_settings.c * pde_settings.c / pde_settings.stepX / pde_settings.stepX);
		std::vector<float> d;

		QSurfaceDataRow *row = new QSurfaceDataRow();
		row->reserve(pde_settings.countY);

		for (int y_count = 0; y_count < pde_settings.countY; ++y_count)
		{
			float u1 = pde_settings.c * pde_settings.c / pde_settings.stepY / pde_settings.stepY * prev_array.at(x_count > 0 ? x_count - 1 : 0)->at(y_count).y();
			float u2 = (4 / pde_settings.stepT / pde_settings.stepT - 2 * pde_settings.c * pde_settings.c / pde_settings.stepY / pde_settings.stepY) * prev_array.at(x_count)->at(y_count).y();
			float u3 = pde_settings.c * pde_settings.c / pde_settings.stepY / pde_settings.stepY * prev_array.at(x_count < 99 ? x_count + 1 : 99)->at(y_count).y();
			d.push_back(u1 + u2 + u3);
		}

		solve_tridiagonal_equation(a, b, c, d, pde_settings.countX);

		for (int y_count = 0; y_count < pde_settings.countY; ++y_count)
		{
			auto& prev_vector = prev_array.at(x_count)->at(y_count);
			z_val = d[y_count];
			row->push_back(QVector3D(prev_vector.x(), z_val, prev_vector.z()));
		}
		new_array->push_back(row);
	}
	return new_array;
}

QSurfaceDataArray* PdeSolver::alternating_direction_method_X(const PdeSettings& pde_settings, const QSurfaceDataArray& prev_array)
{
	QSurfaceDataArray* cur_array = new QSurfaceDataArray;

	float z_val = 0.0f;
	for (int y_count = 0; y_count < pde_settings.countY; ++y_count)
	{
		std::vector<float> a(pde_settings.countX, pde_settings.c * pde_settings.c / pde_settings.stepX / pde_settings.stepX);
		std::vector<float> b(pde_settings.countX, 2 / pde_settings.stepT + 2 * pde_settings.c * pde_settings.c / pde_settings.stepX / pde_settings.stepX);
		std::vector<float> c(pde_settings.countX, -pde_settings.c * pde_settings.c / pde_settings.stepX / pde_settings.stepX);
		std::vector<float> d;

		QSurfaceDataRow *row = new QSurfaceDataRow();
		row->reserve(pde_settings.countX);

		for (int x_count = 0; x_count < pde_settings.countX; ++x_count)
		{
			float u1 = pde_settings.c * pde_settings.c / pde_settings.stepY / pde_settings.stepY * prev_array.at(x_count)->at(y_count > 0 ? y_count - 1 : 0).y();
			float u2 = (4 / pde_settings.stepT / pde_settings.stepT - 2 * pde_settings.c * pde_settings.c / pde_settings.stepY / pde_settings.stepY) * prev_array.at(x_count)->at(y_count).y();
			float u3 = pde_settings.c * pde_settings.c / pde_settings.stepY / pde_settings.stepY * prev_array.at(x_count)->at(y_count < 99 ? y_count + 1 : 99).y();
			d.push_back(u1 + u2 + u3);
		}

		solve_tridiagonal_equation(a, b, c, d, pde_settings.countX);
		for (int x_count = 0; x_count < pde_settings.countX; ++x_count)
		{
			auto& prev_vector = prev_array.at(x_count)->at(y_count);
			z_val = d[x_count];
			row->push_back(QVector3D(prev_vector.x(), z_val, prev_vector.z()));
		}
		cur_array->push_back(row);
	}
	return cur_array;
}

std::shared_ptr<QVector<QSurfaceDataArray*>> PdeSolver::solve(const PdeSettings& pde_settings)
{
	QVector<QSurfaceDataArray*> vector;
	vector.reserve(pde_settings.countT);

	vector.push_back(get_initial_array(pde_settings));

	int t_count = 1;
	float coef = pde_settings.c * pde_settings.c * pde_settings.stepT;
	for (float t_val = pde_settings.minT + pde_settings.stepT; t_val < pde_settings.maxT; t_val += pde_settings.stepT)
	{
		//QSurfaceDataArray* array_half_stepT = new QSurfaceDataArray;
		//QSurfaceDataArray* array_whole_stepT = new QSurfaceDataArray;
		//array_half_stepT->reserve(pde_settings.countX);
		//array_whole_stepT->reserve(pde_settings.countX);

		QSurfaceDataArray* half_new_array = alternating_direction_method_X(pde_settings, *vector[t_count - 1]);
		QSurfaceDataArray* new_array = alternating_direction_method_X(pde_settings, *half_new_array);
		vector.push_back(new_array);

		//vector.push_back(alternating_direction_method_X(pde_settings, *vector[t_count - 1]));

		++t_count;
	}

	return std::make_shared<QVector<QSurfaceDataArray*>>(vector);
}
*/
