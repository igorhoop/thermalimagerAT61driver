// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 INFRAREDTEMPSDK_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// INFRAREDTEMPSDK_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。

// #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
// 	#ifdef INFRAREDTEMPSDK_EXPORTS
// 	#define INFRAREDTEMPSDK_API __declspec(dllexport)
// 	#else
// 	#define INFRAREDTEMPSDK_API __declspec(dllimport)
// 	#endif
// #else
// 	
// #endif


//#include "afxwin.h"
#include "InfraredTempSDK_def.h"
#include "IRNet.h"
#include "VSNETStructDef.h"

#include "HyvDataApp.h"
#include "HyvStream.h"
#include "HyvSNMac.h"
#ifdef JUST_WIN32
#include "CamSearchLib.h"
#include "HyvAlarmIsotherm.h"
#endif

#include <list>

#ifdef JUST_LINUX
#ifndef Infrared_PRT
#define Infrared_PRT(fmt...)   \
    do {\
        printf("[%s]-%d: ", __FUNCTION__, __LINE__);\
        printf(fmt);\
    }while(0)
#endif
#endif



using namespace std;
#ifdef JUST_WIN32
#ifndef WM_STARTUPCALLBACK
#define WM_STARTUPCALLBACK (WM_USER + 100)
#endif
#elif defined(JUST_LINUX)
#ifndef WM_STARTUPCALLBACK
#define WM_STARTUPCALLBACK (100)
#endif
#endif // JUST_WIN32

#ifdef SYS_LINUX
extern  char _binary_outPaletteFTII_dat_start[];
extern  int  _binary_outPaletteFTII_dat_size;
#endif

typedef void(*VideoCallBack0)(char *pBuffer, long BufferLen, int width, int height, void *pContext);
typedef void(*TempCallBack)(char *pBuffer, long BufferLen, void *context);
typedef void(*SerialCallBack)(char *pRecvDataBuff, int BuffSize, void *context);
typedef void(WINAPI *MessageCallBack)(IRNETHANDLE hHandle, WPARAM wParam, LPARAM  lParam, void *context);
typedef void(*AlarmCallBack)(char* message, void *context);
typedef void(*SnapCallBack)(int m_ch, char *pBuffer, int size, void *context);

/**
* @brief set device type
* @param[in] iType  0:A  1:B   2:C
* @param[in] UserName     User Name
* @param[in] Password     Password
* @return
*/
INFRAREDTEMPSDK_API void sdk_set_type(int iType, char* UserName, char* Password);

