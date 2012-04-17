#define mpn_toomN_sqr mpn_toom8_sqr
#define mpn_toomN_sqr_itch mpn_toom8_sqr_itch
#define MIN_AN MAX(SQR_TOOM8_THRESHOLD,MPN_TOOM8_SQR_MINSIZE)
#define MAX_AN (SQR_FFT_THRESHOLD * 2)

#include "toom-sqr-shared.h"
