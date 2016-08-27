
/*******************************************************************************
 *               Copyright (C) 2016, Huawei Tech. Co., Ltd.
 *                      ALL RIGHTS RESERVED
 *******************************************************************************/

/** @defgroup bsp.h

 *  @author  Claud
 *  @version 1.0
 *  @date    2016/8/26 \n

 *
 * @{
 */

#ifndef __BSP_H__ 
#define __BSP_H__ 


#include "os_def.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /*  __cpluscplus */
#endif /*  __cpluscplus */

#define MAGIC_HEAD 0xDEADBEEF

#define USER_ID_SIZE 4
#define CPU_ID_SIZE 12

#define KEY_UNKNOW_ID				0xFF

typedef enum {
	CMD_SET_CONFIG_ID          		= 0x02,
    CMD_BOND_COMMAND_ID 			= 0x03,
    CMD_FACTORY_COMMAND_ID 			= 0x06,
    CMD_PRIV_ID						= 0x77,
    CMD_PRIVTEST_ID					= 0xFE,
	CMD_UNKNOW_ID					= 0xFF
}CMDID_RECEIVE_E;

typedef enum {
    KEY_SET_TIME        			= 0x01,
    KEY_FACTORYRESET          		= 0x08,
	KEY_REBOOT          			= 0x09
}SETCFG_RECEIVE_E; /* 0x02 */

typedef enum {
    KEY_SENSOR_DATA_GET				= 0x08,
    KEY_SENSOR_DATA_SEND			= 0x09,
    KEY_SENSOR_CALI_REQ				= 0x0A
}FCTMSG_RECEIVE_E; /* 0x06 */	

typedef enum {
	KEY_BASICINF_QUREY         		= 0x80,
	KEY_BASICINF_REPLY         		= 0x81,
    KEY_SELFTEST					= 0x82
}PRIVCMD_RECEIVE_E; /* 0x77 */

typedef enum {
    KEYDBG_LED_CTR        	 		= 0x50,
    KEYDBG_STORAGE_EMU     			= 0x51,
    KEYDBG_BATTERY_VOL_GET 			= 0x52,
    KEYDBG_BATTERY_VOL_SEND			= 0x53
}DBGMSG_RECEIVE_E; /* 0xFE */	

typedef enum {
	KEY_ACTIVATE        			= 0x05,
    KEY_ACTIVATE_REPLY        		= 0x06,
}BONDCMD_RECEIVE_E; /* 0x03 */

#define USARTMSG_MAGIC 							0x7E
#define USARTMSG_MAX_LEN 						64

/* header */
#define MSG_HDR_MAGIC_OFF						0
#define MSG_HDR_MAGIC_SIZE						1
#define MSG_HDR_LEN_OFF							(MSG_HDR_MAGIC_OFF+MSG_HDR_MAGIC_SIZE)
#define MSG_HDR_LEN_SIZE						2
#define MSG_HDR_DATACRC_OFF						(MSG_HDR_LEN_OFF+MSG_HDR_LEN_SIZE)
#define MSG_HDR_DATACRC_SIZE					2
#define MSG_HDR_TOTAL_SIZE						(MSG_HDR_MAGIC_SIZE+MSG_HDR_LEN_SIZE+MSG_HDR_DATACRC_SIZE)

/* subheader */
#define MSG_SUBHDR_CMD_OFF						0
#define MSG_SUBHDR_CMD_SIZE						1
#define MSG_SUBHDR_KEY_OFF						(MSG_SUBHDR_CMD_OFF+MSG_SUBHDR_CMD_SIZE)
#define MSG_SUBHDR_KEY_SIZE						1
#define MSG_SUBHDR_TOTAL_SIZE					(MSG_SUBHDR_CMD_SIZE+MSG_SUBHDR_KEY_SIZE)


