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
    std::function<QVector2D(QVector2D)> V2 = NULL;
    QString V1_str = "<still not working>";
    QString V2_str = "<still not working>";

	float c = 2.0f; 
	float m= 1.0f;

	int countX = 100;
	int countY = 100; 
    int countT = 200;

	float minX = -10.0f; 
	float maxX = 10.0f;
	float minY = -10.0f; 
	float maxY = 10.0f; 
	float minT = 0.0f; 
    float maxT = 0.7f;

	float stepX;
	float stepY;
	float stepT;

    void reset(QVariantMap& map);

    QVariantMap toQVariantMap();

    QScriptEngine* m_ScriptEngine;
private:
};

#endif //PDE_SETTINGS_H
