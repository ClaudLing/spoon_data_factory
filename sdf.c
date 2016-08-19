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
#include <unistd.h>
#include "os_def.h"
#include "sdf.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /*  __cpluscplus */
#endif /*  __cpluscplus */

#define WAVE_RATE 4
#define WAVE_PERIODS_IN_SEC (INEMO_SAMPLE_RATE/WAVE_RATE)


/** About date and time */
#define BEIJINGTIME 8

#define DAY        (60*60*24)
#define YEARFIRST  2001
#define YEARSTART  (365*(YEARFIRST-1970) + 8)
#define YEAR400    (365*4*100 + (4*(100/4 - 1) + 1))
#define YEAR100    (365*100 + (100/4 - 1))
#define YEAR004    (365*4 + 1)
#define YEAR001    365



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

u16 CRCTALBES[] =
{
	0x0000, 0xCC01, 0xD801, 0x1400,
	0xF001, 0x3C00, 0x2800, 0xE401,
	0xA001, 0x6C00, 0x7800, 0xB401,
	0x5000, 0x9C01, 0x8801, 0x4400
};

static u32 raw_save_window = 0;
static u32 raw_save_limit = 0;
static u32 flash_offset = LOG_OFF;
static u32 raw_save_count = 0;
static SAVEDATA_HEADER_T *save_header;
static u32 userid = 0;
static u32 rec_num = 1;
static u32 rec_interval = 5;
static u32 totalbuffersize = 0;
static u32 endless = 0;

