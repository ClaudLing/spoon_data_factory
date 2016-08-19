/*******************************************************************************
 *               Copyright (C) 2016, Huawei Tech. Co., Ltd.
 *                      ALL RIGHTS RESERVED
 *******************************************************************************/

/** @defgroup sdf.h

 *  @author  Claud
 *  @version 1.0
 *  @date    2016/8/18 \n

 *  history:\n
 *          1. 2016/8/18, Claud, create this file\n\n
 *
 *
 * @{
 */

#ifndef __SDF_H__ 
#define __SDF_H__ 


#include "os_def.h"


#define MAGIC_HEAD 0xDEADBEEF

#define USER_ID_SIZE 4
#define CPU_ID_SIZE 12

typedef struct INEMO_DATA
{
    s16 x;
    s16 y;
    s16 z;
    s16 r;
}INEMO_DATA;

typedef struct REC_DATA
{
    INEMO_DATA inemoX0;
    INEMO_DATA inemoG0;
    INEMO_DATA inemoM0;
    INEMO_DATA inemoX1;
    INEMO_DATA inemoG1;
    INEMO_DATA inemoM1;
    s32 angel0;
    s32 angel1;
}REC_DATA;


typedef struct rec_time {
  u8 tv_sec;			/* Seconds.	[0-60] (1 leap second) */
  u8 tv_min;			/* Minutes.	[0-59] */
  u8 tv_hour;			/* Hours.	[0-23] */
  u8 tv_mday;			/* Day.		[1-31] */
  u8 tv_mon;			/* Month.	[0-11] */
  u8 tv_year;			/* Year - 2000. */
}rec_time;

typedef struct
{
    u32 magic; // 魔鬼数字 0xDEADBEEF 4
    u16 crc; //CRC16，不包括自己和包头 2
    u16 dataType; // 数据类型 2
    u16 totalLength; // 包长度 2
    u16 packageIndex; // 包索引 2
    u8 userID[USER_ID_SIZE]; // 用户ID 4
    u8 cpuID[CPU_ID_SIZE]; // CPUID 12
    rec_time timeStamp; // 时间戳 6
    u16 read_flag; // 发送标识 0xff为未发送 其他为已发送 2
} SAVEDATA_HEADER_T;

#define SAVEDATA_HEADER_SIZE sizeof(SAVEDATA_HEADER_T)

typedef struct S_TRAS_PACKET
{
    u8 magic;
    u16 len;
    u8 type;
    u8 seqid;
    u32 frameid;
    u32 metaid;    
    void * data;
    u16 crc;
}S_TRAS_PACKET,*S_TRAS_PACKET_PTR;


/**
 *	Rom Map
 */
#define BootloaderAddress       	(0x08000000)
#define BootloaderSize          	(0x4000)
#define ApplicationAddress      	(0x08004000)       		// APP程序首地址
#define ApplicationSize        		(0x30000)          		// 程序预留空间


/**
 *	Spi-Flash Map
 */
#define FLASH_BLOCK_SIZE_DEFAULT 	(0x10000)  // 64KiB

#define IMG_IDX_0 					(0)
#define IMG_IDX_1 					(1)
#define IMG_IDX_MAX 				(2)

#define IMG_APP_HDR_SZ 				(32)
#define IMG_VER_OFF 				(0)
#define IMG_LEN_OFF 				(4)
#define IMG_INFO_OFF 				(8)
#define IMG_VER_CRC_OFF 			(IMG_APP_HDR_SZ - 2)

#define IMG_APP_SIZE_MAX 			(ApplicationSize - IMG_APP_HDR_SZ)

#define IMG0_HDR_START 				(0)
#define IMG0_APP_START 				(IMG0_HDR_START + IMG_APP_HDR_SZ)

#define IMG1_HDR_START 				(((int) (ApplicationSize) + (FLASH_BLOCK_SIZE_DEFAULT - 1)) & ~(FLASH_BLOCK_SIZE_DEFAULT - 1))
#define IMG1_APP_START 				(IMG1_HDR_START + IMG_APP_HDR_SZ)

#define LOG_OFF 					(IMG1_HDR_START*2)
#define LOG_SIZE 					(0x700000)

#define RSVD_OFF 					(LOG_OFF + LOG_SIZE)

/**
 *   Sensor parameter
 *
 */
#define INEMO_SAMPLE_RATE			(200)

/**
 *  RAW save parameter
 *
 */
#define RAW_SAVE_SIZE_ALIGN 		(512)
#define RAW_SAVE_INEMO_UNIT 		((RAW_SAVE_SIZE_ALIGN-SAVEDATA_HEADER_SIZE)/sizeof(REC_DATA))
#define RAW_SAVE_WINDOW_DEFAULT 	(30)




typedef struct date_time {
  s32 tv_sec;			/* Seconds.	[0-60] (1 leap second) */
  s32 tv_min;			/* Minutes.	[0-59] */
  s32 tv_hour;			/* Hours.	[0-23] */
  s32 tv_wday;			/* Day of week.	[0-6] */
  s32 tv_mday;			/* Day.		[1-31] */
  s32 tv_mon;			/* Month.	[0-11] */
  s32 tv_year;			/* Year - 2000. */  
}date_time;

#define CENTURY_BASE	2000

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /*  __cpluscplus */
#endif /*  __cpluscplus */


#ifdef __cplusplus
#if __cplusplus
}
#endif /*  __cpluscplus */
#endif /*  __cpluscplus */


#endif  /* __SDF_H__ */

/** @}*/
