#define mpn_toomN_sqr mpn_toom4_sqr
#define mpn_toomN_sqr_itch mpn_toom4_sqr_itch
#define MIN_AN MAX(SQR_TOOM4_THRESHOLD,MPN_TOOM4_SQR_MINSIZE)

#include "toom-sqr-shared.h"
