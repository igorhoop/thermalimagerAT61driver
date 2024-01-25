//IRNet.h
#ifndef __IRNET_H__
#define __IRNET_H__

#include "VSNETStructDef.h"

#if defined(PLATFORM_WIN_X64)
typedef __int64 INT_PFW, *PINT_PFW;
typedef unsigned __int64 UINT_PFW, *PUINT_PFW;

#else
typedef int INT_PFW, *PINT_PFW;
typedef unsigned int UINT_PFW, *PUINT_PFW;

typedef float FLOAT;
#endif // PLATFORM_WIN_X64

#ifndef CSTYLE
#ifdef _cplusplus
#define CSTYLE extern "C"
#else
#define CSTYLE
#endif
#endif//CSTYLE

#ifndef CNET_APIIMPORT
#if defined(WIN32)
#ifdef NET_API
#define CNET_APIIMPORT  CSTYLE __declspec(dllexport)
#else
#define CNET_APIIMPORT  CSTYLE __declspec(dllimport)
#endif // NET_API
#elif defined (SYS_LINUX)
#ifdef NET_API
#define CNET_APIIMPORT  CSTYLE __attribute__((visibility("default")))//__attribute__((__stdcall))
#else
#define CNET_APIIMPORT  CSTYLE 
#endif

#endif
#endif//CNET_APIIMPORT

#ifndef IRNET_HANDLE
#define IRNET_HANDLE
typedef void *IRNETHANDLE;
#endif // !IRNET_HANDLE

#ifndef IRNET_HANDLE_VALID
#define IRNET_HANDLE_VALID(handle) (handle != (IRNETHANDLE)-1)
#endif // !IRNET_FAILED

#ifndef CALLINGCONVEN
#ifdef WIN32
#define CALLINGCONVEN __stdcall
#else 
#define CALLINGCONVEN
#endif
#endif // !CALLINGCONVEN


#define LAUMSG_LINKMSG          1             ///< connect server link message

#define LAUMSG_ALARM            4             ///< sensor alarm
#define LAUMSG_OUTPUTSTATUS     5             ///< alarm out status

#define LAUMSG_SERVERRECORD     11            ///< server record status

#define LAUMSG_DISKFULL                23     ///<disk full
#define LAUMSG_DISKERROR               24     ///<disk error
#define LAUMSG_ACCESSVIOLATION         25     ///<access violation

#define LAUMSG_SERSTART                26     ///<server start
#define LAUMSG_SERSTOP                 27     ///<server stop


#define LAUMSG_UPDATESTREAMINFO        30     ///<update streaminfo

#define LAUMSG_ALARMMSG_GLOBAL_TEMP    117    ///< Alarm for global temperature limit
#define LAUMSG_ALARMMSG_REGION_TEMP    118    ///<Alarm for region temperature
#define LAUMSG_ALARMMSG_IN             119    ///<Alarm

#define LAUMSG_IR_HIGHT_ALARM          9001   ///<Alarm for IR Hight Temperature
#define LAUMSG_IR_LOW_ALARM            9002   ///<Alarm for IR Low Temperature

#define LAUMSG_ALARMMSG_FIRE           9003    ///<fire alarm
#define LAUMSG_ALARMMSG_TEMPDIFF       9004    ///< temperature compare alarm
#define LAUMSG_ALARMMSG_ITELLIGENT_ANALYSIS       9005    ///< itelligent analysis

/**@brief 客户端通道信息回调函数指针
* @param[in] hHandle    连接句柄
* @param[in] wParam     左参数
* @param[in] lParam     右参数
* @param[in] context    保留
* @return 无
*/
typedef void (WINAPI *CCICALLBACK)(IRNETHANDLE hHandle, WPARAM wParam, LPARAM lParam, void *context);

/**@brief raw数据回调
* @param[in] data       raw数据地址
* @param[in] width      宽
* @param[in] height     高
* @param[in] context    保留
* @see IRNET_ClientRegRawCallback
*/
typedef void(WINAPI*RAWCALLBACK)(char* data, int width, int height, void* context);

/**@brief 完成回调事件
* @param[in] status     raw数据地址
* @param[in] url        设备IP
* @param[in] sername    设备名称
* @param[in] username   用户名
* @param[in] wserport   端口号
* @param[in] context    自定义数据
* @return 无
*/
typedef void(WINAPI*FINISHCALLBACK)(int status,char *url, char *sername, char *username, WORD wserport,void * context);


/**@brief 服务器通道上线通道回调函数
* @param[in] sername     服务器名称
* @param[in] url         服务器IP
* @param[in] port        服务器端口
* @param[in] serialno    服务器序列号
* @param[in] pyhmac      服务器MAC地址
* @param[in] channels    服务器通道数
* @param[in] AlarmInNum  报警输入总数
* @param[in] AlarmOutNum 报警输出总数
* @param[in] ch          当前上限服务器通道号
* @param[in] bLogin      是否注册上线 TRUE-上线 FALSE-下线
* @param[in] hChHandle   通道句柄
*/
typedef int(WINAPI *RVSCHANNELCALLBACK)(const char *sername, const char *url, WORD port, const char *serialno, unsigned char pyhmac[6], int channels, int AlarmInNum, int AlarmOutNum, int ch, BOOL bLogin, IRNETHANDLE hChHandle);

/**@brief 服务器报警消息回调函数
* @param[in] hHandle 通道句柄
* @param[in] wParam  消息参数
* @param[in] lParam  消息参数
* @param[in] context 回调函数上下文
*/
typedef void(WINAPI *RVSALARMCALLBACK)(IRNETHANDLE hHandle, WPARAM wParam, LPARAM lParam, void *context);

/** @brief 串口数据回调
* @param[in] hSerial       透明串口句柄，IRNET_ClientSerialStart返回值
* @param[in] pRecvDataBuff 接收数据指针；(为空时表示连接断开)
* @param[in] BuffSize      数据长度；(< 0时表示连接断开)
* @param[in] context       用户上下文；
* @attention 1.该回调函数是接收到数据就回调，所以一次回调不一定是一个完整的数据，也有可能一次回调包括多个完整的数据。所以请针对不同协议进行解析
             2.该回调内不能执行阻塞操作。推荐方式为：该回调只做简单的拷贝操作，将数据拷贝到用户自定数据区中，拷贝后立刻返回，用户在用户自定数据区中解析协议
*/
typedef void(WINAPI *pfSerialDataCallBack)(IRNETHANDLE hSerial, char *pRecvDataBuff, int BuffSize, void *context);

/**
*@brief FFF的温度数据
*/
struct FFFTemperatureData 
{
    float*          temperatueData; /**<温度数据首地址*/
    unsigned short  width;          /**<图像的宽*/
    unsigned short  height;         /**<图像的高*/
};

/**
* @brief jpeg抓拍 数据回调
* @param[in]    handle      IRNET_ClientJpegCapStart返回值
* @param[in]    m_ch        通道
* @param[in]    pBuffer     抓拍数据首地址
* @param[in]    size        抓拍数据大小
* @param[in]    extraData   额外数据(当抓拍的数据为FFF格式时，此字段不为空时表示 FFFTemperatureData )
* @param[in]    userdata    用户数据
* @attention extraData效果暂时仅windows下实现
* @see pfJpegdataCallbackGeneral
*/
typedef void(WINAPI *pfJpegdataCallback)(IRNETHANDLE hHandle, int m_ch, char *pBuffer, int size, void* extraData, void *userdata);

/**
* @brief jpeg抓拍 数据回调
* @param[in]    handle      IRNET_ClientJpegCapStart返回值
* @param[in]    m_ch        通道
* @param[in]    pBuffer     抓拍数据首地址
* @param[in]    size        抓拍数据大小
* @param[in]    userdata    用户数据
* @see pfJpegdataCallback
*/
typedef void(WINAPI *pfJpegdataCallbackGeneral)(IRNETHANDLE hHandle, int m_ch, char *pBuffer, int size, void *userdata);

/**@brief 客户端通道信息结构*/
typedef struct{
	char *m_sername;                    ///<server name
	char *m_username;                   ///<user name
	char *m_password;                   ///<password
	WORD m_tranType;                    ///<transmit type   
	WORD m_playstart;                   ///<is start view,linux下该参数必须为0
	BYTE m_ch;                          ///<channel
	HWND m_hVideohWnd;                  ///<video wnd handle
	HWND m_hChMsgWnd;                   ///<message wnd handle
	UINT m_nChmsgid;                    ///<message id  
	int  m_buffnum;                     ///<buff num
	int  m_useoverlay;                  ///<is use overlay
	COLORREF nColorKey;                 ///<color key(reserved)
	char url[40];                       ///<url
    CCICALLBACK m_messagecallback;      ///<message callback function
	void *context;                      ///<message callback context
}CHANNEL_CLIENTINFO;


/**@brief 本地图像抓拍错误码*/
enum CaptureErrCode
{
    EN_CEC_SUCCESS              =   0x10,   /**<成功                      */
    EN_CEC_PARAM_INVALID        =   0x11,   /**<参数无效                  */
    EN_CEC_PATH_INVALID         =   0x12,   /**<文件路径无效              */
    EN_CEC_MEM_NOT_ENOUGH_RAW   =   0x13,   /**<内存不足或无raw数据       */
    EN_CEC_WRITE_RAW            =   0x14,   /**<写入raw数据失败           */
    EN_CEC_MEM_NOT_ENOUGH_VIS   =   0x15,   /**<内存不足或无可见光数据    */
    EN_CEC_WRITE_VIS            =   0x16,   /**<写入可见光数据失败        */
    EN_CEC_MEM_NOT_ENOUGH_PRE   =   0x17,   /**<内存不足或无预览数据      */
    EN_CEC_WRITE_PRE            =   0x18,   /**<写入预览数据失败          */
    EN_CEC_WRITE_SENSOR_INFO    =   0x19,   /**<写入传感器信息失败        */
    EN_CEC_WRITE_CALIB_INFO     =   0x1a,   /**<写入校准信息失败          */
    EN_CEC_WRITE_ENV_INFO       =   0x1b,   /**<写入环境信息失败          */
    EN_CEC_WRITE_PALETTE_INFO   =   0x1C,   /**<写入调色板信息失败        */
    EN_CEC_WRITE_STRENGTH_INFO  =   0x1D,   /**写入强度信息失败           */
    EN_CEC_COLLECT_FILE_INFO    =   0x1E,   /**<写入搜集文件信息失败      */
    EN_CEC_MEM_NOT_ENOUGH       =   0x1F,   /**<内存不足                  */
    EN_CEC_DECODE_TEMPERATURE   =   0x20,   /**<解码温度失败              */
    EN_CEC_UNDEFINE             =   0x21,   /**<未知错误                  */
};

/**@brief 本地图像抓拍的文件类型 
*/
enum FileType
{
    EN_FT_SDK_LCR           =   0,  /**<热图      */
    EN_FT_SDK_CHANNEL_JPG   =   1,  /**<通道jpg   */
    EN_FT_SDK_CHANNEL_BMP   =   2,  /**<通道bmp   */
};

/**! 
* @brief    设备测温外部环境
* @warning  fWinTrans+fWinRefl<1.0
*/
typedef struct
{
	float	fEmissivity;	///< 场景发射率(0-1)
	float	fWinTrans;		///< 窗口传输率(0-1)
	float	fWinTemp;		///< 窗口温度[K]
	float	fWinRefl;		///< 窗口反射率(0-1)
	float	fReflTemp;		///< 窗口反射温度[k]
	float	fAtmTrans;		///< 大气传输率(0-1) 
	float	fAtmTemp;		///< 大气温度[K]
	float	fBkgTemp;		///< 背景温度[K]
	float	fDistance;		///< 距离[meters]
	float	fHumidity;		///< 相对湿度[0-1]
    float   fRadRate;       ///< 辐射率[0-1]
    float   fEnvTemp;       ///< 环境温度[℃]
    int     osdena;         ///< 水印使能
}DEV_ENV_INFO;

/**@brief 设备温宽参数*/
typedef struct 
{
	float	fTempMin;		///< 最小温度[C]
	float	fTempMax;		///< 最大温度[C]
	int	    bAuto;			///< 自动测温(为TRUE-忽略fTempMin，fTempMax)
}DEV_TEMP_SPAN;

/**@brief 设备基本信息*/
struct DeviceBaseInfo 
{
    char *servername;   ///<设备名
    char *username;     ///<用户名
    char *password;     ///<密码
    char *url;          ///<设备IP
    WORD wserport;      ///<端口
};

/**@brief 调色板模式*/
typedef enum
{
    DEV_PALETE_WHITEHOT     = 0,      /**< 白热         */
    DEV_PALETE_BLACKHOT     = 1,      /**< 黑热         */
    DEV_PALETE_FUSION       = 2,      /**< 融合         */
    DEV_PALETE_HOTMETAL     = 2,      /**< HOTMETAL     */
    DEV_PALETE_RAINBOW      = 3,      /**< 彩虹         */
    DEV_PALETE_GLOBOW       = 4,      /**< GLOBOW       */
    DEV_PALETE_IRON         = 5,      /**< 铁红         */
    DEV_PALETE_IRON2        = 6,      /**< IRON2        */
    DEV_PALETE_SEPIA        = 7,      /**< SEPIA        */
    DEV_PALETE_COLOR        = 8,      /**< COLOR        */
    DEV_PALETE_COLOR2       = 9,      /**< COLOR2       */
    DEV_PALETE_ICEFIRE      = 10,     /**< ICEFIRE      */
    DEV_PALETE_RAIN         = 11,     /**< RAIN         */
    DEV_PALETE_REDHOT       = 12,     /**< REDHOT       */
    DEV_PALETE_GREENHOT     = 13,     /**< 绿热         */
    DEV_PALETE_GREYRED      = 14,     /**< 灰红         */
    DEV_PALETE_LAVA         = 15,     /**< 熔岩         */
    DEV_PALETE_INSTALERT    = 16,     /**< InstAlert    */
    DEV_PALETE_ARCTIC       = 17,     /**< 极光         */
    DEV_PALETE_MAX,
}DEV_PALETTE_MODE;

/**@brief 录像文件格式*/
enum RecordType
{
    RECDT_INVALID               =   10,     /**<录像格式无效                      */
    RECDT_AUTO_BY_FILE_NAME     =   11,     /**<根据文件名自动判断文件格式()      */
    RECDT_PRIVATE_MP4           =   12,     /**<私有mp4文件(只能用私有播放器播放) */
    RECDT_MP4                   =   13,     /**<MP4                               */
    RECDT_MOV                   =   14,     /**<MOV                               */
    RECDT_ASF                   =   15,     /**<asf                               */
    RECDT_AVI                   =   16,     /**<avi                               */
};


/**@brief 融合视图模式*/
enum EnumViewMode
{
    EN_MODE_VIS     = 1,    /**< 可见光视图  */
    EN_MODE_FUSION  = 2,    /**< 融合视图    */
};

/**@brief 设备校准文件类型*/
enum DevCalibType 
{
    EN_DCT_TEMP         = 0x40,  /**<出厂温度                               */
    EN_DCT_K            = 0x41,  /**<K值                                    */
    EN_DCT_SENSOR       = 0x42,  /**<sensor                                 */
    EN_DCT_BLIND        = 0x43,  /**<盲元                                   */
    EN_DCT_TABLE_TEMP   = 0x44,  /**<温度表  (之前的温度表导入合并到此处)   */
    EN_DCT_B            = 0x45,  /**<B值                                    */
    EN_DCT_COMPENSATE   = 0x46,  /**<距离测温补偿                           */
};
/**@brief 入侵区域 */
typedef struct _INTRUSION_AREA
{
	unsigned short usX;				///< 左上角X坐标
	unsigned short usY;				///< 左上角Y坐标
	unsigned short usWidth;			///< 区域宽度，单位：像素
	unsigned short usHeight;		///< 区域高度，单位：像素
} LI_INTRUSION_AREA;
/**@brief 智能分析类型*/
enum IntelligentAnalysisType
{
    EN_IAT_INVALID          = 0,    /**<无效              */
    EN_IAT_CORDON           = 1,    /**<警戒线1           */
    EN_IAT_REGION_ENTRY     = 2,    /**<进入区域          */
    EN_IAT_REGION_LEAVE     = 3,    /**<离开区域          */
    EN_IAT_GOODS_LEFT       = 4,    /**<物品遗留          */
    EN_IAT_GOODS_MOVE       = 5,    /**<物品搬迁          */
    EN_IAT_TARFILTER_MIN    = 6,    /**<过滤目标-最小尺寸 */
    EN_IAT_TARFILTER_MAX    = 7,    /**<过滤目标-最大尺寸 */
    EN_IAT_REGIONSHIELD     = 8,    /**<区域屏蔽          */
    EN_IAT_MAX              = 9,
};
        
