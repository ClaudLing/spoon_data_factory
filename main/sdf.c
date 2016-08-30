/*******************************************************************************
 *               Copyright (C) 2016, Huawei Tech. Co., Ltd.
 *                      ALL RIGHTS RESERVED
 *******************************************************************************/

/** @defgroup sdf.c

 *  @author  Claud
 *  @version 1.0
 *  @date    2016/8/18 \n
 *
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include "os_def.h"
#include "bsp.h"
#include "crc.h"
#include "sdf.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /*  __cpluscplus */
#endif /*  __cpluscplus */


#define MOD_NAME 								"SDF"
#define MOD_TRACE								1

#define MOD_DEBUG_IN 							0x0001
#define MOD_DEBUG_OUT 							0x0002
#define MOD_DEBUG_LOG 							0x0010
#define MOD_DEBUG_INF 							0x0020
#define MOD_DEBUG_DBG 							0x0040
#define MOD_DEBUG_WRN	 						0x0100
#define MOD_DEBUG_ERR 							0x0200
#define MOD_DEBUG_ALL 							0xFFFF

#if MOD_TRACE
#define MOD_FLAGS 								(MOD_DEBUG_DBG | MOD_DEBUG_WRN | MOD_DEBUG_ERR)
#define MOD_PRINT(flag, fmt, ...) \
        do{ \
              if (MOD_FLAGS & flag){ \
                  printf(fmt, ##__VA_ARGS__);} \
        } while(0)
#else
#define MOD_PRINT(fmt, ...)
#endif


#define WAVE_RATE 4
#define WAVE_PERIODS_IN_SEC (INEMO_SAMPLE_RATE/WAVE_RATE)


/** About date and time */
#define BEIJINGTIME 8
#define FEBRUARY		2
#define	STARTOFTIME		1970
#define SECDAY			86400L
#define SECYR			(SECDAY * 365)
#define	leapyear(year)		((year) % 4 == 0)
#define	days_in_year(a) 	(leapyear(a) ? 366 : 365)

#define SEND_SIZE  16

#define TRANS_HDR_MAGIC_OFF 			0
#define TRANS_HDR_MAGIC_SIZE 			1
#define TRANS_HDR_LEN_OFF 				(TRANS_HDR_MAGIC_OFF+TRANS_HDR_MAGIC_SIZE)
#define TRANS_HDR_LEN_SIZE 				2
#define TRANS_HDR_TYPE_OFF 				(TRANS_HDR_LEN_OFF+TRANS_HDR_LEN_SIZE)
#define TRANS_HDR_TYPE_SIZE				1
#define TRANS_HDR_SEQID_OFF 			(TRANS_HDR_TYPE_OFF+TRANS_HDR_TYPE_SIZE)
#define TRANS_HDR_SEQID_SIZE			1
#define TRANS_HDR_FRAMEID_OFF 			(TRANS_HDR_SEQID_OFF+TRANS_HDR_SEQID_SIZE)
#define TRANS_HDR_FRAMEID_SIZE			4
#define TRANS_HDR_METAID_OFF 			(TRANS_HDR_FRAMEID_OFF+TRANS_HDR_FRAMEID_SIZE)
#define TRANS_HDR_METAID_SIZE			4

#define TRANS_HDR_HDR_SIZE				(TRANS_HDR_MAGIC_SIZE+TRANS_HDR_LEN_SIZE+TRANS_HDR_TYPE_SIZE\
    									+TRANS_HDR_SEQID_SIZE+TRANS_HDR_FRAMEID_SIZE+TRANS_HDR_METAID_SIZE)

#define MAX_CONNECT_TIMES 				5

#define MAX_REC_NUM						20

typedef struct tagS_REC_STAT
{
    u32 userid;
    rec_time time;
    u8 chipid[CPU_ID_SIZE];
    u32 size;
    u32 pages;
    s32 stat; /* 0: Complete 1: Incomplete 2: Damage */
}S_REC_STAT,*S_REC_STAT_PTR;

s32 math_table[] = {
/*度     sin    cos    */
0, 0, 10000,
1, 175,  9998,
2, 349,  9994,
3, 523,  9986,
4, 698,  9976,
5, 872,  9962,
6, 1045,  9945,
7, 1219,  9925,
8, 1392,  9903,
9, 1564,  9877,
10, 1736,  9848,
11, 1908,  9816,
12, 2079,  9781,
13, 2250,  9744,
14, 2419,  9703,
15, 2588,  9659,
16, 2756,  9613,
17, 2924,  9563,
18, 3090, 9511,
19, 3256,  9455,
20, 3420,  9397,
21, 3584,  9336,
22,  3746,  9272,
23,  3907,  9205,
24,  4067,  9135,
25,  4226,  9063,
26,  4384,  8988,
27,  4540,  8910,
28,  4695,  8829,
29,  4848,  8746,
30,  5000,  8660,
31,  5150,  8572,
32,  5299,  8480,
33,  5446,  8387,
34,  5592,  8290,
35,  5736,  8192,
36, 5878,  8090,
37, 6018,  7986,
38, 6157,  7880,
39, 6293,  7771,
40, 6428,  7660,
41, 6561,  7547,
42, 6691,  7431,
43, 6820,  7314,
44, 6947,  7193,
45, 7071,  7071,
46, 7193,  6947,
47, 7314,  6820,
48, 7431,  6691,
49, 7547,  6561,
50, 7660,  6428,
51, 7771,  6293,
52, 7880,  6157,
53, 7986,  6018,
54, 8090,  5878,
55, 8192,  5736,
56, 8290,  5592,
57, 8387,  5446,
58, 8480,  5299,
59, 8572,  5150,
60, 8660,  5000,
61, 8746,  4848,
62, 8829,  4695,
63, 8910,  4540,
64, 8988,  4384,
65, 9063,  4226,
66, 9135,  4067,
67, 9205,  3907,
68, 9272,  3746,
69, 9336,  3584,
70, 9397,  3420,
71, 9455,  3256,
72, 9511,  3090,
73, 9563,  2924,
74, 9613,  2756,
75, 9659,  2588,
76, 9703,  2419,
77, 9744,  2250,
78, 9781,  2079,
79, 9816,  1908,
80, 9848,  1736,
81, 9877,  1564,
82, 9903,  1392,
83, 9925,  1219,
84, 9945,  1045,
85, 9962,  872,
86, 9976,  698,
87, 9986,  523,
88, 9994,  349,
89, 9998,  175,
90, 10000,  0,
};


static u32 dbg = 0;
static u32 raw_save_window = 0;
static u32 raw_save_limit = 0;
static u32 raw_save_count = 0;
static SAVEDATA_HEADER_T *save_header;
static u32 rec_num = 1;
static u32 rec_interval_in_ms = 5;
static u32 endless = 0;
static u32 delay_in_ms = 0;
static u32 bytes_on_send = 0;
static s8 *chipidstr = NULL;
static u32 mix = 1;
static s8 infile[128];
static u32 divmode = 0;



u8 *pagebuf = NULL;
u8 *sendbuf = NULL;
u8 *msgbuf = NULL;
u16 msglen = 0;

u32 global_trans_idx = 0;

int sock_fd = 0;
struct sockaddr_in ser_addr;
s8 server_ip[32];
s32 server_port = 9203;
date_time datetime;
fd_set writefd;
S_REC_STAT *rec_stat_list = NULL;

static s32 pen_params_init()
{
    raw_save_window = RAW_SAVE_WINDOW_DEFAULT;
    raw_save_limit = INEMO_SAMPLE_RATE * raw_save_window;
    return 0;
}

static s32 dyn_params_init()
{
    srand((unsigned)time(NULL));
    return 0;
}


/* 取当前时间，精确到微秒 */
s32 get_date_and_time(int cst, date_time *datetime)
{
    u32 i;
    s32 hms, day;
    s32 leapsToDate;
    s32 lastYear;
    long sec = 0;
    int MonthOffset[] = { 0,31,59,90,120,151,181,212,243,273,304,334 };
    int month_days[12] = {	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    struct timeval tv;
    
    memset(&tv, 0, sizeof(tv));
    gettimeofday(&tv, NULL);
    sec = tv.tv_sec;

    if (cst){
        sec = sec + (60*60)*BEIJINGTIME;
    }
    day = sec / SECDAY;
    hms = sec % SECDAY;

    datetime->tv_hour = hms / 3600;
    datetime->tv_min = (hms % 3600) / 60;
    datetime->tv_sec = (hms % 3600) % 60;

    for (i = STARTOFTIME; day >= days_in_year(i); i++) {
        day -= days_in_year(i);
    }
    datetime->tv_year = i;

    if (leapyear(datetime->tv_year)) {
        month_days[FEBRUARY-1] = 29;
    }
    for (i = 1; day >= month_days[i-1]; i++) {
        day -= month_days[i-1];
    }
    month_days[FEBRUARY-1] = 28;
    datetime->tv_mon = i;

    datetime->tv_mday = day + 1;
    lastYear = datetime->tv_year - 1;
    leapsToDate = lastYear/4 - lastYear/100 + lastYear/400;      

    if ((datetime->tv_year%4 == 0) &&
       ((datetime->tv_year%100 != 0) || (datetime->tv_year%400 == 0)) &&
       (datetime->tv_mon > 2)) {
        day = 1;
    } else {
        day = 0;
    }
    day += lastYear*365 + leapsToDate + MonthOffset[datetime->tv_mon-1] + datetime->tv_mday; 
    datetime->tv_wday=day%7;
    return 0;
}


static s32 reset_header(u8 *header)
{
    SAVEDATA_HEADER_T *save_header = (SAVEDATA_HEADER_T *)header;
    get_date_and_time(0, &datetime);

    save_header->magic = MAGIC_HEAD;
    save_header->dataType = 0;
    save_header->packageIndex = 0;
    save_header->timeStamp.tv_year = (u8)((datetime.tv_year - CENTURY_BASE) & 0xFF);
    save_header->timeStamp.tv_mon  = (u8)(datetime.tv_mon & 0xFF);
    save_header->timeStamp.tv_mday = (u8)(datetime.tv_mday & 0xFF);
    save_header->timeStamp.tv_hour = (u8)(datetime.tv_hour & 0xFF);
    save_header->timeStamp.tv_min  = (u8)(datetime.tv_min & 0xFF);
    save_header->timeStamp.tv_sec  = (u8)(datetime.tv_sec & 0xFF);
    save_header->read_flag = 0xFFFF;
    save_header->totalLength = SAVEDATA_HEADER_SIZE - 12;        /**< 从userID开始 */
    get_userid((u8*)&(save_header->userID), sizeof(save_header->userID));
    *((u32*)(save_header->userID)) = os_htonl(*((u32*)(save_header->userID)));
    get_chipid((u8*)&(save_header->cpuID), sizeof(save_header->cpuID));
    save_header->crc = com_crc((u8*)&(save_header->dataType), SAVEDATA_HEADER_SIZE - 6);
    return 0;
}


/**
 *  angle = (360) * n / (WAVE_PERIODS_IN_SEC) , n = [0,WAVE_PERIODS_IN_SEC]
 *  output = 1000 * sin(angle)
*/
static s32 sens_data_gen(REC_DATA* sens)
{
    u16 angle = 0;
    u16 off = raw_save_count%WAVE_PERIODS_IN_SEC;
    s32 val = 0;
    u32 userid = 0;
    (void)get_userid((u8 *)(&userid), sizeof(userid));

    memset(sens, 0x00, sizeof(REC_DATA));
    angle = (360) * off / (WAVE_PERIODS_IN_SEC);

    if (0 <= angle && angle < 90) {
        val = math_table[3*angle+1];
    }
    else if (90 <= angle && angle < 180) {
        angle = 180 - angle;
        val = math_table[3*angle+1];
    }
    else if (180 <= angle && angle < 270) {
        angle = angle - 180;
        val = (-1) * math_table[3*angle+1];
    }
    else {
        angle = 360 - angle;
        val = (-1) * math_table[3*angle+1];
    }
    val = (val * 1000) / 10000 + 1000;
    /* rand use userid as seed */
    val += (100 - userid % 200);
    sens->inemoX0.z = val;
    sens->inemoG0.x = rand() % 20 - 10;
    sens->inemoG0.y = rand() % 20 - 10;
    sens->inemoG0.z = rand() % 20 - 10;
    MOD_PRINT(MOD_DEBUG_INF, "%5d %3d %3d %3d\n", sens->inemoX0.z, sens->inemoG0.x, sens->inemoG0.y, sens->inemoG0.z);
    return 0;
}

int init_clinet()
{
    if (server_ip[0] == 0x00) {
        printf("Please indicate a server!\n");
        return -1;
    }

	// create socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        MOD_PRINT(MOD_DEBUG_ERR, "Create socket failed\n");  
        return -1;
    }

    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;  
    ser_addr.sin_port = htons(server_port);
    inet_aton((s8 *)server_ip, (struct in_addr *)&ser_addr.sin_addr);
	return 0;
}


s32 connect_server()  
{  
    fd_set fds_read, fds_write;  
    struct timeval tval;  
    int selret = 0;    
    int ntrytimes = 0;
    int paddrlen = sizeof(ser_addr);
    struct sockaddr *pser_addr = (struct sockaddr *)&ser_addr;

    if (connect(sock_fd, pser_addr, paddrlen) < 0) {
        if (errno == EISCONN) {  
            printf("Connect socket completed\n");
            return 0;
        }  
        if(errno != EINPROGRESS && errno != EALREADY && errno != EWOULDBLOCK) {
            printf("Connect socket failed\n");  
            return -1;  
        }  
	    else {  
	        printf("Connect socket does not completed\n");  
	    }  
    }  
    else  {  
        printf("Connect socket completed\n");  
    	return 0;  
    }  
    
    tval.tv_sec = 3;  
    tval.tv_usec = 0;  
      
    while(1 && ntrytimes < MAX_CONNECT_TIMES)  {  
        FD_ZERO(&fds_read);  
        FD_SET(sock_fd, &fds_read);  
          
        FD_ZERO(&fds_write);  
        FD_SET(sock_fd, &fds_write);  
        printf("Before select\n");          
        selret = select(sock_fd + 1, &fds_read, &fds_write, NULL, &tval);  
        printf("After select\n");   
        if(selret < 0)  {  
            if(errno == EINTR)   {  
                ntrytimes++;  
                continue;  
            }  
          	else  { 
                printf("select failed\n");  
                return -1;  
          	}  
        }  
        else if(selret == 0)  {  
            printf("onnect socket timeout\n");  
            ntrytimes++;  
            continue;  
        }  
        else  {  
            printf("select default\n");               
            if(FD_ISSET(sock_fd, &fds_read) || FD_ISSET(sock_fd, &fds_write))  
            {  
                int error = 0;  
                int len = sizeof(error);  
                int rc = getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, (void *) &error, &len);  
                if(rc == -1)  {  
                    printf("Connection is closed\n");  
                    return -1;  
                }  
                else if(error) {  
                    printf("Connection is closed\n");  
                    return -1;  
                }  
                else {  
                    printf("Connection is ok\n");  
              		return 0;  
                }                    
            }  
            else  {  
                printf("No descriptor is ready\n");  
             	continue;  
            }  
        }  
    }        
    return -1;  
}

