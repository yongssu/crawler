#pragma once
/////////////////////////////////////////////////////////////////////
/// author：suys
/// date:2015/7/30
/////////////////////////////////////////////////////////////////////
#include <WinSock2.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")

// @brief 阻塞和非阻塞
enum TypeSocket{BLOCKINGSOCKET, NONBLOCKINGSOCKET};

///////////////////////////////////////////////////////////
/// @brief socket基类
/// @note 负责发送和接收消息，server和client公有功能
///////////////////////////////////////////////////////////
class Socket
{
protected:
	friend class SocketServer;
	friend class ClientSelect;
public:
	int *m_refCounter;
	SOCKET m_socket;     /// @brief socket创建存储
	Socket(SOCKET s);    /// @brief 构造函数，创建socket
	Socket();            /// @brief 构造函数
private:
	static int socketCounter;  /// @brief socket计数
	static void start();       /// @brief winsocket初始化 
	static void end();         /// @brief winsocket初始化结束
	
public:
	std::string receiveLine();      /// @brief 接收一行消息
	std::string receiveBytes();     /// @brief 接收块消息
	void sendLine(std::string str); /// @brief 发送行消息
	void sendBytes(const std::string& str);  ///@brief 发送块消息
	void close();  /// @brief 关闭socket
public:
	Socket(const Socket& sock);        /// @brief 拷贝构造函数
	Socket& operator=(Socket& sock);   /// @brief 等号重载
	virtual ~Socket();                 /// @brief 虚析构函数
};


///////////////////////////////////////////////////////////////////
/// @brief 服务端类
///////////////////////////////////////////////////////////////////
class SocketServer :public Socket
{
public:
	/// @brief 服务端构造函数
	/// @note  构造服务端程序
	/// @param[in] port 绑定端口
	/// @param[in] connections 连接数
	/// @param[in] type 阻塞和非阻塞，默认为阻塞
	SocketServer(int port, int connections, TypeSocket type = BLOCKINGSOCKET);
	/// @brief 是否接收客户端请求
	Socket* accept_s();
};

//////////////////////////////////////////////////////////////////
/// @brief 客户端类
//////////////////////////////////////////////////////////////////
class SocketClient :public Socket
{
public:
	/// @brief 客户端构造程序
	/// @param[in] host 域名
	/// @param[in] port 服务端口
	SocketClient(const std::string &host, int port);
};


//////////////////////////////////////////////////////////////////
/// @brief SocketSelect类
//////////////////////////////////////////////////////////////////
class SocketSelect
{
private:
	fd_set m_fds;
public:
	SocketSelect(Socket const* const s1, Socket const* const s2 = NULL, TypeSocket type = BLOCKINGSOCKET);
	bool readable(Socket const* const s);
};