/**@brief 入侵检测回调
* @param[in] hHandle                通道连接句柄
* @param[in] LI_INTRUSION_AREA      检测到的入侵区域数据地址
* @param[in] areaCount              检测到的入侵区域数量
* @return 无
*/
typedef void (WINAPI*INTRDETECCALLBACK)(IRNETHANDLE hHandle,LI_INTRUSION_AREA*pArea,int areaCount);
/**@brief 智能分析-目标过滤回调
* @param[in] hHandle          通道连接句柄
* @param[in] targetWidth      目标尺寸的宽
* @param[in] targetHeight     目标尺寸的高
* @param[in] context          自定义数据
* @return 无
*/
typedef void (CALLBACK*TargeFilterCallback)(IRNETHANDLE hHandle, int targetWidth, int targetHeight, void*context);

/**@brief 监听服务器主动注册信息结构*/
typedef struct
{
    char *m_rvsbindurl;        ///<RVS bind url，NULL if all url
    WORD m_rvsbindport;        ///<RVS bind port
}RVSINFOREG;

/**@brief 转发服务器信息结构*/
typedef struct
{
    char m_multiip[16];                 ///<multicast ipaddress
    WORD m_wMultiPort;                  ///<multicast port
    WORD m_wLocaport;                   ///<local port
    int  m_videobuff;                   ///<video buff num
    int  m_audiobuff;                   ///<audio buff num
    int  m_channuser;                   ///<channel user count
    int  m_totaluser;                   ///<totola user count
    int  (WINAPI *m_UserCheckcallback)(char *m_url, char *username, char *password);	///<user check callback function
    int  (WINAPI *m_UserConnectcallback)(char *m_sername, int channel, BOOL bStart, char *m_userurl, char *username, char *password, int transtype, IRNETHANDLE handle);	///<user login logout callback function
}IRNET_REDIRECTORINFO;

/**@brief 测温回调函数指针
* @param[in] hHandle        连接句柄
* @param[in] fTemperature   温度数据地址。大小为图像的宽×高,单位℃
* @param[in] uWidth         图像的宽
* @param[in] uHeight        图像的高
* @param[in] tempSpan       温宽
* @param[in] context        与IRNET_ClientRegTempCallBack的context是同一份，此处只是做回传
* @see IRNET_ClientRegTempCallBack
*/
typedef void(CALLBACK*TEMPCALLBACK)(IRNETHANDLE hHandle, float fTemperature[], UINT uWidth, UINT uHeight, DEV_TEMP_SPAN* tempSpan, void*context);
/**
 * @brief                   对讲回调
 * @param[in]   pbuff       音频数据,如果为NULL,需要停止对讲
 * @param[in]   size        数据大小,如果<=0,需要停止对讲
 * @param[in]   context     用户上下文
 * @attention 不能在回调函数中停止对讲
 * @see IRNET_ClientTalkStart
*/
typedef void(WINAPI *TALKCALLBACK)(char *pbuff, int size, void * context);
/**
 * @brief 显示回调
 * @param[in] m_y       Y分量
 * @param[in] m_u       U分量
 * @param[in] m_v       V分量
 * @param[in] stridey   Y跨度
 * @param[in] strideuv  UV跨度
 * @param[in] width     图像宽
 * @param[in] height    图像高
 * @param[in] context   用户上下文
 * @see IRNET_ClientShowcallback
*/
typedef void(WINAPI *SHOWCALLBACK)(BYTE *m_y,
    BYTE *m_u,
    BYTE *m_v,
    int stridey,
    int strideuv,
    int width,
    int height,
    void *context);
