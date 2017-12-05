#ifndef MATH_MODULE_H
#define MATH_MODULE_H

#include <vector>
#include "../pde_solver/pde_settings.h"
#include <QVector2D>

namespace MathModule
{
    void solve_tridiagonal_equation(std::vector<float>& a, std::vector<float>& b, std::vector<float>& c, std::vector<float>& d, int n);

    float solve_heat_equation_explicitly(QVector2D x, float t, const PdeSettings& set);
}

#endif // MATH_MODULE_H
