#include <math.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include "consts.h"
#include "utils.h"
#include "common.h"

inline static double func_u(double t, double x, double y) { return U; }

inline static double func_v(double t, double x, double y) { return V; }

inline void print_data_to_files(double *phi, double *density, double *residual, int tl) {
    double x0 = get_center_x() + tl*TAU * func_u(tl*TAU, get_center_x(), get_center_y());
    double y0 = get_center_y() + tl*TAU* func_v(tl*TAU, get_center_x(), get_center_y());
    print_surface("phi", NX, NY, HX, HY, tl, A, C, x0, y0, TAU, U, V, phi);
    print_surface("rho", NX, NY, HX, HY, tl, A, C, x0, y0, TAU, U, V, density);
//    print_surface("res", NX, NY, HX, HY, tl, A, C, get_center_x_2(), get_center_y_2(), TAU,
//                  U, V, residual);
    double *err_lock = calc_error_2(HX, HY, tl * TAU, density);
    print_surface("err-l", NX, NY, HX, HY, tl, A, C, x0, y0, TAU, U, V, err_lock);
    delete[] err_lock;
}

inline static double analytical_solution_circle(double t, double x, double y) {
    double x0 = get_center_x() + t * func_u(t, x, y);
    double y0 = get_center_y() + t * func_v(t, x, y);
    double value = (x - x0) * (x - x0) + (y - y0) * (y - y0);
    if (value <= R_SQ) return INN_DENSITY;
    return OUT_DENSITY;
}

double *calc_error_22(double hx, double hy, double tt, double *solution) {
    double *res = new double[XY];
    for (int i = 0; i < NX_1; i++)
        for (int j = 0; j < NY_1; j++)
            res[i * NY_1 + j] = solution[i * NY_1 + j]
                                - analytical_solution_circle(tt, A + hx * i, C + hy * j);
    return res;
}

