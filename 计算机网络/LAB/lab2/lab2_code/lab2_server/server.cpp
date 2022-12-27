#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <WinSock2.h>
#include <fstream>
#include <iostream>
using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#define	SERVER_PORT	12340		//端口号
#define	SERVER_IP	"0.0.0.0"	//IP地址
const int BUFFER_LENGTH = 1026;	//缓冲区大小，（以太网中 UDP 的数据帧中包长度应小于 1480 字节）
const int SEND_WIND_SIZE = 4;	//发送窗口的大小为10，GBN 中应满足 W+1<=N （W：发送窗口大小，N：序列号个数）
								//本例取序列号 0...19 共 20 个
								//如果将窗口大小设置为1，则为停-等协议
const int RECV_WIND_SIZE = 4;	//数据接收窗口的大小
const int SEQ_SIZE = 20;		//序列号的个数，从 0~19 共计20个
								//由于发送数据第一个字节如果值为0，则数据发送会失败
								//因此接收端序号为1~20，与发送端一一对应
const int PACKET_NUM_MAX = 100; //数据最多被拆分为 PACKET_NUM_MAX 份

BOOL ack[SEQ_SIZE];	//收到 ack 情况，对应 0~19 的ack
int curSeq;	//当前数据包的 seq
int curAck;	//当前等待确认的 ack
int totalSeq;		//收到的包的总数
int totalPacket;	//需要发送的包的总数
char buffer[BUFFER_LENGTH];	// 数据发送接收缓冲区

typedef struct SRWindow { //数据收发窗口，左闭右开
	int left;
	int right;

} SRWindow;

void getCurTime(char* ptime); //获取当前时间
bool seqIsAvailable(); //当前序列号是否可用，即是否可以接收数据报（可能仍然在等待上一批的数据报）
void timeoutHandler(); //超时重传
void ackHandler(char c); //累计确认 ACK
void TestGBN(SOCKET sockServer, SOCKADDR_IN* addrClient, char buffer[]);
void Download(SOCKET sockServer, SOCKADDR_IN* addrClient, char buffer[]);
void Upload(SOCKET socketClient, SOCKADDR_IN* addrServer, float packetLossRatio, float ackLossRatio, char buffer[]);
void DownloadSR(SOCKET sockServer, SOCKADDR_IN* addrClient, char buffer[]);
void UploadSR(SOCKET sockServer, SOCKADDR_IN* addrClient, float packetLossRatio, float ackLossRatio, char buffer[]);
BOOL lossInLossRatio(float lossRatio);

