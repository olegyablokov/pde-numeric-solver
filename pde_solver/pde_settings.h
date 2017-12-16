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
    float V2(QVector2D x) const;  /**< The initial function 𝛿u/𝛿t(x, 0) */

    float c = 2.0f;     /**< A constant (e.g. for the heat equation: 𝛿u/𝛿t = c^2 * Δu) */
    float m = 1.0f;     /**< The scale coefficient for V1 and V2 functions (i.e. V1(x) -> V1(x / m) and the same for V2) */

    int countX = 50;    /**< The number of nodes along the X axis */
    int countY = 50;    /**< The number of nodes along the Y axis */
    int countT = 1;     /**< The number of nodes along the T axis */

    float minX;         /**< The minimum X value of the grid */
    float maxX;         /**< The maximum X value of the grid */
    float minY;         /**< The minimum Y value of the grid */
    float maxY;         /**< The maximum Y value of the grid */
    float minT;         /**< The minimum T value of the grid */
    float maxT;         /**< The maximum T value of the grid */

    float stepX = 0.1;  /**< The step along the X axis */
    float stepY = 0.1;  /**< The step along the Y axis */
    float stepT = 0.1;  /**< The step along the T axis */

    /**
     * @brief A method for changing the object data from a QVariantMap.
     *
     * If an entry is missing, no change for the entry will be applied.
     */
    void reset(QVariantMap& map);

    /**
     * @brief A method for getting a QvariantMap of the members of the object.
     *
     * The map has a following form:\n
     * map\n
     * {\n
     *      "V1" : "(15 * pow(E, -R * R / 100)) * (sin(R * 2))"\n
     *      "V2" : "R"\n
     *      "countX" : 50\n
     *      ...\n
     * }
     */
    QVariantMap toQVariantMap() const;

    /**
     * @brief A method for getting descriptions for the members of the object.
     *
     * The map has a following form:\n
     * map\n
     * {\n
     *      "V1" : "The initial function u(x, 0)"\n
     *      "V2" : "The initial function 𝛿u/𝛿t(x, 0)"\n
     *      "countX" : "The number of nodes along the X axis"\n
     *      ...\n
     * }
     */
    QVariantMap getQVariantMapToolTips() const;

private:
    QString V1_str = "30*pow(E, -(abs(x)+abs(y))/5)*sin((abs(x)+abs(y)))";
    QString V2_str = "R";

    void set_boundaries();
    float evaluate_expression(QString expression, QVector2D x) const;
};

#endif //PDE_SETTINGS_H
