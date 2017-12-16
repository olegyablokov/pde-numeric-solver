#include "pde_settings.h"
#include <QVector2D>
#include <algorithm>
#include <cassert>

PdeSettings::PdeSettings()
{
    set_boundaries();
}

PdeSettings::PdeSettings(QVariantMap& map)
{
    reset(map);
}

PdeSettings::~PdeSettings()
{
}

float PdeSettings::evaluate_expression(QString expression, QVector2D x) const
{
    float R = qSqrt(x[0] * x[0] + x[1] * x[1]);

    //TODO: why problems with declaring QScriptEngine instance in class?
    QScriptEngine m_ScriptEngine1;

    expression.replace("x", QString::number(x[0] / m));
    expression.replace("y", QString::number(x[1] / m));
    expression.replace("R", QString::number(R / (m * m)));
    expression.replace("sqrt", "Math.sqrt");
    expression.replace("sin", "Math.sin");
    expression.replace("cos", "Math.cos");
    expression.replace("tan", "Math.tan");
    expression.replace("abs", "Math.abs");
    expression.replace("pow", "Math.pow");
    expression.replace("PI", "Math.PI");
    expression.replace("E", "Math.E");
    expression.replace("--", "-");

    return float(m_ScriptEngine1.evaluate(expression).toNumber());
}

float PdeSettings::V1(QVector2D x) const
{
    return evaluate_expression(V1_str, x);
}

float PdeSettings::V2(QVector2D x) const
{
    return evaluate_expression(V2_str, x);
}

void PdeSettings::reset(QVariantMap& map)
{
    if (map.contains("V1")) V1_str = map["V1"].value<QString>();
    if (map.contains("V2")) V2_str = map["V2"].value<QString>();

    if (map.contains("c")) c = map["c"].value<float>();
    if (map.contains("m")) m = map["m"].value<float>();

    if (map.contains("countX(Y)"))
    {
        countX = map["countX(Y)"].value<int>();
        countY = countX;
    }
    if (map.contains("countT")) countT = map["countT"].value<int>();

    if (map.contains("stepX(Y)"))
    {
        stepX = map["stepX(Y)"].value<float>();
        stepY = stepX;
    }
    if (map.contains("stepT")) stepT = map["stepT"].value<float>();

    //ensure the grid fits the area
    set_boundaries();
}

QVariantMap PdeSettings::toQVariantMap() const
{
    QVariantMap map;

    map.insert("V1", V1_str);
    map.insert("V2", V2_str);

    map.insert("c", c);
    map.insert("m", m);

    map.insert("countX(Y)", countX);
    map.insert("countT", countT);

    map.insert("stepX(Y)", stepX);
    map.insert("stepT", stepT);

    return map;
}

QVariantMap PdeSettings::getQVariantMapToolTips() const
{
    QVariantMap map;

    map.insert("V1", "The initial function u(x, 0)");
    map.insert("V2", "The initial function 𝛿u/𝛿t(x, 0)");

    map.insert("c", "A constant (e.g. for the heat equation: 𝛿u/𝛿t = c^2 * Δu)");
    map.insert("m", "The scale coefficient for V1 and V2 functions (i.e. V1(x) -> V1(x / m) and the same for V2)");

    map.insert("countX(Y)", "The number of nodes along the X and Y axis");
    map.insert("countT", "The number of nodes along the T axis");

    map.insert("stepX(Y)", "The step along the X and Y axis");
    map.insert("stepT", "The step along the T axis");

    return map;
}

void PdeSettings::set_boundaries()
{
    maxX = (stepX * countX) / 2;
    minX = -maxX;

    maxY = (stepY * countY) / 2;
    minY = -maxY;

    maxT = (stepT * countT);
    minT = 0;
}
