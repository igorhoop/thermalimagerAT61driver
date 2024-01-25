#pragma once

#include <iostream>
#include "HyvSdk.h"

#define TRANSPARENT_SERIAL_FRAME_START 		(0x7D)
#define TRANSPARENT_SERIAL_FRAME_END 		(0x7B)
#define TRANSPARENT_SERIAL_WRITE 			(0xff)
#define TRANSPARENT_SERIAL_WRITE_HEAD_SIZE 	(0x4)
#define TRANSPARENT_SERIAL_READ 			(0x00)
#define TRANSPARENT_SERIAL_READ_HEAD_SIZE 	(0x4)
#define TRANSPARENT_SERIAL_STATUS_FAILED 	(0xff)
#define TRANSPARENT_SERIAL_STATUS_SUCCESS 	(0x00)

#define RECV_BUF_NORMAL_SIZE 		(1500)
#define SND_BUF_NORMAL_SIZE 		(1280)
#define TRANSPARENT_SERIAL_TCP_PORT 	36399

class HYVSTREAM_API CHyvDataApp
{
private:
	CHyvDataApp();
public:
	// 创建对象
	static CHyvDataApp * Create();

	// 初始化连接
	// addr,要连接的IP地址
	// 返回值  0成功，-1失败
	int Sock_Connect(char* addr);

	// write_len,要发送数据的长度
	// sData,要发送的数据
	// resultData,接收的数据，即响应报文
	// 返回值-1：write失败  0：write失败，需重新write   1：write成功
	int Uart_Write(uint32_t write_len, unsigned char* sData, unsigned char* resultData);

	// read_len,要读取数据的长度
	// timeout,超时设置
	// resultData,接收的数据，即响应报文
	// 返回值-1：read失败  0：read失败，需重新read   成功返回要读取的数据长度
	int Uart_Read(uint32_t read_len, uint32_t timeout, unsigned char* resultData);

	void Sock_Close();

	// 断开连接，销毁对象
	void Release();

private:
	unsigned char rcvbuf[RECV_BUF_NORMAL_SIZE];
	unsigned char sndbuf[SND_BUF_NORMAL_SIZE];
	SOCKET sockClient;
};

