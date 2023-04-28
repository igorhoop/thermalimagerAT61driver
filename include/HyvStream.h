// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� HYVSTREAM_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// HYVSTREAM_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
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

// �����Ǵ� HyvStream.dll ������
class HYVSTREAM_API CHyvStream {
public:
	// TODO:  �ڴ�������ķ�����
	static void InitSdk(); //��ʼ��SDK, �����ȵ�����ֻ�ܵ���һ��
	static void UnInitSdk();
	static CHyvStream *Create(std::string const& username, std::string const& password);

	void Release();
	void SetVideoCallBack(VideoCallBack pVideoCallBack, void *pContext);
	void SetOtherCallBack(OtherCallBack pOtherCallBack, void *pContext);
	void SetMsgCallBack(MsgCallBack pMsgCallBack, void *pContext);
	
	void SetTransType(int iTransType);//���ô��䷽ʽ��TCP��UDP
	void SetDecvType(int iDecvType); //����Decv����
	bool Start(char *strUrl);   //������Ƶ����ʼrtsp�����ûص�
	void Stop();                //ֹͣ��Ƶ,�ر�����Rtsp
	
	void GetVideoSize(int &iWidth, int &iHeight); //��ȡ������Ƶ���

	void RunThreadDecodeSws();//�̹߳�������,Ϊ�����������ܼ���
#ifdef WIN32
	static unsigned int WINAPI ThreadDecodeSws(LPVOID lpParam);//�̺߳�������ÿ10ms����һ��RunThreadDecodeSws
#else
	static void * ThreadDecodeSws(LPVOID lpParam);
#endif

private:
	CHyvStream(void);
	virtual ~CHyvStream(void);

public:
	void Init(int codecID, int iWidt, int iHeight);//��ʼ�����룬��Ҫ��rtsp��handleframe����(����ı估������ʼ�����)
	void ReturnMessage(int iMsgType, bool bSuccess);//������Ϣ
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
	int m_iCnt;//����������

#ifdef WIN32
	HANDLE m_hDecodeSwsThread;
#else
	pthread_t m_hDecodeSwsThread;
#endif
};

//extern HYVSTREAM_API int nHyvStream;
//HYVSTREAM_API int fnHyvStream(void);
