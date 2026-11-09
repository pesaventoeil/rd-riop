/* NIST ITS-90 thermocouple reference functions (NIST Monograph 175), types K, J, T.
 * E/N/R/S/B: TODO (same structure, coefficients not yet entered). */
#include <math.h>
#include "dce_ng/temp_path.h"

/* Compact exp() for the type-K exponential term (argument range about [-185, 0]). Avoids pulling
 * libm exp + its 2 KB table into the RIOP M7 image, whose ITCM is full. Range reduction by ln2,
 * degree-12 Taylor on |r| <= ln2/2, exponent built directly. Relative error < 1e-15 (host test). */
double tp_exp(double x)
{
    if (x < -700.0) return 0.0;
    if (x > 700.0) x = 700.0;
    const double ln2 = 0.69314718055994530942;
    double kf = x / ln2;
    int k = (int)(kf < 0 ? kf - 0.5 : kf + 0.5);
    double r = (x - (double)k * 6.93147180369123816490e-01) - (double)k * 1.90821492927058770002e-10; /* Cody-Waite ln2 hi/lo */
    double term = 1.0, sum = 1.0;
    for (int n = 1; n <= 12; n++) { term *= r / (double)n; sum += term; }
    union { double d; unsigned long long u; } sc;
    sc.u = (unsigned long long)(k + 1023) << 52;
    return sum * sc.d;
}

static double poly(const double *c, int n, double x)
{
    double y = 0.0;
    for (int i = n - 1; i >= 0; i--) y = y * x + c[i];
    return y;
}

/* Type K forward */
static const double K_NEG[] = {0.0, 0.394501280250E-01, 0.236223735980E-04, -0.328589067840E-06,
    -0.499048287770E-08, -0.675090591730E-10, -0.574103274280E-12, -0.310888728940E-14,
    -0.104516093650E-16, -0.198892668780E-19, -0.163226974860E-22};
static const double K_POS[] = {-0.176004136860E-01, 0.389212049750E-01, 0.185587700320E-04,
    -0.994575928740E-07, 0.318409457190E-09, -0.560728448890E-12, 0.560750590590E-15,
    -0.320207200030E-18, 0.971511471520E-22, -0.121047212750E-25};
/* Type K inverse */
static const double KI0[] = {0.0, 2.5173462E+01, -1.1662878E+00, -1.0833638E+00, -8.9773540E-01,
    -3.7342377E-01, -8.6632643E-02, -1.0450598E-02, -5.1920577E-04};
static const double KI1[] = {0.0, 2.508355E+01, 7.860106E-02, -2.503131E-01, 8.315270E-02,
    -1.228034E-02, 9.804036E-04, -4.413030E-05, 1.057734E-06, -1.052755E-08};
static const double KI2[] = {-1.318058E+02, 4.830222E+01, -1.646031E+00, 5.464731E-02,
    -9.650715E-04, 8.802193E-06, -3.110810E-08};
/* Type J */
static const double J_NEG[] = {0.0, 0.503811878150E-01, 0.304758369300E-04, -0.856810657200E-07,
    0.132281952950E-09, -0.170529583370E-12, 0.209480906970E-15, -0.125383953360E-18,
    0.156317256970E-22};
static const double JI0[] = {0.0, 1.9528268E+01, -1.2286185E+00, -1.0752178E+00, -5.9086933E-01,
    -1.7256713E-01, -2.8131513E-02, -2.3963370E-03, -8.3823321E-05};
static const double JI1[] = {0.0, 1.978425E+01, -2.001204E-01, 1.036969E-02, -2.549687E-04,
    3.585153E-06, -5.344285E-08, 5.099890E-10};
/* Type T */
static const double T_NEG[] = {0.0, 0.387481063640E-01, 0.441944343470E-04, 0.118443231050E-06,
    0.200329735540E-07, 0.901380195590E-09, 0.226511565930E-10, 0.360711542050E-12,
    0.384939398830E-14, 0.282135219250E-16, 0.142515947790E-18, 0.487686622860E-21,
    0.107955392700E-23, 0.139450270620E-26, 0.797951539270E-30};
static const double T_POS[] = {0.0, 0.387481063640E-01, 0.332922278800E-04, 0.206182434040E-06,
    -0.218822568460E-08, 0.109968809280E-10, -0.308157587720E-13, 0.454791352900E-16,
    -0.275129016730E-19};
static const double TI0[] = {0.0, 2.5949192E+01, -2.1316967E-01, 7.9018692E-01, 4.2527777E-01,
    1.3304473E-01, 2.0241446E-02, 1.2668171E-03};
static const double TI1[] = {0.0, 2.592800E+01, -7.602961E-01, 4.637791E-02, -2.165394E-03,
    6.048144E-05, -7.293422E-07};

#define N(a) ((int)(sizeof(a) / sizeof((a)[0])))

int tc_temp_to_mv(tc_type_t t, double tc, double *mv)
{
    switch (t) {
    case TC_K:
        if (tc < -270.0 || tc > 1372.0) return -1;
        if (tc < 0.0) *mv = poly(K_NEG, N(K_NEG), tc);
        else {
            double e = tc - 126.9686;
            *mv = poly(K_POS, N(K_POS), tc) + 0.1185976 * tp_exp(-1.183432E-04 * e * e);
        }
        return 0;
    case TC_J:
        if (tc < -210.0 || tc > 760.0) return -1;
        *mv = poly(J_NEG, N(J_NEG), tc);
        return 0;
    case TC_T:
        if (tc < -270.0 || tc > 400.0) return -1;
        *mv = tc < 0.0 ? poly(T_NEG, N(T_NEG), tc) : poly(T_POS, N(T_POS), tc);
        return 0;
    }
    return -1;
}

int tc_mv_to_temp(tc_type_t t, double mv, double *tc)
{
    switch (t) {
    case TC_K:
        if (mv < -5.892 || mv > 54.887) return -1;
        *tc = mv < 0.0 ? poly(KI0, N(KI0), mv) : mv < 20.644 ? poly(KI1, N(KI1), mv) : poly(KI2, N(KI2), mv);
        return 0;
    case TC_J:
        if (mv < -8.096 || mv > 42.920) return -1;
        *tc = mv < 0.0 ? poly(JI0, N(JI0), mv) : poly(JI1, N(JI1), mv);
        return 0;
    case TC_T:
        if (mv < -5.604 || mv > 20.873) return -1;
        *tc = mv < 0.0 ? poly(TI0, N(TI0), mv) : poly(TI1, N(TI1), mv);
        return 0;
    }
    return -1;
}

int tc_compensated_temp(tc_type_t t, double meas_mv, double cjc_c, double *tc)
{
    double cj_mv;
    if (tc_temp_to_mv(t, cjc_c, &cj_mv)) return -1;
    return tc_mv_to_temp(t, meas_mv + cj_mv, tc);
}