/**
 * @brief 视频原始码流回调
 * @param[in] pbuff         帧数据
 * @param[in] headsize      帧头大小
 * @param[in] datasize      数据大小
 * @param[in] timetick      时间戳
 * @param[in] biskeyframe   关键帧标记
 * @param[in] context       用户上下文
 * @note 需要自行解码数据时,帧数据内容[pbuff,pbuff+datasize)
 * @see IRNET_ClientStartNomalCap
*/
typedef void(WINAPI *ORIGINALVIDEO)(char *pbuff, int headsize, int datasize, int timetick, int biskeyframe, void *context);
/**
* @brief 音频原始码流回调
* @param[in] pbuff         帧数据
* @param[in] headsize      帧头大小
* @param[in] datasize      数据大小
* @param[in] timetick      时间戳
* @param[in] biskeyframe   关键帧标记
* @param[in] context       用户上下文
* @note 需要自行解码数据时,帧数据内容[pbuff,pbuff+datasize)
* @see IRNET_ClientStartNomalCap
*/
typedef void(WINAPI *ORIGINALAUDIO)(char *pbuff, int headsize, int datasize, int timetick, int biskeyframe, void *context);
/**
* @brief    设备参数命令
* @note     opt的参数值
* @see      IRNET_ClientMessageOpen,IRNET_ClientMessageOpt,IRNET_ClientMessageClose
*/
enum MessageOpt
{
    MESSAGE_SERVERCHS                               = 1,        /**<获得通道，探头，报警输出的数目          */
    MESSAGE_CMD_RESET                               = 2,        /**<重启服务器                              */
    MESSAGE_CMD_GETGLOBALPARAM                      = 3,        /**<获取全局参数                            */
    MESSAGE_CMD_SETGLOBALPARAM                      = 4,        /**<设置全局参数                            */
    MESSAGE_GETCHANNELPARAM                         = 5,        /**<获取通道参数                            */
    MESSAGE_SETCHANNELPARAM                         = 6,        /**<设置通道参数                            */
    MESSAGE_CMD_PARAMDEFAULT                        = 7,        /**<恢复出厂默认参数(普通)                   */
    MESSAGE_CMD_GETSERIAL                           = 8,        /**<获取串口参数                            */
    MESSAGE_CMD_SETSERIAL                           = 9,        /**<设置串口参数                            */
    MESSAGE_CMD_GETSYSTIME                          = 10,       /**<获取系统时间                            */
    MESSAGE_CMD_SETSYSTIME                          = 11,       /**<设置系统时间                            */
    MESSAGE_CMD_GETSERIALNO                         = 12,       /**<获取服务器序列号                        */
    MESSAGE_CMD_GETSYSUSER                          = 13,       /**<获得服务器的用户                        */
    MESSAGE_CMD_SETSYSUSER                          = 14,       /**<设置服务器的用户                        */
    MESSAGE_CMD_OUTPUTCTRL                          = 15,       /**<控制报警输出(用户发送)                  */
    MESSAGE_CMD_OUTPUTSTATUS                        = 16,       /**<报警输出状态                            */
    MESSAGE_CMD_PTZCMDFILE                          = 17,       /**<设置云台控制命令升级文件                */
    MESSAGE_CMD_PTZCMDNAME                          = 18,       /**<获得云台控制命令文件的名称              */
    MESSAGE_CMD_SETSUBCHANNELPARAM                  = 19,       /**<设置服务器子通道参数                    */
    MESSAGE_CMD_CAPTUREJPEG                         = 20,       /**<启动jpeg抓图(图像服务器硬盘)            */
    MESSAGE_CMD_GETDISKSTATE                        = 21,       /**<获取磁盘状态                            */
    MESSAGE_CMD_FORMATDISK                          = 22,       /**<格式化磁盘                              */
    MESSAGE_CMD_ENCKEYFRAME                         = 23,       /**<让服务器编码一个I帧                     */
    MESSAGE_CMD_GETPPPOEPARAM                       = 24,       /**<获取PPPOE参数                           */
    MESSAGE_CMD_SETPPPOEPARAM                       = 25,       /**<设置PPPOE参数                           */
    MESSAGE_CMD_GETSERIAL232                        = 26,       /**<485和232合并 获取串口232参数            */
    MESSAGE_CMD_SETSERIAL232                        = 27,       /**<设置串口232参数                         */
    MESSAGE_CMD_GETDHCP                             = 28,       /**<获取DHCP功能                            */
    MESSAGE_CMD_SETDHCP                             = 29,       /**<设置DHCP功能                            */
    MESSAGE_CMD_GETUPNPCFG                          = 30,       /**<获取UPNP配置参数                        */
    MESSAGE_CMD_SETUPNPCFG                          = 31,       /**<设置UPNP配置参数                        */
    MESSAGE_CMD_GETUPNPSTATUS                       = 32,       /**<获取UPNP状态                            */
    MESSAGE_CMD_GETMAILCFG                          = 33,       /**<获取邮件配置信息                        */
    MESSAGE_CMD_SETMAILCFG                          = 34,       /**<设置邮件配置信息                        */
    MESSAGE_CMD_GETTIMERCAP                         = 35,       /**<获取定时抓拍参数                        */
    MESSAGE_CMD_SETTIMERCAP                         = 36,       /**<设置定时抓拍参数                        */
    MESSAGE_CMD_GETSYSSUPPORTEX                     = 37,       /**<获取系统扩展支持信息                    */
    MESSAGE_CMD_GETDVRSENSORALARM                   = 38,       /**<获取DVR探头报警参数                     */
    MESSAGE_CMD_SETDVRSENSORALARM                   = 39,       /**<设置DVR套头报警参数                     */
    MESSAGE_CMD_GETENCODETYPE                       = 40,       /**<获取编码类型                            */
    MESSAGE_CMD_SETENCODETYPE                       = 41,       /**<设置编码类型                            */
    MESSAGE_CMD_MAILTEST                            = 42,       /**<邮件测试                                */
    MESSAGE_CMD_GETRTSPPARAMEX                      = 43,       /**<获取RTSP参数                            */
    MESSAGE_CMD_SETRTSPPARAMEX                      = 44,       /**<设置RTSP参数                            */
    MESSAGE_CMD_GETTIMEZONE                         = 45,       /**<获取服务器时区参数                      */
    MESSAGE_CMD_SETTIMEZONE                         = 46,       /**<设置服务器时区参数                      */
    MESSAGE_CMD_GETCHANNELOSD_EX                    = 47,       /**<设置OSD叠加(拓展)                       */
    MESSAGE_CMD_SETCHANNELOSD_EX                    = 48,       /**<获取OSD叠加(拓展)                       */
    MESSAGE_CMD_GETFLASHINFO                        = 49,       /**<获得flash版本信息                       */
    MESSAGE_CMD_GETUPNPPORTINFO                     = 50,       /**<获取UPNP端口信息                        */
    MESSAGE_CMD_SET_FFCCTRL                         = 51,       /**<快门控制                                */
    MESSAGE_CMD_GET_CAPTURETYPE                     = 52,       /**<获取抓拍类型                            */
    MESSAGE_CMD_SET_CAPTURETYPE                     = 53,       /**<设置抓拍类型                            */
    MESSSGE_CMD_GET_OSDPARAM                        = 54,       /**<获取OSD参数                             */
    MESSSGE_CMD_SET_OSDPARAM                        = 55,       /**<设置OSD参数                             */
    MESSSGE_CMD_GET_IROSDPARAM                      = 56,       /**<获取红外OSD参数                         */
    MESSSGE_CMD_SET_IROSDPARAM                      = 57,       /**<设置红外OSD参数                         */
    MESSSGE_CMD_GET_REGIONTEMPPARAM                 = 58,       /**<获取区域测温参数(普通)                  */
    MESSSGE_CMD_SET_REGIONTEMPPARAM                 = 59,       /**<设置区域测温参数(普通)                  */
    MESSSGE_CMD_GET_REGIONTEMPALARM                 = 60,       /**<获取区域测温报警参数                    */
    MESSSGE_CMD_SET_REGIONTEMPALARM                 = 61,       /**<设置区域测温报警参数                    */
    MESSSGE_CMD_GET_REGIONTEMPVALUE                 = 62,       /**<获取区域测温温度值                      */
    MESSSGE_CMD_GET_VIDEOOUTMODE                    = 63,       /**<获取视频输出模式                        */
    MESSSGE_CMD_SET_VIDEOOUTMODE                    = 64,       /**<设置视频输出模式                        */
    MESSSGE_CMD_GET_TEMPDATA                        = 65,       /**<获取测温数据                            */
    MESSSGE_CMD_GET_GLOBALTEMPALARM                 = 66,       /**<获取全局测温报警参数                    */
    MESSSGE_CMD_SET_GLOBALTEMPALARM                 = 67,       /**<设置全局测温报警参数                    */
    MESSAGE_CMD_GET_IR_VIPARAM                      = 68,       /**<获取红外图像处理基本参数                */
    MESSAGE_CMD_SET_IR_VIPARAM                      = 69,       /**<设置红外图像处理基本参数                */
    MESSAGE_CMD_GET_IR_DDEPARAM                     = 70,       /**<获取红外图像处理DDE参数                 */
    MESSAGE_CMD_SET_IR_DDEPARAM                     = 71,       /**<设置红外图像处理DDE参数                 */
    MESSAGE_CMD_GET_IR_STRETCHMODE                  = 72,       /**<获取红外图像处理拉伸模式                */
    MESSAGE_CMD_SET_IR_STRETCHMODE                  = 73,       /**<设置红外图像处理拉伸模式                */
    MESSAGE_CMD_GET_TAUGAINMODE                     = 74,       /**<获取TAU增益模式                         */
    MESSAGE_CMD_SET_TAUGAINMODE                     = 75,       /**<设置TAU增益模式                         */
    MESSAGE_CMD_GET_GPSINFO                         = 76,       /**<获取GPS信息                             */
    MESSAGE_CMD_SET_GPSINFO                         = 77,       /**<设置GPS信息                             */
    MESSAGE_CMD_GETDVRSYSUSEREX                     = 78,       /**<获取设备用户信息                        */
    MESSAGE_CMD_SETDVRSYSUSEREX                     = 79,       /**<设置设备用户信息                        */
    MESSAGE_CMD_REC_REMOTE_BEG                      = 80,       /**<远程录像                                */
    MESSAGE_CMD_REC_REMOTE_END                      = 81,       /**<远程录像                                */
    MESSAGE_CMD_GET_MSDSTATUS                       = 82,       /**<SD卡空间获取                            */
    MESSAGE_CMD_GET_CAL_TEMP_IN_AREA                = 83,       /**<获取画框测温                            */
    MESSAGE_CMD_SET_CAL_TEMP_IN_AREA                = 84,       /**<设置画框测温                            */
    MESSAGE_CMD_GET_PWMUART_CFG                     = 85,       /**<获取串口配置                            */
    MESSAGE_CMD_SET_PWMUART_CFG                     = 86,       /**<设置串口配置                            */
    MESSAGE_CMD_GET_TEMP_ALARM_PARAM                = 87,       /**<温度超限报警                            */
    MESSAGE_CMD_SET_TEMP_ALARM_PARAM                = 88,       /**<温度超限报警                            */
    MESSAGE_CMD_GET_RADMET_PARAM                    = 89,       /**<辐射率配置                              */
    MESSAGE_CMD_SET_RADMET_PARAM                    = 90,       /**<辐射率配置                              */
    MESSAGE_CMD_GET_STILLCAPTURE                    = 91,       /**<定时抓拍控制                            */
    MESSAGE_CMD_SET_STILLCAPTURE                    = 92,       /**<定时抓拍控制                            */
    MESSAGE_CMD_GET_ALARM_INPUT_STATUS              = 93,       /**<获取报警输入探头状态                    */
    MESSAGE_CMD_GET_TEMPALARMCTRL                   = 94,       /**<高温报警控制                            */
    MESSAGE_CMD_SET_TEMPALARMCTRL                   = 95,       /**<高温报警控制                            */
    MESSAGE_CMD_GET_SDCARDFULLCTRL                  = 96,       /**<SD卡卡满控制                            */
    MESSAGE_CMD_SET_SDCARDFULLCTRL                  = 97,       /**<SD卡卡满控制                            */
    MESSAGE_CMD_GET_FIREPOINTPARAM                  = 98,       /**<获取火点参数                            */
    MESSAGE_CMD_SET_FIREPOINTPARAM                  = 99,       /**<设置火点参数                            */
    MESSAGE_CMD_GET_FIREPOINT                       = 100,      /**<获取火点                                */
    MESSAGE_CMD_GET_FIREBEHAVIOURPARAM              = 101,      /**<获取火势参数                            */
    MESSAGE_CMD_SET_FIREBEHAVIOURPARAM              = 102,      /**<设置火势参数                            */
    MESSAGE_CMD_GET_FIREBEHAVIOUR                   = 103,      /**<获取火势                                */
    MESSAGE_CMD_GET_OSD_REGION_TRANSPARENT_PARAM    = 104,      /**<OSD区域透明参数*/
    MESSAGE_CMD_SET_OSD_REGION_TRANSPARENT_PARAM    = 105,      /**<OSD区域透明参数*/
    MESSAGE_CMD_GET_BUZZER_ENABLE                   = 106,      /**<蜂鸣器使能*/
    MESSAGE_CMD_SET_BUZZER_ENABLE                   = 107,      /**<蜂鸣器使能*/
    MESSAGE_CMD_GETREMOTEHOST                       = 108,      /**<远程主机*/
    MESSAGE_CMD_SETREMOTEHOST                       = 109,      /**<远程主机*/
    MESSAGE_CMD_GET_REGIONTEMPALARMCTRL             = 110,      /**<区域温度报警控制*/
    MESSAGE_CMD_SET_REGIONTEMPALARMCTRL             = 111,      /**<区域温度报警控制*/
    MESSAGE_CMD_GET_TEMPMEASURETYPE                 = 112,      /**<带测温类型的区域测温参数*/
    MESSAGE_CMD_SET_TEMPMEASURETYPE                 = 113,      /**<带测温类型的区域测温参数*/
    MESSAGE_CMD_GET_AREATEMPCOMPARECTRL             = 114,      /**<区域温度对比控制*/
    MESSAGE_CMD_SET_AREATEMPCOMPARECTRL             = 115,      /**<区域温度对比控制*/
    MESSAGE_CMD_GET_REGIONOSDNAME                   = 116,      /**<区域名称OSD*/
    MESSAGE_CMD_SET_REGIONOSDNAME                   = 117,      /**<区域名称OSD*/
    MESSAGE_GETALARMPARAM                           = 118,      /**<报警参数*/
    MESSAGE_SETALARMPARAM                           = 119,      /**<报警参数*/
    MESSAGE_CMD_GETMOTIONPARAM                      = 120,      /**<移动侦测参数*/
    MESSAGE_CMD_SETMOTIONPARAM                      = 121,      /**<移动侦测参数*/
    MESSAGE_CMD_GETVIDEOMASK                        = 122,      /**<视频屏蔽参数*/
    MESSAGE_CMD_SETVIDEOMASK                        = 123,      /**<视频屏蔽参数*/
    MESSAGE_CMD_GETCHANNELOSD                       = 124,      /**<视频叠加参数*/
    MESSAGE_CMD_SETCHANNELOSD                       = 125,      /**<视频叠加参数*/
    MESSAGE_CMD_AFFIRMUSER                          = 126,      /**<验证用户*/
    MESSAGE_CMD_GETTIMEDRESET                       = 127,      /**<定时重启*/
    MESSAGE_CMD_SETTIMEDRESET                       = 128,      /**<定时重启*/
    MESSAGE_CMD_GETHIDEALARM                        = 129,      /**<遮挡参数*/
    MESSAGE_CMD_SETHIDEALARM                        = 130,      /**<遮挡参数*/
    MESSAGE_CMD_GETSUBCHANNELPARAM                  = 131,      /**<子通道参数*/
    MESSAGE_CMD_GETRECORDPARAM                      = 132,      /**<硬盘录像参数*/
    MESSAGE_CMD_SETRECORDPARAM                      = 133,      /**<硬盘录像参数*/
    MESSAGE_CMD_GETJPEGCAPPARAM                     = 134,      /**<JPEG抓拍参数*/
    MESSAGE_CMD_SETJPEGCAPPARAM                     = 135,      /**<JPEG抓拍参数*/
    MESSAGE_CMD_GETSENSORALARM                      = 136,      /**<报警探头数据*/
    MESSAGE_CMD_SETSENSORALARM                      = 137,      /**<报警探头数据*/
    MESSAGE_CMD_GETAUDIOPARAM                       = 138,      /**<音频参数*/
    MESSAGE_CMD_SETAUDIOPARAM                       = 139,      /**<音频参数*/
    MESSAGE_CMD_GETCDMAPARAM                        = 140,      /**<CDMA参数*/
    MESSAGE_CMD_SETCDMAPARAM                        = 141,      /**<CDMA参数*/
    MESSAGE_CMD_GETWIFIPARAM                        = 142,      /**<Wifi参数*/
    MESSAGE_CMD_SETWIFIPARAM                        = 143,      /**<Wifi参数*/
    MESSAGE_CMD_GETLINKCONFIG                       = 144,      /**<连接配置参数*/
    MESSAGE_CMD_SETLINKCONFIG                       = 145,      /**<连接配置参数*/
    MESSAGE_CMD_GETDDNSEXPARAM                      = 146,      /**<DDNS附加参数*/
    MESSAGE_CMD_SETDDNSEXPARAM                      = 147,      /**<DDNS附加参数*/
    MESSAGE_CMD_GETEXALARMPARAM                     = 148,      /**<外部报警参数*/
    MESSAGE_CMD_SETEXALARMPARAM                     = 149,      /**<外部报警参数*/
    MESSAGE_CMD_GETHUMITUREPRAM                     = 150,      /**<温湿度报警参数*/
    MESSAGE_CMD_SETHUMITUREPRAM                     = 151,      /**<温湿度报警参数*/
    MESSAGE_CMD_GETVIDEOOFFSET                      = 152,      /**<视频偏移量*/
    MESSAGE_CMD_SETVIDEOOFFSET                      = 153,      /**<视频偏移量*/
    MESSAGE_CMD_GETVIDEOMASKAREA                    = 154,      /**<视频屏蔽区*/
    MESSAGE_CMD_SETVIDEOMASKAREA                    = 155,      /**<视频屏蔽区*/
    MESSAGE_CMD_GETSNMPCONFIG                       = 156,      /**<网络管理参数*/
    MESSAGE_CMD_SETSNMPCONFIG                       = 157,      /**<网络管理参数*/
    MESSAGE_CMD_GETALARMSMS                         = 158,      /**<报警短信参数*/
    MESSAGE_CMD_SETALARMSMS                         = 159,      /**<报警短信参数*/
    MESSAGE_CMD_GETALARMTYPE                        = 160,      /**<探头类型*/
    MESSAGE_CMD_SETALARMTYPE                        = 161,      /**<探头类型*/
    MESSAGE_CMD_GETFTP                              = 162,      /**<FTP信息*/
    MESSAGE_CMD_SETFTP                              = 163,      /**<FTP信息*/
    MESSAGE_CMD_GETCCDPARAM                         = 164,      /**<CCD参数*/
    MESSAGE_CMD_SETCCDPARAM                         = 165,      /**<CCD参数*/
    MESSAGE_CMD_GETPLATFORMINFO                     = 166,      /**<平台信息*/
    MESSAGE_CMD_SETPLATFORMINFO                     = 167,      /**<平台信息*/
    MESSAGE_CMD_GETVI2VO                            = 168,      /**<图像环出使能*/
    MESSAGE_CMD_SETVI2VO                            = 169,      /**<图像环出使能*/
    MESSAGE_CMD_GETALARMFTPUPLOAD                   = 170,      /**<探头报警联动上传FTP参数*/
    MESSAGE_CMD_SETALARMFTPUPLOAD                   = 171,      /**<探头报警联动上传FTP参数*/
    MESSAGE_CMD_GETMOTIONCONTACTEX                  = 172,      /**<移动侦测报警联动拓展参数*/
    MESSAGE_CMD_SETMOTIONCONTACTEX                  = 173,      /**<移动侦测报警联动拓展参数*/
    MESSAGE_CMD_GETDVRMOTIONALARM                   = 174,      /**<DVR移动侦测报警参数*/
    MESSAGE_CMD_SETDVRMOTIONALARM                   = 175,      /**<DVR移动侦测报警参数*/
    MESSAGE_CMD_GETDVRHIDEALARM                     = 176,      /**<DVR视频遮挡报警参数*/
    MESSAGE_CMD_SETDVRHIDEALARM                     = 177,      /**<DVR视频遮挡报警参数*/
    MESSAGE_CMD_GETDVROTHERALARM                    = 178,      /**<DVR其他报警参数*/
    MESSAGE_CMD_SETDVROTHERALARM                    = 179,      /**<DVR其他报警参数*/
    MESSAGE_CMD_SETBKDISK                           = 180,      /**<DVR备份盘*/
    MESSAGE_CMD_GETSMARTINFO                        = 181,      /**<DVR硬盘SMART信息*/
    MESSAGE_CMD_GETVIEWPARAM                        = 182,      /**<V0参数*/
    MESSAGE_CMD_SETVIEWPARAM                        = 183,      /**<V0参数*/
    MESSAGE_CMD_GETLOOPVIEW                         = 184,      /**<轮询控制参数*/
    MESSAGE_CMD_SETLOOPVIEW                         = 185,      /**<轮询控制参数*/
    MESSAGE_CMD_GETDVRUSER                          = 186,      /**<DVR用户信息*/
    MESSAGE_CMD_SETDVRUSER                          = 187,      /**<DVR用户信息*/
    MESSAGE_CMD_GETDVRCHANNELOSD                    = 188,      /**<DVR视频叠加参数*/
    MESSAGE_CMD_SETDVRCHANNELOSD                    = 189,      /**<DVR视频叠加参数*/
    MESSAGE_CMD_GETVIDEOOFFSETEX                    = 190,      /**<视频偏移量*/
    MESSAGE_CMD_SETVIDEOOFFSETEX                    = 191,      /**<视频偏移量*/
    MESSAGE_CMD_GETDEVICEID                         = 192,      /**<设备ID*/
    MESSAGE_CMD_SETDEVICEID                         = 193,      /**<设备ID*/
    MESSAGE_CMD_GETKEYBOARDEX                       = 194,      /**<键盘参数*/
    MESSAGE_CMD_SETKEYBOARDEX                       = 195,      /**<键盘参数*/
    MESSAGE_CMD_GETCHOOSEPTZ                        = 196,      /**<选择云台*/
    MESSAGE_CMD_SETCHOOSEPTZ                        = 197,      /**<选择云台*/
    MESSAGE_CMD_GETDVRDOUBLEBITS                    = 198,      /**<选择的码流数*/
    MESSAGE_CMD_SETDVRDOUBLEBITS                    = 199,      /**<选择的码流数*/
    MESSAGE_CMD_GETPROTOCOLPARAM                    = 200,      /**<选择的协议*/
    MESSAGE_CMD_SETPROTOCOLPARAM                    = 201,      /**<选择的协议*/
    MESSAGE_CMD_GETFTPTIMEREC                       = 202,      /**<FTP定时录像*/
    MESSAGE_CMD_SETFTPTIMEREC                       = 203,      /**<FTP定时录像*/
    MESSAGE_CMD_GETSUBENCODETYPE                    = 204,      /**<选择的编码类型*/
    MESSAGE_CMD_SETSUBENCODETYPE                    = 205,      /**<选择的编码类型*/
    MESSAGE_CMD_GETPROLOOP                          = 206,      /**<预置点轮询*/
    MESSAGE_CMD_SETPROLOOP                          = 207,      /**<预置点轮询*/
    MESSAGE_CMD_GETAUDIOSILENT                      = 208,      /**<音频静音参数*/
    MESSAGE_CMD_SETAUDIOSILENT                      = 209,      /**<音频静音参数*/
    MESSAGE_CMD_GETDVRUSERSTATE                     = 210,      /**<用户状态*/
    MESSAGE_CMD_SETDVRUSERSTATE                     = 211,      /**<用户状态*/
    MESSAGE_CMD_GETREDUNDANCERECORD                 = 212,      /**<冗余录像状态*/
    MESSAGE_CMD_SETREDUNDANCERECORD                 = 213,      /**<冗余录像状态*/
    MESSAGE_CMD_GETTHIRDENCODEPARA                  = 214,      /**<第三码流参数*/
    MESSAGE_CMD_SETTHIRDENCODEPARA                  = 215,      /**<第三码流参数*/
    MESSAGE_CMD_GETTHIRDENCODETYPE                  = 216,      /**<第三码流类型*/
    MESSAGE_CMD_SETTHIRDENCODETYPE                  = 217,      /**<第三码流类型*/
    MESSAGE_CMD_GETPTZCTRLTIME                      = 218,      /**<云台锁定控制时间*/
    MESSAGE_CMD_SETPTZCTRLTIME                      = 219,      /**<云台锁定控制时间*/
    MESSAGE_CMD_GETAUDIOPARAMTYPE                   = 220,      /**<音频参数*/
    MESSAGE_CMD_SETAUDIOPARAMTYPE                   = 221,      /**<音频参数*/
    MESSAGE_CMD_GETDVRVOPARAM                       = 222,      /**<DVR的V0输出参数*/
    MESSAGE_CMD_SETDVRVOPARAM                       = 223,      /**<DVR的V0输出参数*/
    MESSAGE_CMD_GETVLCPARAM                         = 224,      /**<VLC参数*/
    MESSAGE_CMD_SETVLCPARAM                         = 225,      /**<VLC参数*/
    MESSAGE_CMD_GETNTPPARAM                         = 226,      /**<NTP参数*/
    MESSAGE_CMD_SETNTPPARAM                         = 227,      /**<NTP参数*/
    MESSAGE_CMD_GETBKDISK                           = 228,      /**<备份用硬盘*/
    MESSAGE_CMD_GETVIDEOINTERESTAREA                = 229,      /**<视频感兴趣区域参数*/
    MESSAGE_CMD_SETVIDEOINTERESTAREA                = 230,      /**<视频感兴趣区域参数*/
    MESSAGE_CMD_GETVCAABILITY                       = 231,      /**<支持的智能分析能力*/
    MESSAGE_CMD_GETCCDPARAMEX                       = 232,      /**<Sensor参数*/
    MESSAGE_CMD_SETCCDPARAMEX                       = 233,      /**<Sensor参数*/
    MESSAGE_CMD_GETSTORAGE                          = 234,      /**<硬盘组信息(新)*/
    MESSAGE_CMD_SETSTORAGE                          = 235,      /**<硬盘组信息(新)*/
    MESSAGE_CMD_GETSMART                            = 236,      /**<硬盘smart信息(新)*/
    MESSAGE_CMD_GETEXACTTIMEZONE                    = 237,      /**<准确的时区*/
    MESSAGE_CMD_SETEXACTTIMEZONE                    = 238,      /**<准确的时区*/
    MESSAGE_CMD_GETTRAFFICLIGHTCFG                  = 239,      /**<红绿灯配置*/
    MESSAGE_CMD_SETTRAFFICLIGHTCFG                  = 240,      /**<红绿灯配置*/
    MESSAGE_CMD_GETRECORDSTREAMTYPE                 = 241,      /**<录像的码流类型*/
    MESSAGE_CMD_SETRECORDSTREAMTYPE                 = 242,      /**<录像的码流类型*/
    MESSAGE_CMD_GETWIFIMODE                         = 243,      /**<wifi模式*/
    MESSAGE_CMD_SETWIFIMODE                         = 244,      /**<wifi模式*/
    MESSAGE_CMD_GETALARMSHORTMSGPARAM               = 245,      /**<联动短信参数*/
    MESSAGE_CMD_SETALARMSHORTMSGPARAM               = 246,      /**<联动短信参数*/
    MESSAGE_CMD_GETNETERRORPARAM                    = 247,      /**<网络异常报警参数*/
    MESSAGE_CMD_SETNETERRORPARAM                    = 248,      /**<网络异常报警参数*/
    MESSAGE_CMD_GETDISKSELECTPARAM                  = 249,      /**<设备存储参数*/
    MESSAGE_CMD_SETDISKSELECTPARAM                  = 250,      /**<设备存储参数*/
    MESSAGE_CMD_GETDAYNIGHTPARAM                    = 251,      /**<昼夜转换参数*/
    MESSAGE_CMD_SETDAYNIGHTPARAM                    = 252,      /**<昼夜转换参数*/
    MESSAGE_CMD_GETVIDEOINTYPE                      = 253,      /**<视频输入参数*/
    MESSAGE_CMD_SETVIDEOINTYPE                      = 254,      /**<视频输入参数*/
    MESSAGE_CMD_GETWIFIENABLE                       = 255,      /**<wifi和3g支持信息*/
    MESSAGE_CMD_GETNOISEMODEPARAM                   = 256,      /**<降噪参数*/
    MESSAGE_CMD_SETNOISEMODEPARAM                   = 257,      /**<降噪参数*/
    MESSAGE_CMD_GETVPNINFO                          = 258,      /**<VPN参数*/
    MESSAGE_CMD_SETVPNINFO                          = 259,      /**<VPN参数*/
    MESSAGE_CMD_GETWIFIINFO                         = 260,      /**<wifi参数*/
    MESSAGE_CMD_GETPHOTO_SENSITIVE_TYPE             = 261,      /**<光敏触发类型*/
    MESSAGE_CMD_SETPHOTO_SENSITIVE_TYPE             = 262,      /**<光敏触发类型*/
    MESSAGE_CMD_GET_ENC_VPP_MAIN                    = 263,      /**<编码视频预处理设置*/
    MESSAGE_CMD_SET_ENC_VPP_MAIN                    = 264,      /**<编码视频预处理设置*/
    MESSAGE_CMD_SET_ENC_VPP_SUB                     = 265,      /**<编码视频预处理设置*/
    MESSAGE_CMD_GET_ENC_VPP_SUB                     = 266,      /**<编码视频预处理设置*/
    MESSAGE_CMD_SET_ENC_VPP_THIRD                   = 267,      /**<编码视频预处理设置*/
    MESSAGE_CMD_GET_ENC_VPP_THIRD                   = 268,      /**<编码视频预处理设置*/
    MESSAGE_CMD_SET_ENC_PROFILE_MAIN                = 269,      /**<编码级别设置*/
    MESSAGE_CMD_GET_ENC_PROFILE_MAIN                = 270,      /**<编码级别设置*/
    MESSAGE_CMD_SET_ENC_PROFILE_SUB                 = 271,      /**<编码级别设置*/
    MESSAGE_CMD_GET_ENC_PROFILE_SUB                 = 272,      /**<编码级别设置*/
    MESSAGE_CMD_SET_ENC_PROFILE_THIRD               = 273,      /**<编码级别设置*/
    MESSAGE_CMD_GET_ENC_PROFILE_THIRD               = 274,      /**<编码级别设置*/
    MESSAGE_CMD_GET_CCD_RGB_DEFAULT                 = 275,      /**<sensor的RGB参数*/
    MESSAGE_CMD_SET_CCD_AI_CHECK                    = 276,      /**<AI校正值*/
    MESSAGE_CMD_GET_REMOTESTREAMTYPE                = 277,      /**<远程主机上传的码流类型*/
    MESSAGE_CMD_SET_REMOTESTREAMTYPE                = 278,      /**<远程主机上传的码流类型*/
    MESSAGE_CMD_GET_ENCRYPTIONTYPE                  = 279,      /**<加密方式(邮件参数)*/
    MESSAGE_CMD_SET_ENCRYPTIONTYPE                  = 280,      /**<加密方式(邮件参数)*/
    MESSAGE_CMD_GETSENSORPARAM_DEVTYPE              = 281,      /**<远程参数中的设备类型*/
    MESSAGE_CMD_SETSENSORPARAM_DEVTYPE              = 282,      /**<远程参数中的设备类型*/
    MESSAGE_CMD_GETDEVICECFG                        = 283,      /**<NVR数模转换参数*/
    MESSAGE_CMD_SETDEVICECFG                        = 284,      /**<NVR数模转换参数*/
    MESSAGE_CMD_GETCHANNELRECORDPLANEX              = 285,      /**<服务器通道参数:录像计划*/
    MESSAGE_CMD_SETCHANNELRECORDPLANEX              = 286,      /**<服务器通道参数:录像计划*/
    MESSAGE_CMD_GETMOTIONALARMAREA                  = 287,      /**<移动侦测区域*/
    MESSAGE_CMD_SETMOTIONALARMAREA                  = 288,      /**<移动侦测区域*/
    MESSAGE_CMD_GET_OSDBORDER                       = 289,      /**<OSD边框参数*/
    MESSAGE_CMD_SET_OSDBORDER                       = 290,      /**<OSD边框参数*/
    MESSAGE_CMD_GET_RESOLUTION                      = 291,      /**<码流支持的分辨率*/
    MESSAGE_CMD_GET_QOS_TOS                         = 292,      /**<音视频QOS优先级*/
    MESSAGE_CMD_SET_QOS_TOS                         = 293,       /**<音视频QOS优先级*/
    MESSAGE_CMD_GET_SNMP_CFG                        = 294,      /**<SNMP参数*/
    MESSAGE_CMD_SET_SNMP_CFG                        = 295,      /**<SNMP参数*/
    MESSAGE_CMD_GET_IPV6_CFG                        = 296,      /**<配置IPV6*/
    MESSAGE_CMD_SET_IPV6_CFG                        = 297,      /**<配置IPV6*/
    MESSAGE_CMD_GET_SENSORVERSION                   = 298,      /**<获取Sensor版本号*/
    MESSAGE_CMD_GET_WINCALIPARAM                    = 299,      /**<窗口补偿参数*/
    MESSAGE_CMD_SET_WINCALIPARAM                    = 300,      /**<窗口补偿参数*/
    MESSAGE_CMD_GET_SMT_ANALY_ADVANCE               = 301,      /**<智能分析-高级参数*/
    MESSAGE_CMD_SET_SMT_ANALY_ADVANCE               = 302,      /**<智能分析-高级参数*/
    MESSAGE_CMD_GET_DAYNIGHTPARAM_EX                = 303,      /**<昼夜参数*/
    MESSAGE_CMD_SET_DAYNIGHTPARAM_EX                = 304,      /**<昼夜参数*/
    MESSAGE_CMD_GET_IROSD2VISPARAM                  = 305,      /**<可见光叠加测温osd参数*/
    MESSAGE_CMD_SET_IROSD2VISPARAM                  = 306,      /**<可见光叠加测温osd参数*/
    MESSAGE_CMD_GET_CHANNELTYPE                     = 307,      /**<通道类型*/
    MESSAGE_CMD_GET_ANTIFLICKER                     = 308,      /**<抗闪参数*/
    MESSAGE_CMD_SET_ANTIFLICKER                     = 309,      /**<抗闪参数*/
    MESSAGE_CMD_GET_TEMPREGIONOFFSET                = 310,      /**<测温区位置偏移(可见光通道有效)*/
    MESSAGE_CMD_SET_TEMPREGIONOFFSET                = 311,      /**<测温区位置偏移(可见光通道有效)*/
    MESSAGE_CMD_GET_TEMPVALUE_EX                    = 312,      /**<测温温度值(拓展)*/
    MESSAGE_CMD_GET_SENSOR_SERIALNO                 = 313,      /**<获取sensor序列号                                 */
    MESSAGE_CMD_SET_SENSOR_SERIALNO                 = 314,      /**<设置sensor序列号                                 */
    MESSAGE_CMD_GET_SWITCHMODE                      = 315,      /**<切换校准模式                                     */
    MESSAGE_CMD_SET_SWITCHMODE                      = 316,      /**<切换校准模式                                     */
    MESSAGE_CMD_TAU_FACDEFAULT                      = 317,      /**<TAU恢复默认                                      */
    MESSAGE_CMD_TAU_SAVEPARAM                       = 318,      /**<TAU保存参数                                      */
    MESSAGE_CMD_GET_TAU_SEGMENT                     = 319,      /**<TAU段编号                                        */
    MESSAGE_CMD_SET_TAU_SEGMENT                     = 320,      /**<TAU段编号                                        */
    MESSAGE_CMD_GET_HT_BLACK_CLIPPING               = 321,      /**<黑边裁剪                                         */
    MESSAGE_CMD_SET_HT_BLACK_CLIPPING               = 322,      /**<黑边裁剪                                         */
    MESSAGE_CMD_GET_DENOISEPARAM                    = 323,      /**<降噪                                             */
    MESSAGE_CMD_SET_DENOISEPARAM                    = 324,      /**<降噪                                             */
    MESSAGE_CMD_GET_ROLLPARAM                       = 325,      /**<翻转                                             */
    MESSAGE_CMD_SET_ROLLPARAM                       = 326,      /**<翻转                                             */
    MESSAGE_CMD_GET_DIGITALOUTMODE                  = 327,      /**<数字输出模式                                     */
    MESSAGE_CMD_SET_DIGITALOUTMODE                  = 328,      /**<数字输出模式                                     */
    MESSAGE_CMD_GET_FFCMODE                         = 329,      /**<FFC模式                                          */
    MESSAGE_CMD_SET_FFCMODE                         = 330,      /**<FFC模式                                          */
    MESSAGE_CMD_GET_FFCFRAMEPARAM                   = 331,      /**<自动FFC帧数参数                                  */
    MESSAGE_CMD_SET_FFCFRAMEPARAM                   = 332,      /**<自动FFC帧数参数                                  */
    MESSAGE_CMD_GET_FFCTEMPPARAM                    = 333,      /**<自动FFC温度参数                                  */
    MESSAGE_CMD_SET_FFCTEMPPARAM                    = 334,      /**<自动FFC温度参数                                  */
    MESSAGE_CMD_GET_VTEMPTEMP                       = 335,      /**<VTEMP温度参数                                    */
    MESSAGE_CMD_SET_VTEMPTEMP                       = 336,      /**<VTEMP温度参数                                    */
    MESSAGE_CMD_GET_VTEMPREGION                     = 337,      /**<VTEMP区域参数                                    */
    MESSAGE_CMD_SET_VTEMPREGION                     = 338,      /**<VTEMP区域参数                                    */
    MESSAGE_CMD_GET_KENABLE                         = 339,      /**<K值生效                                          */
    MESSAGE_CMD_SET_KENABLE                         = 340,      /**<K值生效                                          */
    MESSAGE_CMD_GET_BENABLE                         = 341,      /**<B值生效                                          */
    MESSAGE_CMD_SET_BENABLE                         = 342,      /**<B值生效                                          */
    MESSAGE_CMD_GET_BLINDENABLE                     = 343,      /**<盲元生效                                         */
    MESSAGE_CMD_SET_BLINDENABLE                     = 344,      /**<盲元生效                                         */
    MESSAGE_CMD_GET_FILTEENABLE                     = 345,      /**<时域滤波生效                                     */
    MESSAGE_CMD_SET_FILTEENABLE                     = 346,      /**<时域滤波生效                                     */
    MESSAGE_CMD_GET_FILTEPARAM                      = 347,      /**<时域滤波系数                                     */
    MESSAGE_CMD_SET_FILTEPARAM                      = 348,      /**<时域滤波系数                                     */
    MESSAGE_CMD_GET_TRAN                            = 349,      /**<透传                                             */
    MESSAGE_CMD_SET_TRAN                            = 350,      /**<透传                                             */
    MESSAGE_CMD_GET_SMART_DATA                      = 351,      /**<smartData配置                                    */
    MESSAGE_CMD_SET_SMART_DATA                      = 352,      /**<smartData配置                                    */
    MESSAGE_CMD_GET_SENSOR_TEMP                     = 353,      /**<机芯传感器温度                                   */
    MESSAGE_CMD_SET_WRITE_FLASH                     = 354,      /**<固化flash                                        */
    MESSAGE_CMD_GET_SENSOR_STATE                    = 355,      /**<当前sensor状态                                   */
    MESSAGE_CMD_GET_OBJTEMP_FLUXRANGE               = 356,      /**<温度像素范围                                     */
    MESSAGE_CMD_GET_TAUNUCTABLEINDEXES              = 357,      /**<获取当前支持的NUC Table索引集合                  */
    MESSAGE_CMD_GET_TAUNUCTABLEFUNCTION             = 358,      /**<获取当前TAU所支持的 NUC Table检测功能            */
    MESSAGE_CMD_SET_TAUNUCTABLEFUNCTION             = 359,      /**<设置当前TAU所支持的 NUC Table检测功能            */
    MESSAGE_CMD_WNDCALIB_STARTUP                    = 360,      /**<窗口校准 启动                                    */
    MESSAGE_CMD_WNDCALIB_CAPBASEDATA                = 361,      /**<抓取第N个黑体的基准数据                          */
    MESSAGE_CMD_WNDCALIB_CAPOBJDATA                 = 362,      /**<抓取第N个黑体的目标数据                          */
    MESSAGE_CMD_WNDCALIB_CALC                       = 363,      /**<计算窗口校准系数                                 */
    MESSAGE_CMD_GET_WEBROMVER                       = 364,      /**<获取网页(ROM)版本                                */
    MESSAGE_CMD_FILLLIGHT_ON                        = 365,      /**<补光灯开启                                       */
    MESSAGE_CMD_FILLLIGHT_OFF                       = 366,      /**<补光灯关闭                                       */
    MESSAGE_CMD_GET_ONVIFPARAM                      = 367,      /**<onvif参数                                        */
    MESSAGE_CMD_SET_ONVIFPARAM                      = 368,      /**<onvif参数                                        */
    MESSAGE_CMD_GET_IMAGEFUSEPARAM_EX               = 369,      /**<图像融合参数                                     */
    MESSAGE_CMD_SET_IMAGEFUSEPARAM_EX               = 370,      /**<图像融合参数                                     */
    MESSAGE_CMD_GET_FUSEOFFSET                      = 371,      /**<融合偏移                                         */
    MESSAGE_CMD_SET_FUSEOFFSET                      = 372,      /**<融合偏移                                         */
    MESSAGE_CMD_GET_IMAGEFLIPPARAM                  = 373,      /**<图像翻转参数                                     */
    MESSAGE_CMD_SET_IMAGEFLIPPARAM                  = 374,      /**<图像翻转参数                                     */
    MESSAGE_CMD_GET_PIXLETEMPERATURE                = 375,      /**<获取像素点的温度                                 */
    MESSAGE_CMD_SET_PIXLETEMPERATURE                = 376,      /**<设置像素点位置                                   */
    MESSAGE_CMD_GET_POLYGONTEMPPARAM                = 377,      /**<获取多边形测温多边形参数                         */
    MESSAGE_CMD_SET_POLYGONTEMPPARAM                = 378,      /**<设置多边形测温多边形参数                         */
    MESSAGE_CMD_GET_HUMANCALIPARAM                  = 379,      /**<人体测温校准参数                                 */
    MESSAGE_CMD_SET_HUMANCALIPARAM                  = 380,      /**<人体测温校准参数                                 */
    MESSAGE_CMD_GET_TEMPSHIELDPARAM                 = 381,      /**<测温区屏蔽                                       */
    MESSAGE_CMD_SET_TEMPSHIELDPARAM                 = 382,      /**<测温区屏蔽                                       */
    MESSAGE_CMD_HUMANCALI_MANUAL                    = 383,      /**<人体测温校准参数手动                             */
    MESSAGE_CMD_HUMANCALI_RESTORE                   = 384,      /**<人体测温校准参数恢复                             */
    MESSAGE_CMD_GET_HUMANCALI_VISMATCHIR            = 385,      /**<红外和可见光图像对应参数(可见光+参数对应红外)    */
    MESSAGE_CMD_SET_HUMANCALI_VISMATCHIR            = 386,      /**<红外和可见光图像对应参数(可见光+参数对应红外)    */
    MESSAGE_CMD_GET_HUMANCALI_TEMPTABLE             = 387,      /**<人体测温校准温度表                               */
    MESSAGE_CMD_SET_HUMANCALI_TEMPTABLE             = 388,      /**<人体测温校准温度表                               */
    MESSAGE_CMD_GET_TEMPUNIT                        = 389,      /**<温度单位                                         */
    MESSAGE_CMD_SET_TEMPUNIT                        = 390,      /**<温度单位                                         */
    MESSAGE_CMD_GET_TEMPINFOLIST                    = 391,      /**<获取全局和区域测温温度值信息                     */
    MESSAGE_CMD_GET_TEMPALARMCTRLEX                 = 392,      /**<高温报警控制(支持4个级别)                        */
    MESSAGE_CMD_SET_TEMPALARMCTRLEX                 = 393,      /**<高温报警控制(支持4个级别)                        */
    MESSAGE_CMD_GET_REGIONTEMPALARMCTRLEX           = 394,      /**<区域温度报警控制(支持4个级别)                    */
    MESSAGE_CMD_SET_REGIONTEMPALARMCTRLEX           = 395,      /**<区域温度报警控制(支持4个级别)                    */
    MESSAGE_CMD_GET_GB28181PARAM                    = 396,      /**<GB28181参数*/
    MESSAGE_CMD_SET_GB28181PARAM                    = 397,      /**<GB28181参数*/
    MESSAGE_CMD_GET_REGIONTEMPPARAM_EX2             = 398,      /**<区域测温参数(支持24个区域)*/
    MESSAGE_CMD_SET_REGIONTEMPPARAM_EX2             = 399,      /**<区域测温参数(支持24个区域)*/
    MESSAGE_CMD_GET_POLYGONTEMPPARAM_EX             = 400,      /**<多边形测温参数(支持24个区域)*/
    MESSAGE_CMD_SET_POLYGONTEMPPARAM_EX             = 401,      /**<多边形测温参数(支持24个区域)*/
    MESSAGE_CMD_GET_SMARTANALYSIS_ENABLE            = 402,      /**<智能分析功能使能*/
    MESSAGE_CMD_SET_SMARTANALYSIS_ENABLE            = 403,      /**<智能分析功能使能*/
    MESSAGE_CMD_GET_SMARTANALYSIS_ALARM             = 404,      /**<智能分析报警控制*/
    MESSAGE_CMD_SET_SMARTANALYSIS_ALARM             = 405,      /**<智能分析报警控制*/
    MESSAGE_CMD_GET_TEMPOSDMODE                     = 406,      /**<温度OSD模式*/
    MESSAGE_CMD_SET_TEMPOSDMODE                     = 407,      /**<温度OSD模式*/
    MESSAGE_CMD_GET_GPSINFO_ABIF                    = 408,      /**<GPS信息(ABIF)*/
    MESSAGE_CMD_GET_CENTER_OSD                      = 409,      /**<中心点OSD使能*/
    MESSAGE_CMD_SET_CENTER_OSD                      = 410,      /**<中心点OSD使能*/
    MESSAGE_CMD_PARAMDEFAULT_DEEP                   = 411,      /**<恢复默认参数(深度)*/
    MESSAGE_CMD_GET_ONVIFPORT                       = 412,      /**<onvif端口号*/
    MESSAGE_CMD_SET_ONVIFPORT                       = 413,      /**<onvif端口号*/
    MESSAGE_CMD_MAX                                 = 414,      /**<上限*/
};

