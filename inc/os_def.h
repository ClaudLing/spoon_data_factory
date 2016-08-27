/*******************************************************************************
 *               Copyright (C) 2016, Huawei Tech. Co., Ltd.
 *                      ALL RIGHTS RESERVED
 *******************************************************************************/

/** @defgroup os_def.h

 *  @author  Claud
 *  @version 1.0
 *  @date    2016/8/19 \n

 *  history:\n
 *          1. 2016/8/19, Claud, create this file\n\n
 *
 *
 * @{
 */

#ifndef __OS_DEF_H__
#define __OS_DEF_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /*  __cpluscplus */
#endif /*  __cpluscplus */


typedef int   s32;
typedef short s16;
typedef char  s8;

typedef unsigned int    u32;
typedef unsigned short  u16;
typedef unsigned char   u8;

#define ROUND_UP(x, align) 				(((int) (x) + (align - 1)) & ~(align - 1))
#define ROUND_DOWN(x, align) 			((int)(x) & ~(align - 1))

#define os_htonl(n) \
    ((n & 0xff) << 24) \
    | ((n & 0xff00) << 8) \
    | ((n & 0xff0000UL) >> 8) \
    | ((n & 0xff000000UL) >> 24)


#ifdef __cplusplus
#if __cplusplus
}
#endif /*  __cpluscplus */
#endif /*  __cpluscplus */


#endif  /* __OS_DEF_H__ */

/** @}*/