/**
* @brief Initialize the SDK
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_initialize();

INFRAREDTEMPSDK_API IRNETHANDLE sdk_create();

INFRAREDTEMPSDK_API int sdk_loginDevice(IRNETHANDLE hHandle, ChannelInfo stinfo);

/**
* @brief release the SDK
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_release(IRNETHANDLE p);

/**
* @brief search device
* @param[in] devLst  Device structure pointer
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_search_device(IRNETHANDLE p, DeviceList &devLst);

INFRAREDTEMPSDK_API void __stdcall SetMessageCallBack(IRNETHANDLE p, MessageCallBack pMessageCallBack, void *pContext);
INFRAREDTEMPSDK_API int __stdcall SetDeviceVideoCallBack(IRNETHANDLE p, VideoCallBack0 pVideoCallBack, void *pContext);
INFRAREDTEMPSDK_API int __stdcall SetTempCallBack(IRNETHANDLE p, TempCallBack pTempCallBack, void *pContext);
INFRAREDTEMPSDK_API int __stdcall SetSerialCallBack(IRNETHANDLE p, ChannelInfo stinfo, SerialCallBack pSerialCallBack, void *pContext);
INFRAREDTEMPSDK_API int __stdcall SetAlarmCallBack(IRNETHANDLE p, char* ip, AlarmCallBack pAlarmCallBack, void *pContext);
INFRAREDTEMPSDK_API int __stdcall SetSnapCallBack(IRNETHANDLE p, ChannelInfo stinfo, SnapCallBack pSnapCallBack, void *pContext);
INFRAREDTEMPSDK_API int __stdcall SetSnapGeneralCallBack(IRNETHANDLE p, ChannelInfo stinfo, SnapCallBack pSnapCallBack, void *pContext);
INFRAREDTEMPSDK_API int sdk_CapSingle(IRNETHANDLE p, ChannelInfo stinfo);

INFRAREDTEMPSDK_API int sdk_start_url(IRNETHANDLE p, char* ip);
INFRAREDTEMPSDK_API int sdk_stop_url(IRNETHANDLE p);

/**
* @brief Send data through serial port
* @param[in] p sdk_create return value
* @param[in] pSendBuff Send data pointer
* @param[in] BuffSize  Data length
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_serial_cmd_send(IRNETHANDLE p, char *pSendBuff, DWORD BuffSize);

/**
* @brief receive data through serial port
* @param[in] p sdk_create return value
* @param[out] pSendBuff receive data pointer
* @param[out] BuffSize  Data length
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_serial_cmd_receive(IRNETHANDLE p, char *pRecvBuff, int *BuffSize);

/**
* @brief modify device ip
* @param[in] p sdk_create return value
* @param[in] stinfo  device struct
* @param[in] DstIP  distance ip
* @param[in] port  port
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_set_device_ip(IRNETHANDLE p, ChannelInfo stinfo, const char* DstIP, int port);

/**
* @brief OSD switch
* @param[in] p sdk_create return value
* @param[in] stinfo  device struct
* @param[in] iSwitch 0:off 1:on
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_osd_switch(IRNETHANDLE p, ChannelInfo stinfo, int iSwitch);

/**
* @brief OSD Parameter overlay
* @param[in] p sdk_create return value
* @param[in] stinfo  device struct
* @param[in] osd_p Parameter struct
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_SetInfOsd(IRNETHANDLE p, ChannelInfo stinfo, const INF_OSD &osd_p);

/**
* @brief load OSD Parameter
* @param[in] p sdk_create return value
* @param[in] stinfo  device struct
* @param[in] iOSD  OSD switch
* @param[in] osd_p Parameter struct
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_LoadParamOsd(IRNETHANDLE p, ChannelInfo stinfo, int* iOSD, INF_OSD *osd_p);

/**
* @brief Environment variable settings
* @param[in] p sdk_create return value
* @param[in] stinfo  device struct
* @param[in] envir_data Environment variable struct
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_set_envir_param(IRNETHANDLE p, ChannelInfo stinfo, envir_param envir_data);

INFRAREDTEMPSDK_API int SetReflect(IRNETHANDLE p, ChannelInfo stinfo, signed int i32value);
INFRAREDTEMPSDK_API int SetAirTemp(IRNETHANDLE p, ChannelInfo stinfo, signed int i32value);
INFRAREDTEMPSDK_API int SetHumidity(IRNETHANDLE p, ChannelInfo stinfo, signed int i32value);
INFRAREDTEMPSDK_API int SetEmiss(IRNETHANDLE p, ChannelInfo stinfo, signed int i32value);
INFRAREDTEMPSDK_API int SetDistance(IRNETHANDLE p, ChannelInfo stinfo, signed int i32value);

/**
* @brief read Environment variable
* @param[in] p sdk_create return value
* @param[in] stinfo  device struct
* @param[out] envir_data Environment variable struct
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_get_envir_param(IRNETHANDLE p, ChannelInfo stinfo, envir_param* envir_data);

INFRAREDTEMPSDK_API int GetReflect(IRNETHANDLE p, ChannelInfo stinfo, signed int* p32value);
INFRAREDTEMPSDK_API int GetAirTemp(IRNETHANDLE p, ChannelInfo stinfo, signed int* p32value);
INFRAREDTEMPSDK_API int GetHumidity(IRNETHANDLE p, ChannelInfo stinfo, signed int* p32value);
INFRAREDTEMPSDK_API int GetEmiss(IRNETHANDLE p, ChannelInfo stinfo, signed int* p32value);
INFRAREDTEMPSDK_API int GetDistance(IRNETHANDLE p, ChannelInfo stinfo, signed int* p32value);

/**
* @brief Environment variables take effect
* @param[in] p sdk_create return value
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_envir_effect(IRNETHANDLE p);

/**
* @brief calibration
* @param[in] p sdk_create return value
* @param[in] stinfo  device struct
* @param[in] type
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_shutter_correction(IRNETHANDLE p, ChannelInfo stinfo, int type);

/**
* @brief Pseudo color switch
* @param[in] p sdk_create return value
* @param[in] stinfo  device struct
* @param[in] color_plate Pseudo color type
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_set_color_plate(IRNETHANDLE p, ChannelInfo stinfo, int color_plate);

/**
* @brief get Pseudo color no
* @param[in] p sdk_create return value
* @param[out] color_plate Pseudo color type
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_get_color_plate(IRNETHANDLE p, ChannelInfo stinfo, int *color_plate);

/**
* @brief read sn & pn
* @param[in] p sdk_create return value
* @param[in] stinfo  device struct
* @param[out] strSN SN
* @param[out] strPN PN
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_get_SN_PN(IRNETHANDLE p, ChannelInfo stinfo, char *strSN, char* strPN);

/**
* @brief read FPA temp
* @param[in] p sdk_create return value
* @param[out] fTemp FPA temp
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_get_FPA_temp(IRNETHANDLE p, float *fTemp);

/**
* @brief read camera temp
* @param[in] p sdk_create return value
* @param[out] fTemp camera temp
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_get_camera_temp(IRNETHANDLE p, float *fTemp);

/**
* @brief  read Area array width
* @param[in] p sdk_create return value
* @param[out] iValue Area array width
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_get_width(IRNETHANDLE p, int *iValue);

/**
* @brief read Area array height
* @param[in] p sdk_create return value
* @param[out] iValue Area array height
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_get_height(IRNETHANDLE p, int *iValue);

/**
* @brief read Temperature imaging status
* @param[in] p sdk_create return value
* @param[out] iValue Temperature imaging 0:off 1:on
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_get_TempImaging(IRNETHANDLE p, int *iValue);

/**
* @brief  get Temperature unit
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @param[out] iUnit      Temperature unit  0:Celsius   1:Kelvin    2:Fahrenheit
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_get_temp_unit(IRNETHANDLE p, ChannelInfo stinfo, int *iUnit);

/**
* @brief  set Temperature unit
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @param[in] iUnit      Temperature unit  0:Celsius   1:Kelvin    2:Fahrenheit
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_set_temp_unit(IRNETHANDLE p, ChannelInfo stinfo, int iUnit);

/**
* @brief  get Temperature measurement configuration
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @param[in] iIndex        region no
* @param[out] temp_config      Temperature config struct
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_get_temp_configuration(IRNETHANDLE p, ChannelInfo stinfo, int iIndex, Alarm_Config& temp_config);

/**
* @brief  set temp alarm
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @param[in] m_regionIndex        0:frame >0 region
* @param[in] alarm_config    alarm configuration
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_set_temp_alarm(IRNETHANDLE p, ChannelInfo stinfo, int m_regionIndex, Alarm_Config alarm_config);

/**
* @brief get area temperature(max min avg) B15_1.0.1_planA
* @param[in] p sdk_create return value
* @param[in] stinfo  device struct
* @param[in] iIndex  region no  7-frame
* @param[out] area_temp  area temperature struct
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_get_temp_data(IRNETHANDLE p, ChannelInfo stinfo,  int iIndex, Area_Temp &area_temp);

/**
* @brief  Convert to Celsius
* @param[in] iType product type 0:Human body temperature measurement  1:Industrial temperature measurement
* @param[in] iTempImaging Temperature imaging 0:off 1:on
* @param[in] usValue Input temperature  K*10
* @param[out] fTempC Celsius
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_Convert_to_Celsius(int iType, int iTempImaging, unsigned short usValue, float* fTempC);

/**
* @brief  read wtr status
* @param[in] p sdk_create return value
* @param[out] iStatus wtr status 0:off  1:on
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_get_wtr_status(IRNETHANDLE p, int* iStatus);

/**
* @brief  set wtr status
* @param[in] p sdk_create return value
* @param[in] iStatus wtr status 0:off  1:on
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_set_wtr_status(IRNETHANDLE p, int iStatus);

/**
* @brief  set wtr low Threshold
* @param[in] p sdk_create return value
* @param[in] iThreshold wtr low Threshold * 10000
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_set_wtr_low_threshold(IRNETHANDLE p, int iThreshold);

/**
* @brief  Get wtr low Threshold
* @param[in] p sdk_create return value
* @param[out] iThreshold wtr low Threshold * 10000
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_get_wtr_low_threshold(IRNETHANDLE p, int* iThreshold);

/**
* @brief  set wtr high Threshold
* @param[in] p sdk_create return value
* @param[in] iThreshold wtr high Threshold * 10000
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_set_wtr_high_threshold(IRNETHANDLE p, int iThreshold);

/**
* @brief  Get wtr high Threshold
* @param[in] p sdk_create return value
* @param[out] iThreshold wtr high Threshold * 10000
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_get_wtr_high_threshold(IRNETHANDLE p, int* iThreshold);

/**
* @brief  Get Image frame rate
* @param[in] p sdk_create return value
* @param[in] stinfo  device struct
* @param[out] iFrameRate Image frame rate
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_get_image_framerate(IRNETHANDLE p, ChannelInfo stinfo, int* iFrameRate);

/**
* @brief  Set Image frame rate
* @param[in] p sdk_create return value
* @param[in] stinfo  device struct
* @param[in] iFrameRate Image frame rate
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_set_image_framerate(IRNETHANDLE p, ChannelInfo stinfo, int iFrameRate);

/**
* @brief  Get temp frame rate
* @param[in] p sdk_create return value
* @param[in] stinfo  device struct
* @param[out] iFrameRate temp frame rate
* @return 0:success, -1:fail  1:not support
*/
INFRAREDTEMPSDK_API int sdk_get_temp_framerate(IRNETHANDLE p, ChannelInfo stinfo, int* iFrameRate);

