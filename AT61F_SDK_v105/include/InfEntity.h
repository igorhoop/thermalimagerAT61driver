#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define JUST_WIN32
#elif defined(linux) || defined(__linux) || defined(__linux__)
#define JUST_LINUX
#endif

#ifdef JUST_LINUX
#include "LinuxDef.h"
#endif

#define FRAME_INDEX 256

#define FRAME_LT_START          (0xAA)
#define FRAME_LT_BACK           (0x55)
#define FRAME_TYPE              (0x00)
#define FRAME_TYPE01            (0x01)
#define FRAME_END_EB            (0xEB)
#define FRAME_END_AA            (0xAA)

#define MAX_WAIT_RESPONSE_TIME   100
#define MAX_LOCAL_BUF_LEN		512

typedef enum IR_OPERATE_e  //������ö��
{
	IR_OPERATE_READ = 0x00,
	IR_OPERATE_WRITE = 0x01,
	IR_OPERATE_DO = 0x02,
	IR_OPERATE_READ_CK = 0x03,
	IR_OPERATE_WRITE_CK = 0x04,
} IR_OPERATE, *P_IR_OPERATE;

#define LT_OPERATE_COMMAND0             0x07
#define CMD_LT_TEMP_FRAME_MAX_MES       0x27 //��֡��������¶�/����(read only)
#define CMD_LT_TEMP_FRAME_MIN_MES       0x29 //��֡������С�¶�/����(read only)
#define CMD_LT_TEMP_FRAME_CEN_MES       0x2c //��֡���������¶�/����(read only)
#define CMD_LT_TEMP_FRAME_AVR           0x2a //��֡���¾�ֵ�¶�     (read only)
#define CMD_LT_TEMP_USER_REFLECT        0x0f //�����¶�
#define CMD_LT_TEMP_USER_AIRTEMP        0x10 //���������¶�
#define CMD_LT_TEMP_USER_HUMIDITY       0x11 //����ʪ��
#define CMD_LT_TEMP_USER_EMISS          0x12 //������
#define CMD_LT_TEMP_USER_DISTANCE       0x13 //����
#define CMD_LT_CALCULATE_ENV_PARAMS     0x18 //���������������¼���
#define CMD_LT_WTR_LOW_THRESHOLD        0x1d
#define CMD_LT_WTR_HIGH_THRESHOLD       0x1e
#define CMD_LT_TEMP_SWITCH              0x71 //�¶ȳ���Tֵ�л�ʹ��
#define CMD_LT_TEMP_WTR                 0xf0
#define CMD_LT_TEMP_DISPLAY_STYLE       0x02 //�¶���ʾ��ʽ��0x00-���϶� C��0x01-�����ģ�273.15K����0x02-���ϣ�F��


