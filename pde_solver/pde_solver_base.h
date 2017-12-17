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
#include "pde_solver_structs.h"

/**
 * @brief The base class for pde solvers.
 *
 * PdeSolverBase is the base class for pde solvers that are used for generating numerical solutions of pde equations.\n
 * For inheriting please implement the get_solution(const PdeSettings& set) method. The output data must be emited in a form of a Qt signal solution_generated(PdeSolverBase::GraphSolution_t).
 */
class PdeSolverBase : public QObject
{
    Q_OBJECT

public:
    PdeSolverBase(QObject *parent = NULL);
    virtual ~PdeSolverBase();

    virtual QVector<PdeSolver::SolutionMethod_t> get_implemented_methods();

public slots:
    /**
     * @brief The method which just emits the solve_invoked(const PdeSettings&) signal.
     */
    void solve(const PdeSettings& set, PdeSolver::SolutionMethod_t method);

private slots:
    /**
     * @brief The method which is called when a solution generation is requested.
     *
     * The method should emit the solution_generated(PdeSolverBase::GraphSolution_t) signal when the data is generated.
     * @param set settings used for generating a solution
     * @see solution_generated(PdeSolverBase::GraphSolution_t)
     */
    virtual void get_solution(const PdeSettings& set, PdeSolver::SolutionMethod_t method);

signals:
    /**
     * @brief The signal which is emmited when the graph data is generated.
     * @see solve(const PdeSettings& set)
     */
    void solution_generated(PdeSolver::GraphSolution_t);

    /**
     * @brief The signal which is emmited when the solve(const PdeSettings& set) method is invoked.
     *
     * The signal is meant to be emmited when a request for a solution is recieved.\n
     * It is connected in the base constructor to the get_solution(const PdeSettings& set) method.
     * @see solve(const PdeSettings& set)
     */
    void solve_invoked(const PdeSettings&, PdeSolver::SolutionMethod_t);

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
    PdeSolver::GraphDataSlice_t get_initial_conditions_in_polar_coords(const PdeSettings& set);

    PdeSolver::GraphDataSlice_t get_initial_conditions_in_cartesian_coords(const PdeSettings& set);

    /**
     * @brief The method is used for clearing a slice. If not use it, memory leacks will appear.
     */
    void clear_graph_data_slice(PdeSolver::GraphDataSlice_t& data_slice);
};

#endif //PDE_SOLVER_H
