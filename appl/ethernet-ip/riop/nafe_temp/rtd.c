/* IEC 60751 Callendar-Van Dusen for Pt RTDs (alpha 0.00385). */
#include <math.h>
#include "dce_ng/temp_path.h"

#define CVD_A 3.9083e-3
#define CVD_B (-5.775e-7)
#define CVD_C (-4.183e-12)

double rtd_cvd_ohms(double t, double r0)
{
    double r = 1.0 + CVD_A * t + CVD_B * t * t;
    if (t < 0.0) r += CVD_C * (t - 100.0) * t * t * t;
    return r0 * r;
}

double rtd_cvd_temp_c(double r, double r0)
{
    double z = r / r0;
    /* Closed form valid for t >= 0 (quadratic) */
    double t = (-CVD_A + sqrt(CVD_A * CVD_A - 4.0 * CVD_B * (1.0 - z))) / (2.0 * CVD_B);
    if (z >= 1.0) return t;
    /* Below 0 C: Newton on the full equation, seeded by the quadratic */
    for (int i = 0; i < 20; i++) {
        double f = rtd_cvd_ohms(t, r0) - r;
        double d = r0 * (CVD_A + 2.0 * CVD_B * t + CVD_C * (4.0 * t * t * t - 300.0 * t * t));
        double dt = f / d;
        t -= dt;
        if (fabs(dt) < 1e-9) break;
    }
    return t;
}

double rtd_lead_compensate(temp_wiring_t w, double r_a, double r_b, double lead_ohms)
{
    switch (w) {
    case TEMP_WIRE_2: return r_a - lead_ohms;  /* lead_ohms = total loop lead resistance */
    case TEMP_WIRE_3: return r_a - r_b;        /* r_b = one lead, measured on the second sense pair */
    case TEMP_WIRE_4:
    default: return r_a;                       /* sense pair carries ~nA; leads drop out */
    }
}
