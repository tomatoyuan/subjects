/*
* THIS FILE IS FOR IP TEST
*/
// system support
#include "sysInclude.h"

extern void ip_DiscardPkt(char* pBuffer,int type);

extern void ip_SendtoLower(char*pBuffer,int length);

extern void ip_SendtoUp(char *pBuffer,int length);

extern unsigned int getIpv4Address();

// implemented by students

typedef unsigned char	uint8_t;
typedef unsigned short	uint16_t;
typedef unsigned int	uint32_t;

int stud_ip_recv(char *pBuffer,unsigned short length)
{
    	uint8_t version = pBuffer[0] >> 4;	// 取出第0行第1个字节的高四位-->version:版本号
    	uint8_t ip_head_length = pBuffer[0] & 0xf; // 取出第0行第1个字节的低四位-->IHL:首部长度
	uint16_t TTL = (unsigned short)pBuffer[8]; 	// 取出第2行第8个字节，即报文的第行第一个字节-->TTL:生存时间
	// uint16_t head_check_sum = ntohs(*(uint16_t *)(pBuffer + 10)); // 将第2行第3、4个字节16位数由网络字节顺序转换为主机字节顺序
	uint32_t dest_addr = ntohl(*(uint32_t*)(pBuffer + 16)); // 将第4行的32位目的IP地址取出
	
	// TTL为0，该报文发送超时
	if (TTL <= 0) {
		ip_DiscardPkt(pBuffer, STUD_IP_TEST_TTL_ERROR);
		return 1;
	}
	
	// IP 版本号错误，应该是IPV4
	if (version != 4) {
		ip_DiscardPkt(pBuffer, STUD_IP_TEST_VERSION_ERROR);
		return 1;
	}
	
	// 头部长度错，IHL以4字节为单位，IPV4报文首部至少要包含固定部分的5行，即至少包含20个字节，IHL>=5才合理
	if (ip_head_length < 5) {
		ip_DiscardPkt(pBuffer, STUD_IP_TEST_HEADLEN_ERROR);
		return 1;
	}
	
	// 目的地址错：目标地址既不是本机地址，也不是广播地址，说明不是发送给本机的
	if (dest_addr != getIpv4Address() && dest_addr != 0xffffffff) {
		ip_DiscardPkt(pBuffer,STUD_IP_TEST_DESTINATION_ERROR);  
		return 1;
	}
	

	// 检验校验和
	uint16_t sum = 0; 
	int tot_byte_num = ip_head_length << 2; // 将IHL乘4转化字节数，方便指针移动
	for (int i = 0; i < tot_byte_num; i+=2) { // 校验和是对每16位做求和运算
		//sum += (uint16_t)pBuffer[i];
		uint16_t temp = ((uint8_t)pBuffer[i] << 8) + (uint8_t)pBuffer[i+1];
		if (temp + sum > 0xffff) {
			sum += 1;	
		}
		sum += temp;
	}
	//while (sum > 0xffff) sum = (sum >> 16) + (sum & 0xffff); // 溢出处理

	if (sum != 0xffff) { // 加和不为全1，则报文出现错误，丢弃
		ip_DiscardPkt(pBuffer, STUD_IP_TEST_CHECKSUM_ERROR);
		return 1;
	}

	// 上面的检验过程全部通过，则正确接收该报文
 	ip_SendtoUp(pBuffer,length); 
	return 0;
}

int stud_ip_Upsend(char *pBuffer, unsigned short len, unsigned int srcAddr,
                   unsigned int dstAddr, byte protocol, byte ttl)
{
	char *IPBuffer = (char *)malloc((20 + len) * sizeof(char)); // 给报文分配首部+数据部分空间
	memset(IPBuffer, 0, len+20); // 此步清零必须有，否则后面计算校验和会出错
	IPBuffer[0] = 0x45;	// 设置版本号为4，报文头部长为5
	uint16_t total_tength =  htons(len + 20); // 报文总长度
	memcpy(IPBuffer + 2, &total_tength, 2); // 将报文总长度放进报文第0行后2个字节
	IPBuffer[8] = ttl;  	// TTL
	IPBuffer[9] = protocol; // 协议
    
	uint32_t src = htonl(srcAddr);	// 源IP地址
	uint32_t dis = htonl(dstAddr);	// 目的IP地址
	memcpy(IPBuffer + 12, &src, 4);	// 装载源IP地址
	memcpy(IPBuffer + 16, &dis, 4);	// 装载目的IP地址
     
 
	uint16_t head_check_sum = 0;
	uint16_t sum = 0; 
	uint16_t temp = 0;
	for (int i = 0; i < 20; i+=2) { // 校验和是对每16位做求和运算
		//sum += (uint16_t)pBuffer[i];
		temp = ((uint8_t)IPBuffer[i] << 8 )+ (uint8_t)IPBuffer[i+1];
		if (temp + sum > 0xFFFF) {
			sum += 1;	
		}
		sum += temp;
	}

	head_check_sum = htons(0xFFFF - sum); // 取反

	memcpy(IPBuffer + 10, &head_check_sum, 2); // 装载校验和
	memcpy(IPBuffer + 20, pBuffer, len); // 装载数据部分
	ip_SendtoLower(IPBuffer,len+20); // 发送分组
	return 0;  
}