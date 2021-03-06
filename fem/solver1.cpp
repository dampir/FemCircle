#include <math.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include "consts.h"
#include "common.h"

inline static double analytical_solution_circle(double x, double y) {
    double x0 = get_center_x();
    double y0 = get_center_y();
    double value = (x - x0) * (x - x0) + (y - y0) * (y - y0);
    if (value <= R_SQ) return INN_DENSITY;
    return OUT_DENSITY;
}

inline static double func_u(double time_value, double x, double y) { return U; }

inline static double func_v(double time_value, double x, double y) { return V; }

static double get_phi(int ii, int jj, double *density, double time_value) {
    double x1 = 0.;
    double y1 = 0.;
    double x2 = 0.;
    double y2 = 0.;
    double x3 = 0.;
    double y3 = 0.;
    double x4 = 0.;
    double y4 = 0.;

    //  обход с нижней левой против часовой
    if (ii > 0 && ii < NX && jj > 0 && jj < NY) {
        // p1
        x1 = A + ii * HX + HX / 2.;
        y1 = C + jj * HY - HY / 2.;
        // p2
        x2 = A + ii * HX + HX / 2.;
        y2 = C + jj * HY + HY / 2.;
        // p3
        x3 = A + ii * HX - HX / 2.;
        y3 = C + jj * HY + HY / 2.;
        // p4
        x4 = A + ii * HX - HX / 2.;
        y4 = C + jj * HY + HY / 2.;
    }
    else if (ii == NX && jj == 0) { // point (0,0) omega_{i,j}
        // p1
        x1 = B;
        y1 = C;
        // p2
        x2 = B;
        y2 = C + HY / 2.;
        // p3
        x3 = B - HX / 2.;
        y3 = C + HY / 2.;
        // p4
        x4 = B - HX / 2.;
        y4 = C;
    }
    else if (ii == NX && jj == NY) { // point (1,0) omega_{i-1,j}
        x1 = B;
        y1 = C - HY / 2.;
        x2 = B;
        y2 = D;
        x3 = B - HX / 2.;
        y3 = D;
        x4 = B - HX / 2.;
        y4 = C - HY / 2.;
    }
    else if (ii == 0 && jj == NY) { // point (1,1)  omega_{i-1,j-1}
        // p1
        x1 = A + HX / 2.;
        y1 = D - HY / 2.;
        // p2
        x2 = A + HX / 2.;
        y2 = D;
        // p3
        x3 = A;
        y3 = D;
        // p4
        x4 = A;
        y4 = D - HY / 2.;
    }
    else if (ii == 0 && jj == NY) { // point (0,1)  omega_{i,j-1}
        // p1
        x1 = A + HX / 2.;
        y1 = C;
        // p2
        x2 = B + HX / 2.;
        y2 = C + HY / 2.;
        // p3
        x3 = A;
        y3 = C + HY / 2.;
        // p4
        x4 = A;
        y4 = C;
    }
    else if (jj == 0 && ii > 0 && ii < NX) {  // G1 left boundary
        // p1
        x1 = A + ii * HX + HX / 2.;
        y1 = C;
        // p2
        x2 = A + ii * HX + HX / 2.;
        y2 = C + HY / 2.;
        // p3
        x3 = A + ii * HX - HX / 2.;
        y3 = C + HY / 2.;
        // p4
        x4 = A + ii * HX - HX / 2.;
        y4 = C;
    }
    else if (ii == NX && jj > 0 && jj < NY) { // G2 bottom boundary
        x1 = B;
        y1 = C + jj * HY - HY / 2.;
        x2 = B;
        y2 = C + jj * HY + HY / 2.;
        x3 = B - HX / 2.;
        y3 = C + jj * HY + HY / 2.;
        x4 = B - HX / 2.;
        y4 = C + jj * HY - HY / 2.;
    }
    else if (jj == NY && ii > 0 && ii < NX) { // G3 right boundary
        // p1
        x1 = B + ii * HX + HX / 2.;
        y1 = D - HY / 2.;
        // p2
        x2 = A + ii * HX + HX / 2.;
        y2 = D;
        //p3
        x3 = A + ii * HX - HX / 2.;
        y3 = D;
        //p4
        x4 = A + ii * HX - HX / 2.;
        y4 = D - HY / 2.;
    }
    else if (ii == 0 && jj > 0 && jj < NY) { // G4 top boundary
        // p1
        x1 = A + HX / 2.;
        y1 = C + jj * HY - HY / 2.;
        // p2
        x2 = A + HX / 2.;
        y2 = C + jj * HY + HY / 2.;
        // p3
        x3 = A;
        y3 = C + jj * HY + HY / 2.;
        // p4
        x4 = A;
        y4 = C + jj * HY - HY / 2.;
    }

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

    int nx = 10;
    int ny = 10;
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

            // formula 4
            double dens = density[sq_i * NY_1 + sq_j] * (1 - (real_x - x) / HX) * (1 - (real_y - y) / HY)
                          + density[(sq_i + 1) * NY_1 + sq_j] * ((real_x - x) / HX) * (1 - (real_y - y) / HY)
                          + density[(sq_i + 1) * NY_1 + sq_j + 1] * ((real_x - x) / HX) * ((real_y - y) / HY)
                          + density[sq_i * NY_1 + sq_j + 1] * (1 - (real_x - x) / HX) * ((real_y - y) / HY);

            double a11 = (x2 - x1) + (x1 + x3 - x2 - x4) * ideal_y;
            double a12 = (x4 - x1) + (x1 + x3 - x2 - x4) * ideal_x;
            double a21 = (y2 - y1) + (y1 + y3 - y2 - y4) * ideal_y;
            double a22 = (y4 - y1) + (y1 + y3 - y2 - y4) * ideal_x;
            double jakob = a11 * a22 - a21 * a12;

            phi += mes * dens * jakob;
        }
    }

    return phi;
}

