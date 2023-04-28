#pragma once

#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include "HyvSdk.h"

/* �ͻ��������豸ǰ׺: 0x7d 0x7c */
#define CLT_SEARCH_REQUEST1		0x7D
#define CLT_SEARCH_REQUEST2 	0x7C
/* �ɹ��豸�˻ظ�: 0x7d 0x7d  */
#define DEV_REPLY1 			0x7D
#define DEV_REPLY2 			0x7D

/* ����˻ظ�*/
#define MODIFY_IP_SUCCESSREPLY1	0x7D
#define MODIFY_IP_SUCCESSREPLY2	0x7D
#define MODIFY_IP_FAILREPLY1	0x7B
#define MODIFY_IP_FAILREPLY2	0x7B

#define BUFFER_SIZE 		1500
#define CLIENT_SEARCH_DEV_PORT 	36377
#define CLIENT_MODIFY_IP_PORT	36388 //�޸�IP��TCP��ʽ

using namespace std;

class HYVSTREAM_API CHyvSNMac
{
private:
	CHyvSNMac();
	int ParseInfo(char *buffer, char *sn, char *mac);
	int CheckIpMasksGateway(char *sIP,char *sMask,char *sGateway);
private:
	SOCKET sockClient;
	vector<string> ip_lists;
#ifdef WIN32
	SOCKADDR_IN addrClient;
#else
	struct sockaddr_in addrClient;
#endif
	int addr_len;
public:
	// ��������
	static CHyvSNMac * Create();

	// ��ʼ��socket
	int Init();
	
	// �ر�socket
	void Uninit();
	
	// ��ȡ��������IP���������ips��
	int GetLocalIPs(vector<string>& ips);
	
	// �����豸��Ϣ
	// iOvertime:��ʱʱ��,��λ����
	// ip������ip������ĸ��������͹㲥��Ϣ
	// ����ֵ  0:�ɹ�  -1:ʧ��  1:ʧ�ܣ����²���
	int SearchDevice(int iOvertime,const char *ip);

	// ��ȡSN��Mac1���豸IP
	// sn,Ҫ��ȡ��SN
	// mac,Ҫ��ȡ��Mac1
	// sIp,Ҫ��ȡ��IP
	// ����ֵ-1��ʧ��  1:��ȡ���ݳ�ʱ  2����ȡʧ�ܻ��ȡ���ݷǷ�   0����ȡSN,Mac1,IP�ɹ�
	int GetSNMac(char *sn, char *mac,char *sIp);

	// �޸��豸IP
	// sUsr:�˻���,sPassword:����,sSN:���к�(��GetSNMac��ȡ), sNetName:��������(Ŀǰ��"eth0")
	// sSourceIP:Ҫ�޸ĵ�ԴIP��ַ,sDestIp:Ҫ�޸ĵ�Ŀ��IP��ַ,sMask:��������,sGateway:����
	// ����ֵ-2:ʧ�� -1:������������룬���أ�Ŀ��IP��ƥ��  1:�޸�ʧ�� 0:�޸ĳɹ�
	int Modify(char *sUsr, char *sPassword, char *sSN, char *sNetName, char *sSourceIP, char *sDestIP, char *sMask, char *sGateway);


private:
	unsigned char sndrcvbuf[BUFFER_SIZE];
};
