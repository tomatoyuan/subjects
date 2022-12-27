//#include "stdafx.h"
#include <stdio.h>
#include <Windows.h>
#include <process.h>
#include <string.h>
#include <tchar.h> 

#pragma comment(lib, "Ws2_32.lib") // 链接Ws2_32.lib这个库

#define MAXSIZE 65507 // 发送数据报文的最大长度 
#define HTTP_PORT 80 // http服务器端口

// Http 重要头部文件
struct HttpHeader {
	char method[4]; // POST或者GET，注意有些为CONNECT，本实验暂不考虑
	char url[1024]; // 请求的url
	char host[1024]; // 目标主机
	char cookie[1024 * 10]; // cookie
	HttpHeader() {
		ZeroMemory(this, sizeof(HttpHeader));
	}
};

/*** 选做 1：设计并实现一个支持 Cache 功能的 HTTP 代理服务器 ***********/

#define DATE_LENGTH 50 //时间字节数
#define CACHE_NUM 50  //定义最大缓存数量

// 代理服务器 Cache 存储头部文件中的有用信息
struct CacheHttpHeader {
	char method[4];
	char url[1024];
	char host[1024];
	//char cookie[1024 * 10]; // cookie 没必要记录，太占内存
	CacheHttpHeader() {
		ZeroMemory(this, sizeof(CacheHttpHeader));
	}
};

struct Cache {
	CacheHttpHeader httpHeader;
	char buffer[MAXSIZE]; // 存储报文返回的内容
	char date[DATE_LENGTH]; // 缓存内容的最后修改时间
	Cache() {
		ZeroMemory(this->buffer, MAXSIZE);
		ZeroMemory(this->buffer, DATE_LENGTH);
	}
};

Cache cache[CACHE_NUM]; // 缓存地址
int cache_index = 0; // 记录当前已经存储的cache的数量

/*** 选做 2：扩展 HTTP 代理服务器 *************************************/
bool function2_on = false; // 是否启用选作2的屏蔽功能

#define MAX_SIZE_INVALID_WEB 10 // 最多可以禁止访问的网站数量
const char* invalid_website[MAX_SIZE_INVALID_WEB] = {"http://www.hit.edu.cn/"};
const int invalid_website_num = 1; // 禁用的网站数量

#define MAX_SIZE_INVALID_USER 10 // 最多可以禁用的用户数量
const char* invalid_user[10] = {"127.0.0.2"}; //被屏蔽的用户IP,以127.0.0.1为例
const int invalid_user_num = 1; // 禁用的用户数量

#define MAX_SIZE_FISHING_URL 10 // 最多可以记录的钓鱼网站原网站的数量
const char* fishing_src_url[MAX_SIZE_FISHING_URL] = {"http://xg.hit.edu.cn/"}; //钓鱼网站原网址
const char* fishing_dest = "http://jwes.hit.edu.cn/";//钓鱼网站目标网址
const char* fishing_dest_host = "jwts.hit.edu.cn";//钓鱼目的地址主机名
const int finshing_src_url_num = 1; // 钓鱼网站原网站的数量

// 建立HTTP通信部分函数
BOOL InitSocket();
void ParseHttpHead(char* buffer, HttpHeader* httpHeader);
BOOL ConnectToServer(SOCKET* serverSocket, char* host);
unsigned int __stdcall ProxyThread(LPVOID lpParameter);
void GoToError(LPVOID lpParameter);
// 添加 cache 缓存部分函数
BOOL IsHttpEqual(HttpHeader* httpHeader, CacheHttpHeader* httpHeader_in_cache);
int FindInCache(HttpHeader* httpHeader);
void AddDateToHttpHeader(char* buffer, char* date);
// 扩展 HTTP 代理服务器部分函数
BOOL ForbidInvalidWebsite(HttpHeader* httpHeader);
BOOL ForbidInvalidUser(char* userIP);
void GoFishing(HttpHeader* httpHeader);

