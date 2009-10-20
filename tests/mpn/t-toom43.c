#define M 4
#define N 3

#define mpn_toomMN_mul mpn_toom43_mul
#define mpn_toomMN_mul_itch mpn_toom43_mul_itch

/* an >= 25 || bn >= 19 imply s + t >= 5, which is required
 * by a simplified toom43 function. */
#define mpn_toomMN_mul_ok(an, bn)			\
  ( (an >= 25 || bn >= 19)				\
    && (bn) + 3 <= (an) && 2*(an) + 10 <= 4 * (bn))

#define MIN_BLOCK 7
#define COUNT 2000

#include "toom-shared.h"