s32 close_client()
{     
	close(sock_fd); 
    return 0;
}


s32 send_data(s8 * sendbuf, s32 snebuflen)
{  
	s32 sendlen = 0;
	s32 leftlen = 0;
    s32 left_retry_len = 0;   
	s32 retlen = 0;
	s8 *ptr = sendbuf;  
	fd_set fds_write;  
    struct timeval tval;  
    s32 ret = 0;
    u8 retries = 3;
    u32 i = 0;
    
    tval.tv_sec = 3;  
    tval.tv_usec = 0;
    leftlen = snebuflen;

	if (dbg == 2) {	    
	    for (i = 0; i < snebuflen; i++) {
	        printf("%02X ", (u8)sendbuf[i]);
	    }
	    printf("\n===> %d\n", snebuflen);
	}
    while (leftlen) {
    	sendlen = (leftlen <= bytes_on_send) ? leftlen : bytes_on_send; 
	    FD_ZERO(&fds_write);  
	    FD_SET(sock_fd, &fds_write);
	    retlen = send(sock_fd, ptr, sendlen, 0);
	    if (retlen < sendlen)  {  
	        if (retlen < 0)  {  
	            if(errno != EWOULDBLOCK && errno != ENOBUFS && errno != EAGAIN && errno != EINTR)  {
	                printf("Unsolved error(%s)!\n", strerror(errno));
	            	return -1;  
	            }
		        else  {
	                printf("Send null\n");
		            retlen = 0;  
	            }
	        }  
            retries = 3;
            leftlen -= retlen;            
            ptr += retlen;
            left_retry_len = sendlen - retlen;
            sendlen = left_retry_len;            
	        while(1)  {
	            FD_ZERO(&fds_write);  
	            FD_SET(sock_fd, &fds_write);  
	        	ret = select(sock_fd+1, NULL, &fds_write, NULL, &tval);  
	        	if (ret < 0)  {  
	                if (errno == EINTR)  {
	                    continue;  
					} 
	                else {
	                    printf("%s:%d, select failed\n", __FILE__, __LINE__);  
		                return -1;
		            }  
				}
	            else if (ret == 0) {  
	                printf("%s:%d, select timeout, no descriptors can be read or written", __FILE__, __LINE__);  
	              	continue;  
	        	}  
	       		else  {  
		            if (FD_ISSET(sock_fd, &fds_write)) {                
		                do {  
		                    retlen = send(sock_fd, ptr, sendlen, 0);
		                    if (retlen < 0)  {  
		                        if (errno == EAGAIN || errno == EWOULDBLOCK)
		                        	break;
		                        else  if (errno == EINTR)
		                        	retlen = 0;  
			                    else  
			                        printf("Send data error(%s)\n", strerror(errno));
		                    }
		                    leftlen -= retlen;            
				            ptr += retlen;
				            left_retry_len = sendlen - retlen;
				            sendlen = left_retry_len;
                            retries--;                            
		                } while(leftlen & retries);
                        if (leftlen) {
                            printf("Send data final error\n");
                            return -1;
                        }
		            }
		            else  {  
		                return -1;  
		            }  
	        	}  
	        }  
	    }
        else {
            leftlen -= retlen;  
            sendlen = leftlen;  
            ptr += retlen;  
        }
    }
    return 0;  
}  

