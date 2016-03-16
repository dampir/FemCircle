#include <utils.h>
#include <cmath>
#include "gtest/gtest.h"
#include "consts.h"
#include "solver1.h"
#include "solver2.h"
#include "tecplot.h"

class FemFixture : public ::testing::Test {
protected:
    virtual void TearDown() {
    }

    virtual void SetUp() {
    }

public:
    FemFixture() : Test() {
    }
};

class FemFixture1 : public ::testing::Test {
protected:
    virtual void TearDown() {
    }

    virtual void SetUp() {
    }

public:
    FemFixture1() : Test() {
    }
};

TEST_F(FemFixture, test1) {
    double tme = 0.;

    double d = 0;

    for (int i = 0; i < 2; ++i) {
        switch (i) {
            case 0:
                d = 50.;
                break;
            case 1:
                d = 100.;
                break;
            case 2:
                d = 200.;
                break;
            case 3:
                d = 400.;
                break;
            case 4:
                d = 800.;
                break;
            case 5:
                d = 1600.;
                break;
            default:
                break;
        }

        A = 0.;
        B = 1.;
        C = 0.;
        D = 1.;
        R_SQ = 0.25 * 0.25;
        INN_DENSITY = 1.;
        OUT_DENSITY = 0.;
        JAK_ITER_CNT = 3000;

        TAU = 1. / d;
        OX_LEN = (int) d;
        OY_LEN = (int) d;
        OX_LEN_1 = OX_LEN + 1;
        OY_LEN_1 = OY_LEN + 1;
        HX = (B - A) / OX_LEN;
        HY = (D - C) / OY_LEN;
        TIME_STEP_CNT = (int) d;
        XY_LEN = OX_LEN_1 * OY_LEN_1;
        U_VELOCITY = 0.;
        V_VELOCITY = 0.;
        printf("\nOX_LEN = %d OY_LEN = %d\n", OX_LEN, OY_LEN);
        double *density = solve_1(tme);
        double *err = calc_error_1(HX, HY, density);
        double x0 = get_center_x_1();
        double y0 = get_center_y_1();
        print_surface("test1_rho", OX_LEN, OY_LEN, HX, HY, TIME_STEP_CNT, A, C, x0, y0, TAU, U_VELOCITY,
                      V_VELOCITY, density);
        print_surface("test1_err", OX_LEN, OY_LEN, HX, HY, TIME_STEP_CNT, A, C, x0, y0, TAU, U_VELOCITY,
                      V_VELOCITY, err);
        double l1 = get_l1_norm(HX, HY, OX_LEN_1, OY_LEN_1, err);
        double l_inf = get_l_inf_norm(OX_LEN_1, OY_LEN_1, err);
        printf("l1 %le \n", l1);
        printf("l_inf %le\n", l_inf);
        delete[] density;
        delete[] err;
    }
}

TEST_F(FemFixture, test2) {
    double tme = 0.;

    double d = 0;

    for (int i = 2; i < 4; ++i) {
        switch (i) {
            case 0:
                d = 50.;
                break;
            case 1:
                d = 100.;
                break;
            case 2:
                d = 200.;
                break;
            case 3:
                d = 400.;
                break;
            case 4:
                d = 800.;
                break;
            case 5:
                d = 1600.;
                break;
            default:
                break;
        }

        A = 0.;
        B = 1.;
        C = 0.;
        D = 1.;
        R_SQ = 0.1 * 0.1;
        INN_DENSITY = 1.;
        OUT_DENSITY = 0.;

        OX_LEN = (int) d;
        OY_LEN = (int) d;
        OX_LEN_1 = OX_LEN + 1;
        OY_LEN_1 = OY_LEN + 1;
        HX = (B - A) / OX_LEN;
        HY = (D - C) / OY_LEN;

        // (u, v) = (a, a)
        double a = 1;
        U_VELOCITY = a;
        V_VELOCITY = a;
        TAU = std::min(HX / (3. * a), HY / (3. * a));
        printf("\nHX = %le\n", HX);
        printf("HY = %le\n", HY);
        printf("TAU = %le\n", TAU);

        TIME_STEP_CNT = (int) 1;
        XY_LEN = OX_LEN_1 * OY_LEN_1;

        printf("OX_LEN = %d OY_LEN = %d\n", OX_LEN, OY_LEN);
        double *density = solve_2(tme);
        double *err = calc_error_2(HX, HY, TAU * TIME_STEP_CNT, density);
        double x0 = get_center_x_2();
        double y0 = get_center_y_2();
        print_surface("test2_rho", OX_LEN, OY_LEN, HX, HY, TIME_STEP_CNT, A, C, x0, y0, TAU, U_VELOCITY,
                      V_VELOCITY, density);
        print_surface("test2_err", OX_LEN, OY_LEN, HX, HY, TIME_STEP_CNT, A, C, x0, y0, TAU, U_VELOCITY,
                      V_VELOCITY, err);
        double l1 = get_l1_norm(HX, HY, OX_LEN_1, OY_LEN_1, err);
        double l_inf = get_l_inf_norm(OX_LEN_1, OY_LEN_1, err);
        printf("l1 %le \n", l1);
        printf("l_inf %le\n", l_inf);
        delete[] density;
        delete[] err;
    }
}