/**
 * @brief 初始化SDK，加载SDK资源
 * @param[in] m_nMessage        应用程序的一个用户自定义消息
 * @param[in] m_hWnd            应用程序中一个窗口句柄
 * @param[in] m_messagecallback 消息回调函数接口
 * @param[in] context           用户上下文
 * @param[in] key               解密字符串，默认为空
 * @return 1-表示成功，0-表示失败
 * @note 设备连接断开或者连接成功等消息可以通过回调函数m_messagecallback异步通知
 * @see IRNET_ClientCleanup
 * @code 
 //回调函数参数说明
 hHandle [IN] 连接句柄，IRNET_ClientStart返回值
 wParam  [IN] 参数1
 lParam  [IN] 参数2
 context [IN] 用户上下文 
 * @endcode
*/
CNET_APIIMPORT int  CALLINGCONVEN IRNET_ClientStartup(UINT m_nMessage,HWND m_hWnd,void (WINAPI *m_messagecallback)(IRNETHANDLE hHandle,WPARAM wParam,LPARAM lParam,void *context) = NULL,void *context = NULL, char *key = NULL);

/**
 * @brief 读取消息
 * @param[in,out] m_sername  设备名称
 * @param[in,out] m_url      设备IP地址
 * @param[in,out] m_port     设备端口
 * @param[in,out] m_ch       设备通道
 * @param[in,out] wParam     主消息
 * @param[in,out] lParam     次消息
 * @return 1表示成功，0表示失败
*/
CNET_APIIMPORT int  CALLINGCONVEN IRNET_ClientReadMessage(char *m_sername,char *m_url,WORD *m_port,int *m_ch,WPARAM *wParam,LPARAM *lParam);