s32 gen_page_data(u8 *start)
{
    u8 *bufcur = start;
    u8 i = 0;
    for (i = 0; i < RAW_SAVE_INEMO_UNIT; i++) {
        sens_data_gen((REC_DATA* )bufcur);
        save_header->crc = com_crc_iv(bufcur, sizeof(REC_DATA), save_header->crc);
        save_header->totalLength += sizeof(REC_DATA);
        raw_save_count++;
        bufcur += sizeof(REC_DATA);
    }
    return 0;
}

s32 send_trans_packet(u32 len)
{
    u8 * send = NULL;
    u32 sendlen = len;
    u8 type = rand()%20;

    if (!mix)
    	type = 0;

	if (1 == type) {
	    send = sendbuf + sendlen;
	    memcpy(send, msgbuf, msglen);
	    sendlen += msglen;
        if (1 == dbg) {
            printf("+ ");
        }else if (2 == dbg) {
            printf("+ \n");
        }
	    send_data((s8 *)sendbuf, sendlen);
	}
	else if (2 == type) {
        if (1 == dbg) {
            printf("* ");
        }
        else if (2 == dbg) {
            printf("* \n");
        }
	    send_data((s8 *)sendbuf, sendlen);
	    send_data((s8 *)msgbuf, msglen);
	}
	else  {
        if (1 == dbg) {
            printf(". ");
        }
	    send_data((s8 *)sendbuf, sendlen);        
	}    
	usleep(1000*delay_in_ms);
    return 0;
}