// 代理相关参数
SOCKET ProxyServer;
sockaddr_in ProxyServerAddr;
const int ProxyPort = 10240;

// 由于新的连接都是用新线程进行处理，对线程的频繁的创建和销毁特别浪费资源
// 可以使用线程池技术提高服务器效率
//const int ProxyThreadMaxNum = 20;
//HANDLE ProxyThreadHandle[ProxyThreadMaxNum] = {0};
//DWORD ProxyThreadDW[ProxyThreadMaxNum] = {0};

struct ProxyParam {
	SOCKET clientSocket;
	SOCKET serverSocket;
};

int _tmain(int argc, _TCHAR* argv[]) {
	printf("代理服务器正在启动\n");
	printf("初始化...\n");
	/* 初始化代理服务器 */
	if (!InitSocket()) {
		printf("socket 初始化失败\n");
		return -1;
	}
	printf("代理服务器正在运行，监听窗口 %d\n", ProxyPort);
	SOCKET acceptSocket = INVALID_SOCKET; // 初始化服务器为没有接收到请求的状态
	ProxyParam* lpProxyParam; // 定义一个结构体指针，指向客户创建的socket和服务器创建的socket
	HANDLE hThread; // 创建一个空的函数句柄(void *)
	DWORD dwThreadID; // 创建一个常量用来记录线程的ID
	SOCKADDR_IN acceptAddr; //自定义变量，用来获得用户的IP
	int addr_len = sizeof(SOCKADDR);
	// 代理服务器不断监听
	while (true) {
		acceptSocket = accept(ProxyServer, (SOCKADDR*)&acceptAddr, &(addr_len)); // 代理服务器监听是否有客户端的代理请求
		lpProxyParam = new ProxyParam; // 给结构体指针分配空间来存储客户和代理服务器创建的socket信息
		if (lpProxyParam == NULL) { // 空间分配失败，丢弃本次请求
			continue;
		}
		// 如果开启 选作二 的功能，且客户是禁用的用户，则拒绝对本次请求的响应
		if (function2_on && ForbidInvalidUser(inet_ntoa(acceptAddr.sin_addr))) {
			closesocket(acceptSocket);
		}
		//printf("here: %s\n", inet_ntoa(acceptAddr.sin_addr));
		// 空间分配成功
		lpProxyParam->clientSocket = acceptSocket; // 记录客户创建新的socket
		hThread = (HANDLE)_beginthreadex(NULL, 0, &ProxyThread, (LPVOID)lpProxyParam, 0, 0); // 代理服务器为该客户的代理请求创建一个线程来执行请求
		CloseHandle(hThread); // 请求执行完毕，收回该线程
		Sleep(200); // 间隔20ms再次监听
	}
	closesocket(ProxyServer); // 停止代理服务，结束监听模式后，关闭代理服务器的socket
	WSACleanup(); // 解除与socket库(Ws2_32.dll)的绑定并且释放socket库所占用的系统资源	
	return 0;
}