typedef enum IR_FEATURE_e	//����ָ�����
{
	//  ������                                      ֵ   ����  ����˵��    �����ֽ���                 ˵����Ĭ�Ϸ��� OK��

	IR_FEATURE_USER_INFO_SN = 0,		//0   WR,��о��SN�� ����Ŀǰʹ��6λ    ���غ��ϴ�ʱ ͨ������ĳ����ж�ʹ�ó���
	IR_FEATURE_USER_INFO_PN,        //1   WR,��о��PN�� ����Ŀǰʹ��14λ   ���غ��ϴ�ʱ ͨ������ĳ����ж�ʹ�ó���  ������LB6230CTT20001
	IR_FEATURE_USER_INFO_WIDTH,        //2   R, ̽����FPA�Ŀ��   0            ֻ�����������ز���2�ֽ�
	IR_FEATURE_USER_INFO_HEIGHT,        //3   R, ̽����FPA�ĸ��   0            ֻ�����������ز���2�ֽ�
	IR_FEATURE_USER_TMPR_FPA,        //4   R, FPA�¶�           0            ֻ�����������ز���2�ֽ�  �¶�=��������/100
	IR_FEATURE_USER_TMPR_ENV,        //5   R, ��о�¶�          0            ֻ�����������ز���2�ֽ�  ��������=�¶�*100
	IR_FEATURE_USER_TEMPSENSOR_TYPE,			//6   R�� ���ز���һ�� ��0x88��ʾ�ڲ��´���0x80��ʾ�ⲿ�´�
	IR_FEATURE_NUC_STATUS_EN = 14,//        			    //14   W, bit0 ͼ���ȶ����� 0 dis 1 en
	IR_FEATURE_USER_RESERVED_7_TO_15,		//7-15 Ԥ������

	IR_FEATURE_USER_SETT_UPLOAD = 16,        //16   Ԥ������
	IR_FEATURE_USER_SETT_SAVE,        //17  W, �������湦��      0            ���û����ò������浽flash����λ������ǰ���ñ���
	IR_FEATURE_USER_SETT_RESTORE_FACTORY,       //18  DO �ָ���������      0            ���������ò����ָ�����ǰ����
	IR_FEATURE_USER_SETT_RESTART,               //19  DO ��о����          0             ��о����
	IR_FEATURE_USER_SETT_BAUD,                  //20  DO �޸�ͨѶ������    1            ���� 0x01:�Զ�ʶ��(�ɿ�ȱ ����Ҫ����ʶ��ʧ��) 0x02:9600 0x04:19200 0x08 :38400 0x10:115200 0x20:921600

	IR_FEATURE_USER_NUC_AUTOSHUT,        //21 WR �����Զ�/�ֶ����� 1              ����1�ֽڣ�0���ֶ��� 1���Զ���
	IR_FEATURE_USER_NUC_SHUT_TYPE,        //22 WR �ֶ���������      1              ����1�ֽڣ�0/1������У�� 2������У��
	IR_FEATURE_USER_NUC_TIME_INTERVAL,        //23 WR �Զ�����ʱ����  1
	IR_FEATURE_USER_NUC_TEMP_CHANGE,        //24 WR �Զ������¶ȼ��  1

	IR_FEATURE_USER_TEC_CHANGE_EN = 25,        //25 WR TEC�Զ��л�����    1             ����1�ֽ�  0���� 1 ��
	IR_FEATURE_DRC_CLASS = 25,  		   					    //25   DO��  ����1����DRC����ѡ���ܹ�16��������dde��drc�в��ֲ���
																//���Ϊ0����ʹ���û��ɵ���������ֵ������ʹ��flash�е�1-16��Ԥ�趨ֵ��

																IR_FEATURE_USER_INFO_VERSION,        //26 R  ��ȡ��ǰ�豸�汾 
																IR_FEATURE_USER_DISCONNECT,        //27 DO  �Ͽ���������  �޷������� 
																R_FEATURE_USER_RESERVED_28_TO_32,          //29-41 Ԥ������	
																										   //IR_FEATURE_USER_TEST_IMAGE         ,      //28 DO  ����ͼ�񿪹�      1             ����1�ֽ�  0���� 1 ��
																										   IR_FEATURE_DRC_DDE_DETAIL_GAIN = 30,        				//30  W,����1��������ϸ������ϵ����5λ������3λС��
																										   IR_FEATURE_DRC_AGC_HIGHTHROWPERCENT_SET = 33,			//33  W,����1����������ֱ��ͼ�ĸ��ױ���lowThrowPercent,��λ��1/1024.
																										   IR_FEATURE_DRC_AGC_HPLATEAUVALUE = 36,					//36  W,PE�ĸ�ƽֵ̨
																										   IR_FEATURE_DRC_AGC_LPLATEAUVALUE = 37,					//37  W,PE�ĵ�ƽֵ̨
																										   IR_FEATURE_DRC_AGC_ITTMEAN = 38,							//38  W,  1������ ITTMEAN
																										   IR_FEATURE_DRC_AGC_MAXGAIN_SET = 40,						//40  W,����AGC�в��ұ���������
																										   IR_FEATURE_USER_RESERVED_38_TO_41,          //38-41 Ԥ������

																										   IR_FEATURE_USER_VIDEO_ZOOM = 42,        //42  WR ���ӱ䱶          1             0:���䱶 1��2�� 2��4�� 3��8��
																										   IR_FEATURE_USER_VIDEO_CROSS_EN,        //43  WR ʮ�ֹ�꿪��      1             0����äԪ�� 1 ��äԪ�� 0x02���û����أ� 0x03���û���꿪
																										   IR_FEATURE_USER_VIDEO_CROSS_MOVE,        //44  DO ʮ�ֹ�궯��      1             äԪ�궨 0������ 1������ 2������ 3������ 4������ ���λBIT7 0 ��С���� 1 ��󲽷���  �û���� 5������ 6������ 7������ 8������ 9������ ���λBIT7 0 ��С���� 1 ��󲽷�
																										   IR_FEATURE_USER_VIDEO_COLORMAP,        //45  WR ��ɫ������        1             ��������COLORMAP_TYPE_e
																										   IR_FEATURE_USER_DIGITAL_SOURCE,        //46 WR ������ƵԴ        1               ��������DIGITAL_SOURCE_e
																										   IR_FEATURE_USER_DIGITAL_TYPE,        //47 WR ������Ƶ����ӿ�  1               ��������DIGITAL_TYPE_e 
																										   IR_FEATURE_USER_VIDEO_FLIP,		//48 WR ͼ��ת          1              8bit���� ���� bit0 = 1����ת��bit1=1���ҷ�ת��bit2=1���£�bit3=1��������
																										   IR_FEATURE_USER_BF_EN,        //49 WR �˲�����            1            ˫���˲����أ���λ����ʾ�˲����ع��ܣ� 0 �� 1��
																										   IR_FEATURE_USER_AGC_FROZEN,        //50  DO ����һ��00 ���� 01 ������� 
																										   IR_FEATURE_USER_OSD_VIDEO_EN,        //51 WR ģ����Ƶ���� just for LT
																										   IR_FEATURE_USER_DIGITAL_TYPE_LT,        //52 WR LT��Ƶ
																										   IR_FEATURE_USER_RESERVED_52_TO_57,        //53-57 Ԥ������

																										   IR_FEATURE_USER_AGC_MODE = 58,   	    //58 WR��AGCģʽ��        1               ��������AGC_ALGORITHM_e��0�ֶ���1�Զ�0��PE����2�Զ�1(LE)��
																										   IR_FEATURE_USER_AGC_CONTRAST,   	    //59 WR,���öԱȶ�        1               ��Χ0-255
																										   IR_FEATURE_USER_AGC_BRIGHT,	   	//60  WR, �������ȣ�       2               ��Χ0-511
																										   IR_FEATURE_USER_AGC_GAMMA = 61,		//61  WR, GAMMAУ��        1               ����gammaУ��ָ����������ָ������10������λ0.1 ���������ʾ 1.1 �·� 11
																										   IR_FEATURE_USER_AGC_ANALOG_ON_OFF = 61,		//61 ģ����Ƶ���� 
																										   IR_FEATURE_USER_DDE_EN,		//62  WR  DDE״̬          1               0���� 1����
																										   IR_FEATURE_USER_DDE_GRADE,        //63  WR  DDE��λ          1               Ŀǰ��Χ0-7 ������չ
																										   IR_FEATURE_USER_AGC_CONTRAST_STEP,        //64 �Աȶ�����ֵ
																										   IR_FEATURE_USER_AGC_BRIGHTNESS_STEP,        //65 ���Ȳ���ֵ
																										   IR_FEATURE_OSD_CURSOR_MOVE = 68,
																										   IR_FEATURE_USER_RESERVED_66_TO_80,        //66-80 Ԥ������

																										   IR_FEATURE_USER_BLIND_SCAN = 81,		//81  DO   äԪɨ��        0              
																										   IR_FEATURE_USER_BLIND_OP,        //82  DO   äԪ���ز���    1               0�����õ�ǰäԪ 1�������ϴ����� 2�� ����äԪ��FLASH

																										   IR_FEATURE_USER_K_IMAGE,       //83  DO  ��ȡ��ǰͼ������ 1               ������ΪDOʱ ��ȡ��ǰͼ��Ϊ 0������ 1���������� ����Kֵ
																										   IR_FEATURE_USER_K_CAL,       //84  DO  �Զ�����Kֵ
																										   IR_FEATURE_USER_K_OP,       //85  DO  �û�K����ز���  1               0�����浱ǰKֵ 1���ָ�����Kֵ ������ �ϴ�/���� Kֵ �ݲ�ʹ�� Ԥ����չ��

																										   IR_FEATURE_USER_FIRM_UPLOAD,       //86  W/D  �̼�����      W:128/D:2      �̼����´�С(KB)����=��С*8 ����ʵ�� ��λ������ ��ָ��DO ���سɹ�֮�󣬿�ʼʹ�ø�ָ���W ���� ÿ�η���128B���� ������8*��С ��

																										   IR_FEATURE_USER_FIRM_UPLOADK,       //87  W/R/D  ���ٹ̼�����   W/R/D          �̼����´�С(KB)����=��С ����ʵ�� ��λ������ ��ָ���W���� ���ʹ�����һ��1K�� ���سɹ�֮�󣬿�ʼ���ز��� ÿ�η���1KB���ݣ�ֻ�й̼����ݣ� �����ʹ�С ��
																																			   //������ɺ󣬷���ָ���Read������ȡ��λ������У��(4�ֽ�У��)��У��ʧ����ʾ�������أ��ɹ����棬�·�DO�������浽flash
																																			   IR_FEATURE_USER_RESERVED_88_TO_99,       // 88-99 Ԥ������

																																			   IR_FEATURE_USER_KEY = 100,       // 100 DO  ���ⰴ��        1               ���λbit7 ��Ϊ������� ����1 ���� 0  ��λ��Ӧ bit0:M�� bit1: +��  bit2:-�� bit3:C��
																																			   IR_FEATURE_USER_BLACKFACE_SWITCH = 124,       // 100 DO  ���ⰴ��        1               ���λbit7 ��Ϊ������� ����1 ���� 0  ��λ��Ӧ bit0:M�� bit1: +��  bit2:-�� bit3:C��
																																			   IR_FEATURE_USER_BLACKFACE_TEMP = 125,       // 100 DO  ���ⰴ��        1               ���λbit7 ��Ϊ������� ����1 ���� 0  ��λ��Ӧ bit0:M�� bit1: +��  bit2:-�� bit3:C��
																																			   IR_FEATURE_USER_BLACKFACE_LOCATION = 126,       // 100 DO  ���ⰴ��        1               ���λbit7 ��Ϊ������� ����1 ���� 0  ��λ��Ӧ bit0:M�� bit1: +��  bit2:-�� bit3:C��
																																			   IR_FEATURE_USER_RESERVED_101_TO_159, //101-159 ������������Ԥ��Ԥ������
																																			   IR_FEATURE_NS_FLASH_COLORMAP = 140,        				//140 10��ɫ��
																																			   IR_FEATURE_NS_FLASH_LOGO = 141,//LOGO����
																																			   IR_FEATURE_TEMPER_ALARM_EN = 160,      // 160 wr  ���±���ʹ��     1               0�� 1��
																																			   IR_FEATURE_NS_AUTO_CALC_GG,							//161	��ȡ���� W��0��ȡ�����㣬1���� 2���
																																			   IR_FEATURE_MOTOR_MOVE = 162,     //162  DO  ��ͷ�������      4              ����1 1�ֽ� ֵ0��ֹͣ 1 ��ת 2 ��ת ������2 1���ֽ� ����PWMռ�ձ� ����3 2���ֽ� ���ò��� �����ת��������
																																			   IR_FEATURE_NS_CALC_GG_PARA = 162,//162 д �� ����һ  1.����_ϵ��A  2.���� _ϵ��B  3.����_Range 4.�����²����� LimitMaxT
																																												//���� ����һ 1. ��ȡ_A 2.��ȡ_B  3. ��ȡ_renge 4.��ȡ�¶�����MaxT

																																												IR_FEATURE_RTD323_FPA_AVERAGE = 196, 					 //196(ԭ176)	 R,��ȡ����ƽ��ֵ��count��	 2���ֽ�
																																												IR_FEATURE_RTD323_TEC_TEMP = 206,				 //206(ԭ222)		 WR,����TEC�¶�

																																												IR_FEATURE_RESERVED_163_TO_255,     //163 �ͻ����ƹ��� Ԥ������

} IR_FEATURE_USER, *P_IR_FEATURE_USER;