static double get_phi_integ_trapezium(int ii, int jj, double *density, double time_value) {
    double x1 = 0.;
    double y1 = 0.;
    double x2 = 0.;
    double y2 = 0.;
    double x3 = 0.;
    double y3 = 0.;
    double x4 = 0.;
    double y4 = 0.;

    if (ii > 0 && ii < NX && jj > 0 && jj < NY) {
        // p1 (x_{i-1/2}, y_{j-1/2})
        x1 = A + ii * HX - HX / 2.;
        y1 = C + jj * HY - HY / 2.;
        // p2 (x_{i+1/2}, y_{j-1/2})
        x2 = A + ii * HX + HX / 2.;
        y2 = C + jj * HY - HY / 2.;
        // p3 (x_{i+1/2}, y_{j+1/2})
        x3 = A + ii * HX + HX / 2.;
        y3 = C + jj * HY + HY / 2.;
        // p4 (x_{i-1/2}, y_{j+1/2})
        x4 = A + ii * HX - HX / 2.;
        y4 = C + jj * HY + HY / 2.;
        if (x1 <= A || x1 >= B || x2 <= A || x2 >= B || x3 <= A || x3 >= B || x4 <= A || x4 >= B
            || y1 <= C || y1 >= D || y2 <= C || y2 >= D || y3 <= C || y3 >= D || y4 <= C || y4 >= D)
            printf("ERROR INDEX i=%d j=%d : x1=%.8le * y1=%.8le ** x2=%.8le * y2=%.8le ** x3=%.8le * y3=%.8le ** "
                           "x4=%.8le * y4%.8le\n ", ii, jj, x1, y1, x2, y2, x3, y3, x4, y4);
    }
    else if (ii == NX && jj == NY) { // point (1,1)  omega_{i-1,j-1}
        // p1 (x_{NX-1/2}, y_{NY-1/2})
        x1 = B - HX / 2.;
        y1 = D - HY / 2.;
        // p2 (B, y_{NY-1/2})
        x2 = B;
        y2 = D - HY / 2.;
        // p3 (B, D)
        x3 = B;
        y3 = D;
        // p4 (x_{NX-1/2}, D)
        x4 = B - HX / 2.;
        y4 = D;
        if (x1 <= A || x1 > B || x2 <= A || x2 > B || x3 <= A || x3 > B || x4 <= A || x4 > B
            || y1 <= C || y1 > D || y2 <= C || y2 > D || y3 <= C || y3 > D || y4 <= C || y4 > D)
            printf("ERROR INDEX i=%d j=%d : x1=%.8le * y1=%.8le ** x2=%.8le * y2=%.8le ** x3=%.8le * y3=%.8le ** "
                           "x4=%.8le * y4%.8le\n ", ii, jj, x1, y1, x2, y2, x3, y3, x4, y4);
    }
    else if (jj == NY && ii > 0 && ii < NX) { // G3 -- top boundary
        // p1 (x_{i-1/2}, y_{NY-1/2})
        x1 = A + ii * HX - HX / 2.;
        y1 = D - HY / 2.;
        // p2 (x_{i+1/2}, y_{NY-1/2})
        x2 = A + ii * HX + HX / 2.;
        y2 = D - HY / 2.;
        //p3 (x_{i+1/2}, D)
        x3 = A + ii * HX + HX / 2.;
        y3 = D;
        //p4 (x_{i-1/2}, D)
        x4 = A + ii * HX - HX / 2.;
        y4 = D;
        if (x1 <= A || x1 >= B || x2 <= A || x2 >= B || x3 <= A || x3 >= B || x4 <= A || x4 >= B
            || y1 <= C || y1 > D || y2 <= C || y2 > D || y3 <= C || y3 > D || y4 <= C || y4 > D)
            printf("ERROR INDEX i=%d j=%d : x1=%.8le * y1=%.8le ** x2=%.8le * y2=%.8le ** x3=%.8le * y3=%.8le ** "
                           "x4=%.8le * y4%.8le\n ", ii, jj, x1, y1, x2, y2, x3, y3, x4, y4);
    }
    else if (ii == NX && jj > 0 && jj < NY) { // G2 -- right boundary
        // p1 (x_{NX-1/2}, y_{j-1/2})
        x1 = B - HX / 2.;
        y1 = C + jj * HY - HY / 2.;
        // p2 (B, y_{j-1/2})
        x2 = B;
        y2 = C + jj * HY - HY / 2.;
        // p3 (B, y_{j+1/2})
        x3 = B;
        y3 = C + jj * HY + HY / 2.;
        // p4 (x_{NX-1/2}, y_{j+1/2})
        x4 = B - HX / 2.;
        y4 = C + jj * HY + HY / 2.;
        if (x1 <= A || x1 > B || x2 <= A || x2 > B || x3 <= A || x3 > B || x4 <= A || x4 > B
            || y1 <= C || y1 >= D || y2 <= C || y2 >= D || y3 <= C || y3 >= D || y4 <= C || y4 >= D)
            printf("ERROR INDEX i=%d j=%d : x1=%.8le * y1=%.8le ** x2=%.8le * y2=%.8le ** x3=%.8le * y3=%.8le ** "
                           "x4=%.8le * y4%.8le\n ", ii, jj, x1, y1, x2, y2, x3, y3, x4, y4);
    }
    else
        printf("ERROR! INDEX i=%d j=%d ", ii, jj);

    double u = func_u(time_value, x1, y1);
    double v = func_v(time_value, x1, y1);
    x1 = x1 - TAU * u;
    y1 = y1 - TAU * v;
    u = func_u(time_value, x2, y2);
    v = func_v(time_value, x2, y2);
    x2 = x2 - TAU * u;
    y2 = y2 - TAU * v;
    u = func_u(time_value, x3, y3);
    v = func_v(time_value, x3, y3);
    x3 = x3 - TAU * u;
    y3 = y3 - TAU * v;
    u = func_u(time_value, x4, y4);
    v = func_v(time_value, x4, y4);
    x4 = x4 - TAU * u;
    y4 = y4 - TAU * v;
    if (x1 <= A || x1 >= B || x2 <= A || x2 >= B || x3 <= A || x3 >= B || x4 <= A || x4 >= B
        || y1 <= C || y1 >= D || y2 <= C || y2 >= D || y3 <= C || y3 >= D || y4 <= C || y4 >= D)
        printf("Time level %.8le! ERROR INDEX i=%d j=%d : x1=%.8le * y1=%.8le ** x2=%.8le * y2=%.8le ** x3=%.8le * y3=%.8le ** "
                       "x4=%.8le * y4=%.8le\n ", time_value, ii, jj, x1, y1, x2, y2, x3, y3, x4, y4);

    int nx = IDEAL_SQ_SIZE_X;
    int ny = IDEAL_SQ_SIZE_Y;

    double x_step = 1. / nx;
    double y_step = 1. / ny;

    // get right part for jakoby
    double phi = 0.;
    double mes = x_step * y_step;
    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {

            double ideal_x = i * x_step + x_step / 2.;
            double ideal_y = j * y_step + y_step / 2.;

            double a11 = (x2 - x1) + (x1 + x3 - x2 - x4) * ideal_y;
            double a12 = (x4 - x1) + (x1 + x3 - x2 - x4) * ideal_x;
            double a21 = (y2 - y1) + (y1 + y3 - y2 - y4) * ideal_y;
            double a22 = (y4 - y1) + (y1 + y3 - y2 - y4) * ideal_x;
            double jakob = a11 * a22 - a21 * a12;

            // point (x_i,y_j)
            ideal_x = i * x_step;
            ideal_y = j * y_step;

            double real_x = x1 + (x2 - x1) * ideal_x + (x4 - x1) * ideal_y
                            + (x1 + x3 - x2 - x4) * ideal_x * ideal_y;
            double real_y = y1 + (y2 - y1) * ideal_x + (y4 - y1) * ideal_y
                            + (y1 + y3 - y2 - y4) * ideal_x * ideal_y;

            if (real_x < A || real_y < C || real_x > B || real_y > D) {
                printf("Time level %.8le! ERROR INDEX i=%d j=%d : REAL x=%.8le * y=%.8le ** IDEAL x=%.8le * y=%.8le \n",
                       time_value, ii, jj, real_x, real_y, ideal_x, ideal_y);
                printf("1: %.8le * %.8le ** 2: %.8le * %.8le ** 3: %.8le * %.8le ** 4: %.8le * %.8le\n",
                       x1, y1, x2, y2, x3, y3, x4, y4);
            }

            // find out in which square real point was placed
            int sq_i = (int) ((real_x - A) / HX);
            int sq_j = (int) ((real_y - C) / HY);
            if (sq_i < 0 || sq_j < 0 || sq_i > NX - 1 || sq_j > NY - 1) {
                printf("Time level %.8le! ERROR INDEX i=%d j=%d : i*=%d j*=%d\n", time_value,
                       ii, jj, sq_i, sq_j);
            }
            double x = A + sq_i * HX;
            double y = C + sq_j * HY;

            // formula 4
            double dens_1 = density[sq_i * NY_1 + sq_j] * (1 - (real_x - x) / HX) * (1 - (real_y - y) / HY)
                            + density[(sq_i + 1) * NY_1 + sq_j] * ((real_x - x) / HX) * (1 - (real_y - y) / HY)
                            + density[(sq_i + 1) * NY_1 + sq_j + 1] * ((real_x - x) / HX) * ((real_y - y) / HY)
                            + density[sq_i * NY_1 + sq_j + 1] * (1 - (real_x - x) / HX) * ((real_y - y) / HY);


            // point (x_{i+1},y_j)
            ideal_x = (i + 1) * x_step;
            ideal_y = j * y_step;
            real_x = x1 + (x2 - x1) * ideal_x + (x4 - x1) * ideal_y
                     + (x1 + x3 - x2 - x4) * ideal_x * ideal_y;
            real_y = y1 + (y2 - y1) * ideal_x + (y4 - y1) * ideal_y
                     + (y1 + y3 - y2 - y4) * ideal_x * ideal_y;
            if (real_x < A || real_y < C || real_x > B || real_y > D) {
                printf("Time level %.8le! ERROR INDEX i=%d j=%d : REAL x=%.8le * y=%.8le ** IDEAL x=%.8le * y=%.8le \n",
                       time_value, ii, jj, real_x, real_y, ideal_x, ideal_y);
                printf("1: %.8le * %.8le ** 2: %.8le * %.8le ** 3: %.8le * %.8le ** 4: %.8le * %.8le\n",
                       x1, y1, x2, y2, x3, y3, x4, y4);
            }

            // find out in which square real point was placed
            sq_i = (int) ((real_x - A) / HX);
            sq_j = (int) ((real_y - C) / HY);
            if (sq_i < 0 || sq_j < 0 || sq_i > NX - 1 || sq_j > NY - 1) {
                printf("Time level %.8le! ERROR INDEX i=%d j=%d : i*=%d j*=%d\n", time_value,
                       ii, jj, sq_i, sq_j);
            }
            x = A + sq_i * HX;
            y = C + sq_j * HY;

            // formula 4
            double dens_2 = density[sq_i * NY_1 + sq_j] * (1 - (real_x - x) / HX) * (1 - (real_y - y) / HY)
                            + density[(sq_i + 1) * NY_1 + sq_j] * ((real_x - x) / HX) * (1 - (real_y - y) / HY)
                            + density[(sq_i + 1) * NY_1 + sq_j + 1] * ((real_x - x) / HX) * ((real_y - y) / HY)
                            + density[sq_i * NY_1 + sq_j + 1] * (1 - (real_x - x) / HX) * ((real_y - y) / HY);


            // point (x_{i+1},y_{j+1})
            ideal_x = (i + 1) * x_step;
            ideal_y = (j + 1) * y_step;
            real_x = x1 + (x2 - x1) * ideal_x + (x4 - x1) * ideal_y
                     + (x1 + x3 - x2 - x4) * ideal_x * ideal_y;
            real_y = y1 + (y2 - y1) * ideal_x + (y4 - y1) * ideal_y
                     + (y1 + y3 - y2 - y4) * ideal_x * ideal_y;
            if (real_x < A || real_y < C || real_x > B || real_y > D) {
                printf("Time level %.8le! ERROR INDEX i=%d j=%d : REAL x=%.8le * y=%.8le ** IDEAL x=%.8le * y=%.8le \n",
                       time_value, ii, jj, real_x, real_y, ideal_x, ideal_y);
                printf("1: %.8le * %.8le ** 2: %.8le * %.8le ** 3: %.8le * %.8le ** 4: %.8le * %.8le\n",
                       x1, y1, x2, y2, x3, y3, x4, y4);
            }

            // find out in which square real point was placed
            sq_i = (int) ((real_x - A) / HX);
            sq_j = (int) ((real_y - C) / HY);
            if (sq_i < 0 || sq_j < 0 || sq_i > NX - 1 || sq_j > NY - 1) {
                printf("Time level %.8le! ERROR INDEX i=%d j=%d : i*=%d j*=%d\n", time_value,
                       ii, jj, sq_i, sq_j);
            }
            x = A + sq_i * HX;
            y = C + sq_j * HY;

            // formula 4
            double dens_3 = density[sq_i * NY_1 + sq_j] * (1 - (real_x - x) / HX) * (1 - (real_y - y) / HY)
                            + density[(sq_i + 1) * NY_1 + sq_j] * ((real_x - x) / HX) * (1 - (real_y - y) / HY)
                            + density[(sq_i + 1) * NY_1 + sq_j + 1] * ((real_x - x) / HX) * ((real_y - y) / HY)
                            + density[sq_i * NY_1 + sq_j + 1] * (1 - (real_x - x) / HX) * ((real_y - y) / HY);


            // point (x_i,y_{j+1})
            ideal_x = i * x_step;
            ideal_y = (j + 1) * y_step;
            real_x = x1 + (x2 - x1) * ideal_x + (x4 - x1) * ideal_y
                     + (x1 + x3 - x2 - x4) * ideal_x * ideal_y;
            real_y = y1 + (y2 - y1) * ideal_x + (y4 - y1) * ideal_y
                     + (y1 + y3 - y2 - y4) * ideal_x * ideal_y;
            if (real_x < A || real_y < C || real_x > B || real_y > D) {
                printf("Time level %.8le! ERROR INDEX i=%d j=%d : REAL x=%.8le * y=%.8le ** IDEAL x=%.8le * y=%.8le \n",
                       time_value, ii, jj, real_x, real_y, ideal_x, ideal_y);
                printf("1: %.8le * %.8le ** 2: %.8le * %.8le ** 3: %.8le * %.8le ** 4: %.8le * %.8le\n",
                       x1, y1, x2, y2, x3, y3, x4, y4);
            }

            // find out in which square real point was placed
            sq_i = (int) ((real_x - A) / HX);
            sq_j = (int) ((real_y - C) / HY);
            if (sq_i < 0 || sq_j < 0 || sq_i > NX - 1 || sq_j > NY - 1) {
                printf("Time level %.8le! ERROR INDEX i=%d j=%d : i*=%d j*=%d\n", time_value,
                       ii, jj, sq_i, sq_j);
            }
            x = A + sq_i * HX;
            y = C + sq_j * HY;

            // formula 4
            double dens_4 = density[sq_i * NY_1 + sq_j] * (1 - (real_x - x) / HX) * (1 - (real_y - y) / HY)
                            + density[(sq_i + 1) * NY_1 + sq_j] * ((real_x - x) / HX) * (1 - (real_y - y) / HY)
                            + density[(sq_i + 1) * NY_1 + sq_j + 1] * ((real_x - x) / HX) * ((real_y - y) / HY)
                            + density[sq_i * NY_1 + sq_j + 1] * (1 - (real_x - x) / HX) * ((real_y - y) / HY);

            phi += (dens_1 + dens_2 + dens_3 + dens_4) * jakob;
        }
    }

    phi = 0.25 * phi * mes;
    if (fabs(phi) < fabs(DBL_MIN_TRIM)) phi = 0;

    return phi;
}

