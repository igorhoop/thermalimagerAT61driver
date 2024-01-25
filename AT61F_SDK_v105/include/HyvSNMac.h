#pragma once

#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include "HyvSdk.h"

/* 客户端搜索设备前缀: 0x7d 0x7c */
#define CLT_SEARCH_REQUEST1		0x7D
#define CLT_SEARCH_REQUEST2 	0x7C
/* 成功设备端回复: 0x7d 0x7d  */
#define DEV_REPLY1 			0x7D
#define DEV_REPLY2 			0x7D

/* 服务端回复*/
#define MODIFY_IP_SUCCESSREPLY1	0x7D
#define MODIFY_IP_SUCCESSREPLY2	0x7D
#define MODIFY_IP_FAILREPLY1	0x7B
#define MODIFY_IP_FAILREPLY2	0x7B

#define BUFFER_SIZE 		1500
#define CLIENT_SEARCH_DEV_PORT 	36377
#define CLIENT_MODIFY_IP_PORT	36388 //修改IP用TCP方式

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
	// 创建对象
	static CHyvSNMac * Create();

	// 初始化socket
	int Init();
	
	// 关闭socket
	void Uninit();
	
	// 获取所有网卡IP，结果放在ips中
	int GetLocalIPs(vector<string>& ips);
	
	// 查找设备信息
	// iOvertime:超时时间,单位毫秒
	// ip：网卡ip，针对哪个网卡发送广播信息
	// 返回值  0:成功  -1:失败  1:失败，重新查找
	int SearchDevice(int iOvertime,const char *ip);

	// 获取SN和Mac1及设备IP
	// sn,要获取的SN
	// mac,要获取的Mac1
	// sIp,要获取的IP
	// 返回值-1：失败  1:获取数据超时  2：获取失败或获取数据非法   0：获取SN,Mac1,IP成功
	int GetSNMac(char *sn, char *mac,char *sIp);

	// 修改设备IP
	// sUsr:账户名,sPassword:密码,sSN:序列号(由GetSNMac获取), sNetName:网络名称(目前是"eth0")
	// sSourceIP:要修改的源IP地址,sDestIp:要修改的目标IP地址,sMask:子网掩码,sGateway:网关
	// 返回值-2:失败 -1:传入的子网掩码，网关，目标IP不匹配  1:修改失败 0:修改成功
	int Modify(char *sUsr, char *sPassword, char *sSN, char *sNetName, char *sSourceIP, char *sDestIP, char *sMask, char *sGateway);


private:
	unsigned char sndrcvbuf[BUFFER_SIZE];
};
