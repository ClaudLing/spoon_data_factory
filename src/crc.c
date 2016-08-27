/*******************************************************************************
 *               Copyright (C) 2016, Huawei Tech. Co., Ltd.
 *                      ALL RIGHTS RESERVED
 *******************************************************************************/

/** @defgroup crc.c

 *  @author  Claud
 *  @version 1.0
 *  @date    2016/8/26 \n
 * @{
 */


#include "os_def.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /*  __cpluscplus */
#endif /*  __cpluscplus */

u16 CRCTALBES[] =
{
	0x0000, 0xCC01, 0xD801, 0x1400,
	0xF001, 0x3C00, 0x2800, 0xE401,
	0xA001, 0x6C00, 0x7800, 0xB401,
	0x5000, 0x9C01, 0x8801, 0x4400
};

static u16 crc_iv(u8 *msg, u32 length, u16 crc)
{
	u32 i;
	u8 chChar;

	for (i = 0; i < length; i++) {
		chChar = *msg++;
		crc  = CRCTALBES[(chChar ^ crc) & 15] ^ (crc >> 4);
		crc = CRCTALBES[((chChar >> 4) ^ crc) & 15] ^ (crc >> 4);
	}
	return crc;
}

u16 com_crc_iv(u8 *msg, u32 length, u16 crc)
{
	return crc_iv(msg, length, crc);
}

u16 com_crc(u8 *msg, u32 length)
{
    return crc_iv(msg, length, 0xffff);
}




#ifdef __cplusplus
#if __cplusplus
}
#endif /*  __cpluscplus */
#endif /*  __cpluscplus */




/** @}*/