s32 gen_basic_info_passthrouth_packet()
{
    u16 crc = 0;
    u32 hdrlen = TRANS_HDR_MAGIC_SIZE+TRANS_HDR_LEN_SIZE+TRANS_HDR_TYPE_SIZE+TRANS_HDR_SEQID_SIZE;
    u8 spoonmsg[MSG_MAX_LEN];
    u16 spoonmsglen = 0;
    u8 *passthrouthmsgbody = NULL;
    
    (void)gen_basic_info_msg(spoonmsg, &spoonmsglen);
    passthrouthmsgbody = (u8 *)&spoonmsg[MSG_HDR_TOTAL_SIZE+MSG_SUBHDR_TOTAL_SIZE];
    spoonmsglen -= MSG_HDR_TOTAL_SIZE+MSG_SUBHDR_TOTAL_SIZE;

    msgbuf[TRANS_HDR_MAGIC_OFF] = 0x7E;
    *(u16 *)&msgbuf[TRANS_HDR_LEN_OFF] = hdrlen - TRANS_HDR_MAGIC_SIZE + spoonmsglen + sizeof(crc);
    msgbuf[TRANS_HDR_TYPE_OFF] = 28;
    msgbuf[TRANS_HDR_SEQID_OFF] = 0;
    memcpy((u8 *)&msgbuf[hdrlen], passthrouthmsgbody, spoonmsglen);
    crc = com_crc(&(msgbuf[TRANS_HDR_LEN_OFF]), hdrlen - TRANS_HDR_MAGIC_SIZE + spoonmsglen);
    *(u16 *)&(msgbuf[hdrlen+spoonmsglen]) = crc;

    msglen = hdrlen + spoonmsglen + sizeof(crc);
    return 0;
}