//****************************************
// Method:		InitSocket
// FullName:	InitSocket
// Access:		public
// Returns:		BOOL
// Qualifier:	初始化套接字 
//****************************************
BOOL InitSocket() {
	// 加载套接字库（必须）
	WORD wVersionRequested; //win sockets规范版本，为WORD类型
	WSADATA wsaData; // 用于存储调用WSAStartup()函数后返回的win socket数据
	// 套接字加载时错误提示
	int err;
	// 版本 2.2
	wVersionRequested = MAKEWORD(2, 2); // 高位字节中存储副版本号，低位字节存储主版本号
	// 加载 dll 文件 Socket 库
	err = WSAStartup(wVersionRequested, &wsaData); // 初始化win sock，当返回值为0时，初始化成功
	if (err != 0) { // 初始化win sock失败，打印错误代码以提示
		// 找不到 winsock.dll
		printf("加载 winsock 失败，错误代码为：%d\n", WSAGetLastError());
		return false;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		printf("不能找到正确的 winsock 版本\n");
		WSACleanup();
		return FALSE;
	}
	ProxyServer = socket(AF_INET, SOCK_STREAM, 0); // 创建代理服务器套接字
	if (INVALID_SOCKET == ProxyServer) { // 如果代理服务器套接字还是初始状态，代表创建套接字失败
		printf("创建套接字失败，错误代码为：%d\n", WSAGetLastError());
		return FALSE;
	}
	ProxyServerAddr.sin_family = AF_INET; // 表示套接字使用TCP/IP协议族
	// htonl()函数的意思是将主机顺序的字节转换成网络顺序的字节（避免大小端编码方式不一致导致的错误一致）
	ProxyServerAddr.sin_port = htons(ProxyPort); // 设置通信端口号，ProxyPort是我们已经定义好的10240
	ProxyServerAddr.sin_addr.S_un.S_addr = INADDR_ANY; // 设置服务器套接字的IP地址为特殊值INADDR_ANY，这表示服务器愿意接收来自任何网络设备接口的客户机连接
	if (bind(ProxyServer, (SOCKADDR*)&ProxyServerAddr, sizeof(SOCKADDR)) == SOCKET_ERROR) { // 将服务器创建的套接字标识和服务端口地址绑定
		printf("绑定套接字失败，错误代码为：%d\n", WSAGetLastError());
		return FALSE;
	}
	if (listen(ProxyServer, SOMAXCONN) == SOCKET_ERROR) { // 等待用户连接请求
		printf("监听端口%d失败", ProxyPort);
		return FALSE;
	}
	return TRUE;
}