#pragma pack (1) 
struct DeviceInfo
{
	char szServerName[50];
	char cIP[16]; //the Device IP
};

struct ChannelInfo
{
	int channel;
	char szServerName[50];
	char szUserName[30];
	char szPWD[30];
	char szIP[30];
	WORD wPortNum;
	BYTE byChanNum;
	BYTE byChanNum2;
	int iStreamNum;
	int iStreamNum2;
};

#define MAX_DEVICE_NUM	100
struct DeviceList
{
	int iNumber;      //Device Count
					  //DeviceInfo DevInfo[MAX_DEVICE_NUM];
	ChannelInfo DevInfo[MAX_DEVICE_NUM];
};


struct INF_OSD
{
	INF_OSD(BOOL temp_add, BOOL temp_param_add, BOOL pseudo_add, BOOL lhtp)
	{
		m_g_temp_add = temp_add;
		m_g_temp_param_add = temp_param_add;
		m_pseudo_add = pseudo_add;
		m_low_hight_temp_pos = lhtp;
	}
	BOOL m_g_temp_add;
	BOOL m_g_temp_param_add;
	BOOL m_pseudo_add;
	BOOL m_low_hight_temp_pos;
};

typedef struct
{
	int max_temp;
	int min_temp;
	int avg_temp;
	int cen_temp;
}global_temp;