s32 gen_trans_packet(u32 *totlen)
{
    u8 trans_hdr[16];
    u16 crc = 0;
    u8 * meta = NULL;
    u8 * send = NULL;
    u32 sendlen = 0;

	memset(trans_hdr, 0x00, sizeof(trans_hdr));
    trans_hdr[TRANS_HDR_MAGIC_OFF] = 0x7E;
    *(u16 *)&trans_hdr[TRANS_HDR_LEN_OFF] = save_header->totalLength + 14;
    trans_hdr[TRANS_HDR_TYPE_OFF] = 11;
    trans_hdr[TRANS_HDR_SEQID_OFF] = 0;
    trans_hdr[TRANS_HDR_FRAMEID_OFF] = global_trans_idx++;
    *(u32 *)&(trans_hdr[TRANS_HDR_METAID_OFF]) = save_header->packageIndex;
    
    meta = (u8 *)(save_header->userID);
    crc = com_crc(&(trans_hdr[TRANS_HDR_LEN_OFF]), TRANS_HDR_HDR_SIZE-TRANS_HDR_MAGIC_SIZE);
    crc = com_crc_iv(meta, save_header->totalLength, crc);
	
    memset(sendbuf, 0x00, RAW_SAVE_SIZE_ALIGN);
    send = sendbuf;
    memcpy(send, trans_hdr, TRANS_HDR_HDR_SIZE);
    send += TRANS_HDR_HDR_SIZE;
    memcpy(send, meta, save_header->totalLength);
    send += save_header->totalLength;
    memcpy(send, (u8 *)&crc, 2);
    send += 2;

	sendlen = TRANS_HDR_HDR_SIZE+save_header->totalLength+2;
    *totlen = sendlen;
    return 0;
}

s32 gen_one_record()
{
    u32 i = 0;
    u16 totalpage = ROUND_UP(raw_save_limit,  RAW_SAVE_INEMO_UNIT) / RAW_SAVE_INEMO_UNIT;
	u32 translen = 0;
        
    /** clear counter */
    raw_save_count = 0;

    /* clear header */
    save_header = (SAVEDATA_HEADER_T *)pagebuf;
    memset(save_header, 0x00, SAVEDATA_HEADER_SIZE);
    reset_header((u8 *)save_header);
    printf("#\n");

    for (i = 0; i < totalpage; i++) {
        /** clear meta */
        memset((pagebuf+SAVEDATA_HEADER_SIZE), 0x00, (RAW_SAVE_SIZE_ALIGN-SAVEDATA_HEADER_SIZE));

        /** build page meta  */
        (void)gen_page_data((pagebuf+SAVEDATA_HEADER_SIZE));

		/** generate transfer packer */
		(void)gen_trans_packet(&translen);
        (void)send_trans_packet(translen);

        /** modify header */
        save_header = (SAVEDATA_HEADER_T *)pagebuf;
	    save_header->packageIndex++;
        save_header->crc = com_crc((u8*)&(save_header->dataType), SAVEDATA_HEADER_SIZE - 6);
        save_header->totalLength = SAVEDATA_HEADER_SIZE - 12;
    }
    printf("\n");
    return 0;
}

u32 get_file_size(const s8 *path)  
{  
    u32 filesize = 0;      
    struct stat statbuff;
    if (stat(path, &statbuff) < 0){  
        return 0;  
    }else{  
        filesize = statbuff.st_size;  
    }  
    return filesize;  
}

s32 check_empty(s8 *buf, u32 size)
{
    u32 i = 0;
    for (i = 0; i < size; i++) {
        if (0xFF != buf[i]) {
            return 0;
        }
    }
    return 1;
}

