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
	// ��������
	static CHyvDataApp * Create();

	// ��ʼ������
	// addr,Ҫ���ӵ�IP��ַ
	// ����ֵ  0�ɹ���-1ʧ��
	int Sock_Connect(char* addr);

	// write_len,Ҫ�������ݵĳ���
	// sData,Ҫ���͵�����
	// resultData,���յ����ݣ�����Ӧ����
	// ����ֵ-1��writeʧ��  0��writeʧ�ܣ�������write   1��write�ɹ�
	int Uart_Write(uint32_t write_len, unsigned char* sData, unsigned char* resultData);

	// read_len,Ҫ��ȡ���ݵĳ���
	// timeout,��ʱ����
	// resultData,���յ����ݣ�����Ӧ����
	// ����ֵ-1��readʧ��  0��readʧ�ܣ�������read   �ɹ�����Ҫ��ȡ�����ݳ���
	int Uart_Read(uint32_t read_len, uint32_t timeout, unsigned char* resultData);

	void Sock_Close();

	// �Ͽ����ӣ����ٶ���
	void Release();

private:
	unsigned char rcvbuf[RECV_BUF_NORMAL_SIZE];
	unsigned char sndbuf[SND_BUF_NORMAL_SIZE];
	SOCKET sockClient;
};

