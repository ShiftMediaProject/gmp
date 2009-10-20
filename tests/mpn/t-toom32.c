#define M 3
#define N 2

#define mpn_toomMN_mul mpn_toom32_mul
#define mpn_toomMN_mul_itch mpn_toom32_mul_itch
#define mpn_toomMN_mul_ok(an, bn)		\
  ((bn) + 2 <= (an) && (an) + 5 <= 3 * (bn))

#include "toom-shared.h"