// 主函数
int main(int argc, char* argv[]) {
	//加载套接字库（必须）
	WORD wVersionRequested;
	WSADATA wsaData; //用来存储被WSAStartup函数调用后返回的Windows Sockets数据。它包含Winsock.dll执行的数据。
	//套接字加载时错误提示
	int err;
	//版本 2.2
	wVersionRequested = MAKEWORD(2, 2);
	//加载 dll 文件 Socket 库
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		// 找不到 winsock.dll
		printf("WSAStartup failed with error: %d\n", err);
		return -1;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		printf("Could not find a usable version of Winsock.dll\n");
		WSACleanup();
	}
	else {
		printf("The Winsock 2.2 dll was found okay\n");
	}
	SOCKET sockServer = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	//设置套接字为非阻塞模式
	int iMode = 1;	//1：非阻塞	0：阻塞
	ioctlsocket(sockServer, FIONBIO, (u_long FAR*) &iMode); //非阻塞设置
	SOCKADDR_IN addrServer;	//服务器地址
	//addrServer.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
	addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY); //二者均可
	addrServer.sin_family = AF_INET; //internetwork: UDP, TCP, etc.
	addrServer.sin_port = htons(SERVER_PORT);
	float packetLossRatio = 0.0; //默认包丢失率 0.0
	float ackLossRatio = 0.0; //默认 ACK 丢失率 0.0d
	err = bind(sockServer, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
	if (err) {
		err = GetLastError();
		printf("Could not bind the port %d for socket.Error code is %d\n", SERVER_PORT, err);
		WSACleanup();
		return -1;
	}
	else {
		printf("绑定端口 %d 成功\n", SERVER_PORT);
	}

	SOCKADDR_IN addrClient;	//客户端地址
	int length = sizeof(SOCKADDR);
	//char buffer[BUFFER_LENGTH];	// 数据发送接收缓冲区
	ZeroMemory(buffer, sizeof(buffer));

	int recvSize;
	for (int i = 0; i < SEQ_SIZE; ++i) { // ????????????????????????????
		ack[i] = TRUE;
	}
	while (true) {
		//非阻塞接收，若没有收到数据，则返回值为-1
		recvSize = recvfrom(sockServer, buffer, BUFFER_LENGTH, 0, ((SOCKADDR*)&addrClient), &length);
		//printf("here still right %d\n", recvSize);
		if (recvSize < 0) {
			Sleep(200);
			continue;
		}
		printf("recv from client: %s\n", buffer);
		if (strcmp(buffer, "-time") == 0) {
			getCurTime(buffer);
			sendto(sockServer, buffer, strlen(buffer) + 1, 0, (SOCKADDR*)&addrClient, sizeof(SOCKADDR));
		}
		else if (strcmp(buffer, "-quit") == 0) {
			strcpy_s(buffer, strlen("Good bye!") + 1, "Good bye!");
			sendto(sockServer, buffer, strlen(buffer) + 1, 0, (SOCKADDR*)&addrClient, sizeof(SOCKADDR));
		}
		else if (strcmp(buffer, "-testgbn") == 0) { // 进入 GBN 测试阶段
			TestGBN(sockServer, &addrClient, buffer);
		}
		else if (strcmp(buffer, "-upload") == 0) {
			//printf("upload\n");
			Upload(sockServer, &addrClient, packetLossRatio, ackLossRatio, buffer);
		}
		else if (strcmp(buffer, "-download") == 0) {
			//printf("download\n");
			Download(sockServer, &addrClient, buffer);
		}
		else if (strcmp(buffer, "-SR_upload") == 0) {
			printf("SR_upload\n");
			UploadSR(sockServer, &addrClient, packetLossRatio, ackLossRatio, buffer);
		}
		else if (strcmp(buffer, "-SR_download") == 0) {
			//printf("SR_download\n");
			DownloadSR(sockServer, &addrClient, buffer);
		}
		Sleep(100);
	}
	//关闭套接字，卸载库
	closesocket(sockServer);
	WSACleanup();
	return 0;
}

//****************************************
// Method:	getCurTime
// FullName:	getCurTime
// Access:		public
// Returns:	void
// Qualifier:	获取当前系统事件，结果存入 ptime 中
// Parameter：	char *ptime
//****************************************
void getCurTime(char* ptime) {
	char buffer[128];
	memset(buffer, 0, sizeof(buffer));
	time_t c_time;
	struct tm* p;
	time(&c_time);
	p = localtime(&c_time);
	sprintf_s(buffer, "%d/%d/%d %d:%d:%d",
		p->tm_year + 1900,
		p->tm_mon + 1,
		p->tm_mday,
		p->tm_hour,
		p->tm_min,
		p->tm_sec);
	strcpy_s(ptime, sizeof(buffer), buffer);
}

//****************************************
// Method:	seqIsAvailable
// FullName:	seqIsAvailable
// Access:		public
// Returns:	bool
// Qualifier:	当前序列号 curSeq 是否可用
//****************************************
bool seqIsAvailable() {
	int step;
	step = curSeq - curAck;
	step = step >= 0 ? step : step + SEQ_SIZE;
	//序列号是否在当前发送窗口之内
	if (step >= SEND_WIND_SIZE) {
		return false;
	}
	if (ack[curSeq]) {
		return true;
	}
	return false;
}

//****************************************
// Method:	timeoutHandler
// FullName:	timeoutHandler
// Access:		public
// Returns:	void
// Qualifier:	超时重传处理函数，滑动窗口内的数据帧都要重传
//****************************************
void timeoutHandler() {
	printf("Timer out error.\n");
	int index;
	for (int i = 0; i < SEND_WIND_SIZE; ++i) { //将窗口中的数据记录刷新
		index = (i + curAck) % SEQ_SIZE;
		ack[index] = TRUE;
	}
	// 重传窗口中的全部数据
	totalSeq -= SEND_WIND_SIZE;
	curSeq = curAck;
}