s32 save_single_rec(S_REC_STAT * rec, u32 append)
{
    u32 ret_sz = 0;
    static FILE *dest_fp = NULL;
    SAVEDATA_HEADER_T *page_hdr = (SAVEDATA_HEADER_T *)pagebuf;
    static s8 destfilename[64];    

	if (0 == append) {
        if (dest_fp) {
            MOD_PRINT(MOD_DEBUG_INF, "Close dest file %s \n", destfilename);
            fclose(dest_fp);
        }        
        snprintf(destfilename, sizeof(destfilename)-1, "%04d%02d%02d%02d%02d%02d-%06d-%08X%08X%08X.bin", 
            (u32)(rec->time.tv_year+CENTURY_BASE), rec->time.tv_mon, rec->time.tv_mday, 
            rec->time.tv_hour, rec->time.tv_min, rec->time.tv_sec, rec->userid, 
            *(((u32 *)(rec->chipid))+2), *(((u32 *)(rec->chipid))+1), 
            *(((u32 *)(rec->chipid))+0));
	    if (NULL == (dest_fp = fopen(destfilename, "wb+"))) {
	        MOD_PRINT(MOD_DEBUG_ERR, "Can not open dest file %s \n", destfilename);
	        return -1;
	    }
        else {
            MOD_PRINT(MOD_DEBUG_INF, "Create new dest file %s \n", destfilename);
        }
    }
    
    if (2 == append) {
        if (dest_fp) {
            MOD_PRINT(MOD_DEBUG_INF, "Close dest file %s \n", destfilename);
            fclose(dest_fp);
        }
    }
    else {
	    if ((ret_sz = fwrite((void *)(pagebuf+SAVEDATA_HEADER_SIZE), 
	        (page_hdr->totalLength+12-SAVEDATA_HEADER_SIZE), 
	        1, dest_fp)) != 1) {
	        MOD_PRINT(MOD_DEBUG_ERR, "Write dest file %s error\n", destfilename);
	        fclose(dest_fp);
	        return -1;
	    }
    }
    return 0;
}

s32 parse_log_file(s8 *srcfile)
{
    FILE *src_fp = NULL;
    u32 filesz = 0, left_sz = 0, rw_sz = 0, ret_sz = 0;
    u32 actsz = 0;
    u32 pages = 0;
    u16 crc = 0;
    u32 corrupt = 0;
    SAVEDATA_HEADER_T *page_hdr = NULL;
    
    if (NULL == (src_fp = fopen(srcfile, "rb"))) {
        printf("Cant open src file %s\n", srcfile);
        return -1;
	}

    filesz = get_file_size(srcfile);
    left_sz = filesz;

	while (left_sz) {
	    rw_sz = (left_sz > RAW_SAVE_SIZE_ALIGN) ? RAW_SAVE_SIZE_ALIGN : left_sz;
	    if ((ret_sz = fread((void *)pagebuf, rw_sz, 1, src_fp)) != 1) {
	    	if (src_fp) {
	        	fclose(src_fp);
            }
            rec_stat_list[(rec_num>0)?(rec_num-1):0].size = actsz;
            rec_stat_list[(rec_num>0)?(rec_num-1):0].pages = pages;
            rec_stat_list[(rec_num>0)?(rec_num-1):0].stat = corrupt;
            save_single_rec((S_REC_STAT *)(&rec_stat_list[rec_num-1]), 2);
	    	return -1;
		}
		page_hdr = (SAVEDATA_HEADER_T *)pagebuf;
        if ((0xDEADBEEF != page_hdr->magic) && !check_empty((s8 *)pagebuf, rw_sz)) { /*The end of real data*/
            fclose(src_fp);
            rec_stat_list[(rec_num>0)?(rec_num-1):0].size = actsz;
            rec_stat_list[(rec_num>0)?(rec_num-1):0].pages = pages;
            rec_stat_list[(rec_num>0)?(rec_num-1):0].stat = corrupt;  
            save_single_rec((S_REC_STAT *)(&rec_stat_list[rec_num-1]), 2);
            return 0;
        }
        /* CRC checking */
        crc = com_crc((u8*)&(page_hdr->dataType), SAVEDATA_HEADER_SIZE - 6);
        crc = com_crc_iv((u8*)(pagebuf+SAVEDATA_HEADER_SIZE), (page_hdr->totalLength+12-SAVEDATA_HEADER_SIZE), crc);
        if (crc != page_hdr->crc) {
            printf("CRC checking failed\n");
        	corrupt = 1;
            if (rw_sz == left_sz) { // The end of file
            	fclose(src_fp);
                rec_stat_list[(rec_num>0)?(rec_num-1):0].size = actsz;
	            rec_stat_list[(rec_num>0)?(rec_num-1):0].pages = pages;
	            rec_stat_list[(rec_num>0)?(rec_num-1):0].stat = corrupt;             
                save_single_rec((S_REC_STAT *)(&rec_stat_list[rec_num-1]), 2);
            	return 0;
            }
        }
		if (memcmp((void *)&(rec_stat_list[(rec_num>0)?(rec_num-1):0].time), (void *)&page_hdr->timeStamp, sizeof(rec_time))) {  /* New record */          
            memcpy((void *)&(rec_stat_list[rec_num].time), (void *)&page_hdr->timeStamp, sizeof(rec_time));
            rec_stat_list[rec_num].userid = os_htonl(*(u32 *)page_hdr->userID);
            memcpy(rec_stat_list[rec_num].chipid, page_hdr->cpuID, CPU_ID_SIZE);
            save_single_rec((S_REC_STAT *)(&rec_stat_list[rec_num]), 0);            
            if (rec_num) {
                rec_stat_list[rec_num-1].size = actsz;
	            rec_stat_list[rec_num-1].pages = pages;
	            rec_stat_list[rec_num-1].stat = corrupt;
            }
            rec_num++;
            corrupt = 0;
            actsz = 0;  
            pages = 0;
        }
        /* Old record */
        else {
            if (1 == corrupt) 
            	corrupt = 2;
            save_single_rec((S_REC_STAT *)(&rec_stat_list[rec_num-1]), 1);
        }
        left_sz -= rw_sz;
        actsz += rw_sz;
        pages++;
	}
    return 0;
}

