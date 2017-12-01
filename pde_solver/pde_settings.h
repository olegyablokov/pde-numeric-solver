#ifndef PDE_SETTINGS_H
#define PDE_SETTINGS_H

#include <QtCore/qmath.h>
#include <QScriptEngine>
#include <QVector>
#include <QString>

#include <memory>
#include <functional>
#include <sys/types.h>

class PdeSettings
{
public:    
    PdeSettings();
    PdeSettings(QVariantMap& map);

    ~PdeSettings();

    float V1(QVector2D x) const;
    float V2(QVector2D x) const;

	float c = 2.0f; 
    float m = 1.0f;

	int countX = 100;
	int countY = 100; 
    int countT = 1;

	float minX = -10.0f; 
	float maxX = 10.0f;
	float minY = -10.0f; 
	float maxY = 10.0f; 
	float minT = 0.0f; 
    float maxT = 0.05f;

	float stepX;
	float stepY;
	float stepT;

    void reset(QVariantMap& map);

    QVariantMap toQVariantMap();

    QScriptEngine* m_ScriptEngine;

private:
    QString V1_str = "(15 * pow(E, -R * R / 100)) * (sin(R * 2))";
    QString V2_str = "R";
};

#endif //PDE_SETTINGS_H