/**
* @brief  Set temp frame rate
* @param[in] p sdk_create return value
* @param[in] stinfo  device struct
* @param[in] iFrameRate temp frame rate  C:1-10
* @return 0:success, -1:fail  1:not support
*/
INFRAREDTEMPSDK_API int sdk_set_temp_framerate(IRNETHANDLE p, ChannelInfo stinfo, int iFrameRate);

/**
* @brief  B series login device
* @param[in] p sdk_create return value
* @param[in] stinfo device struct
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_login(IRNETHANDLE p, ChannelInfo stinfo);
INFRAREDTEMPSDK_API int sdk_loginNew(IRNETHANDLE p, std::string ip);

/**
* @brief  B series logout device
* @param[in] p sdk_create return value
* @param[in] ip device ip
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_logout(IRNETHANDLE p, std::string ip);

/**
* @brief  Hardware IO output
* @param[in] p sdk_create return value
* @param[in] stinfo device struct
* @param[in] iSwitch  0:off   1:on
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_set_hw_io_output(IRNETHANDLE p, ChannelInfo stinfo, int iSwitch);

/**
* @brief  Overlay OSD date & title
* @param[in] p sdk_create return value
* @param[in] stinfo device struct
* @param[in] osdContent OSD content
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_set_osd_display(IRNETHANDLE p, ChannelInfo stinfo, Custom_String osdContent);

/**
* @brief  get Overlay OSD date & title
* @param[in] p sdk_create return value
* @param[in] stinfo device struct
* @param[out] osdContent OSD content
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_get_osd_display(IRNETHANDLE p, ChannelInfo stinfo, Custom_String* osdContent);

/**
* @brief  synchronised time
* @param[in] p sdk_create return value
* @param[in] stinfo device struct
* @param[in] timeData  time struct
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_synchronised_time(IRNETHANDLE p, ChannelInfo stinfo, Time_Param timeData);

/**
* @brief  Network device DHCP switch
* @param[in] p sdk_create return value
* @param[in] stinfo device struct
* @param[in] iSwitch  0;off 1:on
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_set_DHCP_on_off(IRNETHANDLE p, ChannelInfo stinfo, int iSwitch);

/**
* @brief  set capture format
* @param[in] p sdk_create return value
* @param[in] stinfo device struct
* @param[in] iFormat capture format     3:jpg   4:jpg+irg
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_set_capture_format(IRNETHANDLE p, ChannelInfo stinfo, int iFormat);

/**
* @brief  Thermal image capture
* @param[in] p sdk_create return value
* @param[in] stinfo device struct
* @param[in] iLocation  0:SD Card  1:local
* @param[in] strPath  iLocation=0,strPath="", iLocation=1,strPath=local path C:\AB\
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_snapshot(IRNETHANDLE p, ChannelInfo stinfo, int iLocation, char* strPath);

/**
* @brief  read Timed capture infomation
* @param[in] p sdk_create return value
* @param[in] stinfo device struct
* @param[out] data  details
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_get_timing_capture(IRNETHANDLE p, ChannelInfo stinfo, Encoding_Format* data);

/**
* @brief  read Timed capture infomation
* @param[in] p sdk_create return value
* @param[in] stinfo device struct
* @param[in] data  details
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_set_timing_capture(IRNETHANDLE p, ChannelInfo stinfo, Encoding_Format data);

/**
* @brief  read Timed recording information
* @param[in] p sdk_create return value
* @param[in] stinfo device struct
* @param[out] data  details
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_get_timing_recording(IRNETHANDLE p, ChannelInfo stinfo, Recoding* data);

/**
* @brief  read Timed recording information
* @param[in] p sdk_create return value
* @param[in] stinfo device struct
* @param[in] data  details
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_set_timing_recording(IRNETHANDLE p, ChannelInfo stinfo, Recoding data);


/**
* @brief  format SD card
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @param[in] iDiskNo   disk no
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_disk_format(IRNETHANDLE p, ChannelInfo stinfo, int iDiskNo);

/**
* @brief  set area position
* @param[in] p sdk_create return value
* @param[in] iIndex region index
* @param[in] stinfo        device struct
* @param[in] area_pos      area position struct
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_set_area_pos(IRNETHANDLE p, ChannelInfo stinfo, int iIndex, Area_pos area_pos);

INFRAREDTEMPSDK_API int sdk_set_area_pos_new(IRNETHANDLE p, ChannelInfo stinfo, int iIndex, Area_pos area_pos);

/**
* @brief  remove area position
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @param[in] iIndex      area index
* @param[in] iMode      0:  point   1:line     2:area
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_remove_area_pos(IRNETHANDLE p, ChannelInfo stinfo, int iIndex, int iMode);

/**
* @brief  Turn off the temperature alarm data push service
* @param[in] p sdk_create return value
* @return 0:success, -1:fail  1: not support
*/
INFRAREDTEMPSDK_API int sdk_close_alarm(IRNETHANDLE p);