TEST_F(FemFixture1, test2_1) {
    double tme = 0.;
    A = 0.;
    B = 1.;
    C = 0.;
    D = 1.;
    R_SQ = 0.099 * 0.099;
    INN_DENSITY = 1.;
    OUT_DENSITY = 0.;
    RES_EPS = 1.e-14;
    DBL_MIN_TRIM = 1.e-16;
    INTEGR_TYPE = 1;

    OX_LEN = (int) 200;
    OY_LEN = (int) 200;
    OX_LEN_1 = OX_LEN + 1;
    OY_LEN_1 = OY_LEN + 1;
    HX = (B - A) / OX_LEN;
    HY = (D - C) / OY_LEN;

    U_VELOCITY = 1.;//1.;
    V_VELOCITY = 1.;//1.;
    TAU = 1.e-3;
    //TIME_STEP_CNT = (int) ((1 - get_center_x_2() - get_center_y_2()) / TAU);
    TIME_STEP_CNT = 4;
    XY_LEN = OX_LEN_1 * OY_LEN_1;

    printf("\nOX_LEN = %d OY_LEN = %d\n", OX_LEN, OY_LEN);
    printf("HX = %le\n", HX);
    printf("HY = %le\n", HY);
    printf("TAU = %le\n", TAU);
    printf("U = %le\n", U_VELOCITY);
    printf("V = %le\n", V_VELOCITY);
    printf("TIME_STEP_CNT = %d\n", TIME_STEP_CNT);

    double *density = solve_2(tme);
    double *err = calc_error_2(HX, HY, TAU * TIME_STEP_CNT, density);
    double *exact0 = get_exact_solution_2(HX, HY, 0);
    double *exactT = get_exact_solution_2(HX, HY, TAU * TIME_STEP_CNT);

    double y0 = get_center_y_2();
    double x0 = get_center_x_2();
    print_surface("test2_1_rho", OX_LEN, OY_LEN, HX, HY, TIME_STEP_CNT, A, C, x0, y0, TAU, U_VELOCITY,
                  V_VELOCITY, density);
    print_surface("test2_1_err", OX_LEN, OY_LEN, HX, HY, TIME_STEP_CNT, A, C, x0, y0, TAU, U_VELOCITY,
                  V_VELOCITY, err);
    print_surface("test2_1_exact", OX_LEN, OY_LEN, HX, HY, 0, A, C, x0, y0, TAU, U_VELOCITY,
                  V_VELOCITY, exact0);
    print_surface("test2_1_exact", OX_LEN, OY_LEN, HX, HY, TIME_STEP_CNT, A, C, x0, y0, TAU, U_VELOCITY,
                  V_VELOCITY, exactT);
    double l1 = get_l1_norm(HX, HY, OX_LEN_1, OY_LEN_1, err);
    double l_inf = get_l_inf_norm(OX_LEN_1, OY_LEN_1, err);
    printf("l1 %le\n", l1);
    printf("l_inf %le\n", l_inf);
    delete[] density;
    delete[] exact0;
    delete[] exactT;
    delete[] err;
}

TEST_F(FemFixture1, test2_2) {
    double tme = 0.;

    double d = 0;
    for (int i = 0; i < 4; ++i) {
        switch (i) {
            case 0:
                d = 50.;
                break;
            case 1:
                d = 100.;
                break;
            case 2:
                d = 200.;
                break;
            case 3:
                d = 400.;
                break;
            default:
                return;
        }

        A = 0.;
        B = 1.;
        C = 0.;
        D = 1.;
        R_SQ = 0.099 * 0.099;
        INN_DENSITY = 1.;
        OUT_DENSITY = 0.;

        OX_LEN = (int) d;
        OY_LEN = (int) d;
        OX_LEN_1 = OX_LEN + 1;
        OY_LEN_1 = OY_LEN + 1;
        HX = (B - A) / OX_LEN;
        HY = (D - C) / OY_LEN;

        U_VELOCITY = 1.;
        V_VELOCITY = 1.;
        TAU = 16. / pow(2., (i + 1));
        TAU *= 10e-3;
        TIME_STEP_CNT = (int) pow(2., i);
        XY_LEN = OX_LEN_1 * OY_LEN_1;

        printf("\nOX_LEN = %d OY_LEN = %d\n", OX_LEN, OY_LEN);
        printf("HX = %le\n", HX);
        printf("HY = %le\n", HY);
        printf("TAU = %le\n", TAU);
        printf("U = %le\n", U_VELOCITY);
        printf("V = %le\n", V_VELOCITY);
        printf("TIME_STEP_CNT = %d\n", TIME_STEP_CNT);

        double *density = solve_2(tme);
        double *err = calc_error_2(HX, HY, TAU * TIME_STEP_CNT, density);
        double x0 = get_center_x_2();
        double y0 = get_center_y_2();
        print_surface("test2_2_rho", OX_LEN, OY_LEN, HX, HY, TIME_STEP_CNT, A, C, x0, y0, TAU, U_VELOCITY,
                      V_VELOCITY, density);
        print_surface("test2_2_err", OX_LEN, OY_LEN, HX, HY, TIME_STEP_CNT, A, C, x0, y0, TAU, U_VELOCITY,
                      V_VELOCITY, err);
        double l1 = get_l1_norm(HX, HY, OX_LEN_1, OY_LEN_1, err);
        double l_inf = get_l_inf_norm(OX_LEN_1, OY_LEN_1, err);
        printf("l1 %le \n", l1);
        printf("l_inf %le\n", l_inf);
        delete[] density;
        delete[] err;
    }
}
