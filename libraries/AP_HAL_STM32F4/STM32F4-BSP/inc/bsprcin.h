#ifndef _BSPRCIN_H_
#define _BSPRCIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define CHx_VALID(i) ((rc_data_ready & (uint32_t)1<<(i)) != 0)
#define SET_CHx_VALID(i) rc_data_ready |= (uint32_t)1<<(i)
#define SET_CHx_INVALID(i) rc_data_ready &= (~((uint32_t)1<<(i)))
#define SET_ALL_CH_VALID()	rc_data_ready = 0x0003ffff

#define BSP_MAX_CH_NUM	18

typedef enum {
		RC_CH_1 = (uint8_t)0,
		RC_CH_2,
		RC_CH_3,
		RC_CH_4,
		RC_CH_5,
		RC_CH_6,
		RC_CH_7,
		RC_CH_8,
		RC_CH_9,
		RC_CH_10,
		RC_CH_11,
		RC_CH_12,
		RC_CH_13,
		RC_CH_14,
		RC_CH_15,
		RC_CH_16,
		RC_CH_17,
		RC_CH_18
}rc_channel_t;

void bspRcinBegin(void); 
void bspRcinEnd(void);
uint8_t bspRcinDataReady(void);
uint16_t bspParseSbusData(uint8_t ch);

#ifdef __cplusplus
}
#endif

#endif

