#define mpn_toomN_sqr mpn_toom8_sqr
#define mpn_toomN_sqr_itch mpn_toom8_sqr_itch
#define MIN_AN MAX(SQR_TOOM8_THRESHOLD,MPN_TOOM8_SQR_MINSIZE)

#include "toom-sqr-shared.h"