/**
* @brief  analyze alarm infomation
* @param[in] p sdk_create return value
* @param[in] strAlarm  Alarm callback return value
* @param[out] alarm_info  Alarm_Info detail
* @return    alarm information struct
*/
INFRAREDTEMPSDK_API int sdk_analyze_alarm_info(IRNETHANDLE p, char* strAlarm, Alarm_Info* alarm_info);

/**
* @brief  reset param
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_reset_param(IRNETHANDLE p, ChannelInfo stinfo);

/**
* @brief  Get Wi-Fi hotspot information
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @param[out] wlan_config     wlan_config struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_get_wlan(IRNETHANDLE p, ChannelInfo stinfo, Wlan_Config* wlan_config);

/**
* @brief  set Wi-Fi hotspot information
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @param[in] wlan_config     wlan_config struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_set_wlan(IRNETHANDLE p, ChannelInfo stinfo, Wlan_Config wlan_config);

/**
* @brief  Get all user information
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @param[out] user_info     user_info struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_get_all_user_info(IRNETHANDLE p, ChannelInfo stinfo, User_Info user_info[USER_NUM]);

/**
* @brief  create new user
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @param[in] user_info     User_New struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_create_new_user(IRNETHANDLE p, ChannelInfo stinfo, User_New user_info);

/**
* @brief  Get a list of online users
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @param[out] user_info     User_Online struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_get_user_online(IRNETHANDLE p, ChannelInfo stinfo, User_Online user_info[USER_NUM]);

/**
* @brief  Get the user information of the specified ID
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @param[in] id            specified ID
* @param[out] user_info     user_info struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_get_user_info(IRNETHANDLE p, ChannelInfo stinfo, int id, User_Info* user_info);

/**
* @brief  Modify user information
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @param[in] user_info     user_info struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_modify_user_info(IRNETHANDLE p, ChannelInfo stinfo, User_Modify user_info);

/**
* @brief  delete user
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @param[in] id            specified ID
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_delete_user(IRNETHANDLE p, ChannelInfo stinfo, int id);

/**
* @brief  Get no operation timeout
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @param[out] timeout       no operation timeout
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_get_no_opr_timeout(IRNETHANDLE p, ChannelInfo stinfo, int* timeout);

/**
* @brief  Set no operation timeout
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @param[in] timeout       no operation timeout
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_set_no_opr_timeout(IRNETHANDLE p, ChannelInfo stinfo, int timeout);

/**
* @brief  Get all group information
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @param[out] group_info     Group_Info struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_get_all_group_info(IRNETHANDLE p, ChannelInfo stinfo, Group_Info group_info[USER_NUM]);

/**
* @brief  create new group
* @param[in] p sdk_create return value
* @param[in] stinfo              device struct
* @param[in] group_info     Group_Info struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_create_new_group(IRNETHANDLE p, ChannelInfo stinfo, Group_Info group_info);

/**
* @brief  Get the group information of the specified ID
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @param[in] id            specified ID
* @param[out] group_info     Group_Info struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_get_group_info(IRNETHANDLE p, ChannelInfo stinfo, int id, Group_Info* group_info);

/**
* @brief  Modify group information
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @param[in] id            specified ID
* @param[in] group_info     Group_Info struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_modify_group_info(IRNETHANDLE p, ChannelInfo stinfo, int id, Group_Info group_info);

/**
* @brief  delete group
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @param[in] id            specified ID
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_delete_group(IRNETHANDLE p, ChannelInfo stinfo, int id);

/**
* @brief  Get device configuration
* @param[in] p sdk_create return value
* @param[in] stinfo              device struct
* @param[out] device_setting     Device_Setting struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_get_device_setting(IRNETHANDLE p, ChannelInfo stinfo, Device_Setting* device_setting);

/**
* @brief  Set device configuration
* @param[in] p sdk_create return value
* @param[in] stinfo              device struct
* @param[in] device_setting     Device_Setting struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_set_device_setting(IRNETHANDLE p, ChannelInfo stinfo, Device_Setting device_setting);

/**
* @brief  delete group
* @param[in] p sdk_create return value
* @param[in] stinfo        device struct
* @param[in] filePath      Absolute path of video file
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_delete_record_file(IRNETHANDLE p, ChannelInfo stinfo, char* filePath);

/**
* @brief  Get video storage directory
* @param[in] p sdk_create return value
* @param[in] stinfo              device struct
* @param[out] record_path        Record_Path struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_get_record_path(IRNETHANDLE p, ChannelInfo stinfo, Record_Path* record_path);

/**
* @brief  Get recording parameters
* @param[in] p sdk_create return value
* @param[in] stinfo              device struct
* @param[out] record_param       Record_Param struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_get_record_param(IRNETHANDLE p, ChannelInfo stinfo, Record_Param* record_param);

/**
* @brief  Set recording parameters
* @param[in] p sdk_create return value
* @param[in] stinfo              device struct
* @param[in] record_param       Record_Param struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_set_record_param(IRNETHANDLE p, ChannelInfo stinfo, Record_Param record_param);

/**
* @brief  Video file retrieval
* @param[in] p sdk_create return value
* @param[in] stinfo              device struct
* @param[in] record_search       Record_Search struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_search_record_file(IRNETHANDLE p, ChannelInfo stinfo, Record_Search record_search, list<char*> &file);

/**
* @brief  Get snap parameters
* @param[in] p sdk_create return value
* @param[in] stinfo              device struct
* @param[out] snap_param         Snap_Param struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_get_snap_param(IRNETHANDLE p, ChannelInfo stinfo, Snap_Param* snap_param);

/**
* @brief  Set snap parameters
* @param[in] p sdk_create return value
* @param[in] stinfo              device struct
* @param[in] snap_param         Snap_Param struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_set_snap_param(IRNETHANDLE p, ChannelInfo stinfo, Snap_Param snap_param);

/**
* @brief  Get GB28181 configuration
* @param[in] p sdk_create return value
* @param[in] stinfo              device struct
* @param[out] snap_param         GB28181_Param struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_get_GB28181_config(IRNETHANDLE p, ChannelInfo stinfo, GB28181_Param* config_param);

/**
* @brief  Set GB28181 configuration
* @param[in] p sdk_create return value
* @param[in] stinfo              device struct
* @param[in] snap_param         GB28181_Param struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_set_GB28181_config(IRNETHANDLE p, ChannelInfo stinfo, GB28181_Param config_param);

/**
* @brief  System upgrade-upload remote upgrade package
* @param[in] p sdk_create return value
* @param[in] stinfo              device struct
* @param[in] format              format
* @param[in] file         remote upgrade package file
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_system_upgrade(IRNETHANDLE p, ChannelInfo stinfo, int format, char* file);

/**
* @brief  start record
* @param[in] p        sdk_create return value
* @param[in] stinfo   device struct
* @param[in] file     video file full path
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_start_record(IRNETHANDLE p, ChannelInfo stinfo, char* file);

/**
* @brief  stop record
* @param[in] p        sdk_create return value
* @param[in] stinfo   device struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_stop_record(IRNETHANDLE p, ChannelInfo stinfo);

/**
* @brief  get temp offline
* @param[in] p        sdk_create return value
* @param[in] file     file full path
* @param[out] pos_info     temp struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_get_temp_offline(char* file, Position_info& pos_info);

/**
* @brief  Thermal image capture
* @param[in] p sdk_create return value
* @param[in] stinfo device struct
* @param[in] strPath  strPath=local path C:\\1.jpg
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_snapshot_jpg(IRNETHANDLE p, ChannelInfo stinfo, char* strPath);

/**
* @brief  get temp data param
* @param[in] p sdk_create return value
* @param[in] stinfo device struct
* @param[out] tempdata_param  TempData_Param struct
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_get_temp_data_param(IRNETHANDLE p, ChannelInfo stinfo, TempData_Param* tempdata_param);

/**
* @brief  set temp data param
* @param[in] p sdk_create return value
* @param[in] stinfo device struct
* @param[in] tempdata_param  TempData_Param struct
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_set_temp_data_param(IRNETHANDLE p, ChannelInfo stinfo, TempData_Param tempdata_param);

/**
* @brief  open jpg
* @param[in] file   jpg file full path
* @param[in] jpg_param jpg_param struct
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_open_jpg_param(char* file, JPG_Param* jpg_param);

/**
* @brief  open jpg
* @param[in] file   jpg file full path
* @param[out] temp_data    temp data
* @param[out] image_data   image data
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_open_jpg_data(char* file, float* temp_data, unsigned char* image_data);

/**
* @brief  get temp offline
* @param[in] temp_data     temp data buffer
* @param[in] width         width
* @param[in] height        height
* @param[out] pos_info     temp struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_get_temp_offline_jpg(float* temp_data, int width, int height, Position_info& pos_info);

/**
* @brief  get temp offline
* @param[in] srcTempBuffer     src temp data
* @param[in] srcEvn_param      src Environment param
* @param[in] dstEvn_param      dst Environment param
* @param[out] dstTempBuffer    dst temp data
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_temp_data_correction(float srcTempBuffer, JPG_envir_param srcEvn_param, JPG_envir_param dstEvn_param, float* dstTempBuffer);

/**
* @brief  stretch temp
* @param[in] stretch_param     stretch param struct
* @param[in] image_data        src image data
* @param[out] out_data          dst image data
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_stretch_temp(Stretch_param stretch_param, unsigned char* image_data, unsigned char* out_data);


/**
* @brief  get irg file param
* @param[in] file     file full path
* @param[out] irg_param     IRG_Param struct
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_get_irg_param(char* file, IRG_Param* irg_param);

/**
* @brief  get irg file image&temp data
* @param[in] file     file full path
* @param[in] colorIndex     Pseudo-color number
* @param[out] temp_data    temp data   K*10
* @param[out] image_data   image data  YUYV
* @return 0:success, -1:fail, 1:not support
*/
INFRAREDTEMPSDK_API int sdk_get_irg_data(char* file, int colorIndex, unsigned short* temp_data, unsigned char* image_data);