typedef struct
{
	int emissivity;
	int airTemp;
	int reflectTemp;
	int humidity;
	int distance;
} envir_param; //Parameters are actual values * 10000

typedef struct
{
	int iFormat;//1:Center 2:Align left  3:Align right
	int iFormatTime;//0:off
				    //1:2020 - 07 - 20 16 : 18 : 30
		            //2 : 2020 - 07 - 20 FRI 16 : 18 : 30
		            //3 : 07 - 20 - 2020 16 : 18 : 30
		            //4 : 07 - 20 - 2020 FRI 16 : 18 : 30
		            //5 : 20 - 07 - 2020 16 : 18 : 30
		            //6 : 20 - 07 - 2020 FRI 16 : 18 : 30
					//C:TimeEnable  0:off  1:on
	int iShow;  //0:Do not show  1:show   C:TitleEnable   0:off  1:on
	int iIndex; //0:time 1/2/3:Custom string
	char m_szString[200];//Content   B:utf-8
	int iWidth; //String width
	int iDeviceWidth; //Area array width
	int iDeviceHeight;  //Area array height
	int iX; //time coordinate
	int iY; //time coordinate
	int iStringX; //string coordinate
	int iStringY; //string coordinate
}Custom_String;//Overlay Custom string

typedef struct
{
	int iTempAvg;//Average regional temperature   Celsius*10
	int iTempCenter;  //Regional center point temperature
	int iTempMax; //Maximum regional temperature
	int iTempMin; //Minimum regional temperature
	int iMaxPosX; //Maximum X coordinate
	int iMaxPosY; //Maximum Y coordinate
	int iMinPosX; //Minimum X coordinate
	int iMinPosY; //Minimum Y coordinate
}Area_Temp;//AT Area Temp