static double get_phi_integ_midpoint(int ii, int jj, double *density, double time_value) {
    double x1 = 0.;
    double y1 = 0.;
    double x2 = 0.;
    double y2 = 0.;
    double x3 = 0.;
    double y3 = 0.;
    double x4 = 0.;
    double y4 = 0.;

    if (ii > 0 && ii < NX && jj > 0 && jj < NY) {
        // p1 (x_{i-1/2}, y_{j-1/2})
        x1 = A + ii * HX - HX / 2.;
        y1 = C + jj * HY - HY / 2.;
        // p2 (x_{i+1/2}, y_{j-1/2})
        x2 = A + ii * HX + HX / 2.;
        y2 = C + jj * HY - HY / 2.;
        // p3 (x_{i+1/2}, y_{j+1/2})
        x3 = A + ii * HX + HX / 2.;
        y3 = C + jj * HY + HY / 2.;
        // p4 (x_{i-1/2}, y_{j+1/2})
        x4 = A + ii * HX - HX / 2.;
        y4 = C + jj * HY + HY / 2.;
        if (x1 <= A || x1 >= B || x2 <= A || x2 >= B || x3 <= A || x3 >= B || x4 <= A || x4 >= B
            || y1 <= C || y1 >= D || y2 <= C || y2 >= D || y3 <= C || y3 >= D || y4 <= C || y4 >= D)
            printf("ERROR INDEX i=%d j=%d : x1=%.8le * y1=%.8le ** x2=%.8le * y2=%.8le ** x3=%.8le * y3=%.8le ** "
                           "x4=%.8le * y4%.8le\n ", ii, jj, x1, y1, x2, y2, x3, y3, x4, y4);
    }
    else if (ii == NX && jj == NY) { // point (1,1)  omega_{i-1,j-1}
        // p1 (x_{NX-1/2}, y_{NY-1/2})
        x1 = B - HX / 2.;
        y1 = D - HY / 2.;
        // p2 (B, y_{NY-1/2})
        x2 = B;
        y2 = D - HY / 2.;
        // p3 (B, D)
        x3 = B;
        y3 = D;
        // p4 (x_{NX-1/2}, D)
        x4 = B - HX / 2.;
        y4 = D;
        if (x1 <= A || x1 > B || x2 <= A || x2 > B || x3 <= A || x3 > B || x4 <= A || x4 > B
            || y1 <= C || y1 > D || y2 <= C || y2 > D || y3 <= C || y3 > D || y4 <= C || y4 > D)
            printf("ERROR INDEX i=%d j=%d : x1=%.8le * y1=%.8le ** x2=%.8le * y2=%.8le ** x3=%.8le * y3=%.8le ** "
                           "x4=%.8le * y4%.8le\n ", ii, jj, x1, y1, x2, y2, x3, y3, x4, y4);
    }
    else if (jj == NY && ii > 0 && ii < NX) { // G3 -- top boundary
        // p1 (x_{i-1/2}, y_{NY-1/2})
        x1 = A + ii * HX - HX / 2.;
        y1 = D - HY / 2.;
        // p2 (x_{i+1/2}, y_{NY-1/2})
        x2 = A + ii * HX + HX / 2.;
        y2 = D - HY / 2.;
        //p3 (x_{i+1/2}, D)
        x3 = A + ii * HX + HX / 2.;
        y3 = D;
        //p4 (x_{i-1/2}, D)
        x4 = A + ii * HX - HX / 2.;
        y4 = D;
        if (x1 <= A || x1 >= B || x2 <= A || x2 >= B || x3 <= A || x3 >= B || x4 <= A || x4 >= B
            || y1 <= C || y1 > D || y2 <= C || y2 > D || y3 <= C || y3 > D || y4 <= C || y4 > D)
            printf("ERROR INDEX i=%d j=%d : x1=%.8le * y1=%.8le ** x2=%.8le * y2=%.8le ** x3=%.8le * y3=%.8le ** "
                           "x4=%.8le * y4%.8le\n ", ii, jj, x1, y1, x2, y2, x3, y3, x4, y4);
    }
    else if (ii == NX && jj > 0 && jj < NY) { // G2 -- right boundary
        // p1 (x_{NX-1/2}, y_{j-1/2})
        x1 = B - HX / 2.;
        y1 = C + jj * HY - HY / 2.;
        // p2 (B, y_{j-1/2})
        x2 = B;
        y2 = C + jj * HY - HY / 2.;
        // p3 (B, y_{j+1/2})
        x3 = B;
        y3 = C + jj * HY + HY / 2.;
        // p4 (x_{NX-1/2}, y_{j+1/2})
        x4 = B - HX / 2.;
        y4 = C + jj * HY + HY / 2.;
        if (x1 <= A || x1 > B || x2 <= A || x2 > B || x3 <= A || x3 > B || x4 <= A || x4 > B
            || y1 <= C || y1 >= D || y2 <= C || y2 >= D || y3 <= C || y3 >= D || y4 <= C || y4 >= D)
            printf("ERROR INDEX i=%d j=%d : x1=%.8le * y1=%.8le ** x2=%.8le * y2=%.8le ** x3=%.8le * y3=%.8le ** "
                           "x4=%.8le * y4%.8le\n ", ii, jj, x1, y1, x2, y2, x3, y3, x4, y4);
    }
    else printf("ERROR! INDEX i=%d j=%d ", ii, jj);

    double u = func_u(time_value, x1, y1);
    double v = func_v(time_value, x1, y1);
    x1 = x1 - TAU * u;
    y1 = y1 - TAU * v;
    u = func_u(time_value, x2, y2);
    v = func_v(time_value, x2, y2);
    x2 = x2 - TAU * u;
    y2 = y2 - TAU * v;
    u = func_u(time_value, x3, y3);
    v = func_v(time_value, x3, y3);
    x3 = x3 - TAU * u;
    y3 = y3 - TAU * v;
    u = func_u(time_value, x4, y4);
    v = func_v(time_value, x4, y4);
    x4 = x4 - TAU * u;
    y4 = y4 - TAU * v;
    if (x1 <= A || x1 >= B || x2 <= A || x2 >= B || x3 <= A || x3 >= B || x4 <= A || x4 >= B
        || y1 <= C || y1 >= D || y2 <= C || y2 >= D || y3 <= C || y3 >= D || y4 <= C || y4 >= D) {
        printf("Time level %.8le! ERROR INDEX i=%d j=%d : x1=%.8le * y1=%.8le ** x2=%.8le * y2=%.8le ** x3=%.8le * y3=%.8le ** "
                       "x4=%.8le * y4=%.8le\n ", time_value, ii, jj, x1, y1, x2, y2, x3, y3, x4, y4);
    }

    int nx = IDEAL_SQ_SIZE_X;
    int ny = IDEAL_SQ_SIZE_Y;

    double x_step = 1. / nx;
    double y_step = 1. / ny;

    // get right part for jakoby
    double phi = 0.;
    double mes = x_step * y_step;
    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {

            double ideal_x = i * x_step + x_step / 2.;
            double ideal_y = j * y_step + y_step / 2.;

            double real_x = x1 + (x2 - x1) * ideal_x + (x4 - x1) * ideal_y
                            + (x1 + x3 - x2 - x4) * ideal_x * ideal_y;
            double real_y = y1 + (y2 - y1) * ideal_x + (y4 - y1) * ideal_y
                            + (y1 + y3 - y2 - y4) * ideal_x * ideal_y;

            // find out in which square real point was placed
            int sq_i = (int) ((real_x - A) / HX);
            int sq_j = (int) ((real_y - C) / HY);
            double x = A + sq_i * HX;
            double y = C + sq_j * HY;

            double a11 = (x2 - x1) + (x1 + x3 - x2 - x4) * ideal_y;
            double a12 = (x4 - x1) + (x1 + x3 - x2 - x4) * ideal_x;
            double a21 = (y2 - y1) + (y1 + y3 - y2 - y4) * ideal_y;
            double a22 = (y4 - y1) + (y1 + y3 - y2 - y4) * ideal_x;
            double jakob = a11 * a22 - a21 * a12;

            // formula 4
            double dens = density[sq_i * NY_1 + sq_j] * (1 - (real_x - x) / HX) * (1 - (real_y - y) / HY)
                          + density[(sq_i + 1) * NY_1 + sq_j] * ((real_x - x) / HX) * (1 - (real_y - y) / HY)
                          + density[(sq_i + 1) * NY_1 + sq_j + 1] * ((real_x - x) / HX) * ((real_y - y) / HY)
                          + density[sq_i * NY_1 + sq_j + 1] * (1 - (real_x - x) / HX) * ((real_y - y) / HY);

            phi += mes * dens * jakob;
        }
    }

    if (fabs(phi) < fabs(DBL_MIN_TRIM)) phi = 0;
    return phi;
}