// ОБХОД ГРАНИЦЫ НЕВЕРНЫЙ! ДЕЛАТЬ КАК В SOLVER2
double *solve_1(double &tme) {
    fflush(stdout);

    double *phi = new double[XY];
    double *prev_density = new double[XY];
    double *density = new double[XY];

    for (int i = 0; i < NX_1; ++i)
        for (int j = 0; j < NY_1; ++j)
            density[NY_1 * i + j] = analytical_solution_circle(HX * i, HY * j);

    for (int i = 0; i < NX_1; ++i)
        for (int j = 0; j < NY_1; ++j)
            prev_density[NY_1 * i + j] = 0.;

    for (int tl = 1; tl <= TIME_STEP_CNT; tl++) {

        // with usage of prev_density we calculate phi function values
        for (int i = 0; i < NX_1; ++i)
            for (int j = 0; j < NY_1; ++j)
                phi[NY_1 * i + j] = get_phi(i, j, density, TAU * tl);

        int ic = 0;
        double maxErr = FLT_MAX;
        while (maxErr > EPS && ic < JAK_ITER_CNT) {
            double rpCoef = 64. / (9. * HX * HY);
            // ДОПИСАТЬ НА НОВЫЕ ИНДЕКСЫ!!!!!
            // point 0,0
            density[NY_1 * NX] =
                    -1. / 3. * (prev_density[NY_1 * (NX - 1)] + prev_density[NY_1 * NX + 1]) -
                    1. / 9. * prev_density[NY_1 * (NX - 1) + 1]
                    + rpCoef * phi[NY_1 * NX];

            // point 1,0
            density[0] = -1. / 3. * (prev_density[1] + prev_density[NY_1])
                         - 1. / 9. * prev_density[NY_1 + 1]
                         + rpCoef * phi[0];

            // point 1,1
            density[NY] = -1. / 3. * (prev_density[NY - 1]
                                      + prev_density[NY_1 + NY])
                          - 1. / 9. * prev_density[NY_1 + NY - 1]
                          + rpCoef * phi[NY];

            // point 0,1
            density[NY_1 * NX + NY] = -1. / 3. * (prev_density[NY_1 * NX + NY - 1]
                                                  + prev_density[NY_1 * (NX - 1) + NY])
                                      - 1. / 9. * prev_density[NY_1 * (NX - 1) + NY - 1]
                                      + rpCoef * phi[NY_1 * NX + NY];

            double bdCoef = 32. / (9. * HX * HY);
            // G1 left boundary
            for (int i = 1; i < NX; ++i) {
                density[NY_1 * i] = -2. / 9. * prev_density[NY_1 * i + 1]
                                    - 1. / 6.
                                          * (prev_density[NY_1 * (i + 1)]
                                             + prev_density[NY_1 * (i - 1)])
                                    - 1. / 18. * (prev_density[NY_1 * (i + 1) + 1] +
                                                  prev_density[NY_1 * (i - 1) + 1])
                                    + bdCoef * phi[NY_1 * i];
            }

            // G2 bottom boundary
            for (int j = 1; j < NY; ++j) {
                density[NY_1 * NX + j] = -2. / 9. * prev_density[NY_1 * (NX - 1) + j]
                                         - 1. / 6. *
                                                   (prev_density[NY_1 * NX + j + 1] +
                                                    prev_density[NY_1 * NX + j - 1])
                                         - 1. / 18. *
                                                   (prev_density[NY_1 * (NX - 1) + j + 1] +
                                                    prev_density[NY_1 * (NX - 1) + j - 1])
                                         + bdCoef * phi[NY_1 * NX + j];
            }

            // G3 right boundary
            for (int i = 1; i < NX; ++i) {
                density[NY_1 * i + NY] = -2. / 9. * prev_density[NY_1 * i + NY - 1]
                                         - 1. / 6. * (prev_density[NY_1 * (i + 1) + NY] +
                                                      prev_density[NY_1 * (i - 1) + NY])
                                         - 1. / 18. * (prev_density[NY_1 * (i + 1) + NY - 1] +
                                                       prev_density[NY_1 * (i - 1) + NY - 1])
                                         + bdCoef * phi[NY_1 * i + NY];
            }

            // G4 top boundary
            for (int j = 1; j < NY; ++j) {
                density[j] = -2. / 9. * prev_density[NY_1 + j]
                             - 1. / 6. * (prev_density[j - 1] +
                                          prev_density[j + 1])
                             - 1. / 18. * (prev_density[NY_1 + j - 1] +
                                           prev_density[NY_1 + j + 1])
                             + bdCoef * phi[j];
            }

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
                    ) + (16. * phi[NY_1 * i + j]) / (9. * HX * HY);
                }
            }
            ++ic;

            maxErr = FLT_MIN;
            for (int i = 0; i < NX_1; ++i) {
                for (int j = 0; j < NY_1; ++j) {
                    double val = fabs(density[i * NY_1 + j] - prev_density[i * NY_1 + j]);
                    if (val > maxErr) maxErr = val;
                }
            }

            memcpy(prev_density, density, XY * sizeof(double));
        }
    }

    delete[] prev_density;
    delete[] phi;
    return density;
}

double *calc_error_1(double hx, double hy, double *solution) {
    double *res = new double[XY];
    for (int i = 0; i < NX_1; i++)
        for (int j = 0; j < NY_1; j++)
            res[i * NY_1 + j] = fabs(solution[i * NY_1 + j]
                                     - analytical_solution_circle(A + hx * i, C + hy * j));
    return res;
}