//****************************************
// Method:	ackHandler
// FullName:	ackHandler
// Access:		public
// Returns:	void
// Qualifier:	收到 ack ，累计确认，取数据帧的第一个字节
//		由于发送数据时，第一个字节（序列号）为 0 （ASCII)时会发送失败，
//		因此做了加 1 处理，此处减一还原
// Parameter:	char c
//****************************************
void ackHandler(char c) {
	unsigned char index = (unsigned char)c - 1;	// 序列号减一
	printf("Recv a ack of %d\n", index);
	if (curAck <= index) {
		for (int i = curAck; i <= index; ++i) { // 累计确认，接收到的ack之前的数据报认为都被接收了
			ack[i] = TRUE;
		}
		curAck = (index + 1) % SEQ_SIZE;
	}
	else {	//ack 超过了最大值，即窗口向前滑动到下一个窗口，确认当前窗口剩余的 ack
			 //将滑动后的新窗口的 index 之前的 ack 确认接收
		for (int i = curAck; i < SEQ_SIZE; ++i) {
			ack[i] = TRUE;
		}
		for (int i = 0; i <= index; ++i) {
			ack[i] = TRUE;
		}
		curAck = index + 1; //更新待接收的 ack 的位置
	}
}
//****************************************
// Method:	TestGBN
// FullName:	TestGBN
// Access:		public
// Returns:	void
// Qualifier:	执行测试GBN数据传输模块
// Parameter:	SOCKET sockServer
// Parameter:	SOCKADDR_IN *addrClient
// Parameter:	buffer[]
//****************************************
const int read_test_data_size = 13;
void TestGBN(SOCKET sockServer, SOCKADDR_IN *addrClient, char buffer[]) {
	//首先 server （server 处于 0 状态）向 client 发送 205 状态码（server 进入 1 状态）
	//server 等待 client 回复 200 状态码，如果收到（server 进入 2 状态），开始传输文件，否则延时等待至超时
	
	// 将测试数据读入内存
	std::ifstream icin; //从硬盘到内存，其实所谓的流缓冲就是内存空间;
	icin.open("../test.txt");
	char data[1024 * read_test_data_size];
	ZeroMemory(data, sizeof(data));
	icin.read(data, 1024 * read_test_data_size);
	icin.close();
	//cout << data << endl;
	totalPacket = sizeof(data) / 1024; //数据包的数量
	ZeroMemory(buffer, sizeof(buffer));
	int recvSize;
	int waitCount = 0;
	printf("Begin to test GBN protocol, please don't abort the process\n");
	//加入一个握手阶段
	//首先服务器向客户端发送一个 205 大小的状态码（自己定义的）表示服务器准备好了，可以发送数据
	//客户端收到 205 之后回复一个 200 大小的状态码，表示客户端准备好了，可以接收数据了
	int length = sizeof(SOCKADDR);
	printf("Shake hands stage\n");
	int stage = 0;
	bool runFlag = true;
	while (runFlag) {
		switch (stage) {
		case 0: //发送 205 阶段
			buffer[0] = 205;
			sendto(sockServer, buffer, strlen(buffer) + 1, 0, (SOCKADDR*)addrClient, sizeof(SOCKADDR));
			Sleep(100);
			stage = 1;
			break;
		case 1: //等待接收 200 阶段，没有收到则计数器 +1，超时则放弃此次“连接”，等待从第一步开始
			recvSize = recvfrom(sockServer, buffer, BUFFER_LENGTH, 0, ((SOCKADDR*)addrClient), &length);
			if (recvSize < 0) {
				++waitCount;
				if (waitCount > 20) {
					runFlag = false;
					printf("Timeout error\n");
					break;
				}
				Sleep(100);
				continue;
			}
			else { // 收到客户端的答复了
				if ((unsigned char)buffer[0] == 200) {
					printf("Begin a file transfer\n");
					printf("File size is %dB, each packet is 1024B and packet total num is %d\n", sizeof(data), totalPacket);
					curSeq = 0;
					curAck = 0;
					totalSeq = 0;
					waitCount = 0;
					stage = 2;
				}
			}
			break;
		case 2:	// 数据传输阶段
			if (seqIsAvailable() && totalSeq <= totalPacket) {
				//发送给客户端的序列号从 1 开始
				buffer[0] = curSeq + 1;
				ack[curSeq] = FALSE;
				//数据发送的过程中应该判断是否传输完成（此处简化了，未实现）
				memcpy(&buffer[1], data + 1024 * totalSeq, 1024); //每次向后偏移一个数据报切片的位置，直到把整个数据包发送出去
				printf("send a packet with a seq of %d\n", curSeq);
				sendto(sockServer, buffer, BUFFER_LENGTH, 0, (SOCKADDR*)addrClient, sizeof(SOCKADDR));
				++curSeq;
				curSeq %= SEQ_SIZE;
				++totalSeq;
				Sleep(100);
			}
			//等待 Ack， 若没有收到，则返回-1，计数器+1
			recvSize = recvfrom(sockServer, buffer, BUFFER_LENGTH, 0, ((SOCKADDR*)addrClient), &length);
			if (recvSize < 0) {
				waitCount++;
				//20 次等待 ack 则超时重传
				if (waitCount > 20) {
					timeoutHandler();
					waitCount = 0;
				}
			}
			else { // 收到 ack
				ackHandler(buffer[0]);
				if (totalSeq >= totalPacket && (unsigned char)buffer[0] == totalPacket) stage = 3; //数据传输完了，结束传输
				waitCount = 0;
			}
			Sleep(100);
			break;
		case 3: // 数据传输完成
			printf("Data send over!\n");
			buffer[0] = 255;
			sendto(sockServer, buffer, strlen(buffer) + 1, 0, (SOCKADDR*)addrClient, sizeof(SOCKADDR));
			Sleep(100);
			runFlag = false;
			break;
		}
	}
}