#define AREA_NUM 7
typedef struct
{
	bool bEnable;//true: on   false: off
	int iIndex; //index
	int iMode; //0:point   1:line   2:area
	int iStartPosX; //Start X coordinate
	int iStartPosY; //Start Y coordinate
	int iEndPosX; //End X coordinate
	int iEndPosY; //End Y coordinate
	bool bHighAlarm;//High temperature alarm     true:on   false:off
	bool bHighAlarmLevel;//High temperature alarm level     true:on   false:off
	bool bLowAlarm;//Low temperature alarm     true:on   false:off
	bool bLowAlarmLevel;//Low temperature alarm level     true:on   false:off
	bool bPopup;//Pop-ups     true:on   false:off
	bool bRing;//Ring     true:on   false:off
	bool bSnap;//snap     true:on   false:off
	int iDebounce; // debounce time unit:s
	int iHighTempThres; //High temperature alarm threshold				 Kelvin*10
	int iHighTempThresLevel1; //High temperature alarm level1 threshold
	int iHighTempThresLevel2; //High temperature alarm level2 threshold
	int iHighTempThresLevel3; //High temperature alarm level3 threshold
	int iLowTempThres; //Low temperature alarm threshold
	int iLowTempThresLevel1; //Low temperature alarm level1 threshold
	int iLowTempThresLevel2; //Low temperature alarm level2 threshold
	int iLowTempThresLevel3; //Low temperature alarm level3 threshold
}Temp_Config;//AT Temperature measurement configuration

typedef struct
{
	int iIndex; //index
	int iPosY; //Y coordinate
	int iPosX; //X coordinate
}Point_pos;//AT point position

typedef struct
{
	int iMode;// AT : 0:  point   1:line     2:area    AT20: 0:area,1:line,2:point
	int iIndex; //index
	int iStartPosX; //StartX coordinate
	int iStartPosY; //StartY coordinate
	int iEndPosX; //EndX coordinate
	int iEndPosY; //EndY coordinate
	int iDeviceWidth; //Area array width
	int iDeviceHeight;  //Area array height

	int                 m_region_enable; // Regional temperature measurement enable
	int                 m_osd_enable;    // Area temperature measurement osd superposition enable
	int                 m_cursor_enable; // Regional temperature measurement high and low temperature cursor superimposition enable
	int                 m_param_enable;  // Regional temperature measurement using local parameters
}Area_pos;//area position

typedef struct
{
	int iType;  // 0: point   1: line   2: area
	int iIndex; // index
	int iAlarmType;//0: high   1:low
	int iLevel; // alarm level
	int iTime;    //Timestamp
	char* alarmTime;    //Time C
}Alarm_Info;

