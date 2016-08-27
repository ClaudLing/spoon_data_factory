/*******************************************************************************
 *               Copyright (C) 2016, Huawei Tech. Co., Ltd.
 *                      ALL RIGHTS RESERVED
 *******************************************************************************/

/** @defgroup crc.h

 *  @author  Claud
 *  @version 1.0
 *  @date    2016/8/26 \n
 *
 * @{
 */


#ifndef __CRC_H__ 
#define __CRC_H__ 

#include "os_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /*  __cpluscplus */
#endif /*  __cpluscplus */



u16 com_crc_iv(u8 *msg, u32 length, u16 crc);
u16 com_crc(u8 *msg, u32 length);



#ifdef __cplusplus
#if __cplusplus
}
#endif /*  __cpluscplus */
#endif /*  __cpluscplus */


#endif  /* __CRC_H__ */

/** @}*/