//****************************************
// Method:	Download
// FullName:	Download
// Access:		public
// Returns:	void
// Qualifier:	客户端请求下载文件
// Parameter:	SOCKET sockServer
// Parameter:	SOCKADDR_IN *addrClient
// Parameter:	buffer[]
//****************************************
const int download_data_size = 5; //最大的分组数
void Download(SOCKET sockServer, SOCKADDR_IN* addrClient, char buffer[]) {
	//首先 server （server 处于 0 状态）向 client 发送 205 状态码（server 进入 1 状态）
	//server 等待 client 回复 200 状态码，如果收到（server 进入 2 状态），开始传输文件，否则延时等待至超时

	// 将测试数据读入内存
	std::ifstream icin; //从硬盘到内存，其实所谓的流缓冲就是内存空间;
	icin.open("../server_send.txt");
	char data[1024 * download_data_size];
	ZeroMemory(data, sizeof(data));
	icin.read(data, 1024 * download_data_size);
	icin.close();

	totalPacket = sizeof(data) / 1024; //数据包的数量
	ZeroMemory(buffer, sizeof(buffer));
	int recvSize;
	int waitCount = 0;
	printf("Begin to Download from server with GBN protocol, please don't abort the process\n");
	//加入一个握手阶段
	//首先服务器向客户端发送一个 205 大小的状态码（自己定义的）表示服务器准备好了，可以发送数据
	//客户端收到 205 之后回复一个 200 大小的状态码，表示客户端准备好了，可以接收数据了
	int length = sizeof(SOCKADDR);
	printf("Shake hands stage\n");
	int stage = 0;
	bool runFlag = true;
	while (runFlag) {
		switch (stage) {
		case 0: //发送 205 阶段
			buffer[0] = 205;
			sendto(sockServer, buffer, strlen(buffer) + 1, 0, (SOCKADDR*)addrClient, sizeof(SOCKADDR));
			Sleep(100);
			stage = 1;
			break;
		case 1: //等待接收 200 阶段，没有收到则计数器 +1，超时则放弃此次“连接”，等待从第一步开始
			recvSize = recvfrom(sockServer, buffer, BUFFER_LENGTH, 0, ((SOCKADDR*)addrClient), &length);
			if (recvSize < 0) {
				++waitCount;
				if (waitCount > 20) {
					runFlag = false;
					printf("Timeout error\n");
					break;
				}
				Sleep(100);
				continue;
			}
			else { // 收到客户端的答复了
				if ((unsigned char)buffer[0] == 200) {
					printf("Begin a file transfer\n");
					printf("File size is %dB, each packet is 1024B and packet total num is %d\n", sizeof(data), totalPacket);
					curSeq = 0;
					curAck = 0;
					totalSeq = 0;
					waitCount = 0;
					stage = 2;
				}
			}
			break;
		case 2:	// 数据传输阶段
			if (seqIsAvailable() && totalSeq <= totalPacket) {
				//发送给客户端的序列号从 1 开始
				buffer[0] = curSeq + 1;
				ack[curSeq] = FALSE;
				//数据发送的过程中应该判断是否传输完成（此处简化了，未实现）
				memcpy(&buffer[1], data + 1024 * totalSeq, 1024); //每次向后偏移一个数据报切片的位置，直到把整个数据包发送出去
				printf("send a packet with a seq of %d\n", curSeq);
				sendto(sockServer, buffer, BUFFER_LENGTH, 0, (SOCKADDR*)addrClient, sizeof(SOCKADDR));
				++curSeq;
				curSeq %= SEQ_SIZE;
				++totalSeq;
				Sleep(100);
			}
			//等待 Ack， 若没有收到，则返回-1，计数器+1
			recvSize = recvfrom(sockServer, buffer, BUFFER_LENGTH, 0, ((SOCKADDR*)addrClient), &length);
			if (recvSize < 0) {
				waitCount++;
				//20 次等待 ack 则超时重传
				if (waitCount > 20) {
					timeoutHandler();
					waitCount = 0;
				}
			}
			else { // 收到 ack
				ackHandler(buffer[0]);
				if (totalSeq >= totalPacket && (unsigned char)buffer[0] == totalPacket) stage = 3; //数据传输完了，结束传输
				waitCount = 0;
			}
			Sleep(100);
			break;
		case 3: // 数据传输完成
			printf("Data send over!\n");
			buffer[0] = 255;
			sendto(sockServer, buffer, strlen(buffer) + 1, 0, (SOCKADDR*)addrClient, sizeof(SOCKADDR));
			Sleep(100);
			runFlag = false;
			break;
		}
	}
}