static double get_phi_integ_b_control_midpoint(int ii, int jj, double *density, double time_value) {
    double x1 = 0.;
    double y1 = 0.;
    double x2 = 0.;
    double y2 = 0.;
    double x3 = 0.;
    double y3 = 0.;
    double x4 = 0.;
    double y4 = 0.;
    double phi = 0.;

    if (ii > 0 && ii < NX && jj > 0 && jj < NY) {
        // p1 (x_{i-1/2}, y_{j-1/2})
        x1 = A + ii * HX - HX / 2.;
        y1 = C + jj * HY - HY / 2.;
        // p2 (x_{i+1/2}, y_{j-1/2})
        x2 = A + ii * HX + HX / 2.;
        y2 = C + jj * HY - HY / 2.;
        // p3 (x_{i+1/2}, y_{j+1/2})
        x3 = A + ii * HX + HX / 2.;
        y3 = C + jj * HY + HY / 2.;
        // p4 (x_{i-1/2}, y_{j+1/2})
        x4 = A + ii * HX - HX / 2.;
        y4 = C + jj * HY + HY / 2.;
        if (x1 <= A || x1 >= B || x2 <= A || x2 >= B || x3 <= A || x3 >= B || x4 <= A || x4 >= B
            || y1 <= C || y1 >= D || y2 <= C || y2 >= D || y3 <= C || y3 >= D || y4 <= C || y4 >= D)
            printf("ERROR INDEX (K LEVEL) i=%d j=%d : x1=%.8le * y1=%.8le ** x2=%.8le * y2=%.8le ** x3=%.8le * y3=%.8le ** "
                           "x4=%.8le * y4%.8le\n ", ii, jj, x1, y1, x2, y2, x3, y3, x4, y4);
    }
    else if (ii == NX && jj == NY) { // point (1,1)  omega_{i-1,j-1}
        // p1 (x_{NX-1/2}, y_{NY-1/2})
        x1 = B - HX / 2.;
        y1 = D - HY / 2.;
        // p2 (B, y_{NY-1/2})
        x2 = B;
        y2 = D - HY / 2.;
        // p3 (B, D)
        x3 = B;
        y3 = D;
        // p4 (x_{NX-1/2}, D)
        x4 = B - HX / 2.;
        y4 = D;
        if (x1 <= A || x1 > B || x2 <= A || x2 > B || x3 <= A || x3 > B || x4 <= A || x4 > B
            || y1 <= C || y1 > D || y2 <= C || y2 > D || y3 <= C || y3 > D || y4 <= C || y4 > D)
            printf("ERROR INDEX (K LEVEL) i=%d j=%d : x1=%.8le * y1=%.8le ** x2=%.8le * y2=%.8le ** x3=%.8le * y3=%.8le ** "
                           "x4=%.8le * y4%.8le\n ", ii, jj, x1, y1, x2, y2, x3, y3, x4, y4);
    }
    else if (jj == NY && ii > 0 && ii < NX) { // G3 -- top boundary
        // p1 (x_{i-1/2}, y_{NY-1/2})
        x1 = A + ii * HX - HX / 2.;
        y1 = D - HY / 2.;
        // p2 (x_{i+1/2}, y_{NY-1/2})
        x2 = A + ii * HX + HX / 2.;
        y2 = D - HY / 2.;
        //p3 (x_{i+1/2}, D)
        x3 = A + ii * HX + HX / 2.;
        y3 = D;
        //p4 (x_{i-1/2}, D)
        x4 = A + ii * HX - HX / 2.;
        y4 = D;
        if (x1 <= A || x1 >= B || x2 <= A || x2 >= B || x3 <= A || x3 >= B || x4 <= A || x4 >= B
            || y1 <= C || y1 > D || y2 <= C || y2 > D || y3 <= C || y3 > D || y4 <= C || y4 > D)
            printf("ERROR INDEX (K LEVEL) i=%d j=%d : x1=%.8le * y1=%.8le ** x2=%.8le * y2=%.8le ** x3=%.8le * y3=%.8le ** "
                           "x4=%.8le * y4%.8le\n ", ii, jj, x1, y1, x2, y2, x3, y3, x4, y4);
    }
    else if (ii == NX && jj > 0 && jj < NY) { // G2 -- right boundary
        // p1 (x_{NX-1/2}, y_{j-1/2})
        x1 = B - HX / 2.;
        y1 = C + jj * HY - HY / 2.;
        // p2 (B, y_{j-1/2})
        x2 = B;
        y2 = C + jj * HY - HY / 2.;
        // p3 (B, y_{j+1/2})
        x3 = B;
        y3 = C + jj * HY + HY / 2.;
        // p4 (x_{NX-1/2}, y_{j+1/2})
        x4 = B - HX / 2.;
        y4 = C + jj * HY + HY / 2.;
        if (x1 <= A || x1 > B || x2 <= A || x2 > B || x3 <= A || x3 > B || x4 <= A || x4 > B
            || y1 <= C || y1 >= D || y2 <= C || y2 >= D || y3 <= C || y3 >= D || y4 <= C || y4 >= D)
            printf("ERROR INDEX (K LEVEL) i=%d j=%d : x1=%.8le * y1=%.8le ** x2=%.8le * y2=%.8le ** x3=%.8le * y3=%.8le ** "
                           "x4=%.8le * y4%.8le\n ", ii, jj, x1, y1, x2, y2, x3, y3, x4, y4);
    }
    else printf("ERROR! INDEX (K LEVEL) i=%d j=%d ", ii, jj);

    double u = func_u(time_value, x1, y1);
    double v = func_v(time_value, x1, y1);
    x1 = x1 - TAU * u;
    y1 = y1 - TAU * v;
    u = func_u(time_value, x2, y2);
    v = func_v(time_value, x2, y2);
    x2 = x2 - TAU * u;
    y2 = y2 - TAU * v;
    u = func_u(time_value, x3, y3);
    v = func_v(time_value, x3, y3);
    x3 = x3 - TAU * u;
    y3 = y3 - TAU * v;
    u = func_u(time_value, x4, y4);
    v = func_v(time_value, x4, y4);
    x4 = x4 - TAU * u;
    y4 = y4 - TAU * v;
    if (x1 <= A || x1 >= B || x2 <= A || x2 >= B || x3 <= A || x3 >= B || x4 <= A || x4 >= B
        || y1 <= C || y1 >= D || y2 <= C || y2 >= D || y3 <= C || y3 >= D || y4 <= C || y4 >= D) {
        //printf("Time level %.8le! ERROR INDEX (K-1 LEVEL) i=%d j=%d : x1=%.8le * y1=%.8le ** x2=%.8le * y2=%.8le ** x3=%.8le * y3=%.8le ** "
        //               "x4=%.8le * y4=%.8le\n ", time_value, ii, jj, x1, y1, x2, y2, x3, y3, x4, y4);
        phi = OUT_DENSITY;
        return phi; //!!!!
    }

    int nx = IDEAL_SQ_SIZE_X;
    int ny = IDEAL_SQ_SIZE_Y;

    double x_step = 1. / nx;
    double y_step = 1. / ny;

    // get right part for jakoby
    double mes = x_step * y_step;
    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {

            double ideal_x = i * x_step + x_step / 2.;
            double ideal_y = j * y_step + y_step / 2.;

            double real_x = x1 + (x2 - x1) * ideal_x + (x4 - x1) * ideal_y
                            + (x1 + x3 - x2 - x4) * ideal_x * ideal_y;
            double real_y = y1 + (y2 - y1) * ideal_x + (y4 - y1) * ideal_y
                            + (y1 + y3 - y2 - y4) * ideal_x * ideal_y;

            // find out in which square real point was placed
            int sq_i = (int) ((real_x - A) / HX);
            int sq_j = (int) ((real_y - C) / HY);
            double x = A + sq_i * HX;
            double y = C + sq_j * HY;

            double a11 = (x2 - x1) + (x1 + x3 - x2 - x4) * ideal_y;
            double a12 = (x4 - x1) + (x1 + x3 - x2 - x4) * ideal_x;
            double a21 = (y2 - y1) + (y1 + y3 - y2 - y4) * ideal_y;
            double a22 = (y4 - y1) + (y1 + y3 - y2 - y4) * ideal_x;
            double jakob = a11 * a22 - a21 * a12;

            // formula 4
            double dens = density[sq_i * NY_1 + sq_j] * (1 - (real_x - x) / HX) * (1 - (real_y - y) / HY)
                          + density[(sq_i + 1) * NY_1 + sq_j] * ((real_x - x) / HX) * (1 - (real_y - y) / HY)
                          + density[(sq_i + 1) * NY_1 + sq_j + 1] * ((real_x - x) / HX) * ((real_y - y) / HY)
                          + density[sq_i * NY_1 + sq_j + 1] * (1 - (real_x - x) / HX) * ((real_y - y) / HY);

            phi += mes * dens * jakob;
        }
    }

    if (fabs(phi) < fabs(DBL_MIN_TRIM)) phi = 0;
    return phi;
}

