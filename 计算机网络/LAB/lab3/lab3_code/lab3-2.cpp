/*
* THIS FILE IS FOR IP FORWARD TEST
*/
#include "sysInclude.h"
#include <vector>
using std::vector;
#include <iostream>
using std::cout;

typedef unsigned char 	uint8_t;
typedef unsigned short 	uint16_t;
typedef unsigned int	uint32_t;

// system support functions
extern void fwd_LocalRcv(char *pBuffer, int length);

extern void fwd_SendtoLower(char *pBuffer, int length, unsigned int nexthop);

extern void fwd_DiscardPkt(char *pBuffer, int type);

extern unsigned int getIpv4Address( );

// implemented by students

struct Route {
	uint32_t dest_mask;  // 目的IP
	uint32_t mask;       // 掩码
	uint32_t masklen;    // 掩码长度
	uint32_t nexthop;    // 下一跳
};

vector<Route> route_table; 

//****************************************
// Method:		stud_Route_Init
// FullName:	stud_Route_Init
// Access:		public
// Returns:		void
// Qualifier:	初始化路由表，本函数将在系统启动的时候被调用
//****************************************
void stud_Route_Init(void) {
	route_table.clear();
	return;
}

//****************************************
// Method:		stud_route_add
// FullName:	stud_route_add
// Access:		public
// Returns:		void
// Qualifier:	向路由表中增加一个新的表项，将参数所传递的路由信息添加到路由表中
// Parameter：	proute ： 指向需要添加路由信息的结构体头部，其数据结构
//					stud_route_msg 的定义如下：
//						typedef struct stud_route_msg
//						{
//							unsigned int dest;
//							unsigned int masklen;
//							unsigned int nexthop;
//						} stud_route_msg;
//****************************************
void stud_route_add(stud_route_msg *proute) {
	Route new_route_table_item;
	new_route_table_item.masklen = ntohl(proute->masklen);  // 将一个无符号长整形数从网络字节顺序转换为主机字节顺序
	new_route_table_item.mask = (1<<31)>>(ntohl(proute->masklen)-1); // 获取掩码，带着高位符号位位移
	new_route_table_item.dest_mask = ntohl(proute->dest); // 路由器存了目标mask
	new_route_table_item.nexthop = ntohl(proute->nexthop);
	route_table.push_back(new_route_table_item);
	return;
}

//****************************************
// Method:		stud_fwd_deal
// FullName:	stud_fwd_deal
// Access:		public
// Returns:		int
//				0 为成功，1 为失败；
// Qualifier:	本函数是 IPv4 协议接收流程的下层接口函数，实验系统从网络中接
//					收到分组后会调用本函数。调用该函数之前已完成 IP 报文的合法性检查，
//					因此学生在本函数中应该实现如下功能：
//					a. 判定是否为本机接收的分组，如果是则调用 fwd_LocalRcv( )；
//					b. 按照最长匹配查找路由表获取下一跳，查找失败则调用 fwd_DiscardPkt( )；
//					c.调用 fwd_SendtoLower( )完成报文发送；
//					d.转发过程中注意 TTL 的处理及校验和的变化。
// Parameter：	pBuffer：指向接收到的 IPv4 分组头部
//				length：IPv4 分组长度
//****************************************
int stud_fwd_deal(char *pBuffer, int length) {

	int TTL = (int)pBuffer[8];  //存储TTL
	int headerChecksum = ntohl(*(uint16_t*)(pBuffer+10)); 
	int DestIP = ntohl(*(uint32_t*)(pBuffer+16));
    	int headsum = pBuffer[0] & 0xf; 


	if(DestIP == getIpv4Address()) { //判断分组地址与本机地址是否相同,特判最后一跳
		fwd_LocalRcv(pBuffer, length); //将 IP 分组上交本机上层协议 
		return 0;
	}

	if(TTL <= 0) { //TTL 判断 小于0 不能转发 丢弃 IP 分组
		fwd_DiscardPkt(pBuffer, STUD_FORWARD_TEST_TTLERROR); //丢弃 IP 分组
		return 1;
	}

	//设置匹配位
	bool Match = false;
	uint32_t longestMatchLen = 0;
	int bestMatch = 0;
	int route_table_len = route_table.size();
	// 判断掩码是否匹配
	for(int i = 0; i < route_table_len; i++) {
		if(route_table[i].masklen > longestMatchLen && route_table[i].dest_mask == (DestIP & route_table[i].mask)) { // 找到匹配度最高的子网掩码
			bestMatch = i;
			Match = true;
			longestMatchLen = route_table[i].masklen;
		}
	}

	if (Match) { //匹配成功
		char *buffer = new char[length];
		memcpy(buffer, pBuffer, length);
		buffer[8]--; //TTL - 1
		int sum = 0;
    		uint16_t localCheckSum = 0;
		//uint16_t temp = 0;
		int tot_byte_num = headsum << 2; // 将IHL乘4转化字节数，方便指针移动
		for (int i = 0; i < tot_byte_num; i+=2) { // 校验和是对每16位做求和运算
			if (i == 10) continue;
			sum += (((uint8_t)buffer[i] << 8) + (uint8_t)buffer[i + 1]);
			if (sum > 0xffff) {
				sum %= 0xffff;
				sum += 1;
			}
		}

		//while (sum > 0xffff) sum = (sum >> 16) + (sum & 0xffff); // 溢出处理
        	//重新计算校验和
        	localCheckSum = htons(0xffff - (uint16_t)sum);
		memcpy(buffer+10, &localCheckSum, sizeof(uint16_t));
		// 发给下一层协议
		fwd_SendtoLower(buffer, length, route_table[bestMatch].nexthop);
		return 0;
	} else { //匹配失败
		fwd_DiscardPkt(pBuffer, STUD_FORWARD_TEST_NOROUTE); //丢弃 IP 分组
		return 1;
	}
	return 1;
}