//****************************************
// Method:	SRDownload
// FullName:	SRDownload
// Access:		public
// Returns:	void
// Qualifier:	客户端请求下载文件，使用SR协议
// Parameter:	SOCKET sockServer
// Parameter:	SOCKADDR_IN *addrClient
// Parameter:	buffer[]
//****************************************
void DownloadSR(SOCKET sockServer, SOCKADDR_IN* addrClient, char buffer[]) {
	//首先 server （server 处于 0 状态）向 client 发送 205 状态码（server 进入 1 状态）
	//server 等待 client 回复 200 状态码，如果收到（server 进入 2 状态），开始传输文件，否则延时等待至超时

	// 将测试数据读入内存
	std::ifstream icin; //从硬盘到内存，其实所谓的流缓冲就是内存空间;
	icin.open("../server_send.txt");
	char data[1024 * download_data_size];
	ZeroMemory(data, sizeof(data));
	icin.read(data, 1024 * download_data_size);
	icin.close();

	SRWindow send_window; //数据发送窗口

	totalPacket = sizeof(data) / 1024; //数据包的数量
	ZeroMemory(buffer, sizeof(buffer));
	int recvSize;
	int waitCount = 0;
	printf("Begin to Download from server with GBN protocol, please don't abort the process\n");
	//加入一个握手阶段
	//首先服务器向客户端发送一个 205 大小的状态码（自己定义的）表示服务器准备好了，可以发送数据
	//客户端收到 205 之后回复一个 200 大小的状态码，表示客户端准备好了，可以接收数据了
	int length = sizeof(SOCKADDR);
	printf("Shake hands stage\n");
	int stage = 0;
	bool runFlag = true;
	while (runFlag) {
		switch (stage) {
		case 0: //发送 205 阶段
			buffer[0] = 205;
			sendto(sockServer, buffer, strlen(buffer) + 1, 0, (SOCKADDR*)addrClient, sizeof(SOCKADDR));
			Sleep(100);
			stage = 1;
			break;
		case 1: //等待接收 200 阶段，没有收到则计数器 +1，超时则放弃此次“连接”，等待从第一步开始
			recvSize = recvfrom(sockServer, buffer, BUFFER_LENGTH, 0, ((SOCKADDR*)addrClient), &length);
			if (recvSize < 0) {
				++waitCount;
				if (waitCount > 20) {
					runFlag = false;
					printf("Timeout error\n");
					break;
				}
				Sleep(100);
				continue;
			}
			else { // 收到客户端的答复了
				if ((unsigned char)buffer[0] == 200) {
					printf("Begin a file transfer\n");
					printf("File size is %dB, each packet is 1024B and packet total num is %d\n", sizeof(data), totalPacket);
					curSeq = 0;
					curAck = 0;
					send_window.left = 0;
					send_window.right = SEND_WIND_SIZE;
					stage = 2;
				}
			}
			break;
		case 2:	// 数据传输阶段
			if (curSeq < totalPacket && curSeq >= send_window.left && curSeq < send_window.right) { //数据报在发送窗口内，则发送
				//发送给客户端的序列号从 1 开始
				buffer[0] = curSeq + 1;
				ack[curSeq] = false; //发送一个数据报后，开始计时，收到ack，时间清0，超时重发
				//数据发送的过程中应该判断是否传输完成（此处简化了，未实现）
				memcpy(&buffer[1], data + 1024 * curSeq, 1024); //每次向后偏移一个数据报切片的位置，直到把整个数据包发送出去
				printf("send a packet with a seq of %d\n", curSeq);
				sendto(sockServer, buffer, BUFFER_LENGTH, 0, (SOCKADDR*)addrClient, sizeof(SOCKADDR));
				++curSeq;
				Sleep(100);
			}
			else if (ack[send_window.left] == false) { //如果窗口最左侧的ack还没有收到，则重发
				buffer[0] = send_window.left + 1;
				memcpy(&buffer[1], data + 1024 * send_window.left, 1024); //每次向后偏移一个数据报切片的位置，直到把整个数据包发送出去
				printf("send a packet with a seq of %d\n", send_window.left);
				sendto(sockServer, buffer, BUFFER_LENGTH, 0, (SOCKADDR*)addrClient, sizeof(SOCKADDR));
				Sleep(100);
			}
			//等待 Ack， 若收到，更新窗口位置
			recvSize = recvfrom(sockServer, buffer, BUFFER_LENGTH, 0, ((SOCKADDR*)addrClient), &length);
			if (recvSize >= 0) { // 收到 ack
				printf("Recv a ack of %d\n", (unsigned char)buffer[0] - 1);
				ack[(unsigned char)buffer[0] - 1] = true; //标记为已接收ack
				while (send_window.left < curSeq && ack[send_window.left]) { //如果数据报已经发送过了，并且接收到了ack，则将窗口向右移动至合适的位置
					send_window.left++;
					send_window.right++;
				}
				if (send_window.left >= totalPacket) stage = 3; //数据传输完了，结束传输
			}
			Sleep(100);
			break;
		case 3: // 数据传输完成
			printf("Data send over!\n");
			buffer[0] = 255;
			sendto(sockServer, buffer, strlen(buffer) + 1, 0, (SOCKADDR*)addrClient, sizeof(SOCKADDR));
			Sleep(100);
			runFlag = false;
			break;
		}
	}
}

