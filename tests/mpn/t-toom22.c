#define M 2
#define N 2

#define mpn_toomMN_mul mpn_toom22_mul
#define mpn_toomMN_mul_itch mpn_toom22_mul_itch
#define mpn_toomMN_mul_ok(an, bn)				\
  ((bn) <= (an)							\
   && (an) - (bn) + 2 < MIN((bn), MUL_KARATSUBA_THRESHOLD))

#include "toom-shared.h"