typedef struct
{
	unsigned char       m_starthour;        // Start Hour
	unsigned char       m_startmin;         // Start Minute
	unsigned char       m_stophour;         // Stop Hour
	unsigned char       m_stopmin;          // Stop Hour
	unsigned char       m_maskweek;         // Week
	unsigned char       bReceive[3];        // Reserved
} Time_Check;

typedef struct
{
	Time_Check          pList[7];           // Time Table
} Time_List_capture;

typedef struct
{
	int						bEnable;        // true: coding on   false: coding off
	int						iCompress;      // Encoding   5:H264   8:H265
	int						iBrc;           // Rate control mode   0:VBR   1:CBR
	int						iImageSize;     //Resolution   5:384*288   9:640*512    16:1024*768     22:1280*1024
	int						iImageQuality;  //Image quality level   1-6(6:the best)  VBR or Snapshot mode is valid
	int						iBitRate;       //Bit rate
	int						iGOP;           //Reserved
	int						iRefMode;       //Reserved
	int						iEnhanceVBRMode;//Reserved

	int						m_ftp_timecapjpg;       // regular capturing FTP uploading
	int						m_CapJpgInterval;       // interval time for regular capturing
	Time_List_capture       m_timeList;             // capture time
}Encoding_Format;

typedef struct
{
	char m_StartTime[10];        // Start time
	char m_EndTime[10];        // End time
}Time_List;

typedef struct
{
	Time_List time_list[5]; //5 time periods
}Schedule;

typedef struct
{
	int bMainEnable;        // true: main stream on   false: main stream off
	int bExtraEnable;       // true: Extra stream on   false: Extra stream off
	Schedule list[7];        // start from sunday
}Recoding;

typedef struct
{
	int iMode;               //0:point   1:line     2:area     3:full frame
	int iIndex;              //index
	int bHighAlarmEnable;   // true: High temperature alarm on   false: High temperature alarm off
	int bLowAlarmEnable;    // true: Low temperature alarm on   false: Low temperature alarm off
	int bHighAlarmLevelEnable;   // true: High temperature alarm level on   false: High temperature alarm level off
	int bLowAlarmLevelEnable;    // true: Low temperature alarm level on   false: Low temperature alarm level off
	int bPopupEnable;       // true: Pop-ups on   false: Pop-ups off
	int bAudioEnable;       // true: ring on   false: ring off
	int bSnapEnable;        // true: snap on   false: snap off
	int iDebounce;           // debounce time unit:s
	int iHighTempThreshold;  //High temperature threshold   A&C:C*10   B:K*10
	int iHighTempThresholdLevel1;  //High temperature threshold Level1   A:C*10   B:K*10
	int iHighTempThresholdLevel2;  //High temperature threshold Level2   A:C*10   B:K*10
	int iHighTempThresholdLevel3;  //High temperature threshold Level3   A:C*10   B:K*10
	int iLowTempThreshold;   //Low temperature threshold   A:C*10   B:K*10
	int iLowTempThresholdLevel1;  //Low temperature threshold Level1   A:C*10   B:K*10
	int iLowTempThresholdLevel2;  //Low temperature threshold Level2   A:C*10   B:K*10
	int iLowTempThresholdLevel3;  //Low temperature threshold Level3   A:C*10   B:K*10
	int iStartPosX; //Start X coordinate
	int iStartPosY; //Start Y coordinate
	int iEndPosX; //End X coordinate
	int iEndPosY; //End Y coordinate

	int                	m_enable;           // function enable
	int					m_typethreshold;	// 0: >m_htempthreshold, 1: <m_ltempthreshold, 2: [m_ltempthreshold, m_htempthreshold], 3: not belong to[m_ltempthreshold, m_htempthreshold]
	int                 m_singlehost;       // label of center for uploading alarm signal
	unsigned char       m_record[16];       // relate to recording
	unsigned char       m_out[8];           // relate to output	1:ON, 0��OFF
	unsigned char       m_enpreno[16];      // relate to preset point
	unsigned char       m_preno[16];        // number of preset point to be called
	unsigned char       m_capjpeg[16];      // relate to capturing
	Time_List_capture   m_timelist;         // detection time table define ETI_TIME_DAYTYPE

	//C use
	int FliterTime;  //Filter time
	int LinkageType[7];  //Linkage type  0:alarm1  1:flash   2:snap  3:record   4: upload ftp   5:SMTP   6:alarm2
	int MoreOrLess;   //alarm type      0:The Max is less than, 1:The Max is more than, 2:The Min is less than��3:The Min is more than��4:The Avg is less than��5:The Avg is more than
	int SlowMovingTemp;  //Hysteresis temperature  C*10
}Alarm_Config;

