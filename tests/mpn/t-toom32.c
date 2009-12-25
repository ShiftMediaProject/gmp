#define mpn_toomMN_mul mpn_toom32_mul
#define mpn_toomMN_mul_itch mpn_toom32_mul_itch

#define MIN_AN 13
#define MIN_BN(an) (((an) + 16) / (size_t) 3)
#define MAX_BN(an) ((an) - 4)

#include "toom-shared.h"
