#ifndef PDE_SOLVER_H
#define PDE_SOLVER_H

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/QValue3DAxis>
#include <QVector>
#include <QList>
#include <QThread>
#include <QObject>

#include <memory>
#include <functional>
#include <sys/types.h>

#include "pde_settings.h"

/**
 * @brief The base class for pde solvers.
 *
 * PdeSolverBase is the base class for pde solvers that are used for generating numerical solutions of pde equations.\n
 * For inheiting please implement the get_solution(const PdeSettings& set) method. The output data must be emited in a form of a Qt signal solution_generated(PdeSolverBase::GraphSolution_t).
 */
class PdeSolverBase : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief A slice of graph data.
     * @tparam QtDataVisualization::QSurfaceDataArray* A pointer to the u(x, t) data slice (with a fixed t)
     * @tparam QtDataVisualization::QSurfaceDataArray* A pointer to the partial derivative 𝛿u/𝛿t(x, t) data slice (with a fixed t)
     */
    typedef std::pair<QtDataVisualization::QSurfaceDataArray*, QtDataVisualization::QSurfaceDataArray*> GraphDataSlice_t;

    /**
     * @brief Slices of graph data.
     * @tparam QList<QtDataVisualization::QSurfaceDataArray*> A list of slices. Here the index of Qlist is time and the array pointers are time slices of the u(x, t) function
     * @tparam QList<QtDataVisualization::QSurfaceDataArray*> A list of slices. Here the index of Qlist is time and the array pointers are time slices of the partial 𝛿u/𝛿t(x, t) function
     */
    typedef std::pair<QList<QtDataVisualization::QSurfaceDataArray*>, QList<QtDataVisualization::QSurfaceDataArray*>> GraphData_t;  // first - u(x,t), second - partial 𝛿u/𝛿t(x,t) (here t is fixed)

    /**
     * @brief The output type of a solution.
     * @see get_solution(const PdeSettings& set)
     */
    typedef struct
    {
        GraphData_t graph_data;
        PdeSettings set;
    } GraphSolution_t;

    PdeSolverBase(QObject *parent = NULL);
    virtual ~PdeSolverBase();

public slots:
    /**
     * @brief The method which just emits the solve_invoked(const PdeSettings&) signal.
     */
    void solve(const PdeSettings& set);

private slots:
    /**
     * @brief The method which is called when a solution generation is requested.
     *
     * The method should emit the solution_generated(PdeSolverBase::GraphSolution_t) signal when the data is generated.
     * @param set settings used for generating a solution
     * @see solution_generated(PdeSolverBase::GraphSolution_t)
     */
    virtual void get_solution(const PdeSettings& set);

signals:
    /**
     * @brief The signal which is emmited when the graph data is generated.
     * @see solve(const PdeSettings& set)
     */
    void solution_generated(PdeSolverBase::GraphSolution_t);

    /**
     * @brief The signal which is emmited when the solve(const PdeSettings& set) method is invoked.
     *
     * The signal is meant to be emmited when a request for a solution is recieved.\n
     * It is connected in the base constructor to the get_solution(const PdeSettings& set) method.
     * @see solve(const PdeSettings& set)
     */
    void solve_invoked(const PdeSettings&);

    /**
     * @brief Used for sending data progress.
     * @param QString the current process description
     * @param int the precentage of the current process completion
     */
    void solution_progress_update(QString, int);

protected:
    /**
     * @brief The method for getting initial conditions (such as u(x, 0) and partial 𝛿u/𝛿t(x, 0)) provided by the set parameter.
     */
    GraphDataSlice_t get_initial_conditions(const PdeSettings& set);

    /**
     * @brief The method is used for clearing a slice. If not use it, memory leacks will appear.
     */
    void clear_graph_data_slice(GraphDataSlice_t& data_slice);
};

#endif //PDE_SOLVER_H