//****************************************
// Method:	Upload
// FullName:	Upload
// Access:		public
// Returns:	BOOL
// Qualifier:	客户端向服务器上传文件
// Parameter：	SOCKET socketClient
// Parameter：	SOCKADDR_IN addrServer
// Parameter：	float packetLossRatio[0,1] 丢包概率
// Parameter：	float ackLossRatio[0,1] 丢ack概率
// Parameter：	char buffer[]
//****************************************
const int upload_data_size = 5; //最大分组数
void Upload(SOCKET sockServer, SOCKADDR_IN* addrClient, float packetLossRatio, float ackLossRatio, char buffer[]) {
	int len = sizeof(SOCKADDR);
	printf("%s\n", "Begin to upload from client with GBN protocol, please don't abort the process");
	printf("The loss ratio of packet is %.2f,the loss ratio of ack is %.2f\n", packetLossRatio, ackLossRatio);
	int waitCount = 0;
	int stage = 0;
	BOOL b;
	unsigned char u_code; //状态码
	unsigned short seq;	//包的序列号
	unsigned short recvSeq; //接收窗口大小为 1，已确认的序列号
	unsigned short waitSeq; //等待的序列号

	// 将测试数据读入内存
	std::ofstream outfile; //从硬盘到内存，其实所谓的流缓冲就是内存空间;
	outfile.open("../server_recv.txt");
	char data[1024 * download_data_size];
	ZeroMemory(buffer, sizeof(buffer));
	//printf("here %s\n", cmd);
	BOOL upload_flag = true;
	while (upload_flag) {
		// 等待 客户端 回复设置为 UDP 阻塞模式
		//ZeroMemory(buffer, sizeof(buffer));
		int recvSize = recvfrom(sockServer, buffer, BUFFER_LENGTH, 0, (SOCKADDR*)addrClient, &len);
		if (recvSize < 0) continue;
		if ((unsigned char)buffer[0] == 255) stage = 3; //收到255，表示数据已经全部发送完毕
		switch (stage) {
		case 0: //等待握手阶段
			u_code = (unsigned char)buffer[0];
			if (u_code == 205) {
				printf("Ready for file upload\n");
				buffer[0] = 200;
				//buffer[1] = '\0';
				sendto(sockServer, buffer, 2, 0, (SOCKADDR*)addrClient, sizeof(SOCKADDR));
				stage = 1;
				recvSeq = 0;
				waitSeq = 1;
			}
			break;
		case 1: //等待接收数据阶段
			seq = (unsigned short)buffer[0];
			if (seq > 255) continue;
			//随机法模拟包是否丢失
			b = lossInLossRatio(packetLossRatio);
			if (b) {
				printf("The packet with seq of %d loss\n", seq);
				continue;
			}
			printf("recv a packet with a seq of %d\n", seq);
			//Sleep(1000);//??????????????????
			if (!(waitSeq - seq)) { //如果是期待的包，正确接收，正常确认即可
				//cout << "wait ：" << waitSeq << endl;
				memcpy(data + 1024 * (waitSeq - 1), &buffer[1], 1024);
				++waitSeq;
				if (waitSeq == upload_data_size + 1) {
					waitSeq = 1;
				}
				//输出数据
				//printf("%s\n", buffer[1]);
				buffer[0] = seq;

				recvSeq = seq;
				buffer[1] = '\0';
			}
			else { //如果当前一个包都没有收到，则等待 Seq 为 1 的数据包，不是则不返回 ACK（因为并没有上一个正确的 ACK）
				if (!recvSeq) continue;
				buffer[0] = recvSeq;
				buffer[1] = '\0';
			}
			b = lossInLossRatio(ackLossRatio);
			if (b) {
				printf("The ack of %d loss\n", (unsigned char)buffer[0]);
				continue;
			}
			sendto(sockServer, buffer, 2, 0, (SOCKADDR*)addrClient, sizeof(SOCKADDR));
			printf("send a ack of %d\n", (unsigned char)buffer[0]);
			break;
		case 3:
			upload_flag = false;
			outfile.write(data, 1024 * download_data_size);
			outfile.close();
			ZeroMemory(buffer, sizeof(buffer));
			break;
		}
		Sleep(100);
	}
}

