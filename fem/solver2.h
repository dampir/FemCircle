#ifndef FEM_CIRCLE_SOLVER_2_H
#define FEM_CIRCLE_SOLVER_2_H

double *solve_2(double &tme);

double *calc_error_2(double hx, double hy, double tt, double *solution);

double get_center_x_2();

double get_center_y_2();

double *get_exact_solution_2(double hx, double hy, double t);

double calc_sum_rho(double *solution);

double calc_sum_abs_rho(double *solution);

#endif //FEM_CIRCLE_SOLVER_2_H