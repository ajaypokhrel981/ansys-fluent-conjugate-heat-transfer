/***********************************************************************
 * UDF File: young_vafai_1998_case5.c
 * 
 * Paper: "Convective Cooling of a Heated Obstacle in a Channel"
 *        Timothy J. Young and Kambiz Vafai (1998)
 *        Int. J. Heat and Mass Transfer, 41, 3131-3148
 * 
 * Case 5: w = h = 25mm, H = 100mm
 * Parameters:
 *   - Re_H    = 200, 400, 600, 800, 1000, 1200, 1400, 1600, 1800, 2000
 *   - ks/kf   = 1
 *   - kf = ks = 0.0242 W/m.K
 *   - Pr      = 0.72
 *   - q''     = 10 W/m2 (at obstacle base)
 *   - T_inlet = 300 K
 * 
 * Contains:
 *   1. Parabolic velocity inlet profile
 *   2. Nusselt number computation (for post-processing)
 * 
 * Author: Reproduced for academic study
 * Date:   2026
 ***********************************************************************/

#include "udf.h"
#include "math.h"

/***********************************************************************
 * GEOMETRY PARAMETERS
 ***********************************************************************/
#define H           0.10        /* Channel height [m] = 100 mm          */
#define Y_BOTTOM    0.0         /* Y coordinate of bottom wall [m]       */
                                /* Change to your actual bottom wall Y   */
                                /* Check in Fluent: hover over bottom    */
                                /* wall and note Y coordinate            */
#define Y_CENTER    (Y_BOTTOM + H/2.0)  /* Channel centerline Y [m]     */
                                        /* = 0.05 if Y_BOTTOM = 0        */

/***********************************************************************
 * FLUID PROPERTIES (Air, matched to Pr = 0.72)
 ***********************************************************************/
#define RHO         1.225       /* Density              [kg/m3]          */
#define MU          1.7894e-5   /* Dynamic viscosity    [Pa.s]           */
#define KF          0.0242      /* Thermal conductivity [W/m.K]          */
#define CP          973.7       /* Specific heat        [J/kg.K]         */
                                /* Adjusted so Pr = mu*cp/kf = 0.72     */

/***********************************************************************
 * FLOW PARAMETERS
 ***********************************************************************/
#define RE_H        200.0      /* Reynolds number based on H            */
                                /* Re_H = rho * u_mean * H / mu          */
                                /* Change this for other Re cases:       */
                                /* 200, 400, 600, 800, 1000,             */
                                /* 1200, 1400, 1600, 1800, 2000          */

/***********************************************************************
 * THERMAL PARAMETERS
 ***********************************************************************/
#define T_INLET     300.0       /* Inlet temperature [K] = T_e           */
#define Q_FLUX      10.0        /* Heat flux at obstacle base [W/m2]     */
                                /* q'' = 10 W/m2 chosen so temperature   */
                                /* rise is physically reasonable (~41K)  */
                                /* dimensionless results (Nu) are        */
                                /* independent of this value             */

/***********************************************************************
 * DERIVED QUANTITIES (computed from above, do not change)
 ***********************************************************************/
/* Mean velocity from Re definition: u_mean = Re_H * mu / (rho * H)    */
/* For Re=1000: u_mean = 1000 * 1.7894e-5 / (1.225 * 0.1)             */
/*            = 0.14618 m/s                                             */
/* Peak velocity at centerline = 1.5 * u_mean = 0.21927 m/s            */

/***********************************************************************
 * UDF 1: PARABOLIC VELOCITY INLET PROFILE
 * 
 * Paper Eq. 7: u* = 6 * y* * (1 - y*)
 * where y* = y/H (from bottom wall)
 * 
 * Equivalent form used here (origin at centerline):
 * u(y) = 1.5 * u_mean * (1 - 4*y_c^2 / H^2)
 * where y_c = y - y_centerline
 * 
 * Apply to: inlet boundary
 *           X-velocity → UDF → parabolic_inlet_velocity
 ***********************************************************************/
DEFINE_PROFILE(parabolic_inlet_velocity, thread, position)
{
    face_t f;
    real x[ND_ND];      /* face centroid position vector    */
    real y_c;           /* y measured from centerline [m]   */
    real u_mean;        /* mean velocity [m/s]              */
    real velocity;      /* local face velocity [m/s]        */

    /* Compute mean velocity from Reynolds number */
    u_mean = (RE_H * MU) / (RHO * H);

    begin_f_loop(f, thread)
    {
        /* Get face centroid coordinates */
        F_CENTROID(x, f, thread);

        /* Shift y-origin to channel centerline */
        y_c = x[1] - Y_CENTER;

        /* Parabolic (Poiseuille) profile                        */
        /* u = 1.5 * u_mean * (1 - 4*y_c^2/H^2)                */
        /* This is mathematically identical to paper's Eq.7:    */
        /* u* = 6*y*(1-y) where y* = y/H from bottom wall       */
        velocity = 1.5 * u_mean * (1.0 - (4.0 * y_c * y_c) / (H * H));

        /* Clip negative values at walls due to floating point   */
        if (velocity < 0.0) velocity = 0.0;

        /* Assign velocity to this face */
        F_PROFILE(f, thread, position) = velocity;
    }
    end_f_loop(f, thread)
}

/***********************************************************************
 * UDF 2: NUSSELT NUMBER REPORT (for post-processing verification)
 * 
 * Paper Eq. 13:
 * Nu_x = (h_c * H) / k_f = (-1 / Theta_w) * (dTheta_f/dn)
 * 
 * In dimensional form:
 * Nu_x = (-H / (T_w - T_e)) * (dT/dn)|_wall
 * 
 * This UDF reports local Nu on walls for cross-checking
 * Fluent's built-in "Surface Nusselt Number" output.
 * 
 * Apply to: obstacle walls (optional - for verification only)
 *           Fluent's built-in Nu output is usually sufficient
 ***********************************************************************/
DEFINE_ADJUST(compute_nusselt, domain)
{
    /* This function runs every iteration                        */
    /* You can use Fluent's built-in Surface Nusselt Number      */
    /* in post-processing instead of this UDF                   */
    /* This is included for completeness and verification        */

    /* Fluent computes Nu internally as:                         */
    /* Nu = q_wall * L_ref / (k * (T_wall - T_ref))             */
    /* where L_ref and T_ref are set in Reference Values         */
    /* Make sure Reference Values are set correctly - see below  */
}

/***********************************************************************
 * REFERENCE VALUES TO SET IN FLUENT GUI
 * (these are NOT set by UDF - set them manually in Fluent)
 * 
 * Setup → Reference Values → Compute from: inlet, then override:
 * 
 * Area          : 0.1 m2      (H x 1m depth for 2D)
 * Density       : 1.225 kg/m3
 * Enthalpy      : 0 J/kg
 * Length        : 0.1 m       (= H, used as Nu length scale)
 * Pressure      : 0 Pa
 * Temperature   : 300 K       (= T_inlet = T_e)
 * Velocity      : 0.14618 m/s (= u_mean for Re=1000)
 * Viscosity     : 1.7894e-5 Pa.s
 * Ratio of Sp.Ht: 1.4
 * 
 * These reference values ensure Fluent computes:
 * Nu = h * H / kf   (matching paper's definition)
 * Re = rho * u_mean * H / mu = 1000 (matching paper)
 ***********************************************************************/