//****************************************
// Method:		ProxyThread
// FullName:	ProxyThread
// Access:		public
// Returns:		unsigned int __stdcall
// Qualifier:	线程执行函数
// Parameter:	LPVOID lpParameter 
//****************************************
unsigned int __stdcall ProxyThread(LPVOID lpParameter) {
	char Buffer[MAXSIZE];
	char* CacheBuffer;
	ZeroMemory(Buffer, MAXSIZE);
	SOCKADDR_IN clientAddr;
	int length = sizeof(SOCKADDR_IN);
	int recvSize;
	int ret;
	recvSize = recv(((ProxyParam*)lpParameter)->clientSocket, Buffer, MAXSIZE, 0); // 代理服务器的线程接收客户端的请求报文，将报文存储在 Buffer 中
	HttpHeader* httpHeader = new HttpHeader(); // 创建一个 HTTP 头部
	if (recvSize <= 0) {
		GoToError(lpParameter);
		return 0;
		//goto error;
	}
	CacheBuffer = new char[recvSize + 1];
	ZeroMemory(CacheBuffer, recvSize + 1);
	memcpy(CacheBuffer, Buffer, recvSize); // 代理服务器将请求报文存储在 CacheBuffer 中
	ParseHttpHead(CacheBuffer, httpHeader); // 代理服务器解析客户端的 TCP 请求报文内容，加入 HTTP 头部
	// 网站过滤：允许/不允许访问某些网站
	if (function2_on && ForbidInvalidWebsite(httpHeader)) { // 如果在被禁用名单中，则禁用该网站
		GoToError(lpParameter);
		return 0;
		//goto error;
	}
	// 网站引导：将用户对某个网站的访问引导至一个模拟网站（钓鱼）
	if (function2_on) {
		GoFishing(httpHeader);
	}
	delete CacheBuffer; // 删除临时报文的缓存
	if (!ConnectToServer(&((ProxyParam*)lpParameter)->serverSocket, httpHeader->host)) { // 代理服务器代替客户端和目标服务器建立连接
		GoToError(lpParameter);
		return 0;
		//goto error;
	}
	printf("代理连接主机 %s 成功\n", httpHeader->host);

	int index = FindInCache(httpHeader); // 寻找 httpHeader 在 cache 中的位置
	if (index > -1) { // 如果缓存命中
		char temp_buf[MAXSIZE];
		ZeroMemory(temp_buf, MAXSIZE);
		memcpy(temp_buf, Buffer, recvSize); // 拷贝客户的请求报文
		AddDateToHttpHeader(temp_buf, cache[index].date); //插入关于修改时间的头行
		printf("================= 缓存命中！ ~-~ ===============\n");
		printf("****************Proxy请求报文*******************\n");
		ret = send(((ProxyParam*)lpParameter)->serverSocket, temp_buf, strlen(temp_buf) + 1, 0);
		recvSize = recv(((ProxyParam *)lpParameter)->serverSocket, temp_buf, MAXSIZE, 0);
		printf("**************Server返回响应报文*****************\n");
		if (recvSize <= 0) {
			GoToError(lpParameter);
			return 0;
			//goto error;
		}
		const char* not_modified = "304"; // 304 代表没有修改
		if (!strncmp(&temp_buf[9], not_modified, strlen(not_modified))) { // 如果缓存命中，直接将 cache 中的内容返回
			ret = send(((ProxyParam *)lpParameter)->clientSocket, cache[index].buffer, strlen(cache[index].buffer) + 1, 0);
			printf("=============== 访问内容未做修改 ===============\n");
			printf("将cache中的缓存返回客户端\n");
			printf("================================================\n");
			GoToError(lpParameter);
			return 0;
			//goto error;
		}
	}
	// 将客户端发送的 HTTP 数据报文直接转发给目标服务器
	ret = send(((ProxyParam*)lpParameter)->serverSocket, Buffer, strlen(Buffer) + 1, 0);
	// 等待目标服务器返回数据
	recvSize = recv(((ProxyParam*)lpParameter)->serverSocket, Buffer, MAXSIZE, 0);
	if (recvSize <= 0) {
		GoToError(lpParameter);
		return 0;
		//goto error;
	}

	/***** 将未缓存的报文加入缓存，将以缓存但发生修改的报文更新 *****/
	char* cache_buffer = new char[MAXSIZE];
	ZeroMemory(cache_buffer, MAXSIZE);
	memcpy(cache_buffer, Buffer, MAXSIZE);
	const char* delim = "\r\n";
	char date[DATE_LENGTH];
	ZeroMemory(date, DATE_LENGTH);
	char* next_str = NULL;
	char* p = strtok_s(cache_buffer, delim, &next_str); // 分割原报文的内容
	bool modify_flag = false;// 是否需要更新缓存 Last-Modified: 
	int modified_str_len = strlen("Last-Modified:");
	while (p) { // 遍历报文头部，直到找到记录时间的字段 Date: 
		if (p[0] == 'L') { // 找到Date: 那一行
			if (strlen(p) > 15) {
				char temp_header[15];
				ZeroMemory(temp_header, sizeof(temp_header));
				memcpy(temp_header, p, modified_str_len);
				if (!strcmp(temp_header, "Last-Modified:")) { // 找到了if-modified-since头行
					memcpy(date, &p[modified_str_len], strlen(p) - modified_str_len); // 记录最新修改时间
					modify_flag = true; // 标记找到了
					break;
				}
			}
		}
		p = strtok_s(NULL, delim, &next_str); // 指针移动到报文中的下一个内容
	}


	if (modify_flag) { // 获取最新修改时间后，添加或者更新 cache 中的缓存
		if (index > -1) { // 已缓存，则更新时间
			printf("+++++++++++++++++ 缓存命中！ ++++++++++++++++++++++++++++\n");
			printf("+++++++++++++++++ 页面被修改过了！ ++++++++++++++++++++++\n");
			//printf("原缓存报文URL为：\n%s\n", cache[index].buffer);
			//printf("更新后报文内容为：\n%s\n", Buffer);
			printf("原缓存报文时间为：%s\n", cache[index].date);
			printf("更新后报文时间为：%s\n", date);
			memcpy(&cache[index].buffer, Buffer, strlen(Buffer)); // 更新报文内容
			memcpy(&cache[index].date, date, strlen(date)); // 更新修改时间
			printf("++++++++++++++++++++ 更新成功！ ++++++++++++++++++++++++++++\n");
		}
		else { // 缓存不命中，即第一次访问，将报文加入缓存
			printf("—————————— 缓存不命中！ —————————————\n");
			int new_index = cache_index % CACHE_NUM; // 防止溢出，cache存储空间满了之后，丢弃最早存储的内容，存入新的报文
			memcpy(&(cache[new_index].httpHeader.host), httpHeader->host, strlen(httpHeader->host)); // 存储客户端用户信息
			memcpy(&cache[new_index].httpHeader.method, httpHeader->method, strlen(httpHeader->method)); // 存储请求命令
			memcpy(&cache[new_index].httpHeader.url, httpHeader->url, strlen(httpHeader->url)); // 存储请求的网址
			//memcpy(&cache[new_index].httpHeader.cookie, httpHeader->cookie, strlen(httpHeader->cookie)); // 存储 cookie 访问记录
			memcpy(&cache[new_index].buffer, Buffer, strlen(Buffer)); // 存储报文内容
			memcpy(&cache[new_index].date, date, strlen(date)); // 存储修改时间
			cache_index++; // 缓存数量+1
			//printf("新添加的报文内容为：\n%s\n", Buffer);
			printf("添加时间为：%s\n", date);
			printf("—————————— 添加cache成功！ ————————————\n");
		}
	}

	// 将目标服务器返回的数据直接转发给客户端
	ret = send(((ProxyParam*)lpParameter)->clientSocket, Buffer, sizeof(Buffer), 0);

	// 错误处理 
error:
	printf("关闭套接字\n");
	Sleep(200);
	closesocket(((ProxyParam*)lpParameter)->clientSocket);
	closesocket(((ProxyParam*)lpParameter)->serverSocket);
	delete lpParameter;
	_endthreadex(0);
	return 0;
}

