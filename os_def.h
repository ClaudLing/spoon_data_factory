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
 *  此文件的详细描述...
 *
 * @{
 */

/* 注: mainpage对于多个文件生成一份chm时，这部分是可选的，请在上面的详细描述中填写完整内容 */
/*! \mainpage
 *
 *  模块说明:\n
 
 *  \section intro_sec 本模块提供的接口:
 *  - ::
 */

#ifndef __OS_DEF_H__ 
#define __OS_DEF_H__ 

/*******************************************************************************
 *   本文件需要包含的其它头文件
*******************************************************************************/




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

#ifdef __cplusplus
#if __cplusplus
}
#endif /*  __cpluscplus */
#endif /*  __cpluscplus */


#endif  /* __OS_DEF_H__ */

/** @}*/