/**
 * @brief 设置SDK连接设备的超时时间和尝试次数
 * @param[in] m_waitnum  等待时间(秒)
 * @param[in] m_trynum   尝试次数
 * @return TRUE表示成功，FALSE表示失败
 * @attention 不要把m_waitnum的值设置得太小，如果通过internet连接服务器，有可能会连接失败
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientWaitTime(int m_waitnum = 6, int m_trynum = 3);

/**
 * @brief 卸载客户端SDK，释放SDK资源
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientStartup
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientCleanup();

///////////////图像显示///////////////////
/**
 * @brief 与设备建立连接，并实时预览图像
 * @param[in] m_url       设备的IP地址或转发服务器的IP地址
 * @param[in] m_pChaninfo CHANNEL_CLIENTINFO的指针
 * @param[in] wserport    设备或转发服务器的端口号
 * @param[in] streamtype  连接的码流类型,0:主码流 1:次码流,此项需要设备支持，默认为主码流
 * @return -1表示失败，其他值为连接句柄
 * @see IRNET_ClientStop
*/
CNET_APIIMPORT IRNETHANDLE  CALLINGCONVEN IRNET_ClientStart(char *m_url,CHANNEL_CLIENTINFO *m_pChaninfo,WORD wserport = 3000,int streamtype = 0);

/**
 * @brief 停止播放，断开与设备的连接
 * @param hHandle [IN] 连接句柄，IRNET_ClientStart的返回值
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientStop(IRNETHANDLE hHandle);

/**
 * @brief 启动图像显示
 * @param[in] hHandle    IRNET_ClientStart的返回值
 * @param[in] decodesign 释放解码器资源标志（1：释放解码器资源，0：不释放解码器资源）
 * @return TRUE表示成功，FALSE表示失败
 * @note 在连接通道时如果m_playstart为FALSE，你需要手动调用该接口来实现图像的显示
 * @see IRNET_ClientStopView,IRNET_ClientStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientStartView(IRNETHANDLE hHandle, BOOL decodesign = TRUE);

/**
 * @brief 停止图像显示
 * @param[in] hHandle IRNET_ClientStart的返回值
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientStartView,IRNET_ClientStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientStopView(IRNETHANDLE hHandle);

/**
 * @brief 设置图像显示的窗口
 * @param[in] hHandle IRNET_ClientStart的返回值
 * @param[in] hWnd    图像窗口
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientSetWnd(IRNETHANDLE hHandle,HWND hWnd);

/**
 * @brief 刷新图像显示的窗口 
 * @param[in] hHandle IRNET_ClientStart的返回值
 * @param[in] rect    设置刷新区域（以后显示的图像将在该区域显示）
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientRefrenshWnd(IRNETHANDLE hHandle,RECT *rect = NULL);

/**
 * @brief 设置图像的延迟时间 
 * @param[in] hHandle   IRNET_ClientStart的返回值
 * @param[in] delaytime 延迟时间，单位为帧,0:不延时，1-100为延时时间，值越大，延时越大，图像的流畅性更好
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientSetDelayTime(IRNETHANDLE hHandle, int delaytime);

/**
 * @brief 启用图像防裂屏
 * @param[in] hHandle  IRNET_ClientStart的返回值
 * @param[in] bsplit   防止裂屏标志
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientPreventImageSplit(IRNETHANDLE hHandle, BOOL bsplit);

/**
 * @brief 获取图像尺寸 
 * @param[in]  hHandle   IRNET_ClientStart的返回值
 * @param[out] m_pWidth  返回的宽度
 * @param[out] m_pHeight 返回的高度
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientGetVideoSize(IRNETHANDLE hHandle, DWORD *m_pWidth, DWORD *m_pHeight);

/**
 * @brief 设置是否显示图像 
 * @param[in] hHandle IRNET_ClientStart的返回值
 * @param[in] bShow   TRUE—显示视频， FALSE—不显示视频
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientSetImageShow(IRNETHANDLE hHandle, BOOL bShow);

/**
 * @brief 获取客户端状态
 * @param[in] hHandle IRNET_ClientStart的返回值
 * @return 错误码:\n
    -1 正在连接设备,中间状态\n 
 -1000 无效的hHandle\n
    0  成功\n
    1  用户停止预览\n
    2  与设备的连接失败\n
    3  设备断开\n
    4  帮定端口失败；在用多播传输输入时，如果多个设备的多播端口相同，会出现该错误\n
    5  分配内存失败，调用系统资源失败，绑定端口失败等\n
    6  连接域名服务器失败\n
 -102  用户名密码错误\n
 -103  系统用户满员，每个设备最大用户数为40\n
 -105  通道用户满员，用TCP或UDP方式每个通道最多可以连接10个用户,整个设备可以连接40个用户，对于多播方式连接，用户数量不受限则\n
 -106  服务器没有指定的通道\n
 -112  没有找到服务器的索引，在通过转发服务器连接时，没有找到指定的服务器名称\n
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT LONG  CALLINGCONVEN IRNET_ClientGetState(IRNETHANDLE hHandle);

/////////////////////////////////回调函数///////////////////////
/**
 * @brief 设置显示的回调函数
 * @param[in] hHandle      IRNET_ClientStart的返回值
 * @param[in] ShowCallBack 回调函数
 * @param[in] context      用户上下文
 * @return TRUE表示成功，FALSE表示失败
 * @par 回调函数参数说明:
 * @code
 m_y      Y数据起始地址
 m_u      U数据起始地址
 m_v      V数据起始地址
 stridey  数据Y的跨度
 strideuv 数据U、V数据的跨度
 width    图像数据的宽度
 height   图像数据的高度
 context  用户传入的上下文
 * @endcode
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientShowcallback(IRNETHANDLE hHandle, SHOWCALLBACK ShowCallBack ,void *context);

/**
 * @brief 设置音频解码的回调函数 
 * @param[in] hHandle          IRNET_ClientStart的返回值
 * @param[in] AudioDecCallBack 回调函数
 * @param[in] context          用户上下文
 * @return TRUE表示成功，FALSE表示失败
 * @par 回调函数参数说明:
 * @code
 pBuffer 解码输出的数据
 size    数据长度
 context 用户传入的上下文
 * @endcode
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientAudioDeccallback(IRNETHANDLE hHandle,void(WINAPI *AudioDecCallBack)(char *pBuffer,int size,void *context),void *context);

/**
 * @brief 设置画图回调函数
 * @param[in] hHandle      IRNET_ClientStart的返回值
 * @param[in] DrawCallBack 回调函数
 * @param[in] context      用户上下文
 * @return TRUE表示成功，FALSE表示失败
 * @par 回调函数参数说明:
 * @code
 hdc     图元文件
 context 用户传入的上下文
 * @endcode
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientDrawCallBack(IRNETHANDLE hHandle,void(WINAPI *DrawCallBack)(HDC hDC,void *context),void *context);
/**
 * @brief 设置图像显示区域（回调方式）
 * @param[in] hHandle      IRNET_ClientStart的返回值
 * @param[in] DrawCallBack 回调函数
 * @param[in] context      用户上下文
 * @return TRUE表示成功，FALSE表示失败
 * @par 回调函数参数说明:
 * @code
 hdc     图元文件
 context 用户传入的上下文
 * @endcode
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientShowImageDrawCallback(IRNETHANDLE hHandle, void(WINAPI *DrawCallBack)(HDC hDC, void *context), void *context);


///////////////////云台/////////////////////////
/**
 * @brief 控制云台
 * @param[in] hHandle   IRNET_ClientStart的返回值
 * @param[in] type      云台控制码 PTZCtrlCode
 * @param[in] value     控制参数[1,10]
 * @param[in] priority  云台控制优先级，需要设备支持
 * @param[in] extrabuff 需要传给设备的额外数据
 * @param[in] extrasize 需要传给设备的额外数据大小
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientStart, PTZCtrlCode 
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientPTZCtrl(IRNETHANDLE hHandle,int type,int value,int priority = 0, char *extrabuff = NULL, int extrasize = 0);

/**
 * @brief 设置云台地址
 * @param[in] hHandle   IRNET_ClientStart的返回值
 * @param[in] m_ptzaddr 云台地址
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientSetPTZAddr(IRNETHANDLE hHandle,BYTE m_ptzaddr);

////////////////录像与数据流///////////////////////////
/**
 * @brief 设置预录像
 * @param[in] hHandle      IRNET_ClientStart的返回值
 * @param[in] m_benable    使能标志
 * @param[in] m_buffsize   缓冲区大小
 * @param[in] m_framecount 缓存的帧数
 * @return TRUE表示成功，FALSE表示失败
 * @attention 在版本6.16D以后，参数m_buffsize不再使用，只有m_framecount有效。在版本6.19.20以后连接成功后自动打开预录，m_framecount为250帧,要重新设置，预录参数需要先停止再设置参数启动。关闭预录，录像文件中间会有三秒钟的间隔。
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientPrerecord(IRNETHANDLE hHandle,BOOL m_benable,int m_buffsize,int m_framecount);

/**
 * @brief 获得通道的视频音频压缩信息
 * @param[in]       hHandle       IRNET_ClientStart的返回值
 * @param[in,out]   m_pStreamInfo 结构VSTREAMINFO的指针
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientGetStreamInfo(IRNETHANDLE hHandle,VSTREAMINFO *m_pStreamInfo);


/**
 * @brief 启动获得原始码流
 * @param[in] hHandle       IRNET_ClientStart的返回值
 * @param[in] m_nomalvideo  获得视频数据的回调函数
 * @param[in] pvideocontext 视频用户上下文
 * @param[in] m_nomalaudio  获得音频数据的回调函数
 * @param[in] paudiocontext 音频用户上下文
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientStartNomalCap(IRNETHANDLE hHandle,
    ORIGINALVIDEO m_nomalvideo, void *pvideocontext,
    ORIGINALAUDIO m_nomalaudio,void *paudiocontext);

/**
 * @brief 停止获取原始码流
 * @param[in] hHandle IRNET_ClientStart的返回值
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientStopNomalCap(IRNETHANDLE hHandle);

/**
 * @brief 清除显示缓存
 * @param[in] hHandle IRNET_ClientStart的返回值
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_CleanVideoDisplayBuffer(IRNETHANDLE hHandle);


////////////////////////OPT//////////////////////
/**
 * @brief 打开远程参数配置连接
 * @param[in] sername  服务器名称,小于等于24个字符；
 * @param[in] url      服务器地址
 * @param[in] username 用户名，小于等于20个字符
 * @param[in] password 密码,小于等于20个字符
 * @param[in] serport  服务器端口号
 * @return -1表示失败，其他值为参数读取句柄
 * @attention 接口调用成功后，必须调用IRNET_ClientMessageClose释放资源，否则有资源泄漏
 * @see IRNET_ClientMessageOpt,IRNET_ClientMessageClose
*/
CNET_APIIMPORT IRNETHANDLE  CALLINGCONVEN IRNET_ClientMessageOpen(char *sername, char *url, char *username, char *password, WORD serport = 3000);

/**
 * @brief 配置参数
 * @param[in]       hHandle 参数读取句柄，IRNET_ClientMessageOpen返回值
 * @param[in]       opt     参数选项 @ref MessageOpt
 * @param[in]       ch      通道
 * @param[in,out]   param1  读取参数列表
 * @param[in,out]   param2  读取参数列表
 * @param[in,out]   param3  读取参数列表
 * @return 未作特殊说明下，TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientMessageOpen,IRNET_ClientMessageClose
*/
CNET_APIIMPORT int   CALLINGCONVEN IRNET_ClientMessageOpt(IRNETHANDLE hHandle, int opt, int ch = 0, void *param1 = NULL, void *param2 = NULL, void *param3 = NULL);

/**
 * @brief 关闭远程参数配置连接
 * @param[in] hHandle 参数读取句柄，IRNET_ClientMessageOpen返回值
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientMessageOpen,IRNET_ClientMessageOpt
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientMessageClose(IRNETHANDLE hHandle);


///////////特定远程参数配置///////////////////////////


/**
 * @brief 获取暂存数据
 * @param[in]    m_sername  设备名称,小于等于24个字符；
 * @param[in]    m_url      设备IP地址
 * @param[in]    m_username 用户名，小于等于20个字符
 * @param[in]    m_password 密码，小于等于20个字符
 * @param[in,out] pbuff      数据指针
 * @param[in]    wserport   设备端口
 * @return 大于等于0表示成功，返回数据长度，小于0表示失败
*/
CNET_APIIMPORT int   CALLINGCONVEN IRNET_ClientGetTempData(char *m_sername, char *m_url, char *m_username, char *m_password, char *pbuff, WORD wserport = 3000);

/////////////////保存重启服务器////////////////////////////////
/**
* @brief 保存参数
* @param[in] m_sername  设备名称，小于等于24字符
* @param[in] m_url      设备IP地址
* @param[in] m_username 用户名，小于等于20字符
* @param[in] m_password 密码，小于等于20字符
* @param[in] wserport   设备端口号
* @return TRUE表示成功，FALSE表示失败
* @attention 如果修改了参数，如果要永久保存参数需要调用该函数。默认情况下设置了参数未保存的话，重启h后会还原上次保存的参数
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientSaveServerPara(char *m_sername, char *m_url, char *m_username, char *m_password, WORD wserport = 3000);


/////////////升级，文件导入//////////////////


/**
 * @brief 启动远程升级设备固件
 * @param[in] m_url       设备IP地址
 * @param[in] m_username  用户名，小于等于20个字符
 * @param[in] m_password  密码，小于等于20个字符
 * @param[in] m_filename  固件文件名
 * @param[in] m_hEndEvent 设备升级结束事件(如果是linux平台，则HANDLE应该是LINUX_EVENT_HANDLE)
 * @param[in] wserport    设备端口号
 * @param[in] m_sername   设备名称
 * @return -1：失败；其他值：为IRNET_ClientUpdateStop的参数
 * @see IRNET_ClientUpdateStop
 * @attention 该接口调用后一定要调用IRNET_ClientUpdateStop释放资源
*/
CNET_APIIMPORT IRNETHANDLE  CALLINGCONVEN IRNET_ClientUpdateStart(char *m_url, char *m_username, char *m_password, char *m_filename, HANDLE m_hEndEvent, WORD wserport = 3000, char *m_sername = NULL);

