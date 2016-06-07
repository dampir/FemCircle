#include <math.h>#include <stdio.h>#include <string.h>#include <float.h>#include "consts.h"#include "timer.h"#include "utils.h"#include "common.h"#include "algorithm"using namespace std;inline void print_data_to_files(double *phi, double *density, double *residual, int tl) {    print_surface("phi", OX_LEN, OY_LEN, HX, HY, tl, A, C, get_center_x(), get_center_y(), TAU,                  U_VELOCITY, V_VELOCITY, phi);    print_surface("rho", OX_LEN, OY_LEN, HX, HY, tl, A, C, get_center_x(), get_center_y(), TAU,                  U_VELOCITY, V_VELOCITY, density);//    print_surface("res", OX_LEN, OY_LEN, HX, HY, tl, A, C, get_center_x(), get_center_y(), TAU,//                  U_VELOCITY, V_VELOCITY, residual);    double *err_lock = calc_error_7(HX, HY, tl * TAU, density);    print_surface("err-l", OX_LEN, OY_LEN, HX, HY, tl, A, C, get_center_x(), get_center_y(),                  TAU, U_VELOCITY, V_VELOCITY, err_lock);    delete[] err_lock;}inline static double func_u(double t, double x, double y) {    return (-y + CENTER_OFFSET_Y) * U_VELOCITY;}inline static double func_v(double t, double x, double y) {    return (x - CENTER_OFFSET_X) * V_VELOCITY;}inline static double analytical_solution_circle(double t, double x, double y) {    double r = 0.2;    double x0 = get_center_x() - r * sin(t * OMEGA);    double y0 = get_center_y() + r * cos(t * OMEGA);    double value = (x - x0) * (x - x0) + (y - y0) * (y - y0);    if (value < R_SQ)        return INN_DENSITY;    return OUT_DENSITY;}static inline double get_distance(double real_x, double real_y, double x, double y) {    double sqX = (real_x - x) * (real_x - x);    double sqY = (real_y - y) * (real_y - y);    double r = sqrt(sqX + sqY);    return r;}static double get_phi_integ_midpoint(int ii, int jj, double *density, double time_value) {    double x1 = 0.;    double y1 = 0.;    double x2 = 0.;    double y2 = 0.;    double x3 = 0.;    double y3 = 0.;    double x4 = 0.;    double y4 = 0.;    get_coordinates_on_curr(ii, jj, x1, y1, x2, y2, x3, y3, x4, y4);    double u = func_u(time_value, x1, y1);    double v = func_v(time_value, x1, y1);    x1 = x1 - TAU * u;    y1 = y1 - TAU * v;    u = func_u(time_value, x2, y2);    v = func_v(time_value, x2, y2);    x2 = x2 - TAU * u;    y2 = y2 - TAU * v;    u = func_u(time_value, x3, y3);    v = func_v(time_value, x3, y3);    x3 = x3 - TAU * u;    y3 = y3 - TAU * v;    u = func_u(time_value, x4, y4);    v = func_v(time_value, x4, y4);    x4 = x4 - TAU * u;    y4 = y4 - TAU * v;    int nx = IDEAL_SQ_SIZE_X;    int ny = IDEAL_SQ_SIZE_Y;    double x_step = 1. / nx;    double y_step = 1. / ny;    // get right part for jakoby    double phi = 0.;    double mes = x_step * y_step;    for (int i = 0; i < nx; ++i) {        for (int j = 0; j < ny; ++j) {            double ideal_x = i * x_step + x_step / 2.;            double ideal_y = j * y_step + y_step / 2.;            double real_x = x1 + (x2 - x1) * ideal_x + (x4 - x1) * ideal_y                            + (x1 + x3 - x2 - x4) * ideal_x * ideal_y;            double real_y = y1 + (y2 - y1) * ideal_x + (y4 - y1) * ideal_y                            + (y1 + y3 - y2 - y4) * ideal_x * ideal_y;            // find out in which square real point was placed            int sq_i = (int) (((real_x - A) + FLT_MIN) / HX);            int sq_j = (int) (((real_y - C) + FLT_MIN) / HY);            double x = A + sq_i * HX;            double y = C + sq_j * HY;            double a11 = (x2 - x1) + (x1 + x3 - x2 - x4) * ideal_y;            double a12 = (x4 - x1) + (x1 + x3 - x2 - x4) * ideal_x;            double a21 = (y2 - y1) + (y1 + y3 - y2 - y4) * ideal_y;            double a22 = (y4 - y1) + (y1 + y3 - y2 - y4) * ideal_x;            double jakob = a11 * a22 - a21 * a12;            // left bottom            x1 = x;            y1 = y;            // right bottom            x2 = x + HX;            y2 = y;            // right top            x3 = x + HX;            y3 = y + HY;            // left top            x4 = x;            y4 = y + HY;            double d1 = get_distance(real_x, real_y, x1, y1);            double d2 = get_distance(real_x, real_y, x2, y2);            double d3 = get_distance(real_x, real_y, x3, y3);            double d4 = get_distance(real_x, real_y, x4, y4);            double minD = std::min(d1, std::min(d2, std::min(d3, d4)));            double dens = 0.;            if (minD == d1) // left bottom            {                dens = density[sq_i * OY_LEN_1 + sq_j];            }            else if (minD == d2) // right bottom            {                dens = density[(sq_i + 1) * OY_LEN_1 + sq_j];            }            else if (minD == d3) // right top            {                dens = density[(sq_i + 1) * OY_LEN_1 + sq_j + 1];            }            else if (minD == d4) // left top            {                dens = density[sq_i * (OY_LEN_1) + sq_j + 1];            }            phi += mes * dens * jakob;        }    }    if (fabs(phi) < fabs(DBL_MIN_TRIM)) phi = 0;    return phi;}double *solve_7(double &tme) {    StartTimer();    fflush(stdout);    double *phi = new double[XY_LEN];    double *prev_density = new double[XY_LEN];    double *density = new double[XY_LEN];    double *residual = new double[XY_LEN];    //<editor-fold desc="fill initial data">    for (int i = 0; i < OX_LEN_1; ++i) {        for (int j = 0; j < OY_LEN_1; ++j) {            density[OY_LEN_1 * i + j] = 0.;            prev_density[OY_LEN_1 * i + j] = 0.;            residual[OY_LEN_1 * i + j] = 0.;            phi[OY_LEN_1 * i + j] = 0.;        }    }    // G1 -- (x_i, 0=C) -- bottom boundary    for (int i = 0; i < OX_LEN_1; ++i) {        prev_density[OY_LEN_1 * i] = analytical_solution_circle(0., A + HX * i, C);        if (fabs(prev_density[OY_LEN_1 * i]) < fabs(DBL_MIN_TRIM)) prev_density[OY_LEN_1 * i] = 0;    }    // G2 -- (OX_LEN=B, y_j) -- right boundary    for (int j = 1; j < OY_LEN; ++j) {        prev_density[OY_LEN_1 * OX_LEN + j] = analytical_solution_circle(0., A + HX * OX_LEN, C + HY * j);        if (fabs(prev_density[OY_LEN_1 * OX_LEN + j]) < fabs(DBL_MIN_TRIM)) prev_density[OY_LEN_1 * OX_LEN + j] = 0;    }    // G3 -- (x_i, OY_LEN=D) -- top boundary    for (int i = 0; i < OX_LEN_1; ++i) {        prev_density[OY_LEN_1 * i + OY_LEN] = analytical_solution_circle(0., A + HX * i, C + HY * OY_LEN);        if (fabs(prev_density[OY_LEN_1 * i + OY_LEN]) < fabs(DBL_MIN_TRIM)) prev_density[OY_LEN_1 * i + OY_LEN] = 0;    }    // G4 -- (0=A, y_j) -- left boundary    for (int j = 1; j < OY_LEN; ++j) {        prev_density[j] = analytical_solution_circle(0., A, C + HY * j);        if (fabs(prev_density[j]) < fabs(DBL_MIN_TRIM)) prev_density[j] = 0;    }    memcpy(density, prev_density, XY_LEN * sizeof(double));    // inner points    for (int i = 1; i < OX_LEN; ++i) {        for (int j = 1; j < OY_LEN; ++j) {            prev_density[OY_LEN_1 * i + j] = analytical_solution_circle(0., A + HX * i, C + HY * j);            //if (fabs(prev_density[OY_LEN_1 * i + j]) < fabs(DBL_MIN_TRIM)) prev_density[OY_LEN_1 * i + j] = 0;        }    }    //</editor-fold>    printf("SUM RHO INIT = %le\n", calc_array_sum(prev_density, OX_LEN_1, OY_LEN_1, 0));    printf("SUM ABS RHO INIT= %le\n", calc_array_sum(prev_density, OX_LEN_1, OY_LEN_1, 1));    fflush(stdout);    double *extrems;    double *extrems_err;    double *err;    double l1_err_vec;    double l1_err_tr;    for (int tl = 1; tl <= TIME_STEP_CNT; tl++) {        //<editor-fold desc="calculate phi">        // G1 -- (x_i, 0=C) -- bottom boundary        for (int i = 1; i < OX_LEN; ++i) {            if (G1[i] == 1) {                phi[OY_LEN_1 * i] = get_phi_integ_midpoint(i, 0, prev_density, TAU * tl);            }        }        // G2 -- (OX_LEN=B, y_j) -- right boundary        for (int j = 1; j < OY_LEN; ++j) {            if (G2[j] == 1) {                phi[OY_LEN_1 * OX_LEN + j] = get_phi_integ_midpoint(OX_LEN, j, prev_density, TAU * tl);            }        }        // G3 -- (x_i, OY_LEN=D) -- top boundary        for (int i = 1; i < OX_LEN; ++i) {            if (G3[i] == 1) {                phi[OY_LEN_1 * i + OY_LEN] = get_phi_integ_midpoint(i, OY_LEN, prev_density, TAU * tl);            }        }        // G4 -- (0=A, y_j) -- left boundary        for (int j = 1; j < OY_LEN; ++j) {            if (G4[j] == 1) {                phi[j] = get_phi_integ_midpoint(0, j, prev_density, TAU * tl);            }        }        // point (0.0)        if (CP00 == 1) {            phi[0] = get_phi_integ_midpoint(0, 0, prev_density, TAU * tl);        }        // point (1.0)        if (CP10 == 1) {            phi[OY_LEN_1 * OX_LEN] = get_phi_integ_midpoint(OX_LEN, 0, prev_density, TAU * tl);        }        // point (0.1)        if (CP01 == 1) {            phi[OY_LEN] = get_phi_integ_midpoint(0, OY_LEN, prev_density, TAU * tl);        }        // point (1,1)        if (CP11 == 1) {            phi[OY_LEN_1 * OX_LEN + OY_LEN] = get_phi_integ_midpoint(OX_LEN, OY_LEN, prev_density, TAU * tl);        }        // inner points        for (int i = 1; i < OX_LEN; ++i)            for (int j = 1; j < OY_LEN; ++j) {                phi[OY_LEN_1 * i + j] = get_phi_integ_midpoint(i, j, prev_density, TAU * tl);            }        //</editor-fold>        // G1 -- (x_i, 0=C) -- bottom boundary        double rpCoef = 2. / (HX * HY);        for (int i = 1; i < OX_LEN; ++i) {            if (G1[i] == 1) {                density[OY_LEN_1 * i] = rpCoef * phi[OY_LEN_1 * i];                if (fabs(density[OY_LEN_1 * i]) < fabs(DBL_MIN_TRIM)) density[OY_LEN_1 * i] = 0;            }        }        // G2 -- (OX_LEN=B, y_j) -- right boundary        for (int j = 1; j < OY_LEN; ++j) {            if (G2[j] == 1) {                density[OY_LEN_1 * OX_LEN + j] = rpCoef * phi[OY_LEN_1 * OX_LEN + j];                if (fabs(density[OY_LEN_1 * OX_LEN + j]) < fabs(DBL_MIN_TRIM)) density[OY_LEN_1 * OX_LEN + j] = 0;            }        }        // G3 -- (x_i, OY_LEN=D) -- top boundary        for (int i = 1; i < OX_LEN; ++i) {            if (G3[i] == 1) {                density[OY_LEN_1 * i + OY_LEN] = rpCoef * phi[OY_LEN_1 * i + OY_LEN];                if (fabs(density[OY_LEN_1 * i + OY_LEN]) < fabs(DBL_MIN_TRIM)) density[OY_LEN_1 * i + OY_LEN] = 0;            }        }        // G4 -- (0=A, y_j) -- left boundary        for (int j = 1; j < OY_LEN; ++j) {            if (G4[j] == 1) { // проверить коэф-ты                density[j] = rpCoef * phi[j];                if (fabs(density[j]) < fabs(DBL_MIN_TRIM)) density[j] = 0;            }        }        rpCoef = 4. / (HX * HY);        // point (0,0)        if (CP00 == 1) {            density[0] = phi[0];            if (fabs(density[0]) < fabs(DBL_MIN_TRIM)) density[0] = 0;        }        // point (1,0)        if (CP10 == 1) {            density[OY_LEN_1 * OX_LEN] = rpCoef * phi[OY_LEN_1 * OX_LEN];            if (fabs(density[OY_LEN_1 * OX_LEN]) < fabs(DBL_MIN_TRIM))                density[OY_LEN_1 * OX_LEN] = 0;        }        // point (0,1)        if (CP01 == 1) {            density[OY_LEN] = rpCoef * phi[OY_LEN];            if (fabs(density[OY_LEN]) < fabs(DBL_MIN_TRIM))                density[OY_LEN] = 0;        }        // point (1,1)        if (CP11 == 1) {            density[OY_LEN_1 * OX_LEN + OY_LEN] = rpCoef * phi[OY_LEN_1 * OX_LEN + OY_LEN];            if (fabs(density[OY_LEN_1 * OX_LEN + OY_LEN]) < fabs(DBL_MIN_TRIM))                density[OY_LEN_1 * OX_LEN + OY_LEN] = 0;        }        rpCoef = 1. / (HX * HY);        for (int i = 1; i < OX_LEN; ++i) {            for (int j = 1; j < OY_LEN; ++j) {                density[OY_LEN_1 * i + j] = rpCoef * phi[OY_LEN_1 * i + j];                if (fabs(density[OY_LEN_1 * i + j]) < fabs(DBL_MIN_TRIM)) density[OY_LEN_1 * i + j] = 0;            }        }        memcpy(prev_density, density, XY_LEN * sizeof(double));        if (tl % 5 == 0) {            err = calc_error_7(HX, HY, TAU * tl, density);            l1_err_vec = get_l1_norm_vec(OX_LEN_1, OY_LEN_1, err);            l1_err_tr = get_l1_norm_int_trapezoidal(HX, HY, OX_LEN, OY_LEN, err); // note! a loop boundary            extrems = calc_array_extrems(density, OX_LEN_1, OY_LEN_1);            extrems_err = calc_array_extrems(err, OX_LEN_1, OY_LEN_1);            printf("tl = %d Sum(Rho)= %le  ERR_VEC= %le  ERR_TR= %le  MAX_RHO= %le"                           "  MAX_ERR= %le\n", tl, calc_array_sum(density, OX_LEN_1, OY_LEN_1, 0),                   l1_err_vec, l1_err_tr, extrems[1], extrems_err[1]);            fflush(stdout);        }        if (tl % 5 == 0) {            print_data_to_files(phi, density, residual, tl);            int fixed_x = (int) (get_center_x() / HX);            int fixed_y = (int) (get_center_y() / HY);            //print_line_along_x("rho", OX_LEN, OY_LEN, HX, HY, tl, A, C, get_center_x(), get_center_y(), TAU,            //                  U_VELOCITY, V_VELOCITY, density, fixed_y);            //print_line_along_y("rho", OX_LEN, OY_LEN, HX, HY, tl, A, C, get_center_x(), get_center_y(), TAU,            //                   U_VELOCITY, V_VELOCITY, density, fixed_x);        }    } // time loop    err = calc_error_7(HX, HY, TAU * TIME_STEP_CNT, density);    l1_err_vec = get_l1_norm_vec(OX_LEN_1, OY_LEN_1, err);    l1_err_tr = get_l1_norm_int_trapezoidal(HX, HY, OX_LEN, OY_LEN, err); // note! a loop boundary    extrems = calc_array_extrems(density, OX_LEN_1, OY_LEN_1);    extrems_err = calc_array_extrems(err, OX_LEN_1, OY_LEN_1);    append_statistics_expl(OX_LEN_1, OY_LEN_1, TAU, l1_err_vec, l1_err_tr, extrems,                           extrems_err, TIME_STEP_CNT);    delete[] prev_density;    delete[] phi;    delete[] err;    delete[] residual;    delete[] extrems;    delete[] extrems_err;    tme = GetTimer() / 1000;    return density;}double *calc_error_7(double hx, double hy, double tt, double *solution) {    double *res = new double[XY_LEN];    for (int i = 0; i < OX_LEN_1; i++)        for (int j = 0; j < OY_LEN_1; j++)            res[i * OY_LEN_1 + j] = fabs(solution[i * OY_LEN_1 + j]                                         - analytical_solution_circle(tt, A + hx * i, C + hy * j));    return res;}double *get_exact_solution_7(double hx, double hy, double t) {    double *res = new double[XY_LEN];    for (int i = 0; i < OX_LEN_1; i++)        for (int j = 0; j < OY_LEN_1; j++)            res[i * OY_LEN_1 + j] = fabs(analytical_solution_circle(t, A + hx * i, C + hy * j));    return res;}