//****************************************
// Method:		GoToError
// FullName:	GoToError
// Access:		public
// Returns:		void
// Qualifier:	错误处理: 网络请求过程出现问题，关闭套接字
// Parameter:	LPVOID lpParameter
//****************************************
void GoToError(LPVOID lpParameter) {
	printf("关闭套接字\n");
	Sleep(200);
	closesocket(((ProxyParam*)lpParameter)->clientSocket);
	closesocket(((ProxyParam*)lpParameter)->serverSocket);
	delete lpParameter;
	_endthreadex(0);
}

//****************************************
// Method:		ParseHttpHead
// FullName:	ParseHttpHead
// Access:		public
// Returns:		void
// Qualifier:	解析 TCP 报文中的 HTTP 头部
// Parameter:	char *buffer
// Parameter:	HttpHeader *httpHeader 
//****************************************
void ParseHttpHead(char* buffer, HttpHeader* httpHeader) { // 传入一个缓存数组，HTTP头部结构体指针
	char* p; // 指向分割后的第一个字符串
	char* ptr; // 存储分割后剩下部分的内容
	const char* delim = "\r\n"; // 利用CLF格式下的换行符作为分割符
	p = strtok_s(buffer, delim, &ptr); // 提取第一行，将buffer中的所有 delim 换成 '/0'。
	printf("%s\n", p); // 打印分割出的第一个字符串
	if (p[0] == 'G') { // GET 方式 
		memcpy(httpHeader->method, "GET", 3); // 在HTTP头部加入 GET 操作请求
		memcpy(httpHeader->url, &p[4], strlen(p) - 13); // 在HTTP头部加入目标服务器 url 网址
	}
	else if (p[0] == 'P') { // POST 方法 
		memcpy(httpHeader->method, "POST", 4); // 在HTTP头部加入 POST 操作请求
		memcpy(httpHeader->url, &p[5], strlen(p) - 14);
	}
	printf("%s\n", httpHeader->url); // 打印请求访问的 url
	p = strtok_s(NULL, delim, &ptr); // 将指针 p 指向下一个分割出的字符串的首地址
	while (p) { // 遍历报文buffer中分割出的剩余的字符串，将命令解析后放入 HTTP 头部
		switch (p[0]) {
		case 'H': // Host
			memcpy(httpHeader->host, &p[6], strlen(p) - 6); // 取出host相关内容放入头部
			break;
		case 'C': // Cookie
			if (strlen(p) > 8) { 
				char header[8];
				ZeroMemory(header, sizeof(header));
				memcpy(header, p, 6);
				if (!strcmp(header, "Cookie")) { // 如果头部字段名匹配成功，则将 cookie 的内容加载到头部
					memcpy(httpHeader->cookie, &p[8], strlen(p) - 8);
				}
			}
			break;
		default:
			break;
		}
		p = strtok_s(NULL, delim, &ptr); // 指针再次移动到下一个分割出的字符串的首地址
	}
}