/**
 * @brief 结束远程升级设备固件
 * @param hHandle [IN] IRNET_ClientUpdateStart的返回值
 * @return 
 0：升级成功\n
 2：连接失败\n
 -102：错误的用户名或密码
 * @attention 需要等到m_hEndEvent后再调用IRNET_ClientUpdateStop，根据IRNET_ClientUpdateStop的返回值判断升级的状态。
 * @see IRNET_ClientUpdateStart
*/
CNET_APIIMPORT int CALLINGCONVEN IRNET_ClientUpdateStop(IRNETHANDLE hHandle);



/**
 * @brief 音量控制
 * @param[in] hHandle  IRNET_ClientStart的返回值
 * @param[in] m_Volume 设置的音量的值，取值范围是0x0000-0xffff:0x0000无声，0xffff最大声音
 * @return TRUE表示成功，FALSE表示失败
 * @attention 声音是在共享模式下使用的，即每个通道口可以启动声音或停止声音，调用函数IRNET_ClientAudioVolume会自动打开声音
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientAudioVolume(IRNETHANDLE hHandle, WORD m_Volume);



/**
 * @brief 设置图像预览区域
 * @param[in] hHandle 通道连接句柄，IRNET_ClientStart的返回值
 * @param[in] pRect   矩形区域，根据图像分辨率最大为(0,0,width,height)
 * @param[in] bEnable 使能开关
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientStart,IRNET_ClientElectronicZoomMoveEnable
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientSetZoomRect(IRNETHANDLE hHandle, RECT *pRect, BOOL bEnable);

/**
 * @brief 设置设备测温环境信息
 * @param[in] hHandle       连接句柄，IRNET_ClientStart的返回值
 * @param[in] devInfo       设备环境信息
 * @param[in] DeviceMode    使用设备模式(FALSE-使用PC端的参数,TRUE-使用设备端的参数)
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientSetDevInfo(IRNETHANDLE hHandle, DEV_ENV_INFO* devInfo, BOOL DeviceMode = FALSE);

/**
* @brief 获取设备测温环境信息
* @param[in] hHandle    连接句柄，IRNET_ClientStart的返回值
* @param[in] devInfo    设备环境信息
* @param[in] DeviceMode 使用设备模式(FALSE-使用PC端的参数,TRUE-使用设备端的参数)
* @return TRUE表示成功，FALSE表示失败
* @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientGetDevInfo(IRNETHANDLE hHandle, DEV_ENV_INFO* devInfo, BOOL DeviceMode = FALSE);

/**
 * @brief 设置测温温宽信息
 * @param[in] hHandle          连接句柄，IRNET_ClientStart的返回值
 * @param[in] tempSpan         温宽信息
 * @param[in] DeviceMode       使用设备模式(FALSE-使用PC端的参数,TRUE-使用设备端的参数)
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientSetTempSpan(IRNETHANDLE hHandle, DEV_TEMP_SPAN* tempSpan, BOOL DeviceMode = FALSE);

/**
* @brief 获取测温温宽信息
* @param[in]        hHandle                 连接句柄，IRNET_ClientStart的返回值
* @param[in,out]    tempSpan                温宽信息
* @param[in]        DeviceMode              使用设备模式(FALSE-使用PC端的参数,TRUE-使用设备端的参数)
* @return           TRUE表示成功，FALSE表示失败
* @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientGetTempSpan(IRNETHANDLE hHandle, DEV_TEMP_SPAN* tempSpan, BOOL DeviceMode = FALSE);

/**
 * @brief 设置调色板模式 
 * @param[in] hHandle       连接句柄，IRNET_ClientStart的返回值
 * @param[in] enMode        调色板模式
 * @param[in] DeviceMode    使用设备模式(FALSE-使用PC端的参数,TRUE-使用设备端的参数)
 * @return TRUE表示成功，FALSE表示失败
 * @attention 设备端的调色板和PC端的调色板支持的数量不一样,以实际操作结果为准
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientSetPaletteMode(IRNETHANDLE hHandle, DEV_PALETTE_MODE enMode, BOOL DeviceMode = FALSE);

/**
* @brief 获取调色板模式
* @param[in]        hHandle       连接句柄，IRNET_ClientStart的返回值
* @param[in,out]    enMode        调色板模式
* @param[in]        DeviceMode    使用设备模式(FALSE-使用PC端的参数,TRUE-使用设备端的参数)
* @return TRUE表示成功，FALSE表示失败
* @attention 设备端的调色板和PC端的调色板支持的数量不一样,以实际操作结果为准
* @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientGetPaletteMode(IRNETHANDLE hHandle, DEV_PALETTE_MODE* enMode, BOOL DeviceMode = FALSE);

/**
 * @brief 注册测温回调
 * @param[in]       hHandle			连接句柄，IRNET_ClientStart的返回值
 * @param[in]       pCallBack		回调函数地址
 * @param[in,out]   tempSpan		温宽
 * @param[in]       context			自定义数据
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientRegTempCallBack(IRNETHANDLE hHandle, TEMPCALLBACK pCallBack, DEV_TEMP_SPAN*tempSpan, void*context);

/**
* @brief 注册获取红外通道的raw数据回调
* @param[in] hHandle     连接句柄，IRNET_ClientStart的返回值
* @param[in] pFunc       回调函数地址
* @param[in] context     保留
* @return TRUE表示成功，FALSE表示失败
* @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientRegRawCallback(IRNETHANDLE hHandle, RAWCALLBACK pFunc, void* context);

/**
* @brief 获取测温温度值
* @param[in] hHandle         连接句柄，IRNET_ClientStart的返回值
* @param[in] tempValue       测温温度值
* @param[in] DeviceMode      使用设备模式
* @return TRUE表示成功，FALSE表示失败
* @deprecated 此接口废弃,用MESSAGE_CMD_GET_TEMPVALUE_EX命令获取 ,IRNET_ClientMessageOpen,IRNET_ClientMessageOpt,IRNET_ClientMessageClose 
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientGetTemperatureValue(IRNETHANDLE hHandle, VSNETTEMPVALUE * tempValue, BOOL DeviceMode = FALSE);

/**
 * @brief 本地图像抓拍
 * @param[in]       hHandle       连接句柄，IRNET_ClientStart的返回值
 * @param[in]       type          文件类型。为EN_FT_SDK_LCR时，dataAddr表示温度数据
 * @param[in]       fileName      文件名(路径+文件名+文件拓展名)
 * @param[in]       quality       图像质量(0-100)
 * @param[in,out]   dataAddr      图像数据地址(外部分配内存) 
 * @param[in,out]   dataSize      数据大小(外部分配的内存最大为:宽×高×3Bytes,当type为EN_FT_SDK_LCR时，分配的内存大小为 红外图像的宽×红外图像的高×sizeof(float) )。成功后修改为实际数据大小,若被修改为0，意味着外部分配内存不够
 * @return 错误码(@ref CaptureErrCode)
 * @attention 本地图像抓拍需要启用数据流解码,否则无法抓拍
 * @attention type为EN_FT_SDK_LCR时，dataAddr表示温度数据
*/
CNET_APIIMPORT int CALLINGCONVEN IRNET_ClientCapture(IRNETHANDLE hHandle, FileType type, char fileName[], int quality = 100, char* dataAddr = NULL, unsigned int* dataSize = 0);

/**
* @brief 启动设备JPEG抓拍回传
* @param[in] m_sername        服务器名称，小于等于24字符
* @param[in] m_url            服务器地址
* @param[in] m_username       连接用户名，小于等于20字符
* @param[in] m_password       连接密码，小于等于20字符
* @param[in] wserport         连接端口
* @param[in] jpegdatacallback 回传回调函数
* @param[in] userdata         用户自定义参数
* @return -1 连接失败\n
* @see IRNET_ClientJpegCapSingle,IRNET_ClientJpegCapSingleEx,IRNET_ClientJpegCapStop
*/
CNET_APIIMPORT IRNETHANDLE  CALLINGCONVEN IRNET_ClientJpegCapStart(char *m_sername,
    char *m_url,
    char *m_username,
    char *m_password,
    WORD wserport,
    pfJpegdataCallback jpegdatacallback,
    void *userdata);

/**
* @brief 启动设备JPEG抓拍回传
* @param[in] m_sername        服务器名称，小于等于24字符
* @param[in] m_url            服务器地址
* @param[in] m_username       连接用户名，小于等于20字符
* @param[in] m_password       连接密码，小于等于20字符
* @param[in] wserport         连接端口
* @param[in] jpegdatacallback 回传回调函数
* @param[in] userdata         用户自定义参数
* @return -1 连接失败\n
* @see IRNET_ClientJpegCapSingle,IRNET_ClientJpegCapSingleEx,IRNET_ClientJpegCapStop
*/
CNET_APIIMPORT IRNETHANDLE  CALLINGCONVEN IRNET_ClientJpegCapStartGeneral(char *m_sername,
    char *m_url,
    char *m_username,
    char *m_password,
    WORD wserport,
    pfJpegdataCallbackGeneral jpegdatacallback,
    void *userdata);

/**
* @brief 启动一次设备抓图回传
* @param[in] hHandle   连接句柄，IRNET_ClientJpegCapStart返回值
* @param[in] m_ch      连接通道
* @param[in] m_quality JPEG压缩质量(1:最低-100:最高)
* @return TRUE表示成功，FALSE表示失败
* @see IRNET_ClientJpegCapStart,IRNET_ClientJpegCapSingleEx,IRNET_ClientJpegCapStop
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientJpegCapSingle(IRNETHANDLE hHandle, int m_ch, int m_quality);

/**
* @brief 启动一次设备抓图回传
* @param[in] hHandle   连接句柄，IRNET_ClientJpegCapStart返回值
* @param[in] m_ch      连接通道
* @param[in] m_quality JPEG压缩质量(1:最低-100:最高)
* @param[in] type      抓拍类型 0x1-可见光 0x10-红外   其他值无效
* @return TRUE表示成功，FALSE表示失败
* @attention 某些设备才支持type参数，如Uranus，通常用IRNET_ClientJpegCapSingle
* @see IRNET_ClientJpegCapStart,IRNET_ClientJpegCapSingle,IRNET_ClientJpegCapStop
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientJpegCapSingleEx(IRNETHANDLE hHandle, int m_ch, int m_quality, int type);

/**
* @brief 停止设备JPEG抓图回传
* @param[in] hHandle 连接句柄，IRNET_ClientJpegCapStart返回值
* @return TRUE表示成功，FALSE表示失败
* @see IRNET_ClientJpegCapStart,IRNET_ClientJpegCapSingle,IRNET_ClientJpegCapSingleEx
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientJpegCapStop(IRNETHANDLE hHandle);

/**
* @brief 录像开始
* @param[in] hHandle     连接句柄，IRNET_ClientStart的返回值
* @param[in] filename    文件名(路径+文件名+拓展名)
* @param[in] filetype    文件类型
* @param[in] framerate   帧率(一般可见光通道是30，红外通道是9)
* @return    TRUE表示成功，FALSE表示失败
* @see IRNET_ClientRecordEnd
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientRecordBegin(IRNETHANDLE hHandle, char filename[], RecordType filetype, float framerate);

/**
* @brief 录像暂停
* @param[in] hHandle     连接句柄，IRNET_ClientStart的返回值
* @return    TRUE表示成功，FALSE表示失败
* @see IRNET_ClientRecordBegin,IRNET_ClientRecordEnd
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientRecordPause(IRNETHANDLE hHandle);

/**
* @brief 录像恢复
* @param[in] hHandle     连接句柄，IRNET_ClientStart的返回值
* @return TRUE表示成功，FALSE表示失败
* @see IRNET_ClientRecordBegin,IRNET_ClientRecordEnd
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientRecordResume(IRNETHANDLE hHandle);

/**
* @brief 录像结束
* @param[in] hHandle     连接句柄，IRNET_ClientStart的返回值
* @return TRUE表示成功，FALSE表示失败
* @see IRNET_ClientRecordBegin
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientRecordEnd(IRNETHANDLE hHandle);

/**
* @brief 融合图像
* @param[in] hMainHandle 连接句柄，IRNET_ClientStart的返回值(红外通道)
* @param[in] hSubHandle  连接句柄，IRNET_ClientStart的返回值(可见光通道)
* @return TRUE表示成功，FALSE表示失败
* @attention 红外通道码流需是raw码流，否则就算接口调用成功，也无融合效果
* @see IRNET_ClientFuseStop
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientFuseStart(IRNETHANDLE hMainHandle, IRNETHANDLE hSubHandle);

/**
* @brief 停止图像融合
* @param[in] hHandle 连接句柄，IRNET_ClientStart的返回值
* @return TRUE表示成功，FALSE表示失败
* @attention 参数hHandle只需要与IRNET_ClientFuseStart中的hMainHandle是同一个即可
* @see IRNET_ClientFuseStart
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientFuseStop(IRNETHANDLE hHandle);

/**
* @brief 设置融合强度
* @param[in] hHandle    连接句柄，IRNET_ClientStart的返回值(红外通道)
* @param[in] byStrength 融合强度0-100
* @param[in] DeviceMode 使用设备模式
* @return TRUE表示成功，FALSE表示失败
* @see IRNET_ClientFuseStart,IRNET_ClientFuseStop,IRNET_ClientGetFusionStrength
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientSetFusionStrength(IRNETHANDLE hHandle, BYTE byStrength, BOOL DeviceMode = FALSE);

/**
* @brief 获取融合强度
* @param[in] hHandle    连接句柄，IRNET_ClientStart的返回值(红外通道)
* @param[in] DeviceMode 使用设备模式
* @return 融合强度 <0 失败 否则成功
* @see IRNET_ClientFuseStart,IRNET_ClientFuseStop,IRNET_ClientSetFusionStrength
*/
CNET_APIIMPORT int CALLINGCONVEN IRNET_ClientGetFusionStrength(IRNETHANDLE hHandle, BOOL DeviceMode = FALSE);

/**
* @brief 设置融合图像可见光的水平偏移
* @param[in] hHandle 连接句柄，IRNET_ClientStart的返回值
* @param[in] iOffset 水平偏移
* @param[in] DeviceMode 使用设备模式
* @return TRUE表示成功，FALSE表示失败
* @see IRNET_ClientFuseStart,IRNET_ClientFuseStop,IRNET_ClientGetFusionOffsetHorz
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientSetFusionOffsetHorz(IRNETHANDLE hHandle, int iOffset, BOOL DeviceMode = FALSE);

/**
* @brief 获取融合图像可见光的水平偏移
* @param[in] hHandle 连接句柄，IRNET_ClientStart的返回值
* @param[in] DeviceMode 使用设备模式
* @return 水平偏移
* @see IRNET_ClientFuseStart,IRNET_ClientFuseStop,IRNET_ClientSetFusionOffsetHorz
*/
CNET_APIIMPORT int CALLINGCONVEN IRNET_ClientGetFusionOffsetHorz(IRNETHANDLE hHandle, BOOL DeviceMode = FALSE);
/**
* @brief 设置融合图像可见光的垂直偏移
* @param[in] hHandle 连接句柄，IRNET_ClientStart的返回值
* @param[in] iOffset 垂直偏移
* @param[in] DeviceMode 使用设备模式
* @return    TRUE表示成功，FALSE表示失败
* @see IRNET_ClientFuseStart,IRNET_ClientFuseStop,IRNET_ClientGetFusionOffsetVert
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientSetFusionOffsetVert(IRNETHANDLE hHandle, int iOffset, BOOL DeviceMode = FALSE);
/**
* @brief 获取融合图像可见光的垂直偏移
* @param[in] hHandle 连接句柄，IRNET_ClientStart的返回值
* @param[in] DeviceMode 使用设备模式
* @return    垂直偏移
* @see IRNET_ClientFuseStart,IRNET_ClientFuseStop,IRNET_ClientSetFusionOffsetVert
*/
CNET_APIIMPORT int CALLINGCONVEN IRNET_ClientGetFusionOffsetVert(IRNETHANDLE hHandle, BOOL DeviceMode = FALSE);