/**
* @brief  get onvif port
* @param[in] p sdk_create return value
* @param[in] stinfo device struct
* @param[out] port  onvif port
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_get_onvif_port(IRNETHANDLE p, ChannelInfo stinfo, unsigned short* port);

/**
* @brief  set onvif port
* @param[in] p sdk_create return value
* @param[in] stinfo device struct
* @param[in] port  dst port
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_set_onvif_port(IRNETHANDLE p, ChannelInfo stinfo, unsigned short port);

/**
* @brief  save param
* @param[in] p sdk_create return value
* @param[in] stinfo device struct
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_save_param(IRNETHANDLE p, ChannelInfo stinfo);

/**
* @brief  get pseudo color picture
* @param[in] filename full path  C:\111.jpg
* @param[in] index    pseudo color index(WhiteHot;BlackHot;Rainbow;RainbowHC;Iron;Lava;Sky;MidGrey;RdGy;PuOr;Special;Red;IceFire;GreenRed;Special2;RedHot;GreenHot;BlueHot;Green;Blue;)
* @param[in] width    picture width
* @param[in] height    picture height   Multiples of 256
* @return 0:success, -1:fail
*/
INFRAREDTEMPSDK_API int sdk_get_pseudo_color_pic(char *filename, int index, int width, int height);

/**
* @brief  get grids param
* @param[in] p sdk_create return value
* @param[in] stinfo device struct
* @param[out] grids_param    Grids_Param struct
* @return 0:success, -1:fail, 1:unsupport
*/
INFRAREDTEMPSDK_API int sdk_get_grids_param(IRNETHANDLE p, ChannelInfo stinfo, Grids_Param& grids_param);

/**
* @brief  set grids param
* @param[in] p sdk_create return value
* @param[in] stinfo device struct
* @param[in] grids_param    Grids_Param struct
* @return 0:success, -1:fail, 1:unsupport
*/
INFRAREDTEMPSDK_API int sdk_set_grids_param(IRNETHANDLE p, ChannelInfo stinfo, Grids_Param grids_param);

/**
* @brief  get grids temp
* @param[in] p sdk_create return value
* @param[in] stinfo device struct
* @param[out] grids_temp    Grids_Temp struct
* @return 0:success, -1:fail, 1:unsupport
*/
INFRAREDTEMPSDK_API int sdk_get_grids_temp(IRNETHANDLE p, ChannelInfo stinfo, Grids_Temp& grids_temp);
