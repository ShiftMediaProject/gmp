#define mpn_toomN_sqr mpn_toom6_sqr
#define mpn_toomN_sqr_itch mpn_toom6_sqr_itch
#define MIN_AN MAX(SQR_TOOM6_THRESHOLD,MPN_TOOM6_SQR_MINSIZE)

#include "toom-sqr-shared.h"