/**
* @brief 设置融合图像视图模式
* @param[in] hHandle    连接句柄，IRNET_ClientStart的返回值
* @param[in] mode       视图模式
* @param[in] DeviceMode 使用设备模式 
* @return    TRUE表示成功，FALSE表示失败
* @see IRNET_ClientFuseStart,IRNET_ClientFuseStop
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientSetFusionViewMode(IRNETHANDLE hHandle, EnumViewMode mode, BOOL DeviceMode = FALSE);
/**
* @brief 注册入侵检测回调
* @param[in] hHandle    连接句柄，IRNET_ClientStart的返回值
* @param[in] pFuncAddr  入侵回调
* @return    TRUE表示成功，FALSE表示失败
* @attention 此功能暂只支持Linux平台
* @see IRNET_ClientIntrDetectBegin,IRNET_ClientIntrDetectEnd
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientRegIntrDetectCallback(IRNETHANDLE hHandle, INTRDETECCALLBACK pFuncAddr);

/**
* @brief 开始入侵检测
* @param[in] hHandle    连接句柄，IRNET_ClientStart的返回值
* @param[in] areaUpper  入侵区域最大数量
* @return    TRUE表示成功，FALSE表示失败
* @attention 此功能暂只支持Linux平台
* @see IRNET_ClientIntrDetectEnd
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientIntrDetectBegin(IRNETHANDLE hHandle,int areaUpper);

/**
* @brief 结束入侵检测
* @param[in] hHandle    连接句柄，IRNET_ClientStart的返回值
* @return    TRUE表示成功，FALSE表示失败
* @attention 此功能暂只支持Linux平台
* @see IRNET_ClientIntrDetectBegin
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientIntrDetectEnd(IRNETHANDLE hHandle);

/**
* @brief 设备校准
* @param[in] info       设备基本信息
* @param[in] type       校准类型
* @param[in] filename   文件名
* @param[in] memaddr    内存地址
* @param[in] memsize    内存大小
* @param[in] group      组
* @param[in] context    自定义内容
* @param[in] pCallback  任务完成回调

* @return -1：失败；其他值>0:成功
* @attention 仅当filename为NULL时，才会解析memaddr
*/
CNET_APIIMPORT IRNETHANDLE  CALLINGCONVEN IRNET_DevCalib(DeviceBaseInfo info, DevCalibType type, char *filename, unsigned char* memaddr = NULL, unsigned int memsize = 0, short group = 0, void* context = NULL, FINISHCALLBACK pCallback = NULL);

/**
 * @brief 注册服务器通道上线检测回调函数
 * @param[in] pchancheck 服务器通道上线检测回调函数 
 * @return    TRUE表示成功，FALSE表示失败
 * @attention 在服务器通道注册回调函数中只能调用IRNET_RVSRegMsgCallback、IRNET_RVSSetChanServerID、IRNET_ClientMediaData三个函数
 不能调用其它SDK函数，调用其它SDK函数将会产生死锁情况。如果要调用其它SDK函数请在通道消息回调函数中通知连接成功后再调用
 * @see IRNET_RVSStartServer
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_RVSRegSerCheckCallback(RVSCHANNELCALLBACK pchancheck);

/**
 * @brief 启动监听服务器主动注册服务
 * @param[in] m_pRvsInfo 绑定监听端口信息，指向结构VSNETRVSINFO的指针
 * @return TRUE表示成功，FALSE表示失败
 * @attention 如果你想使用转发服务，设备上线后必须使用IRNET_RVSSetChanServerID赋予设备在转发服务器中的名字。转发服务请参看IRNET_StartListenClient
 * @see IRNET_RVSStopServer
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_RVSStartServer(RVSINFOREG *m_pRvsInfo);

/**
 * @brief 注册服务器通道报警消息回调函数
 * @param[in] hHandle        通道句柄,IRNET_ClientStart的返回值
 * @param[in] palarmcallback 服务器报警消息回调函数 
 * @param[in] context        回调函数上下文
 * @return TRUE表示成功，FALSE表示失败
  * @see IRNET_RVSStartServer,IRNET_RVSRegSerCheckCallback
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_RVSRegMsgCallback(IRNETHANDLE hHandle, RVSALARMCALLBACK palarmcallback, void *context);

/**
 * @brief 设置服务器的ID，该ID用于转发
 * @param[in] hHandle 通道句柄,IRNET_ClientStart的返回值
 * @param[in] pSerID  服务器ID，作为转发的服务器唯一标识，其它客户端连接时m_sername则为该ID
 * @return TRUE表示成功，FALSE表示失败
 * @attention 要使用客服端通过转发连接该设备时，设备名称就是pSerID设置的值。
  * @see IRNET_RVSStartServer,IRNET_RVSRegSerCheckCallback
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_RVSSetChanServerID(IRNETHANDLE hHandle, char *pSerID);

/**
* @brief 启动或停止数据传输
* @param[in] hHandle 通道句柄,IRNET_ClientStart的返回值
* @param[in] bStart  TRUE：设备将启动视频、音频数据传输给该用户，FALSE：设备停止数据传输
* @return TRUE表示成功，FALSE表示失败
* @attention 该接口只是对于TCP或UDP连接时，可停止数据传输，对于多播不会停止数据传输，但接收数据线程可以无数据
* @see IRNET_RVSStartServer,IRNET_RVSRegSerCheckCallback
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientMediaData(IRNETHANDLE hHandle, BOOL bStart);

/**
 * @brief 停止监听服务器主动注册服务
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_RVSStartServer
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_RVSStopServer();

/**
 * @brief 设置转发服务器参数
 * @param[in] m_pRedirect 指向结构IRNET_REDIRECTORINFO的指针
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_RVSStartServer
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_SetRedirectorInfo(IRNET_REDIRECTORINFO *m_pRedirect);

/**
 * @brief 启动转发服务器
 * @param[in] m_localAddrIP 本机的IP地址，如果为NULL，SDK将自动获取所在主机的IP地址，如果主机有多个IP地址，可以指定一个IP地址进行监听
 * @return 0：成功 4：帮定端口错误 5：分配内存错误
 * @attention IRNET_RVSStartServer 函数是允许设备主动注册上线，也就是推模式上线IRNET_ClientStartNtFile 函数是允许通过转发服务器搜索前端设备的文件，是转发服务器的扩展，目前只有推模式和部分设备支持
 * @see IRNET_StopListenClient
*/
CNET_APIIMPORT int   CALLINGCONVEN IRNET_StartListenClient(char *m_localAddrIP = NULL);

/**
 * @brief 停止转发服务器
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_StartListenClient
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_StopListenClient();

///////////////////透明串口///////////////////////////
/**
 * @brief 开启透明串口连接
 * @param[in] m_sername           设备名称，小于等于24个字符
 * @param[in] m_url               设备地址
 * @param[in] m_username          用户名，小于等于20个字符
 * @param[in] m_password          密码，小于等于20个字符
 * @param[in] iSerialPort         串口号；0：RS485：1：RS232
 * @param[in] pSerialInfo         串口参数
 * @param[in] fSerialDataCallBack 串口接收回调函数
 * @param[in] context             用户上下文
 * @param[in] wserport            设备端口
 * @return -1：连接失败，〉0：透明串口句柄
 * @par VSSERIAL_INFO结构 
 * @code
 typedef struct{
 int baudrate;       //波特率, 50,75,110,150,300,600,1200,2400,4800,9600,19200\n
 char databit;       //数据位, 5,6,7,8\n
 char stopbit;       //停止位, 1,2
 char checkbit;      //效验位, 0-无校验,1-奇校验,2-偶校验,3-固定为1，4-固定为0
 char flowcontrol;   //流控, 0-无流控,1-软流控,2-硬流控
 }VSSERIAL_INFO;
 * @endcode
 * @par 回调函数参数说明
 * @code

 * @endcode
 * @see IRNET_ClientSerialStop,IRNET_ClientSerialRecvRestart,IRNET_ClientSerialRecvPause,IRNET_ClientSerialSendNew
*/
CNET_APIIMPORT IRNETHANDLE  CALLINGCONVEN IRNET_ClientSerialStart(char *m_sername,char *m_url,char *m_username,char *m_password,int iSerialPort,
	VSSERIAL_INFO *pSerialInfo,
    pfSerialDataCallBack fSerialDataCallBack,
	void *context,WORD wserport);

/**
 * @brief 关闭透明串口连接
 * @param[in] hSerial 透明串口句柄，IRNET_ClientSerialStart返回值
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientSerialStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientSerialStop(IRNETHANDLE hSerial);

/**
 * @brief 通过透明串口发送数据
 * @param[in] hSerial   透明串口句柄，IRNET_ClientSerialStart返回值
 * @param[in] pSendBuff 发送数据指针
 * @param[in] BuffSize  数据长度
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientSerialStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientSerialSendNew(IRNETHANDLE hSerial,char *pSendBuff,DWORD BuffSize);

/**
 * @brief 透明串口接收数据暂停
 * @param[in] hSerial 透明串口句柄，IRNET_ClientSerialStart返回值
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientSerialStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientSerialRecvPause(IRNETHANDLE hSerial);

/**
 * @brief 透明串口接收数据重新开始
 * @param[in] hSerial 透明串口句柄，IRNET_ClientSerialStart返回值
 * @return TRUE表示成功，FALSE表示失败
 * @see IRNET_ClientSerialStart
*/
CNET_APIIMPORT BOOL  CALLINGCONVEN IRNET_ClientSerialRecvRestart(IRNETHANDLE hSerial);
/**
* @brief 播放音频
* @param[in] hHandle 通道句柄,IRNET_ClientStart的返回值
* @return TRUE表示成功，FALSE表示失败
* @see IRNET_ClientStart,IRNET_ClientStopAudio
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientPlayAudio(IRNETHANDLE hHandle);
/**
* @brief 停止音频
* @param[in] hHandle 通道句柄,IRNET_ClientStart的返回值
* @return TRUE表示成功，FALSE表示失败
* @see IRNET_ClientStart,IRNET_ClientPlayAudio
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientStopAudio(IRNETHANDLE hHandle);

/**
* @brief 升级rom
* @param[in] m_sername        设备名称，小于等于24字符
* @param[in] m_url            设备IP地址
* @param[in] m_username       连接用户名，小于等于20字符
* @param[in] m_password       连接密码，小于等于20字符
* @param[in] m_wserport       连接端口
* @param[in] m_szRomPath      rom文件路径
* @return void 空
* @attention 不要在多线程中使用该接口,此接口为非线程安全
* @see IRNET_GetUploadRomRst
*/
CNET_APIIMPORT void CALLINGCONVEN IRNET_StartUploadRom(char *m_sername, char *m_url, char *m_username, char *m_password, WORD m_wserport, char* m_szRomPath);