typedef struct
{
	int                 m_year;             // Year
	unsigned char       m_month;            // Month
	unsigned char       m_dayofmonth;       // Day
	unsigned char       m_hour;             // Hour
	unsigned char       m_minute;           // Minute
	unsigned char       m_second;           // Second
} Time_Param;

typedef struct
{
	int enbale;  //true:enable
	char* Encrypt;  //Encryption  NULL or WPA-PSK
	char* id;  //ID
	char* pwd;   //password
}Wlan_Config;

#define USER_NUM 100
typedef struct
{
	int id;  //true:enable
	char* Description;  //Description
	char* GroupName;  //Group Name
	char* ModifiedTime;   //Modified Time
	char* UserName;   //User Name
}User_Info;

typedef struct
{
	int groupId;  //group id
	char* Description;  //Description
	char* Password;  //Password
	char* UserName;   //User Name
}User_New;

typedef struct
{
	int LoginTime;  //Login Time
	int UpdateTime;  //Update Time
	char* Address;  //Address
	char* Token;  //Token
	char* UserName;   //User Name
}User_Online;

typedef struct
{
	int id;  //id
	char* oldPasswd;  //oldPasswd
	char* newPasswd;  //newPasswd
	char* Description;   //Description
}User_Modify;

typedef struct
{
	int Camera;  //Camera settings
	int Network;  //Network settings
	int Osd;  //osd settings
	int Peripherals;  //Peripheral management
	int Storage;  //Storage management
	int System;  //System management
	int SystemMaintain;  //system maintenance
	int Temperature;  //Temperature measurement setting
	int Playback[1]; //Playback management
	int RealMonitor[2];  //real time monitoring
}Group_Authority;

typedef struct
{
	int id;  //id
	char* GroupName;  //group Name
	char* Description;   //Description
	Group_Authority group_authority;
}Group_Info;

typedef struct
{
	char* DeviceName;  //Device Name
	char* Language;   //Language
	char* VideoFormat;   //Video Format
}Device_Setting;

typedef struct
{
	char* EmergencyVideoPath;  //Emergency Video Path
	char* RecordPath;   //Record Path
}Record_Path;

typedef struct
{
	int CutSize;  //Video division size (MB)
	int CutTime;  //Video segmentation time (minutes)
	int CutType;  //Video segmentation type (0: by time, 1 by size, 2: both)
	int RecChn;   //Video channel: 0 means visible light, 1 means infrared, 2 means visible light + infrared
	int RecType;  //Video type (for extension, currently 0, MP4)
	int UseRec;   //Recording enable (1: yes, 0: no)
}Record_Param;

typedef struct
{
	int Begin;  //Begin time
	int Channel;  //Chn selection of video, 0 is visible light, 1 is infrared
	int End;  //end time
	int Path;   //Video storage path (0: video storage directory, 1: emergency video storage directory)
	int Type;  //File type (0: video, 1: picture)
	char* Name[];   //file name
}Record_Search;

typedef struct
{
	int Chn;  //snap channel: 0 means visible light, 1 means infrared, 2 means visible light + infrared
	int Enable;  //snap enable (1: yes, 0: no)
	int Interval;  //Time interval for snap, in seconds
}Snap_Param;

typedef struct
{
	char* AdministrativeDivisionCode;  //Administrative division code
	char* AlarmChNo;				   //Alarm channel number
	int AlarmIndex;					   //Alarm number: 1, 2
	int AlarmLevel;					   //Alarm level: 1, 2, 3, 4, 5, 6
	int ChAlarmLevel;				   //Alarm level: 1, 2, 3, 4, 5, 6
	int ChIndex;					   //Channel number: 1, 2
	char* ChNo;						   //Channel No
	char* DeviceNo;					   //Device No
	int Enable;						   //Enable switch
	int HeartMaxOut;				   //Maximum number of heartbeat timeouts
	int HeartTime;					   //Heartbeat cycle (unit: s)
	char* IdentificationCode;		   //Access module identification code
	int LocalSipPort;				   //Local SIP server port
	char* Password;					   //sign up password
	int RegisterValid;				   //Registration Validity Period (Unit: s)
	char* SipDomain;				   //sip domain
	char* SipServerIp;				   //sip server ip
	char* SipServerNo;				   //SIP server number
	int SipServerPort;				   //sip server port
}GB28181_Param;