//****************************************
// Method:		ConnectToServer
// FullName:	ConnectToServer
// Access:		public
// Returns:		BOOL
// Qualifier:	根据主机创建目标服务器套接字，并连接
// Parameter:	SOCKET * serverSocket
// Parameter:	char * host 
//****************************************
BOOL ConnectToServer(SOCKET* serverSocket, char* host) { // 传入目标服务器套接字，和主机ip
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET; // 目的服务器地址类型为 IPv4
	serverAddr.sin_port = htons(HTTP_PORT); // 要访问的端口是 80
	//  gethostbyname：用域名或主机名获取IP地址，这个函数仅仅支持IPv4
	HOSTENT* hostent = gethostbyname(host); // 返回hostent结构体类型指针，包含本地主机的各种信息
	if (!hostent) { // 获取本地主机信息失败
		return FALSE;
	}
	in_addr Inaddr = *((in_addr*)*hostent->h_addr_list); // 是主机的ip地址，注意，这个是以网络字节序存储的
	serverAddr.sin_addr.s_addr = inet_addr(inet_ntoa(Inaddr)); // inet_ntop()将Inaddr转换成主机序的字符串形式，inet_addr()将一个ip地址字符串转换成一个整数值
	*serverSocket = socket(AF_INET, SOCK_STREAM, 0); // 创建一个流格式套接字（默认为TCP协议），给目标服务器套接字
	if (*serverSocket == INVALID_SOCKET) { // 套接字创建失败
		return FALSE;
	}
	if (connect(*serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) { // 建立主机与目标服务器之间的连接
		closesocket(*serverSocket); // 如果创建失败则销毁创建的目标服务器套接字
		return FALSE;
	}
	return TRUE;
}

//****************************************
// Method:		FindInCache
// FullName:	FindInCache
// Access:		public
// Returns:		int,返回查询内容在cache中的位置下标
// Qualifier:	在cache中查询 httpHeader 的缓存位置
// Parameter:	HttpHeader* httpHeader 待查找的报文头部
// Parameter:	CacheHttpHeader* httpHeader_in_cache 已缓存的报文头部
//****************************************
BOOL IsHttpEqual(HttpHeader* httpHeader, CacheHttpHeader* httpHeader_in_cache) {
	if (strcmp(httpHeader->method, httpHeader_in_cache->method)) return false;
	if (strcmp(httpHeader->host, httpHeader_in_cache->host)) return false;
	if (strcmp(httpHeader->url, httpHeader_in_cache->url)) return false;
	return true;
}

//****************************************
// Method:		FindInCache
// FullName:	FindInCache
// Access:		public
// Returns:		int,返回查询内容在cache中的位置下标
// Qualifier:	在cache中查询 httpHeader 的缓存位置
// Parameter:	HttpHeader *httpHeader
//****************************************
int FindInCache(HttpHeader *httpHeader) {
	for (int index = 0; index < cache_index; ++index) { // 在 cache 中寻找 httpHeader
		if (IsHttpEqual(httpHeader, &cache[index].httpHeader))return index;
	}
	return -1;
}


//****************************************
// Method:		AddDateToHttpHeader
// FullName:	AddDateToHttpHeader
// Access:		public
// Returns:		void
// Qualifier:	修改请求报文（添加 if-modified-since头行），
//				向原服务器确认缓存对象是否是最新版本。
// Parameter:	char* buffer
// Parameter:	char* date
//****************************************
void AddDateToHttpHeader(char* buffer, char* date) {
	const char* str_host = "Host";
	const char* insert_line = "if-modified-since:";
	char temp[MAXSIZE]; // 用于临时保存插入头行位置之后的内容
	ZeroMemory(temp, MAXSIZE);
	char* pos = strstr(buffer, str_host); // 找到Host的位置
	int pos_len = strlen(pos);
	for (int i = 0; i < pos_len; i++) { //将host与之后的部分写入temp
		temp[i] = pos[i];
	}
	*pos = '\0'; // 将Host之前的报文头部内容和要插入的头行隔开
	while (*insert_line != '\0') { // 添加 if-modified-since头行
		*(pos++) = *(insert_line++);
	}
	while (*date != '\0') { // 将最新的修改时间添加在 if-modified-since: 头行的后面
		*(pos++) = *(date++);
	}
	*pos++ = '\r';
	*pos++ = '\n';
	// 将pos之后的字段复制到buffer中
	int temp_len = strlen(temp);
	for (int i = 0; i < temp_len; ++i) {
		*pos++ = temp[i];
	}
}

//****************************************
// Method:		ForbidInvalidWebsite
// FullName:	ForbidInvalidWebsite
// Access:		public
// Returns:		BOOL
// Qualifier:	判断要访问的网站是不是被禁用的网站
// Parameter:	HttpHeader * httpHeader
//****************************************
BOOL ForbidInvalidWebsite(HttpHeader* httpHeader) {
	for (int i = 0; i < invalid_website_num; ++i) {
		// 如果是禁用的网站，返回true
		if (!strcmp(httpHeader->url, invalid_website[i])) {
			printf("/*-------------------------------------------*\\\n");
			printf("网站已禁用！\n");
			printf("网址： %s\n", invalid_website[i]);
			printf("\\*-------------------------------------------*/\n");
			return true;
		}
	}
	return false; // 如果都没匹配上，说明不是被禁用的网站
}

//****************************************
// Method:		ForbidInvalidUser
// FullName:	ForbidInvalidUser
// Access:		public
// Returns:		bool true说明已屏蔽该用户
// Qualifier:	判断发送请求的是不是被禁用的用户
// Parameter:	char* userIP
//****************************************
BOOL ForbidInvalidUser(char* userIP) {
	for (int i = 0; i < invalid_user_num; ++i) {
		// 如果开启了禁用功能，且是禁用的网站，返回true
		if (!strcmp(userIP, invalid_user[i])) {
			printf("/*-------------------------------------------*\\\n");
			printf("不支持该用户访问外部网站！\n");
			printf("用户IP： %s\n", userIP);
			printf("\\*-------------------------------------------*/\n");
			return true;
		}
	}
	return false; // 如果都没匹配上，说明不是被禁用的网站
}

//************************************
//Method : GoFishing
//FullName: GoFishing
//Access: public
//Return : void
//Qualifier:网站引导：将用户对某个网站的访问引导至一个模拟网站（钓鱼)
//Parameter: HttpHeader* httpHeader
//************************************
void GoFishing(HttpHeader* httpHeader)
{
	for (int i = 0; i < finshing_src_url_num; i++)
	{
		if (strstr(httpHeader->url, fishing_src_url[i]))
		{
			//访问的url在钓鱼源网址列表中
			printf("/*-------------------------------------------*\\\n");
			printf("已从源网址：%s 转到 目的网址 ：%s\n", httpHeader->url, fishing_dest);
			printf("\\*-------------------------------------------*/\n");
			memcpy(httpHeader->host, fishing_dest_host, strlen(fishing_dest_host) + 1);
			memcpy(httpHeader->url, fishing_dest, strlen(fishing_dest));
		}
	}
}