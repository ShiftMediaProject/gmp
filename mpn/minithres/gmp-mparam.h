#define MUL_KARATSUBA_THRESHOLD           2
#define MUL_TOOM3_THRESHOLD              17

#define SQR_BASECASE_THRESHOLD            0
#define SQR_KARATSUBA_THRESHOLD           2
#define SQR_TOOM3_THRESHOLD              17

#define MULLOW_BASECASE_THRESHOLD         0
#define MULLOW_DC_THRESHOLD               2
#define MULLOW_MUL_N_THRESHOLD            4

#define DIV_SB_PREINV_THRESHOLD           0  /* always */
#define DIV_DC_THRESHOLD                  6
#define POWM_THRESHOLD                    4

#define HGCD_SCHOENHAGE_THRESHOLD        10
#define GCD_ACCEL_THRESHOLD               2
#define GCD_SCHOENHAGE_THRESHOLD         20
#define GCDEXT_SCHOENHAGE_THRESHOLD      20
#define JACOBI_BASE_METHOD                1

#define MOD_1_NORM_THRESHOLD              0  /* always */
#define MOD_1_UNNORM_THRESHOLD            0  /* always */
#define USE_PREINV_DIVREM_1               1  /* native */
#define USE_PREINV_MOD_1                  1
#define DIVREM_2_THRESHOLD                0  /* always */
#define DIVEXACT_1_THRESHOLD              0  /* always (native) */
#define MODEXACT_1_ODD_THRESHOLD          0  /* always (native) */

#define GET_STR_DC_THRESHOLD              2
#define GET_STR_PRECOMPUTE_THRESHOLD      4
#define SET_STR_THRESHOLD                64

#define MUL_FFT_TABLE  { 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 0}
#define MUL_FFT_MODF_THRESHOLD 32
#define MUL_FFT_THRESHOLD     128

#define SQR_FFT_TABLE  { 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 0}
#define SQR_FFT_MODF_THRESHOLD 32
#define SQR_FFT_THRESHOLD     128