void dump_log_statics(s8 *srcfile)
{
    u32 i = 0;
    
    printf("\n---------------------------------------------------------------------------\n");
    printf("             %s size %d, total rec num %d\n", srcfile, get_file_size(srcfile), rec_num);
    printf("---------------------------------------------------------------------------\n\n");
    for (i = 0; i < rec_num; i++) {
        printf("%d %08X%08X%08X %04d-%02d-%02d %02d:%02d:%02d, size %d pages %d\n", 
                rec_stat_list[0].userid,
                *((u32 *)(rec_stat_list[i].chipid)+2),
                *((u32 *)(rec_stat_list[i].chipid)+1),
                *((u32 *)(rec_stat_list[i].chipid)),
                (u32)(rec_stat_list[i].time.tv_year + CENTURY_BASE), 
                rec_stat_list[i].time.tv_mon, 
                rec_stat_list[i].time.tv_mday, 
                rec_stat_list[i].time.tv_hour, 
                rec_stat_list[i].time.tv_min, 
                rec_stat_list[i].time.tv_sec,                
                rec_stat_list[i].size,
                rec_stat_list[i].pages);
	}
    printf("\n---------------------------------------------------------------------------\n");
}

void catch_signal(int sign)
{
    switch(sign)
    {
	    case SIGPIPE:
            close_client();
            init_clinet();
            connect_server();
            break;
    }
}


void usage()
{
    printf("Usage:\n\n");
    printf("sdf -a<ip> -p<port> -s<unit> -i<interval> -d<delay> -u<userid> -c<chipid> -n<num> -l<debug> -m<mode> -finputfile\n");
    printf("    -a:           ip address of remote server\n");
    printf("    -p:           port number of remote server\n");
    printf("    -s:           packet size\n");
    printf("    -i:           interval between 2 record, ms\n");
    printf("    -d:           interval between 2 packet, ms\n");
    printf("    -u:           the unique id of user\n");
    printf("    -c:           the unique id of device\n");
    printf("    -n:           recored number, -1 means endless\n");
    printf("    -l:           debug level, 0-none 1-tidy 2-detail\n");
    printf("    -m:           mixed mode, 0-disable, 1-mix(default)\n");
    printf("    -f:           input file, output will be separate binary\n");
    printf("\n\n");
    printf("Examples:\n");
	printf("    sdf.exe -a114.215.121.190 -p9203 -s512 -i120000 -d100 -u476289 -c05D9FF313433504B51126828 -n10 -m0 -l2\n");
    printf("    sdf.exe -flog.bin\n");
    
}

/*将大写字母转换成小写字母*/
s32 tolower(s32 c)
{
    if (c >= 'A' && c <= 'Z') {
        return c + 'a' - 'A';
    }
    else {
        return c;
    }
}

/*将小写字母转换成大写字母*/
s32 tohigher(s32 c)
{
    if (c >= 'a' && c <= 'z') {       
        return c - 'a' + 'A';   
    }
    else {
        return c;
    }
}


int htoi(char *s)
{  
    int i;  
    int n = 0;  
    
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X')) {  
        i = 2;
    }  
    else {  
        i = 0;  
    }
    for (; (s[i] >= '0' && s[i] <= '9') 
        || (s[i] >= 'a' && s[i] <= 'z') 
        || (s[i] >='A' && s[i] <= 'Z');++i) {  
        if (tolower(s[i]) > '9') 
            n = 16 * n + (10 + tolower(s[i]) - 'a');  
        else 
            n = 16 * n + (tolower(s[i]) - '0');
    }
    return n;  
}