static double get_phi_integ_exact(int ii, int jj, double *density, double time_value) {
    double x1 = 0.;
    double y1 = 0.;
    double x2 = 0.;
    double y2 = 0.;
    double x3 = 0.;
    double y3 = 0.;
    double x4 = 0.;
    double y4 = 0.;

    if (ii > 0 && ii < NX && jj > 0 && jj < NY) {
        // p1 (x_{i-1/2}, y_{j-1/2})
        x1 = A + ii * HX - HX / 2.;
        y1 = C + jj * HY - HY / 2.;
        // p2 (x_{i+1/2}, y_{j-1/2})
        x2 = A + ii * HX + HX / 2.;
        y2 = C + jj * HY - HY / 2.;
        // p3 (x_{i+1/2}, y_{j+1/2})
        x3 = A + ii * HX + HX / 2.;
        y3 = C + jj * HY + HY / 2.;
        // p4 (x_{i-1/2}, y_{j+1/2})
        x4 = A + ii * HX - HX / 2.;
        y4 = C + jj * HY + HY / 2.;
        if (x1 <= A || x1 >= B || x2 <= A || x2 >= B || x3 <= A || x3 >= B || x4 <= A || x4 >= B
            || y1 <= C || y1 >= D || y2 <= C || y2 >= D || y3 <= C || y3 >= D || y4 <= C || y4 >= D)
            printf("ERROR INDEX (K LEVEL) i=%d j=%d : x1=%.8le * y1=%.8le ** x2=%.8le * y2=%.8le ** x3=%.8le * y3=%.8le ** "
                           "x4=%.8le * y4%.8le\n ", ii, jj, x1, y1, x2, y2, x3, y3, x4, y4);
    }
    else if (ii == NX && jj == NY) { // point (1,1)  omega_{i-1,j-1}
        // p1 (x_{NX-1/2}, y_{NY-1/2})
        x1 = B - HX / 2.;
        y1 = D - HY / 2.;
        // p2 (B, y_{NY-1/2})
        x2 = B;
        y2 = D - HY / 2.;
        // p3 (B, D)
        x3 = B;
        y3 = D;
        // p4 (x_{NX-1/2}, D)
        x4 = B - HX / 2.;
        y4 = D;
        if (x1 <= A || x1 > B || x2 <= A || x2 > B || x3 <= A || x3 > B || x4 <= A || x4 > B
            || y1 <= C || y1 > D || y2 <= C || y2 > D || y3 <= C || y3 > D || y4 <= C || y4 > D)
            printf("ERROR INDEX (K LEVEL) i=%d j=%d : x1=%.8le * y1=%.8le ** x2=%.8le * y2=%.8le ** x3=%.8le * y3=%.8le ** "
                           "x4=%.8le * y4%.8le\n ", ii, jj, x1, y1, x2, y2, x3, y3, x4, y4);
    }
    else if (jj == NY && ii > 0 && ii < NX) { // G3 -- top boundary
        // p1 (x_{i-1/2}, y_{NY-1/2})
        x1 = A + ii * HX - HX / 2.;
        y1 = D - HY / 2.;
        // p2 (x_{i+1/2}, y_{NY-1/2})
        x2 = A + ii * HX + HX / 2.;
        y2 = D - HY / 2.;
        //p3 (x_{i+1/2}, D)
        x3 = A + ii * HX + HX / 2.;
        y3 = D;
        //p4 (x_{i-1/2}, D)
        x4 = A + ii * HX - HX / 2.;
        y4 = D;
        if (x1 <= A || x1 >= B || x2 <= A || x2 >= B || x3 <= A || x3 >= B || x4 <= A || x4 >= B
            || y1 <= C || y1 > D || y2 <= C || y2 > D || y3 <= C || y3 > D || y4 <= C || y4 > D)
            printf("ERROR INDEX (K LEVEL) i=%d j=%d : x1=%.8le * y1=%.8le ** x2=%.8le * y2=%.8le ** x3=%.8le * y3=%.8le ** "
                           "x4=%.8le * y4%.8le\n ", ii, jj, x1, y1, x2, y2, x3, y3, x4, y4);
    }
    else if (ii == NX && jj > 0 && jj < NY) { // G2 -- right boundary
        // p1 (x_{NX-1/2}, y_{j-1/2})
        x1 = B - HX / 2.;
        y1 = C + jj * HY - HY / 2.;
        // p2 (B, y_{j-1/2})
        x2 = B;
        y2 = C + jj * HY - HY / 2.;
        // p3 (B, y_{j+1/2})
        x3 = B;
        y3 = C + jj * HY + HY / 2.;
        // p4 (x_{NX-1/2}, y_{j+1/2})
        x4 = B - HX / 2.;
        y4 = C + jj * HY + HY / 2.;
        if (x1 <= A || x1 > B || x2 <= A || x2 > B || x3 <= A || x3 > B || x4 <= A || x4 > B
            || y1 <= C || y1 >= D || y2 <= C || y2 >= D || y3 <= C || y3 >= D || y4 <= C || y4 >= D)
            printf("ERROR INDEX (K LEVEL) i=%d j=%d : x1=%.8le * y1=%.8le ** x2=%.8le * y2=%.8le ** x3=%.8le * y3=%.8le ** "
                           "x4=%.8le * y4%.8le\n ", ii, jj, x1, y1, x2, y2, x3, y3, x4, y4);
    }
    else printf("ERROR! INDEX (K LEVEL) i=%d j=%d ", ii, jj);

    double u = func_u(time_value, x1, y1);
    double v = func_v(time_value, x1, y1);
    x1 = x1 - TAU * u;
    y1 = y1 - TAU * v;
    u = func_u(time_value, x2, y2);
    v = func_v(time_value, x2, y2);
    x2 = x2 - TAU * u;
    y2 = y2 - TAU * v;
    u = func_u(time_value, x3, y3);
    v = func_v(time_value, x3, y3);
    x3 = x3 - TAU * u;
    y3 = y3 - TAU * v;
    u = func_u(time_value, x4, y4);
    v = func_v(time_value, x4, y4);
    x4 = x4 - TAU * u;
    y4 = y4 - TAU * v;
    if (x1 <= A || x1 >= B || x2 <= A || x2 >= B || x3 <= A || x3 >= B || x4 <= A || x4 >= B
        || y1 <= C || y1 >= D || y2 <= C || y2 >= D || y3 <= C || y3 >= D || y4 <= C || y4 >= D) {
        printf("Time level %.8le! ERROR INDEX (K-1 LEVEL) i=%d j=%d : x1=%.8le * y1=%.8le ** x2=%.8le * y2=%.8le ** x3=%.8le * y3=%.8le ** "
                       "x4=%.8le * y4=%.8le\n ", time_value, ii, jj, x1, y1, x2, y2, x3, y3, x4, y4);

        return 0.;// на всякий случай
    }

    int sq_i = (int) ((x1 - A) / HX);
    int sq_j = (int) ((y1 - C) / HY);


    // get right part for jakoby
    int ip = ii - 1;
    int jp = jj - 1;

//    if (ip != sq_i || jp != sq_j) {
//        printf("ALARM! (%d %d) != (%d %d)\n", ip, jp, sq_i, sq_j);
//    }

    double mes = HX * HY;
    double dens = density[ip * NY_1 + jp]
                  + density[(ip + 1) * NY_1 + jp]
                  + density[(ip + 1) * NY_1 + jp + 1]
                  + density[ip * NY_1 + jp + 1];
    dens = dens / 4.;
    double phi = dens * mes;

    if (fabs(phi) < fabs(DBL_MIN_TRIM)) phi = 0;
    return phi;
}

