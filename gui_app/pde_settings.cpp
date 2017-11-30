#include "pde_settings.h"
#include <QVector2D>
#include <algorithm>
#include <cassert>

PdeSettings::PdeSettings()
{
    stepX = (maxX - minX) / float(countX);
    stepY = (maxY - minY) / float(countY);
    stepT = (maxT - minT) / float(countT);

    m_ScriptEngine = new QScriptEngine();
}

PdeSettings::PdeSettings(QVariantMap& map)
{
    reset(map);
    //m_ScriptEngine = new QScriptEngine();
}

PdeSettings::~PdeSettings()
{
    //delete m_ScriptEngine;
}

float PdeSettings::V1(QVector2D x) const
{
    float R = qSqrt(x[0] * x[0] + x[1] * x[1]);

    //TODO: why problems with declaring QScriptEngine instance in class?
    QScriptEngine m_ScriptEngine1;

    QString expression = V1_str;
    expression.replace("x", QString::number(x[0]));
    expression.replace("y", QString::number(x[1]));
    expression.replace("R", QString::number(R));
    expression.replace("sqrt", "Math.sqrt");
    expression.replace("sin", "Math.sin");
    expression.replace("cos", "Math.cos");
    expression.replace("pow", "Math.pow");
    expression.replace("PI", "Math.PI");
    expression.replace("E", "Math.E");

    return float(m_ScriptEngine1.evaluate(expression).toNumber());
}

void PdeSettings::reset(QVariantMap& map)
{
    if (map.contains("V1")) V1_str = map["V1"].value<QString>();
    //if (map.contains("V2_str")) V2_str = map["V2_str"].value<QString>();

    if (map.contains("c")) countX = map["c"].value<float>();
    if (map.contains("m")) countX = map["m"].value<float>();

    if (map.contains("countX(Y)"))
    {
        countX = map["countX(Y)"].value<int>();
        countY = countX;
    }
    if (map.contains("countT")) countT = map["countT"].value<int>();

    if (map.contains("minX")) minX = map["minX"].value<float>();
    if (map.contains("maxX")) maxX = map["maxX"].value<float>();
    if (map.contains("minY")) minY = map["minY"].value<float>();
    if (map.contains("maxY")) maxY = map["maxY"].value<float>();
    if (map.contains("minT")) minT = map["minT"].value<float>();
    if (map.contains("maxT")) maxT = map["maxT"].value<float>();

    stepX = (maxX - minX) / float(countX);
    stepY = (maxY - minY) / float(countY);
    stepT = (maxT - minT) / float(countT);
}

QVariantMap PdeSettings::toQVariantMap()
{
    QVariantMap map;

    map.insert("V1", V1_str);
    //map.insert("V2_str", V2_str);

    map.insert("c", c);
    map.insert("m", m);

    map.insert("countX(Y)", countX);
    //map.insert("countY", countY);
    map.insert("countT", countT);

    map.insert("minX", minX);
    map.insert("maxX", maxX);
    map.insert("minY", minY);
    map.insert("maxY", maxY);
    map.insert("minT", minT);
    map.insert("maxT", maxT);

    return map;
}
