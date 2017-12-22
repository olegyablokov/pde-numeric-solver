/*
    Copyright (c) 2017 Oleg Yablokov

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

**/

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
    enum CoordsType
    {
        Cartesian = 0,
        Polar = 1       /**< If Polar, the x-axis becomes the angular axis and the z-axis becomes the radial axis (source: Qt docs) */
    };
    CoordsType m_CoordsType = CoordsType::Cartesian;

    PdeSettings();
    PdeSettings(const PdeSettings& other);
    PdeSettings(CoordsType coords_type, int dim = -1);

    /**
     * @brief A constructor for creating an object from a QVariantMap.
     * @see reset(QVariantMap& map)
     */
    PdeSettings(QVariantMap& map);

    ~PdeSettings();


    void set_coords_type(CoordsType new_type, int new_dim = -1);
    void set_coords_dim(int new_dim);

    float V1(QVector2D x) const;  /**< The initial function u(x, 0) */
    float V2(QVector2D x) const;  /**< The initial function 𝛿u/𝛿t(x, 0) (if used) */

    float c = 2.0f;     /**< A constant (e.g. for the heat equation: 𝛿u/𝛿t = c^2 * Δu) */
    float m = 1.0f;     /**< The scale coefficient for V1 and V2 functions (i.e. V1(x) -> V1(x / m) and the same for V2) */

    int m_Dim = 2;      /**< The dimension */

    struct CoordGridSet_t
    {
        int count = 10;             /**< The number of nodes along the axis */
        float step = 0.1;           /**< The step along the axis */
        float min = 0;              /**< The minimum value along the axis */
        float max = 1;              /**< The maximum value along the axis */
        QString label = "<label>";  /**< e.g. "X1", "R", "T" etc. */
        QString descr = "<descr>";  /**< e.g. "The time axis" */

        CoordGridSet_t() {}
        CoordGridSet_t(int count_, float step_, float min_, float max_, QString label_ = "<label>", QString descr_ = "<descr>")
        { count = count_; step = step_; min = min_; max = max_; label = label_; descr = descr_; }
    };
    QVector<CoordGridSet_t> m_Coords;

    const CoordGridSet_t* get_coord_by_label(QString label) const;

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
