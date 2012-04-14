#define mpn_toomN_sqr mpn_toom3_sqr
#define mpn_toomN_sqr_itch mpn_toom3_sqr_itch
#define MIN_AN MAX(SQR_TOOM3_THRESHOLD,MPN_TOOM3_SQR_MINSIZE)

#include "toom-sqr-shared.h"
