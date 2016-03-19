#ifndef FEM_CIRCLE_CONSTS_H
#define FEM_CIRCLE_CONSTS_H

// area is [AxB]x[CxD]
extern double A;
extern double B;
extern double C;
extern double D;
extern int OX_LEN;
extern int OY_LEN;
extern int OX_LEN_1;
extern int OY_LEN_1;
extern int XY_LEN;
extern double TAU;
extern int TIME_STEP_CNT;
extern int JAK_ITER_CNT;
extern double HX;
extern double HY;
extern double R_SQ; // radius of circle in second power
extern double INN_DENSITY; // density inside circle
extern double OUT_DENSITY; // density out of circle boundary
extern double U_VELOCITY; // u velocity
extern double V_VELOCITY; // v velocity
extern double DBL_MIN_TRIM; // trim number to zero if it less than DBL_MIN_TRIM
extern double RES_EPS; // EPS value for residual
extern int INTEGR_TYPE; // Type of integration 1 = midpoint, 2 - trapezium
extern int IDEAL_SQ_SIZE_X; // Size of ideal square by x
extern int IDEAL_SQ_SIZE_Y; // Size of ideal square by y

#define EPS 10e-8

#endif //FEM_CIRCLE_CONSTS_H