//****************************************
// Method:	UploadSR
// FullName:	UploadSR
// Access:		public
// Returns:	void
// Qualifier:	客户端从服务器下载文件,使用 SR 协议
// Parameter：	SOCKET socketClient
// Parameter：	SOCKADDR_IN addrServer
// Parameter：	float packetLossRatio[0,1] 丢包概率
// Parameter：	float ackLossRatio[0,1] 丢ack概率
// Parameter：	char buffer[]
//****************************************
void UploadSR(SOCKET sockServer, SOCKADDR_IN* addrClient, float packetLossRatio, float ackLossRatio, char buffer[]) {
	int len = sizeof(SOCKADDR);
	printf("%s\n", "Begin to let client download with GBN protocol, please don't abort the process");
	printf("The loss ratio of packet is %.2f,the loss ratio of ack is %.2f\n", packetLossRatio, ackLossRatio);
	int waitCount = 0;
	int stage = 0;
	BOOL b;
	unsigned char u_code; //状态码
	unsigned short seq;	//包的序列号
	BOOL recv_mark[PACKET_NUM_MAX]; //标记某个编号的数据是否被接收
	memset(recv_mark, false, sizeof(recv_mark)); //初始化为没有数据报被接收
	SRWindow recv_window; //数据接收窗口
	// 将测试数据读入内存
	std::ofstream outfile; //从硬盘到内存，其实所谓的流缓冲就是内存空间;
	outfile.open("../server_recv.txt");
	char data[1024 * download_data_size];

	//printf("here %s\n", cmd);
	BOOL download_flag = true;
	while (download_flag) {
		// 等待 server 回复设置为 UDP 阻塞模式
		int recvSize = recvfrom(sockServer, buffer, BUFFER_LENGTH, 0, (SOCKADDR*)addrClient, &len);
		if (recvSize < 0) continue;
		if ((unsigned char)buffer[0] == 255) stage = 3; //收到255，表示数据已经全部发送完毕
		switch (stage) {
		case 0: //等待握手阶段
			u_code = (unsigned char)buffer[0];
			if (u_code == 205) {
				printf("Ready for file upload\n");
				buffer[0] = 200;
				buffer[1] = '\0';
				sendto(sockServer, buffer, 2, 0, (SOCKADDR*)addrClient, sizeof(SOCKADDR));
				stage = 1;
				recv_window.left = 0;
				recv_window.right = RECV_WIND_SIZE;
			}
			break;
		case 1: //等待接收数据阶段
			seq = (unsigned short)buffer[0];
			//随机法模拟包是否丢失
			b = lossInLossRatio(packetLossRatio);
			if (b) {
				printf("The packet with seq of %d loss\n", seq);
				continue;
			}
			printf("recv a packet with a seq of %d\n", seq);
			if (seq - 1 >= recv_window.left && seq - 1 < recv_window.right) { //如果seq在数据接收窗口内，则接收，并返回ACK
				//cout << "wait ：" << waitSeq << endl;
				memcpy(data + 1024 * (seq - 1), &buffer[1], 1024);
				recv_mark[seq - 1] = true;
				while (recv_mark[recv_window.left]) { //向右移动窗口至窗口最左侧的数据还未接收到的位置
					recv_window.left++;
					recv_window.right++;
				}
				//输出数据
				//printf("%s\n", buffer[1]);
				buffer[0] = seq;
				buffer[1] = '\0';
			}
			else if (seq - 1 < recv_window.left) { //如果收到重发的已接收过的数据报，直接返回ack即可
				buffer[0] = seq;
				buffer[1] = '\0';
			}
			b = lossInLossRatio(ackLossRatio);
			if (b) {
				printf("The ack of %d loss\n", (unsigned char)buffer[0]);
				continue;
			}
			sendto(sockServer, buffer, 2, 0, (SOCKADDR*)addrClient, sizeof(SOCKADDR));
			printf("send a ack of %d\n", (unsigned char)buffer[0]);
			Sleep(100);
			break;
		case 3:
			download_flag = false;
			outfile.write(data, 1024 * download_data_size);
			outfile.close();
			ZeroMemory(buffer, sizeof(buffer));
			break;
		}
		Sleep(100);
	}
}

//****************************************
// Method:	lossInLossRatio
// FullName:	lossInLossRatio
// Access:		public
// Returns:	BOOL
// Qualifier:	根据丢失率随机生成一个数字，判断是否丢失，丢失则返回 TRUE，否则返回 FALSE
// Parameter：	float lossRatio[0,1]
//****************************************
BOOL lossInLossRatio(float lossRatio) {
	int lossBound = (int)(lossRatio * 100);
	int r = rand() % 101;
	if (r <= lossBound) {
		return TRUE;
	}
	return FALSE;
}