u8 *pagebuf = NULL;
u32 global_trans_idx = 0;

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
#define MOD_FLAGS 								(MOD_DEBUG_WRN | MOD_DEBUG_ERR)
#define MOD_PRINT(flag, fmt, ...) \
        do{ \
              if (MOD_FLAGS & flag){ \
                  printf(fmt, ##__VA_ARGS__);} \
        } while(0)
#else
#define MOD_PRINT(fmt, ...)
#endif

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

s32 Lowlevel_Set_UserID(u8 * buf, u8 len)
{
    u32 userid = *(u32 *)buf;
    s32 ret = 0;

    return ret;
}

s32 get_userid(u8 * buf,u8 len)
{
    s32 ret = 0;

    *(u32 *)buf = userid;
    return ret;   
}

/*
取当前时间，精确到微秒；
*/
int get_date_and_time(int cst, date_time *datetime)
{
    struct timeval tv;
    long sec = 0, usec = 0;
    int yy = 0, mm = 0, dd = 0, hh = 0, mi = 0, ss = 0, ms = 0;
    int ad = 0;
    int y400 = 0, y100 = 0, y004 = 0, y001 = 0;
    int m[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int i;
    memset(&tv, 0, sizeof(tv));
    gettimeofday(&tv, NULL);
    sec = tv.tv_sec;
    usec = tv.tv_usec;
    if (cst){
        sec = sec + (60*60)*BEIJINGTIME;
    }    
    ad = sec/DAY;
    ad = ad - YEARSTART;
    y400 = ad/YEAR400;
    y100 = (ad - y400*YEAR400)/YEAR100;
    y004 = (ad - y400*YEAR400 - y100*YEAR100)/YEAR004;
    y001 = (ad - y400*YEAR400 - y100*YEAR100 - y004*YEAR004)/YEAR001;
    yy = y400*4*100 + y100*100 + y004*4 + y001*1 + YEARFIRST;
    dd = (ad - y400*YEAR400 - y100*YEAR100 - y004*YEAR004)%YEAR001;
    //月 日
    if(0 == yy%1000)
    {
        if(0 == (yy/1000)%4)
        {
            m[1] = 29;
        }
    }
    else
    {
        if(0 == yy%4)
        {
            m[1] = 29;
        }
    }
    for(i = 1; i <= 12; i++)
    {
        if(dd - m[i] < 0)
        {
            break;
        }
        else
        {
            dd = dd -m[i];
        }
    }
    mm = i;
    //小时
    hh = sec/(60*60)%24;
    //分
    mi = sec/60 - sec/(60*60)*60;
    //秒
    ss = sec - sec/60*60;
    ms = usec;
    printf("%d-%02d-%02d %02d:%02d:%02d\n", yy, mm, dd, hh, mi, ss);
    return 1;
}

s32 get_chipid(u8 * buf, u8 len)
{
    u32 *chipid = NULL;

	chipid = (u32 *)buf;
	chipid[0] = userid;
	chipid[1] = 0xFFFFFFFF;
	chipid[2] = 0xEEEEEEEE;
    return 0;
}


static s32 reset_header(u8 *header)
{
    SAVEDATA_HEADER_T *save_header = (SAVEDATA_HEADER_T *)header;
    date_time datetime;
    get_date_and_time(0, &datetime);
    
    save_header->magic = MAGIC_HEAD;
    save_header->dataType = 0;
    save_header->packageIndex = 0;
    save_header->read_flag = 0xffff;
    save_header->timeStamp.tv_year = (u8)((datetime.tv_year - CENTURY_BASE) & 0xFF);
    save_header->timeStamp.tv_mon  = (u8)(datetime.tv_mon & 0xFF);
    save_header->timeStamp.tv_mday = (u8)(datetime.tv_mday & 0xFF);
    save_header->timeStamp.tv_hour = (u8)(datetime.tv_hour & 0xFF);
    save_header->timeStamp.tv_min  = (u8)(datetime.tv_min & 0xFF);
    save_header->timeStamp.tv_sec  = (u8)(datetime.tv_sec & 0xFF);
    save_header->totalLength = SAVEDATA_HEADER_SIZE - 12;        /**< 从userID开始 */
    get_userid(save_header->userID, sizeof(save_header->userID));
    get_chipid(save_header->cpuID, sizeof(save_header->cpuID));
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
    val += (userid % 200 - 100);
    sens->inemoX0.z = val;
    sens->inemoG0.x = rand() % 20 - 10;
    sens->inemoG0.y = rand() % 20 - 10;
    sens->inemoG0.z = rand() % 20 - 10;
    MOD_PRINT(MOD_DEBUG_DBG, "%5d %3d %3d %3d\n", sens->inemoX0.z, sens->inemoG0.x, sens->inemoG0.y, sens->inemoG0.z);
    return 0;
}

s32 build_page_data(u8 *start)
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
}

s32 send_data(u8 *data, u32 len)
{
    u32 i = 0;
    for (i = 0; i < len; i++) {
        MOD_PRINT(MOD_DEBUG_DBG, "%02x ", data[i]);
    }
	return 0;
}

s32 build_trans_packet(u8 *buf)
{
    S_TRAS_PACKET transpkt;
    u16 crc = 0;
    u8 * test = NULL;

	memset(&transpkt, 0x00, sizeof(S_TRAS_PACKET));
    transpkt.magic = 0x7E;
    transpkt.len = save_header->totalLength + 14;
    transpkt.type = 11;
    transpkt.seqid = 0;
    transpkt.frameid = global_trans_idx++;
    transpkt.metaid = save_header->packageIndex;
    transpkt.data = (u8 *)(save_header)+12;        /**< begin at userid */
    crc = com_crc((u8*)&(transpkt.len), 12);
    transpkt.crc = com_crc_iv(transpkt.data, save_header->totalLength, crc);

    send_data((u8 *)&(transpkt.magic), sizeof(transpkt.magic));
    send_data((u8 *)&(transpkt.len), sizeof(transpkt.len)); 
    send_data((u8 *)&(transpkt.type), sizeof(transpkt.type)); 
    send_data((u8 *)&(transpkt.seqid), sizeof(transpkt.seqid));
    send_data((u8 *)&(transpkt.metaid), sizeof(transpkt.metaid));
  	send_data((u8 *)(transpkt.data), save_header->totalLength);
    send_data((u8 *)&(transpkt.crc), 2);

    MOD_PRINT(MOD_DEBUG_DBG, "\n");
    return 0;
}

s32 pack_one_record()
{
    u32 i = 0;
    u16 crc = 0;
    u16 totalpage = ROUND_UP((INEMO_SAMPLE_RATE*raw_save_window),  RAW_SAVE_INEMO_UNIT) / RAW_SAVE_INEMO_UNIT;

    /** clear counter */
    raw_save_count = 0;

    /* clear header */
    save_header = (SAVEDATA_HEADER_T *)pagebuf;
    memset(save_header, 0x00, SAVEDATA_HEADER_SIZE);
    reset_header((u8 *)save_header);
    
    for (i = 0; i < totalpage; i++) {        
        /** clear meta */
        memset((pagebuf+SAVEDATA_HEADER_SIZE), 0x00, (RAW_SAVE_SIZE_ALIGN-SAVEDATA_HEADER_SIZE));     

        /** build page meta  */
        build_page_data((pagebuf+SAVEDATA_HEADER_SIZE));

		/** generate transfer packer */
		build_trans_packet(pagebuf);
        
        /** modify header */
        save_header = (SAVEDATA_HEADER_T *)pagebuf;
	    save_header->packageIndex++;
        save_header->crc = com_crc((u8*)&(save_header->dataType), SAVEDATA_HEADER_SIZE - 6);
        save_header->totalLength = SAVEDATA_HEADER_SIZE - 12;
    }
}

/** 用户 条数 间隔时间 */
int main(int argc, char **argv)
{
    int i = 0;
    
    if (argc < 2) {
        printf("Please indicate userid\n");
        return -1;
    }
    userid = atol(argv[1]);
    if (argc >= 3) {
        rec_num = atol(argv[2]);
        endless = (-1 == rec_num) ? 1 : 0;
        if (argc >= 4) {
	        rec_interval = atol(argv[3]);
	    }
    }
    printf("--------------------------\n");
    printf("USERID: %d\n", userid);
	printf("NUM: %d\n", rec_num);
	printf("INTERVAL: %d\n", rec_interval);
    printf("--------------------------\n\n");

	(void)pen_params_init();
    (void)dyn_params_init();
    
    pagebuf = (u8 *)malloc(RAW_SAVE_SIZE_ALIGN);
    if (!pagebuf) {
        return -1;
    }

	for (i = 0 ; i < rec_num ; i++) {
        pack_one_record();
        printf("pack record(%d) over!\n", i);
	    sleep(rec_interval);
        i = (endless) ? 0 : i;
	}	

    free(pagebuf);
    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /*  __cpluscplus */
#endif /*  __cpluscplus */


/** @}*/
