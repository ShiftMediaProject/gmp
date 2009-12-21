#define mpn_toomMN_mul mpn_toom6h_mul
#define mpn_toomMN_mul_itch mpn_toom6h_mul_itch

#define MIN_AN 46
#define MIN_BN(an) (MAX ((an*3)>>3, 42) )

#include "toom-shared.h"
