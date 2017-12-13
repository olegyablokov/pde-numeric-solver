#ifndef PDE_SETTINGS_H
#define PDE_SETTINGS_H

#include <QtCore/qmath.h>
#include <QScriptEngine>
#include <QVector>
#include <QString>
#include <QToolTip>

#include <memory>
#include <functional>
#include <sys/types.h>

/**
 * @brief A class for storing settings for solving a pde equation (such as the initial function, the time step, the coefficients etc.).
 *
 */
class PdeSettings
{
public:    
    PdeSettings();

    /**
     * @brief A constructor for creating an object from a QVariantMap.
     * @see reset(QVariantMap& map)
     */
    PdeSettings(QVariantMap& map);

    ~PdeSettings();

    float V1(QVector2D x) const;  /**< The initial function u(x, 0) */
    float V2(QVector2D x) const;  /**< The initial function du/dt(x, 0) */

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

    /**
     * @brief A method for changing the object data from a QVariantMap.
     *
     * If an entry is missing, no change for the entry will be applied.
     */
    void reset(QVariantMap& map);

    QVariantMap toQVariantMap();

private:
    QString V1_str = "(15 * pow(E, -R * R / 100)) * (sin(R * 2))";
    QString V2_str = "R";

    float evaluate_expression(QString expression, QVector2D x) const;
};

#endif //PDE_SETTINGS_H
