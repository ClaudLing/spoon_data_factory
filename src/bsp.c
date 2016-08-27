/*******************************************************************************
 *               Copyright (C) 2016, Huawei Tech. Co., Ltd.
 *                      ALL RIGHTS RESERVED
 *******************************************************************************/

/** @defgroup bsp.c

 *  @author  Claud
 *  @version 1.0
 *  @date    2016/8/26 \n
 *
 * @{
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "os_def.h"
#include "crc.h"
#include "bsp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /*  __cpluscplus */
#endif /*  __cpluscplus */

static u32 userid = 0;


s32 get_userid(u8 * buf,u8 len)
{
    s32 ret = 0;
    *(u32 *)buf = userid;
    return ret;
}

s32 set_userid(u32 id)
{
    s32 ret = 0;
    userid = id;
    return ret;
}

s32 get_chipid(u8 * buf, u8 len)
{
    u32 *chipid = NULL;

	chipid = (u32 *)buf;	
	chipid[0] = 0x77777777;
	chipid[1] = 0xFFFFFFFF;
    chipid[2] = userid;
    return 0;
}


static s32 gen_usart_msg(u8 cmd, 
                          u8 key, 
                          u8 *buf, 
                          u16 len, 
                          u8 type,
                          u8 *bufout,
                          u16 *lenout)
{      
    u8 hdr_buf[MSG_HDR_TOTAL_SIZE];
    u8 true_subhdr_buf[MSG_SUBHDR_TOTAL_SIZE];        /**< only for passthrough */
    u16 crc_iv = 0xFFFF;
    u8 *curprt = bufout;
    u16 curlen = 0;

    /** build subheader */
    buf[MSG_SUBHDR_CMD_OFF] = cmd;
    buf[MSG_SUBHDR_KEY_OFF] = key;

	/** build header */
    hdr_buf[MSG_HDR_MAGIC_OFF] = USARTMSG_MAGIC;
    if (PASSTHROUGH_MSG == type) {        
        true_subhdr_buf[MSG_SUBHDR_CMD_OFF] = PASSTHROUGH_MSG_CMDID;
        true_subhdr_buf[MSG_SUBHDR_KEY_OFF] = PASSTHROUGH_MSG_KEYID;
        crc_iv = com_crc(true_subhdr_buf, MSG_SUBHDR_TOTAL_SIZE);
        *((u16 *)&hdr_buf[MSG_HDR_LEN_OFF]) = len+2;
    }
    else {
        *((u16 *)&hdr_buf[MSG_HDR_LEN_OFF]) = len;
    }
    *(u16 *)&hdr_buf[MSG_HDR_DATACRC_OFF] = com_crc_iv(buf, len, crc_iv);

	memcpy(curprt, hdr_buf, MSG_HDR_TOTAL_SIZE);
    curprt += MSG_HDR_TOTAL_SIZE;
    curlen += MSG_HDR_TOTAL_SIZE;

    memcpy(curprt, true_subhdr_buf, MSG_SUBHDR_TOTAL_SIZE);
    curprt += MSG_SUBHDR_TOTAL_SIZE;
    curlen += MSG_SUBHDR_TOTAL_SIZE;

    memcpy(curprt, buf, len);
    curprt += len;
    curlen += len;
    
	*lenout = curlen;
    return 0;
}


s32 gen_basic_info_msg(u8 * buf, u16 *len)
{
    u8 reply[MSG_SUBHDR_TOTAL_SIZE + SEND_BASIC_INF_BODY_SIZE];
    u32 userid = 0;
    
    memset(reply, 0x00, sizeof(reply));

    /* Get chip ID and user ID */
	(void)get_chipid((u8 *)&reply[SEND_BASIC_INF_CPUID_OFF], SEND_BASIC_INF_CPUID_SIZE);
    (void)get_userid((u8 *)&userid, USER_ID_SIZE);
    userid = os_htonl(userid);
    *(u32 *)&reply[SEND_BASIC_INF_USERID_OFF] = userid;

	/* Get software version */
    *(u32 *)&reply[SEND_BASIC_INF_CUR_VER_OFF] = 0x01030000;
    *(u8 *)&reply[SEND_BASIC_INF_MODEL_OFF] = 0x00;
    *(u8 *)&reply[SEND_BASIC_INF_HWVER_OFF] = 0x00;
    *(u32 *)&reply[SEND_BASIC_INF_IMG0_VER_OFF] = 0x01030000;
    *(u32 *)&reply[SEND_BASIC_INF_IMG1_VER_OFF] = 0x01030000;

	/* Get Activation info */
    reply[SEND_BASIC_INF_STATE_OFF] = ACTIVATE;

    gen_usart_msg(CMD_PRIV_ID, KEY_BASICINF_REPLY, reply, sizeof(reply), PASSTHROUGH_MSG, buf, len);
    return 0;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif /*  __cpluscplus */
#endif /*  __cpluscplus */




/** @}*/