/**
 * @brief 获取rom升级状态
 * @return 状态
2: "目前没有升级事件";\n
1: "正在升级";\n
0: "升级成功";\n
-1: "打开rom文件失败";\n
-2: "读取rom文件失败";\n
-3: "url无效";\n
-4: "连接失败";\n
-5:  "发送数据失败";\n
-6: "用户名密码错误";\n
-7: "升级线程启动失败";\n
 	"其他错误
 * @see IRNET_StartUploadRom
*/
CNET_APIIMPORT int CALLINGCONVEN IRNET_GetUploadRomRst();
/**
* @brief 搜索可用的wifi
* @param[in] m_sername        设备名称，小于等于24字符
* @param[in] m_url            设备IP地址
* @param[in] m_username       连接用户名，小于等于20字符
* @param[in] m_password       连接密码，小于等于20字符
* @param[in] m_pWifiList      wifi列表
* @param[in] wserport       连接端口
* @return 0-失败,否则成功
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientWifiSearch(char *m_sername, char *m_url, char *m_username, char *m_password, VSNETWIFISSIDLIST *m_pWifiList, WORD wserport);
/****----------智能分析--------------****/
/**
* @brief 智能分析使能
* @param[in] hHandle    通道句柄,IRNET_ClientStart的返回值
* @param[in] bEnalbe    使能 true-启用 false-禁用
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @see IRNET_ClientIntelligentAnalysisAddShield,IRNET_ClientIntelligentAnalysisGetShield,IRNET_ClientIntelligentAnalysisSetShield,IRNET_ClientIntelligentAnalysisDeleteShield,IRNET_ClientIntelligentAnalysisSaveShield
* @see IRNET_ClientIntelligentAnalysisTargetFilterCallback,IRNET_ClientIntelligentAnalysisSetMinTargetFilter,IRNET_ClientIntelligentAnalysisSetMaxTargetFilter,IRNET_ClientIntelligentAnalysisSaveTargetFilter
* @see IRNET_ClientIntelligentAnalysisAddCordon,IRNET_ClientIntelligentAnalysisGetCordon,IRNET_ClientIntelligentAnalysisSetCordon,IRNET_ClientIntelligentAnalysisCordonDirc,IRNET_ClientIntelligentAnalysisDeleteCordon,IRNET_ClientIntelligentAnalysisSaveCordon
* @see IRNET_ClientIntelligentAnalysisAddSmart,IRNET_ClientIntelligentAnalysisGetSmart,IRNET_ClientIntelligentAnalysisSetSmart,IRNET_ClientIntelligentAnalysisDeleteSmart,IRNET_ClientIntelligentAnalysisSaveSmart
* @attention 智能分析功能暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientIntelligentAnalysisEnable(IRNETHANDLE hHandle, bool bEnalbe);
/**
* @brief 添加屏蔽区
* @param[in] hHandle    通道句柄,IRNET_ClientStart的返回值
* @return 当前的区域索引
* @retval >= 0 当前索引
* @retval <0 失败
* @see IRNET_ClientStart,IRNET_ClientIntelligentAnalysisEnable
*/
CNET_APIIMPORT int CALLINGCONVEN IRNET_ClientIntelligentAnalysisAddShield(IRNETHANDLE hHandle);
/**
* @brief 获取屏蔽区数量
* @param[in] hHandle    通道句柄,IRNET_ClientStart的返回值
* @return 数量
* @retval >= 0 当前数量
* @retval <0 失败
* @see IRNET_ClientStart,IRNET_ClientIntelligentAnalysisEnable
*/
CNET_APIIMPORT int CALLINGCONVEN IRNET_ClientIntelligentAnalysisGetShield(IRNETHANDLE hHandle);
/**
* @brief 设置当前操作的屏蔽区索引
* @param[in] hHandle    通道句柄,IRNET_ClientStart的返回值
* @param[in] regionIndex 区域索引
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @see IRNET_ClientStart,IRNET_ClientIntelligentAnalysisEnable
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientIntelligentAnalysisSetShield(IRNETHANDLE hHandle, int regionIndex);
/**
* @brief 删除指定的屏蔽区
* @param[in] hHandle    通道句柄,IRNET_ClientStart的返回值
* @param[in] regionIndex 区域索引
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @see IRNET_ClientStart,IRNET_ClientIntelligentAnalysisEnable
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientIntelligentAnalysisDeleteShield(IRNETHANDLE hHandle, int regionIndex);
/**
* @brief 保存屏蔽区信息
* @param[in] hHandle    通道句柄,IRNET_ClientStart的返回值
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @see IRNET_ClientStart,IRNET_ClientIntelligentAnalysisEnable
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientIntelligentAnalysisSaveShield(IRNETHANDLE hHandle);
/**
* @brief 设置目标过滤尺寸回调(鼠标绘制尺寸的时候反馈尺寸大小)
* @param[in] hHandle            通道句柄,IRNET_ClientStart的返回值
* @param[in] maxSizeCallback    最大尺寸回调
* @param[in] maxSizeContext     最大尺寸回调附加的自定义数据
* @param[in] minSizeCallback    最小尺寸回调
* @param[in] minSizeContext     最小尺寸回调附加的自定义数据
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @see IRNET_ClientStart,IRNET_ClientIntelligentAnalysisEnable
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientIntelligentAnalysisTargetFilterCallback(IRNETHANDLE hHandle, TargeFilterCallback maxSizeCallback, void* maxSizeContext, TargeFilterCallback minSizeCallback, void* minSizeContext);
/**
* @brief 设置最小目标过滤尺寸
* @param[in] hHandle        通道句柄,IRNET_ClientStart的返回值
* @param[in,out] curWidth   当前尺寸的宽(该参数不为NULL时,将输出当前的宽)
* @param[in,out] curHeight  当前尺寸的高(该参数不为NULL时,将输出当前的高)
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @see IRNET_ClientStart,IRNET_ClientIntelligentAnalysisEnable
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientIntelligentAnalysisSetMinTargetFilter(IRNETHANDLE hHandle, int * curWidth, int*curHeight);
/**
* @brief 设置最大目标过滤尺寸
* @param[in] hHandle        通道句柄,IRNET_ClientStart的返回值
* @param[in,out] curWidth   当前尺寸的宽(该参数不为NULL时,将输出当前的宽)
* @param[in,out] curHeight  当前尺寸的高(该参数不为NULL时,将输出当前的高)
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @see IRNET_ClientStart,IRNET_ClientIntelligentAnalysisEnable
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientIntelligentAnalysisSetMaxTargetFilter(IRNETHANDLE hHandle, int * curWidth, int*curHeight);
/**
* @brief 保存目标过滤尺寸
* @param[in] hHandle    通道句柄,IRNET_ClientStart的返回值
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @attention 最大尺寸的面积比最小尺寸的面积大才能保存
* @see IRNET_ClientStart,IRNET_ClientIntelligentAnalysisEnable
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientIntelligentAnalysisSaveTargetFilter(IRNETHANDLE hHandle);
/**
* @brief 添加警戒线
* @param[in] hHandle    通道句柄,IRNET_ClientStart的返回值
* @param[in] arrowDir 箭头方向 0-左 1-右 2-双向(两侧)
* @return 当前操作的警戒线索引
* @retval >= 0 当前索引
* @retval <0 失败
* @see IRNET_ClientStart,IRNET_ClientIntelligentAnalysisEnable
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT int CALLINGCONVEN IRNET_ClientIntelligentAnalysisAddCordon(IRNETHANDLE hHandle, int arrowDir);
/**
* @brief 获取警戒线数量
* @param[in] hHandle        通道句柄,IRNET_ClientStart的返回值
* @return 当前警戒线数量
* @retval >= 0 数量
* @retval <0 失败
* @see IRNET_ClientStart,IRNET_ClientIntelligentAnalysisEnable
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT int CALLINGCONVEN IRNET_ClientIntelligentAnalysisGetCordon(IRNETHANDLE hHandle);
/**
* @brief 设置当前警戒线索引
* @param[in] hHandle        通道句柄,IRNET_ClientStart的返回值
* @param[in] regionIndex    索引
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @see IRNET_ClientStart,IRNET_ClientIntelligentAnalysisEnable
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientIntelligentAnalysisSetCordon(IRNETHANDLE hHandle, int regionIndex);
/**
* @brief 设置或获取当前警戒线检测方向
* @param[in]     hHandle        通道句柄,IRNET_ClientStart的返回值
* @param[in]     isSet          设置   1-设置 0-获取
* @param[in,out] cordonDirc     当前警戒线的方向 0-左侧 1-右侧 2-两侧\n
                                当isSet=1时 *cordonDirc为输入参数 当0=isSet时 cordonDirc由调用者分配内存
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @see IRNET_ClientStart,IRNET_ClientIntelligentAnalysisEnable
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientIntelligentAnalysisCordonDirc(IRNETHANDLE hHandle, BOOL isSet, int* cordonDirc);
/**
* @brief 删除指定的警戒线
* @param[in] hHandle        通道句柄,IRNET_ClientStart的返回值
* @param[in] regionIndex    索引
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @see IRNET_ClientStart,IRNET_ClientIntelligentAnalysisEnable
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientIntelligentAnalysisDeleteCordon(IRNETHANDLE hHandle, int regionIndex);
/**
* @brief 保存警戒线信息
* @param[in] hHandle    通道句柄,IRNET_ClientStart的返回值
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @see IRNET_ClientStart,IRNET_ClientIntelligentAnalysisEnable
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientIntelligentAnalysisSaveCordon(IRNETHANDLE hHandle );
/**
* @brief 添加智能分析(仅限进入区域、离开区域、物品遗留、物品搬迁)
* @param[in] hHandle    通道句柄,IRNET_ClientStart的返回值
* @param[in] smartType  类型,范围[EN_IAT_REGION_ENTRY,EN_IAT_GOODS_MOVE]
* @return 当前操作的智能分析索引
* @retval >= 0 当前索引
* @retval <0 失败
* @see IRNET_ClientStart,IRNET_ClientIntelligentAnalysisEnable
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT int CALLINGCONVEN IRNET_ClientIntelligentAnalysisAddSmart(IRNETHANDLE hHandle, int smartType);
/**
* @brief 获取添加智能分析数量(仅限进入区域、离开区域、物品遗留、物品搬迁)
* @param[in] hHandle    通道句柄,IRNET_ClientStart的返回值
* @param[in] smartType  类型,范围[EN_IAT_REGION_ENTRY,EN_IAT_GOODS_MOVE]
* @return 智能分析数量
* @retval >= 0 数量
* @retval <0 失败
* @see IRNET_ClientStart,IRNET_ClientIntelligentAnalysisEnable
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT int CALLINGCONVEN IRNET_ClientIntelligentAnalysisGetSmart(IRNETHANDLE hHandle, int smartType);
/**
* @brief 设置当前智能分析索引
* @param[in] hHandle        通道句柄,IRNET_ClientStart的返回值
* @param[in] regionIndex    索引
* @param[in] smartType      类型,范围[EN_IAT_REGION_ENTRY,EN_IAT_GOODS_MOVE]
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @see IRNET_ClientStart,IRNET_ClientIntelligentAnalysisEnable
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientIntelligentAnalysisSetSmart(IRNETHANDLE hHandle, int regionIndex, int smartType);
/**
* @brief 删除当前智能分析
* @param[in] hHandle        通道句柄,IRNET_ClientStart的返回值
* @param[in] regionIndex    索引
* @param[in] smartType      类型,范围[EN_IAT_REGION_ENTRY,EN_IAT_GOODS_MOVE]
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @see IRNET_ClientStart,IRNET_ClientIntelligentAnalysisEnable
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientIntelligentAnalysisDeleteSmart(IRNETHANDLE hHandle, int regionIndex, int smartType);
/**
* @brief 保存智能分析
* @param[in] hHandle    通道句柄,IRNET_ClientStart的返回值
* @param[in] smartType  类型,范围[EN_IAT_REGION_ENTRY,EN_IAT_GOODS_MOVE]
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @see IRNET_ClientStart,IRNET_ClientIntelligentAnalysisEnable
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientIntelligentAnalysisSaveSmart(IRNETHANDLE hHandle, int smartType);
/****----------智能分析--------------****/

/**
* @brief 控制报警输出设备
* @param[in] hHandle    通道句柄,IRNET_ClientStart的返回值
* @param[in] devCH      输出通道
* @param[in] bOn        1-打开,0-关闭
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientOutPut(IRNETHANDLE hHandle, char devCH, BOOL bOn);

/**
* @brief 升级rom(回调函数)
* @param[in] upromhandle    句柄,IRNET_UpgradeROM的返回值
* @param[in] upromresult    rom升级的结果
* @param[in] userdata       调用者自定义数据
* @return 空
* @note upromresult说明\n
0： 升级成功\n
-50:内存不足\n
-49:设备IP地址为空\n
-48:用户名为空\n
-47:密码为空\n
-46:rom文件路径为空\n
-45:线程创建失败\n
-44:无法打开rom文件\n
-43:内存不足,不能创建文件缓存\n
-42:无法读取rom文件的内容\n
-41:设备IP地址无效\n
-40:连接设备失败\n
-39:发送数据失败\n
-38:用户名与密码不匹配\n
其他:未知错误
* @see IRNET_UpgradeROM
*/
typedef void (CALLBACK* UpRomCallback)(IRNETHANDLE upromhandle, int upromresult, void* userdata);

/**
* @brief 升级rom(支持多线程)
* @param[in] m_sername        设备名称，小于等于24字符
* @param[in] m_url            设备IP地址
* @param[in] m_username       连接用户名，小于等于20字符
* @param[in] m_password       连接密码，小于等于20字符
* @param[in] m_wserport       连接端口
* @param[in] m_szRomPath      rom文件路径
* @param[in] callback         升级结束回调函数
* @param[in] userdata         调用者自定义数据(会作为回调函数的自定义数据参数)
* @return 句柄
* @retval -1:接口调用失败,否则调用成功
* @note 此接口如果调用成功,会通过回调函数反馈升级结果，无需调用其他任何接口
*/
CNET_APIIMPORT IRNETHANDLE CALLINGCONVEN IRNET_UpgradeROM(char *m_sername, char *m_url, char *m_username, char *m_password, WORD m_wserport, char* m_szRomPath, UpRomCallback callback, void*userdata);

/**
* @brief 升级(回调函数)
* @param[in] upflashhandle    句柄,IRNET_UpgradeFlash的返回值
* @param[in] upflashresult    flash升级的结果
* @param[in] userdata       调用者自定义数据
* @return 空
* @note upflashresult说明\n
 0： 升级成功\n
-2:不支持的协议\n
-3:用户名与密码不匹配\n
-4:发送数据失败\n
-5:连接设备失败\n
-6:设备IP地址无效\n
-7:无法读取flash文件的内容\n
-8:内存不足,不能创建文件缓存\n
-9:无法打开flash文件\n
-10:线程创建失败\n
-11:flash文件路径为空\n
-12:密码为空\n
-13:用户名为空\n
-14:设备IP地址为空\n
-15:内存不足\n
其他:未知错误
* @see IRNET_UpgradeFlash
*/
typedef void (CALLBACK* UpFlashCallback)(IRNETHANDLE upflashhandle, int upflashresult, void* userdata);

/**
* @brief 升级flash
* @param[in] sername        设备名称，小于等于24字符
* @param[in] url            设备IP地址
* @param[in] username       连接用户名，小于等于20字符
* @param[in] password       连接密码，小于等于20字符
* @param[in] wserport       连接端口
* @param[in] filepath       文件路径
* @param[in] callback       升级结束回调函数
* @param[in] userdata       调用者自定义数据(会作为回调函数的自定义数据参数)
* @return 句柄
* @retval -1:接口调用失败,否则调用成功
* @note 此接口如果调用成功,会通过回调函数反馈升级结果
*/
CNET_APIIMPORT IRNETHANDLE CALLINGCONVEN IRNET_UpgradeFlash(char *sername, char *url, char *username, char *password, WORD wserport, char* filepath, UpFlashCallback callback, void*userdata);
/**
* @brief 电子放大移动使能
* @param[in] hHandle    通道句柄,IRNET_ClientStart的返回值
* @param[in] bEnalbe    使能 true-启用 false-禁用
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @see IRNET_ClientStart,IRNET_ClientSetZoomRect
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientElectronicZoomMoveEnable(IRNETHANDLE hHandle, bool bEnalbe);
/**
* @brief 多边形测温绘制(多边形边框)使能
* @param[in] hHandle    通道句柄,IRNET_ClientStart的返回值
* @param[in] bEnalbe    使能(0-禁用,否则,使能)
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @see IRNET_ClientStart,IRNET_ClientPolygonTempMeasureSetIndex,IRNET_ClientPolygonTempMeasureSave
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientPolygonTempMeasureDrawEnable(IRNETHANDLE hHandle, BOOL bEnalbe);
/**
* @brief 设置多边形测温索引
* @param[in] hHandle    通道句柄,IRNET_ClientStart的返回值
* @param[in] index      区域索引号()
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @see IRNET_ClientStart,IRNET_ClientPolygonTempMeasureDrawEnable,IRNET_ClientPolygonTempMeasureSave
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientPolygonTempMeasureSetIndex(IRNETHANDLE hHandle, int index);
/**
* @brief 保存多边形数据到设备
* @param[in] hHandle    通道句柄,IRNET_ClientStart的返回值
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @see IRNET_ClientStart,IRNET_ClientPolygonTempMeasureDrawEnable,IRNET_ClientPolygonTempMeasureSetIndex
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientPolygonTempMeasureSave(IRNETHANDLE hHandle);
/**
* @brief 本地BMP抓拍(优化)
* @param[in] hHandle    通道句柄,IRNET_ClientStart的返回值
* @param[in] m_filename 输出文件名
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @see IRNET_ClientStart
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientBMPOptimize(IRNETHANDLE hHandle, char const * m_filename);
typedef struct
{
#ifdef SYS_LINUX
	pthread_mutex_t mutex;
	pthread_cond_t	cond;
#endif
}*LINUX_EVENT_HANDLE;
/**
* @brief 获取人头检测时图像的图像大小
* @param[in]        hHandle     通道句柄,IRNET_ClientStart的返回值
* @param[in,out]    videoWidth  视频宽
* @param[in,out]    videoHeight 视频高
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @attention 人头检测功能需依赖raw码流
* @see IRNET_ClientStart,IRNET_ClientSetBodyTemperatureThreshold,IRNET_ClientHeadDetection
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientGetHeadDetectionVideoSize(IRNETHANDLE hHandle, int * videoWidth, int*videoHeight);
/*人头区域信息*/
struct VSNET_HEADAREA
{
	int	iDetId;	///< 检测结果ID
	int	x;		///< 左上角坐标X
	int	y;		///< 左上角坐标Y
	int	w;		///< 矩形宽度
	int	h;		///< 矩形高度
	float temp; ///< 温度
};
/**
* @brief 人头区域信息回调
* @param[in] handle			VSNET_ClientStart的返回值
* @param[in] headArea		人头区域
* @param[in] headAreaCnt	人头区数量
* @param[in] context		自定义数据
* @see IRNET_ClientStart
*/
typedef void (WINAPI*HeadAreaCallback)(IRNETHANDLE handle, VSNET_HEADAREA*headArea, int headAreaCnt,void*context);
/**
* @brief 人头检测回调注册
* @param[in] hHandle		通道句柄
* @param[in] callbackfun	回调函数地址
* @param[in] context		自定义数据
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @attention 人头检测功能需依赖raw码流
* @see IRNET_ClientStart,IRNET_ClientSetBodyTemperatureThreshold,IRNET_ClientGetHeadDetectionVideoSize
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientHeadDetection(IRNETHANDLE hHandle, HeadAreaCallback callbackfun, void*context);
/**
* @brief 设置人体额头正常温度门限
* @param[in] hHandle		通道句柄
* @param[in] threshold		门限
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @note 高于门限时人头区域用红色表示,否则绿色表示
* @attention 人头检测功能需依赖raw码流
* @see IRNET_ClientStart,IRNET_ClientHeadDetection,IRNET_ClientGetHeadDetectionVideoSize
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientSetBodyTemperatureThreshold(IRNETHANDLE hHandle, float threshold);
/**
* @brief 设置人体距离校正参数
* @param[in] hHandle		通道句柄
* @param[in] par			校正参数
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @note 仅raw码流且图像宽度为336像素时有效
* @see IRNET_ClientStart,IRNET_ClientGetBodyDisCorrParam
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientSetBodyDisCorrParam(IRNETHANDLE hHandle, VSNET_TED_CORRECTION_S*par);
/**
* @brief 获取人体距离校正参数
* @param[in] hHandle		通道句柄
* @param[in] par			校正参数
* @return 成功或者失败
* @retval 0-失败
* @retval 1-成功
* @note 仅raw码流且图像宽度为336像素时有效
* @see IRNET_ClientStart,IRNET_ClientSetBodyDisCorrParam
* @attention 该接口暂只支持windows平台
*/
CNET_APIIMPORT BOOL CALLINGCONVEN IRNET_ClientGetBodyDisCorrParam(IRNETHANDLE hHandle, VSNET_TED_CORRECTION_S*par);
/**
 * @brief 启动对讲
 * @param[in] servername        设备名
 * @param[in] url               IP地址
 * @param[in] username          用户名
 * @param[in] password          密码
 * @param[in] serport           端口
 * @param[in] talkcallback      对讲回调
 * @param[in] samplerate        采样率(默认8000)
 * @param[in] context           用户上下文
 * @return 操作结果
 * @retval -1-只能启动一个对讲,-2-音频采集启动失败,-3-IP地址错误,>0-有效句柄,-4-不支持的操作系统
 * @see IRNET_ClientTalkStop
 * @attention 暂只支持windows平台
*/
CNET_APIIMPORT IRNETHANDLE CALLINGCONVEN IRNET_ClientTalkStart(char *servername,
    char *url,
    char *username,
    char *password,
    unsigned short serport,
    TALKCALLBACK talkcallback,
    int samplerate,
    void * context);
/**
 * @brief                   停止对讲
 * @param[in] talkhandle    对讲句柄
 * @return                  操作结果
 * @retval                  1-成功,0-失败
 * @see IRNET_ClientTalkStart
 * @attention 暂只支持windows平台
*/
CNET_APIIMPORT int CALLINGCONVEN IRNET_ClientTalkStop(IRNETHANDLE talkhandle);

#endif//__IRNET_H__
