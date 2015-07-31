#pragma once
/////////////////////////////////////////////////////////////////////
/// author：suys
/// date:2015/7/30
/////////////////////////////////////////////////////////////////////
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#include <hash_set>
#include "socket.h"

using namespace std;

//////////////////////////////////////////////////////////
/// @brief 爬虫类
//////////////////////////////////////////////////////////
class Crawler
{
private:
	string m_url;                     /// @brief 保存初始URL
	queue<string> m_hrefUrl;          /// @brief 保存所有未访问的href
	hash_set<string> m_visitedUrl;    /// @brief 保存所有已经访问过的url
	hash_set<string> m_visitedImg;    /// @brief 保存已经访问过的所有图片
public:
	/// @brief 解析URL，获得主机名，资源名
	/// @param[in]  url 要解析的URL
	/// @param[out] host 解析出来的host
	/// @param[out]  resource 解析出来的资源名
	bool parseURL(const string& url, string& host, string& resource);

	/// @brief 使用Get请求，获得请求页面
	/// @param[in]  url 请求的url
	/// @param[out] response 服务端返回的请求页面内容
	bool getHttpResponse(const string&url, string *&response);

	/// @brief 解析取得当前页面的所有的href以及图片URL
	/// @param[in]  htmlResponse html页面的内容
	/// @param[out] imgurls 存储所有图片的url地址
	void htmlParse(string& htmlResponse, vector<string>& imgurls);

	/// @brief 将url转化为文件名
	/// @param[in] url 需要转化的url
	string toFileName(const string& url);

	/// @brief 将图片下载到img文件夹下
	/// @param[in] imgurls 所有图片的url地址
	/// @param[in] url 图片所对应的url地址
	void downLoadImg(vector<string>& imgurls, const string& url);
	
	/// @brief 下载一个url下的所有图片
	/// @param[in] url 需要下载图片的url
	void bfs(const string& url);

	/// @brief 广度遍历下载所有url的图片
	void start();
public:
	/// @brief 构造函数
	Crawler();
	Crawler(const string &url);
	~Crawler();
};

