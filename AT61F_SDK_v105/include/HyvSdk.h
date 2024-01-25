#ifndef HYVSDK_H
#define HYVSDK_H

#ifdef WIN32 

#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <process.h> 

#ifdef HYVSTREAM_EXPORTS
#define HYVSTREAM_API __declspec(dllexport)
#else
#define HYVSTREAM_API //__declspec(dllimport)
#endif

#else

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>

#define HYVSTREAM_API
#define WINAPI
#define LPVOID void*
typedef void *HANDLE;

typedef  int SOCKET;
#define INVALID_SOCKET  (SOCKET)(~0) 
#define SOCKET_ERROR            (-1) 

inline int GetLastError()
{
	return errno;
}

#define closesocket(x) close(x)   

typedef struct in_addr IN_ADDR;
typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BOOL            *PBOOL;
typedef BOOL             *LPBOOL;
typedef BYTE            *PBYTE;
typedef BYTE             *LPBYTE;
typedef int             *PINT;
typedef int              *LPINT;
typedef WORD            *PWORD;
typedef WORD             *LPWORD;
typedef long             *LPLONG;
typedef DWORD           *PDWORD;
typedef DWORD            *LPDWORD;
typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;
typedef const char* LPCTSTR, *LPCSTR;
typedef char* LPTSTR, *LPSTR;
typedef void* RET_TYPE;

#ifndef TRUE 
#define TRUE                1 
#endif 

#ifndef FALSE 
#define FALSE               0 
#endif   

#ifndef _T 
#define _T(x) x 
#endif 

#ifndef Sleep(x)
#define Sleep(x) sleep(x * 1000)
#endif

#endif

#ifndef uint32_t
typedef unsigned int uint32_t;
#endif

#define IN
#define OUT
#define INOUT

enum DECV_TYPE
{
	DECV_YUV = 0,
	DECV_RGB
};

typedef void(*VideoCallBack)(OUT char* pData, OUT int iWidth, OUT int iHeight, INOUT void* pContext);

#endif    //HYVSDK_H
