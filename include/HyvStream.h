// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 HYVSTREAM_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// HYVSTREAM_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#pragma once

#include "HyvSdk.h"

//#include "HSAVCode.h"
//#include "HVRtspClient.h"

#define MAX_CNT_NUM 500

enum MSG_TYPE
{
	MSG_INIT = 0,
	MSG_PLAY,
	MSG_DEC,
};

enum TRANS_TYPE
{
	TRANS_TCP = 0,
	TRANS_UDP,
};

typedef void (*MsgCallBack)(OUT int iMsgType, OUT bool bSuccess, INOUT void* pContext);
typedef void (*OtherCallBack)(OUT char* pData, OUT int iSize, INOUT void* pContext);

// 此类是从 HyvStream.dll 导出的
class HYVSTREAM_API CHyvStream {
public:
	// TODO:  在此添加您的方法。
	static void InitSdk(); //初始化SDK, 必须先调用且只能调用一次
	static void UnInitSdk();
	static CHyvStream *Create(std::string const& username, std::string const& password);

	void Release();
	void SetVideoCallBack(VideoCallBack pVideoCallBack, void *pContext);
	void SetOtherCallBack(OtherCallBack pOtherCallBack, void *pContext);
	void SetMsgCallBack(MsgCallBack pMsgCallBack, void *pContext);
	
	void SetTransType(int iTransType);//设置传输方式，TCP或UDP
	void SetDecvType(int iDecvType); //设置Decv类型
	bool Start(char *strUrl);   //播放视频，开始rtsp并设置回调
	void Stop();                //停止视频,关闭销毁Rtsp
	
	void GetVideoSize(int &iWidth, int &iHeight); //获取解码视频宽高

	void RunThreadDecodeSws();//线程工作函数,为断流重连功能计数
#ifdef WIN32
	static unsigned int WINAPI ThreadDecodeSws(LPVOID lpParam);//线程函数，会每10ms调用一次RunThreadDecodeSws
#else
	static void * ThreadDecodeSws(LPVOID lpParam);
#endif

private:
	CHyvStream(void);
	virtual ~CHyvStream(void);

public:
	void Init(int codecID, int iWidt, int iHeight);//初始化解码，主要被rtsp中handleframe调用(编码改变及拉流初始会调用)
	void ReturnMessage(int iMsgType, bool bSuccess);//返回消息
protected:
	
private:
	MsgCallBack m_pMsgCallBack;
	OtherCallBack m_pOtherCallBack;
	void* m_pMsgContext;
	void* m_pOtherContext;

	int m_iOtherSize;
	int m_iTransType;
	bool m_bStartPlay;
	bool m_bVideoShow;

	bool m_bIsStopRunThreadDecodeSws;
	char m_pUrl[200];
	int m_iCnt;//断流重连用

#ifdef WIN32
	HANDLE m_hDecodeSwsThread;
#else
	pthread_t m_hDecodeSwsThread;
#endif
};

//extern HYVSTREAM_API int nHyvStream;
//HYVSTREAM_API int fnHyvStream(void);