double *solve_2(double &tme) {

    fflush(stdout);

    int ic = 0;
    double *phi = new double[XY];
    double *prev_density = new double[XY];
    double *density = new double[XY];
    double *residual = new double[XY];

    //<editor-fold desc="Fill initial data">

    for (int i = 0; i < NX_1; ++i) {
        for (int j = 0; j < NY_1; ++j) {
            density[NY_1 * i + j] = 0.;
            prev_density[NY_1 * i + j] = 0.;
            residual[NY_1 * i + j] = 0.;
            phi[NY_1 * i + j] = 0.;
        }
    }

    // G1 -- (x_i, 0=C) -- bottom boundary
    for (int i = 0; i < NX_1; ++i) {
        prev_density[NY_1 * i] = analytical_solution_circle(0., A + HX * i, C);
        if (fabs(prev_density[NY_1 * i]) < fabs(DBL_MIN_TRIM)) prev_density[NY_1 * i] = 0;
    }

    // G2 -- (NX=B, y_j) -- right boundary
    for (int j = 1; j < NY; ++j) {
        prev_density[NY_1 * NX + j] = analytical_solution_circle(0., A + HX * NX, C + HY * j);
        if (fabs(prev_density[NY_1 * NX + j]) < fabs(DBL_MIN_TRIM)) prev_density[NY_1 * NX + j] = 0;
    }

    // G3 -- (x_i, NY=D) -- top boundary
    for (int i = 0; i < NX_1; ++i) {
        prev_density[NY_1 * i + NY] = analytical_solution_circle(0., A + HX * i, C + HY * NY);
        if (fabs(prev_density[NY_1 * i + NY]) < fabs(DBL_MIN_TRIM)) prev_density[NY_1 * i + NY] = 0;
    }

    // G4 -- (0=A, y_j) -- left boundary
    for (int j = 1; j < NY; ++j) {
        prev_density[j] = analytical_solution_circle(0., A, C + HY * j);
        if (fabs(prev_density[j]) < fabs(DBL_MIN_TRIM)) prev_density[j] = 0;
    }

    memcpy(density, prev_density, XY * sizeof(double));

    // inner points
    for (int i = 1; i < NX; ++i) {
        for (int j = 1; j < NY; ++j) {
            prev_density[NY_1 * i + j] = analytical_solution_circle(0., A + HX * i, C + HY * j);
            //if (fabs(prev_density[NY_1 * i + j]) < fabs(DBL_MIN_TRIM)) prev_density[NY_1 * i + j] = 0;
        }
    }

    //</editor-fold>

    double sum_rho = calc_array_sum(prev_density, NX_1, NY_1, 0);
    double sum_abs_rho = calc_array_sum(prev_density, NX_1, NY_1, 1);
    printf("SUM RHO INIT = %le\n", sum_rho);
    printf("SUM ABS RHO INIT= %le\n", sum_abs_rho);
    fflush(stdout);

    double maxRes = FLT_MAX;
    double *extrems = new double[2];
    double *extrems_err = new double[2];

    for (int tl = 1; tl <= TIME_STEP_CNT; tl++) {

        //<editor-fold desc="Calculate phi">

        // with usage of prev_density we calculate phi function values

        // G3 -- (x_i, NY=D) -- top boundary
        for (int i = 1; i < NX; ++i) {
            double integ = 0.;
            if (INTEGR_TYPE == 1) {
                integ = get_phi_integ_midpoint(i, NY, prev_density, TAU * tl);
            }
            else if (INTEGR_TYPE == 2) {
                integ = get_phi_integ_trapezium(i, NY, prev_density, TAU * tl);
            }
            else if (INTEGR_TYPE == 3) {
                integ = get_phi_integ_exact(i, NY, prev_density, TAU * tl);
            }
            else if (INTEGR_TYPE == 4) {
                integ = get_phi_integ_b_control_midpoint(i, NY, prev_density, TAU * tl);
            }
            phi[NY_1 * i + NY] = integ;
        }

        // G2 -- (NX=B, y_j) -- right boundary
        for (int j = 1; j < NY; ++j) {
            double integ = 0.;
            if (INTEGR_TYPE == 1) {
                integ = get_phi_integ_midpoint(NX, j, prev_density, TAU * tl);
            }
            else if (INTEGR_TYPE == 2) {
                integ = get_phi_integ_trapezium(NX, j, prev_density, TAU * tl);
            }
            else if (INTEGR_TYPE == 3) {
                integ = get_phi_integ_exact(NX, j, prev_density, TAU * tl);
            }
            else if (INTEGR_TYPE == 4) {
                integ = get_phi_integ_b_control_midpoint(NX, j, prev_density, TAU * tl);
            }
            phi[NY_1 * NX + j] = integ;
        }

        double integ = 0.;
        if (INTEGR_TYPE == 1) {
            integ = get_phi_integ_midpoint(NX, NY, prev_density, TAU * tl);
        }
        else if (INTEGR_TYPE == 2) {
            integ = get_phi_integ_trapezium(NX, NY, prev_density, TAU * tl);
        }
        else if (INTEGR_TYPE == 3) {
            integ = get_phi_integ_exact(NX, NY, prev_density, TAU * tl);
        }
        else if (INTEGR_TYPE == 4) {
            integ = get_phi_integ_b_control_midpoint(NX, NY, prev_density, TAU * tl);
        }
        // point (1,1)
        phi[NY_1 * NX + NY] = integ;

        // inner points
        for (int i = 1; i < NX; ++i)
            for (int j = 1; j < NY; ++j) {
                double integ = 0.;
                if (INTEGR_TYPE == 1) {
                    integ = get_phi_integ_midpoint(i, j, prev_density, TAU * tl);
                }
                else if (INTEGR_TYPE == 2) {
                    integ = get_phi_integ_trapezium(i, j, prev_density, TAU * tl);
                }
                else if (INTEGR_TYPE == 3) {
                    integ = get_phi_integ_exact(i, j, prev_density, TAU * tl);
                }
                else if (INTEGR_TYPE == 4) {
                    integ = get_phi_integ_b_control_midpoint(i, j, prev_density, TAU * tl);
                }
                phi[NY_1 * i + j] = integ;
            }

        //</editor-fold>

        ic = 0;
        double maxErr = FLT_MAX;
        while (((maxErr > EPS) || (maxRes > RES_EPS)) && (ic < NX_1 * NY_1)) {

            //<editor-fold desc="Calculate density">
            // point 1,1
            double rpCoef = 64. / (9. * HX * HY);
            density[NY_1 * NX + NY] = -1. / 3. * (prev_density[NY_1 * NX + NY - 1]
                                                  + prev_density[NY_1 * (NX - 1) + NY])
                                      - 1. / 9. * prev_density[NY_1 * (NX - 1) + NY - 1]
                                      + rpCoef * phi[NY_1 * NX + NY];
            if (fabs(density[NY_1 * NX + NY]) < fabs(DBL_MIN_TRIM)) density[NY_1 * NX + NY] = 0;

            // G3 -- (x_i, NY=D) -- top boundary
            rpCoef = 32. / (9. * HX * HY);
            for (int i = 1; i < NX; ++i) {
                density[NY_1 * i + NY] = -1. / 3. * prev_density[NY_1 * i + NY - 1]
                                         - 1. / 6. * (prev_density[NY_1 * (i + 1) + NY] +
                                                      prev_density[NY_1 * (i - 1) + NY])
                                         - 1. / 18. * (prev_density[NY_1 * (i + 1) + NY - 1] +
                                                       prev_density[NY_1 * (i - 1) + NY - 1])
                                         + rpCoef * phi[NY_1 * i + NY];
                if (fabs(density[NY_1 * i + NY]) < fabs(DBL_MIN_TRIM)) density[NY_1 * i + NY] = 0;
            }

            // G2 -- (NX=B, y_j) -- right boundary
            rpCoef = 32. / (9. * HX * HY);
            for (int j = 1; j < NY; ++j) {
                density[NY_1 * NX + j] = -1. / 3. * prev_density[NY_1 * (NX - 1) + j]
                                         - 1. / 6. * (prev_density[NY_1 * NX + j - 1] +
                                                      prev_density[NY_1 * NX + j + 1])
                                         - 1. / 18. * (prev_density[NY_1 * (NX - 1) + j - 1] +
                                                       prev_density[NY_1 * (NX - 1) + j + 1])
                                         + rpCoef * phi[NY_1 * NX + j];
                if (fabs(density[NY_1 * NX + j]) < fabs(DBL_MIN_TRIM)) density[NY_1 * NX + j] = 0;
            }

            rpCoef = 16. / (9. * HX * HY);
            for (int i = 1; i < NX; ++i) {
                for (int j = 1; j < NY; ++j) {
                    density[NY_1 * i + j] = -1. / 6. * (
                            prev_density[NY_1 * i + j - 1] + // left
                            prev_density[NY_1 * (i - 1) + j] + // upper
                            prev_density[NY_1 * i + j + 1] + // right
                            prev_density[NY_1 * (i + 1) + j] // bottom
                    ) - 1. / 36. * (
                            prev_density[NY_1 * (i + 1) + j + 1] + // bottom right
                            prev_density[NY_1 * (i + 1) + j - 1] + // bottom left
                            prev_density[NY_1 * (i - 1) + j + 1] + // upper right
                            prev_density[NY_1 * (i - 1) + j - 1]  // upper left
                    ) + rpCoef * phi[NY_1 * i + j];
                    if (fabs(density[NY_1 * i + j]) < fabs(DBL_MIN_TRIM)) density[NY_1 * i + j] = 0;
                }
            }
            ++ic;

            maxErr = FLT_MIN;
            for (int i = 0; i < NX_1; ++i)
                for (int j = 0; j < NY_1; ++j) {
                    double val = fabs(density[i * NY_1 + j] - prev_density[i * NY_1 + j]);
                    if (val > maxErr) maxErr = val;
                }

            //</editor-fold>

            //<editor-fold desc="Calculate residual">

            // point 1,1
            rpCoef = HX * HY / 64.;
            residual[NY_1 * NX + NY] = rpCoef * (
                    9. * density[NY_1 * NX + NY] +
                    3. * (
                            density[NY_1 * NX + NY - 1] +
                            density[NY_1 * (NX - 1) + NY]
                    ) +
                    density[NY_1 * (NX - 1) + NY - 1]
            ) - phi[NY_1 * NX + NY];

            // G3 -- (x_i, NY=D) -- top boundary
            for (int i = 1; i < NX; ++i)
                residual[NY_1 * i + NY] = rpCoef * (
                        18. * density[NY_1 * i + NY] +
                        6. * density[NY_1 * i + NY - 1] +
                        3. * (
                                density[NY_1 * (i + 1) + NY] +
                                density[NY_1 * (i - 1) + NY]
                        ) +
                        density[NY_1 * (i + 1) + NY - 1] +
                        density[NY_1 * (i - 1) + NY - 1]
                ) - phi[NY_1 * i + NY];


            // G2 -- (NX=B, y_j) -- right boundary
            for (int j = 1; j < NY; ++j)
                residual[NY_1 * NX + j] = rpCoef * (
                        18. * density[NY_1 * NX + j] +
                        6. * density[NY_1 * (NX - 1) + j] +
                        3. * (
                                density[NY_1 * NX + j - 1] +
                                density[NY_1 * NX + j + 1]
                        ) +
                        density[NY_1 * (NX - 1) + j - 1] +
                        density[NY_1 * (NX - 1) + j + 1]
                ) - phi[NY_1 * NX + j];

            // inner points
            for (int i = 1; i < NX; ++i) {
                for (int j = 1; j < NY; ++j) {
                    residual[NY_1 * i + j] = rpCoef * (
                            36. * density[NY_1 * i + j] +
                            6. * (
                                    density[NY_1 * i + j - 1] + // left
                                    density[NY_1 * (i - 1) + j] + // upper
                                    density[NY_1 * i + j + 1] + // right
                                    density[NY_1 * (i + 1) + j] // bottom
                            ) +
                            prev_density[NY_1 * (i + 1) + j + 1] + // bottom right
                            prev_density[NY_1 * (i + 1) + j - 1] + // bottom left
                            prev_density[NY_1 * (i - 1) + j + 1] + // upper right
                            prev_density[NY_1 * (i - 1) + j - 1]  // upper left
                    ) - phi[NY_1 * i + j];
                }
            }

            maxRes = FLT_MIN;
            for (int i = 0; i < NX_1; ++i) {
                for (int j = 0; j < NY_1; ++j) {
                    double val = fabs(residual[i * NY_1 + j]);
                    if (val > maxRes) maxRes = val;
                }
            }

            //</editor-fold>

            memcpy(prev_density, density, XY * sizeof(double));
        }

        sum_rho = calc_array_sum(density, NX_1, NY_1, 0);
        sum_abs_rho = calc_array_sum(density, NX_1, NY_1, 1);
        extrems = calc_array_extrems(density, NX_1, NY_1);

        printf("tl = %d IterCount = %d Max(Residual) = %le Sum(Rho) = %le Sum(absRho) = %le "
                       "Min(Rho) = %le Max(Rho) = %le\n",
               tl, ic, maxRes, sum_rho, sum_abs_rho, extrems[0], extrems[1]);
        fflush(stdout);

        if (tl % 1 == 0) {
            print_data_to_files(phi, density, residual, tl);
            double x_0 = get_center_x() + tl * TAU * func_u(0, 0, 0);
            double y_0 = get_center_y() + tl * TAU * func_v(0, 0, 0);
            int fixed_x = (int) (x_0 / HX);
            int fixed_y = (int) (y_0 / HY);

            print_line_along_x("rho", NX, NY, HX, HY, tl, A, C, x_0, y_0, TAU,
                               U, V, density, fixed_y);
            print_line_along_y("rho", NX, NY, HX, HY, tl, A, C, x_0, y_0, TAU,
                               U, V, density, fixed_x);
        }
    }

    double x_0 = get_center_x() + TIME_STEP_CNT * TAU * func_u(0, 0, 0);
    double y_0 = get_center_y() + TIME_STEP_CNT * TAU * func_v(0, 0, 0);
    int fixed_x = (int) (x_0 / HX);
    int fixed_y = (int) (y_0 / HY);

/*    print_line_along_x("rho", NX, NY, HX, HY, TIME_STEP_CNT, A, C, x_0, y_0, TAU,
                       U, V, density, fixed_y);
    print_line_along_y("rho", NX, NY, HX, HY, TIME_STEP_CNT, A, C, x_0, y_0, TAU,
                       U, V, density, fixed_x);
*/
    double *err = calc_error_22(HX, HY, TAU * TIME_STEP_CNT, density);
    print_line_along_x("err", NX, NY, HX, HY, TIME_STEP_CNT, A, C, x_0, y_0, TAU,
                       U, V, err, fixed_y);
    print_line_along_y("err", NX, NY, HX, HY, TIME_STEP_CNT, A, C, x_0, y_0, TAU,
                       U, V, err, fixed_x);
    extrems_err = calc_array_extrems(err, NX_1, NY_1);

    err = calc_error_2(HX, HY, TAU * TIME_STEP_CNT, density);
    double l1_err_vec = get_l1_norm_vec(NX_1, NY_1, err);
    double l1_err_tr = get_l1_norm_int_trapezoidal(HX, HY, NX, NY, err); // note! a loop boundary
    extrems = calc_array_extrems(density, NX_1, NY_1);
    append_statistics(NX_1, NY_1, TAU, ic, l1_err_vec, l1_err_tr, maxRes, extrems, extrems_err, TIME_STEP_CNT);

    delete[] prev_density;
    delete[] phi;
    delete[] err;
    delete[] residual;
    delete[] extrems;
    delete[] extrems_err;
    return density;
}

double *calc_error_2(double hx, double hy, double tt, double *solution) {
    double *res = new double[XY];
    for (int i = 0; i < NX_1; i++)
        for (int j = 0; j < NY_1; j++)
            res[i * NY_1 + j] = fabs(solution[i * NY_1 + j]
                                     - analytical_solution_circle(tt, A + hx * i, C + hy * j));
    return res;
}
double *get_exact_solution_2(double hx, double hy, double t) {
    double *res = new double[XY];
    for (int i = 0; i < NX_1; i++)
        for (int j = 0; j < NY_1; j++)
            res[i * NY_1 + j] = fabs(analytical_solution_circle(t, A + hx * i, C + hy * j));
    return res;
}