#define M 4
#define N 2

#define mpn_toomMN_mul mpn_toom42_mul
#define mpn_toomMN_mul_itch mpn_toom42_mul_itch
#define mpn_toomMN_mul_ok(an, bn)		\
  (an >= 10 && 3*(bn) + 5 <= 2*(an) && (an) + 7 <= 4 * (bn))

#define MIN_BLOCK 4
#define COUNT 2000

#include "toom-shared.h"