/* basic info reply msg */
#define SEND_BASIC_INF_CPUID_OFF				(0+MSG_SUBHDR_TOTAL_SIZE)
#define SEND_BASIC_INF_CPUID_SIZE				(CPU_ID_SIZE)
#define SEND_BASIC_INF_USERID_OFF				(SEND_BASIC_INF_CPUID_OFF+SEND_BASIC_INF_CPUID_SIZE)
#define SEND_BASIC_INF_USERID_SIZE				(USER_ID_SIZE)
#define SEND_BASIC_INF_CUR_VER_OFF				(SEND_BASIC_INF_USERID_OFF+SEND_BASIC_INF_USERID_SIZE)
#define SEND_BASIC_INF_CUR_VER_SIZE				4
#define SEND_BASIC_INF_MODEL_OFF				(SEND_BASIC_INF_CUR_VER_OFF+SEND_BASIC_INF_CUR_VER_SIZE)
#define SEND_BASIC_INF_MODEL_SIZE				1
#define SEND_BASIC_INF_HWVER_OFF				(SEND_BASIC_INF_MODEL_OFF+SEND_BASIC_INF_MODEL_SIZE)
#define SEND_BASIC_INF_HWVER_SIZE				1
#define SEND_BASIC_INF_OTHER_OFF				(SEND_BASIC_INF_HWVER_OFF+SEND_BASIC_INF_HWVER_SIZE)
#define SEND_BASIC_INF_OTHER_SIZE				2
#define SEND_BASIC_INF_IMG0_VER_OFF				(SEND_BASIC_INF_OTHER_OFF+SEND_BASIC_INF_OTHER_SIZE)
#define SEND_BASIC_INF_IMG0_VER_SIZE			4
#define SEND_BASIC_INF_IMG1_VER_OFF				(SEND_BASIC_INF_IMG0_VER_OFF+SEND_BASIC_INF_IMG0_VER_SIZE)
#define SEND_BASIC_INF_IMG1_VER_SIZE			4
#define SEND_BASIC_INF_STATE_OFF				(SEND_BASIC_INF_IMG1_VER_OFF+SEND_BASIC_INF_IMG1_VER_SIZE)
#define SEND_BASIC_INF_STATE_SIZE				1
#define SEND_BASIC_INF_RVD_OFF					(SEND_BASIC_INF_STATE_OFF+SEND_BASIC_INF_STATE_SIZE)
#define SEND_BASIC_INF_RVD_SIZE					1
#define SEND_BASIC_INF_BODY_SIZE				(SEND_BASIC_INF_CPUID_SIZE+SEND_BASIC_INF_USERID_SIZE \
                                                 +SEND_BASIC_INF_CUR_VER_SIZE+SEND_BASIC_INF_MODEL_SIZE \
                                                 +SEND_BASIC_INF_HWVER_SIZE+SEND_BASIC_INF_OTHER_SIZE \
                                                 +SEND_BASIC_INF_IMG0_VER_SIZE+SEND_BASIC_INF_IMG1_VER_SIZE \
                                                 +SEND_BASIC_INF_STATE_SIZE+SEND_BASIC_INF_RVD_SIZE)

#define NOTACTIVATE								0
#define ACTIVATE								1

#define NORMAL_MSG								0
#define PASSTHROUGH_MSG                         1


#define PASSTHROUGH_MSG_CMDID       0xFE
#define PASSTHROUGH_MSG_KEYID       0xFE

#define MSG_MAX_LEN 		(256)


typedef struct TAG_MODEL_INFO
{
    unsigned int version;
    unsigned char model;
	unsigned char hwver;
}MODEL_INFO,*MODEL_INFO_PTR;

s32 get_userid(u8 * buf,u8 len);
s32 set_userid(u32 id);
s32 get_chipid(u8 * buf, u8 len);
s32 gen_basic_info_msg(u8 * buf, u16 *len);


#ifdef __cplusplus
#if __cplusplus
}
#endif /*  __cpluscplus */
#endif /*  __cpluscplus */


#endif  /* __BSP_H__ */

/** @}*/
