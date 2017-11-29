#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <cubature.h>

//int f(unsigned ndim, unsigned npts, const double *x, void *fdata,
//      unsigned fdim, double *fval)
int f(unsigned ndim, const double *x, void *fdata, unsigned fdim, double *fval)
{
    if (ndim != 2 || fdim != 1) return -1;

//    for (int j = 0; j < npts; ++j)
//    {
//        for (int i = 0; i < ndim; ++i)
//            *(fval + j * ndim + i) =  1; //*(x + j * ndim + i);
//    }
    *fval =  1; //*(x + j * ndim + i);
    return 0;
}

//int f(unsigned ndim, const double *x, void *fdata, unsigned fdim, double *fval) {
//    double sigma = *((double *) fdata); // we can pass σ via fdata argument
//    double sum = 0;
//    unsigned i;
//    for (i = 0; i < ndim; ++i) sum += x[i] * x[i];
//    // compute the output value: note that fdim should == 1 from below
//    fval[0] = exp(-sigma * sum);
//    return 0; // success*
//}

int main(int argc, char *argv[])
{
    double xmin[3] = {-2,-2}, xmax[3] = {2,2}, sigma = 0.5, val, err;
    int success = hcubature(1, f, &sigma, 2, xmin, xmax, 0, 0, 1e-4, ERROR_INDIVIDUAL, &val, &err);

//    double xmin[2] = {0, 0}, xmax[2] = {1, 1};
//    double val, err;
//    //error_norm norm;
//    int success = hcubature(1, f, NULL, 2, xmin, xmax, 0, 0.1, 0, ERROR_INDIVIDUAL, &val, &err);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