typedef struct
{
	int iMode;// 0:  point   1:line     2:area    3:frame
	int iStartPosX; //StartX coordinate
	int iStartPosY; //StartY coordinate
	int iEndPosX; //EndX coordinate
	int iEndPosY; //EndY coordinate
	int iMax; // Max  K*10
	int iMin;// Min  K*10
	int iAvg;//Avg K*10
	int iMaxPointX;//max coordinate x
	int iMaxPointY;//max coordinate y
	int iMinPointX;//min coordinate x
	int iMinPointY;//min coordinate y
}Position_info;

typedef struct
{
	int enable;// true: temp data open   false: temp data close
	int fps; //Temperature data frame rate
}TempData_Param;

typedef struct
{
	short version;  // �ļ��汾
	unsigned short width;  //�¶ȵ�����
	unsigned short height;   //�¶ȵ���߶�
	unsigned char createTime[14]; //����ʱ��

	int emiss; //������  *10000
	int airTemp; //�����¶�
	unsigned char lenCamera;  //��ͷ����
	unsigned int distance;  //�������  *10000
	unsigned char relativeHumidity;  //���ʪ��
	int refTemp;  //�����¶�   *10000
	unsigned char productcor[32];  //��������
	unsigned char type[32]; //��Ʒ�ͺ�
	unsigned char serialNo[32]; //��Ʒ���к�
	int longitude;//����  *10000
	int latitude;  //γ��   *10000
	int altitude; //����   *10000
	int remarkLen;  //��ע��Ϣ����
	unsigned char remark[256];
	int atmTrans; //͸����   *10000
	int K0; //K0  *10000
	int B0; //B0  *10000
	int K1; //K1  *10000
	int B1; //B1  *10000
	int pseudoColorIndex;//Pseudo-color number
	int iOffset;
}JPG_Param;

typedef struct
{
	int emissivity;
	int airTemp;
	int reflectTemp;
	int humidity;
	int distance;
	int atmTrans;
	int K0;
	int B0;
	int K1;
	int B1;
} JPG_envir_param; //Parameters are actual values * 10000

typedef struct
{
	int max;       //K*10
	int min;       //K*10
	int frameMax;  //K*10
	int frameMin;  //K*10
	int width;
	int height;
	int pseudoColorIndex;//Pseudo-color number
} Stretch_param;

typedef struct
{
	unsigned short width;  //width
	unsigned short height;   //height

	int emiss; // Emissivity*10000
	int airTemp; //Ambient temperature*10000
	unsigned int distance;  //distance *10000
	unsigned char humidity;  //Humidity*10000
	int refTemp;  //Reflected temperature*10000
	int atmTrans; //Atmospheric transmittance*10000

	int K0; //K0  *10000
	int B0; //B0  *10000
	int K1; //K1  *10000
	int B1; //B1  *10000
	int pseudoColorIndex;//Pseudo-color number
	int unit;  //temp unit
	int gainStatus;  //0:highGain     1:lowGain
}IRG_Param;

#define GRIDS_RGN_MAX_NUM   64  //The maximum number of grids is 64, which is equivalent to 64 regional temperature measurements
typedef struct
{
	char    name[16];           //rgn name
	char    osdType;            //0: not superimposed; 1: lowest temperature; 2: highest temperature; 3: average temperature
	char    osdTrackTempEn;     //Temperature point tracking overlay enable:   0: No overlay; 1: Low temperature point tracking (not supported); 2: High temperature point tracking
	char    reserve[6];
	int     overhtempthreshold; //Super maximum temperature stacking threshold   C*10
}Grids_Attrs;

typedef struct
{
	short           mode; //AGC_GRID_MODE_E 0:disable, 1:4x4, 2:6x6, 3:8x8
	short           reserve;
	Grids_Attrs     gridsAttr[GRIDS_RGN_MAX_NUM];
}Grids_Param;

typedef struct
{
	int               m_maxtemp;          //max C*10
	int               m_mintemp;          //min C*10
	int               m_avgtemp;          //avg C*10
}Grids_Temp_Value;

typedef struct
{
	Grids_Temp_Value      m_region_temp[GRIDS_RGN_MAX_NUM]; //Zone temperature information, number of extended zones 64
}Grids_Temp;

#pragma pack ()