s32 main(s32 argc, s8 **argv)
{
    s32 i = 0;
	u32 userid = 0;
    u32 chipid[3];
    s32 opt;
    s8 chipidseg[12];
    
	signal(SIGPIPE, catch_signal);    
    memset(server_ip, 0x00, sizeof(server_ip));
	while((opt = getopt(argc,argv,"a:p:s:i:d:u:c:n:m:l:f:")) != -1) {
        switch(opt) {
            case 'a':
                strcpy(server_ip, optarg);
                break;
            case 'p':
                server_port = atol(optarg);
                break;
            case 's':
                bytes_on_send = atol(optarg);
                break;
            case 'i':
                rec_interval_in_ms = atol(optarg);
                break;
            case 'd':
                delay_in_ms = atol(optarg);
                break;
            case 'u':
                userid = atol(optarg);
    			set_userid(userid);
                break;
            case 'c':
                chipidstr = optarg;
                if (24 != strlen(chipidstr)) {
                    usage();
    				exit(0);
                }
                memset(chipidseg, 0x00, sizeof(chipidseg));
                strncpy(chipidseg,  chipidstr, 8);
                chipid[2] = htoi(chipidseg);
                memset(chipidseg, 0x00, sizeof(chipidseg));
                strncpy(chipidseg,  (chipidstr+8), 8);
                chipid[1] = htoi(chipidseg);
                memset(chipidseg, 0x00, sizeof(chipidseg));
                strncpy(chipidseg,  (chipidstr+16), 8);
                chipid[0] = htoi(chipidseg);
    			(void)set_chipid((u8 *)chipid, CPU_ID_SIZE);
                break;
            case 'n':
                rec_num = atol(optarg);
        		endless = (-1 == rec_num) ? 1 : 0;
                break;
            case 'l':
                dbg = atol(optarg);
                break;
            case 'm':
                mix = atol(optarg);
                break;
            case 'f':
                strncpy(infile, optarg, strlen(optarg));
                divmode = 1;
                break;
            default:
				printf("Invalid parameter!\n");
				usage();
    			exit(0);
        }
	}

    if ((divmode)) {
        if (argc != 2) {
	        usage();
	        return -1;
        }
    }
    else {
        if (argc != 11) {
	        usage();
	        return -1;
        }
    }    

    pagebuf = (u8 *)malloc(RAW_SAVE_SIZE_ALIGN);
    if (!pagebuf) {
        return -1;
    }
    sendbuf = (u8 *)malloc(1024);
    if (!sendbuf) {
        return -1;
    }
    msgbuf = (u8 *)malloc(MSG_MAX_LEN);
    if (!msgbuf) {
        return -1;
    }
    if (divmode) {
        rec_num = 0;        
        rec_stat_list = (S_REC_STAT *)malloc(MAX_REC_NUM*sizeof(S_REC_STAT));
        if (!rec_stat_list) {
            printf("Memory alloc error!\n");
			return -1;
        }
        memset(rec_stat_list, 0, MAX_REC_NUM*sizeof(S_REC_STAT));                
        parse_log_file(infile);
        dump_log_statics(infile);
    }
	else {
        if (!chipidstr) {
	        chipid[2] = userid;
	        chipid[1] = 0xFFFFFFFF;
	        chipid[0] = 0xFFFFFFFF;
	        (void)set_chipid((u8 *)chipid, CPU_ID_SIZE);
	    }
		get_date_and_time(0, &datetime);
	    
	    printf("-----------------------------------------\n");
	    printf("%-10s: %s:%d\n", "SERVER", server_ip, server_port);
	    printf("%-10s: %d\n", "USERID", userid);
	    printf("%-10s: %08X%08X%08X\n", "CHIPID", chipid[2], chipid[1], chipid[0]);
		printf("%-10s: %d\n", "INTERVAL", rec_interval_in_ms);
	    printf("%-10s: %d\n", "DELAY", delay_in_ms);
	    printf("%-10s: %d\n", "UNIT SIZE", bytes_on_send);
		printf("%-10s: %d\n", "NUM", rec_num);
	    printf("%-10s: %d\n", "MIX", mix);
	   	printf("%-10s: %d\n", "DEBUG", dbg);
	    printf("%-10s: %d-%02d-%02d(%d) %02d:%02d:%02d done!\n", "TIME",
	        datetime.tv_year, datetime.tv_mon, datetime.tv_mday, 
	        datetime.tv_wday, datetime.tv_hour, datetime.tv_min, 
	        datetime.tv_sec);
	    printf("-----------------------------------------\n\n");
    
        (void)pen_params_init();
    	(void)dyn_params_init();
        (void)init_clinet();
    	(void)connect_server();
		gen_basic_info_passthrouth_packet();
		for (i = 0 ; i < rec_num ; i++) {
	        gen_one_record();
	        printf("USERID %d (%d) %d-%02d-%02d(%d) %02d:%02d:%02d done!\n", 
	            userid, i, datetime.tv_year, datetime.tv_mon, datetime.tv_mday, 
	            datetime.tv_wday, datetime.tv_hour, datetime.tv_min, datetime.tv_sec);
		    usleep(1000*rec_interval_in_ms);
	        i = (endless) ? 0 : i;
		}
	    
    }
    if (rec_stat_list) {
        free(rec_stat_list);
    	rec_stat_list = NULL;
    }        
    free(msgbuf);
	free(sendbuf);
    free(pagebuf);
	close_client();
    printf("Exit...\n");
    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /*  __cpluscplus */
#endif /*  __cpluscplus */


/** @}*/
