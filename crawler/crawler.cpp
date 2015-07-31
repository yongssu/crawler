#define _CRT_SECURE_NO_WARNINGS
#include "crawler.h"

Crawler::Crawler()
	:m_url("")
{
	// 创建文件夹，保存图片和网页文本文件  
	CreateDirectoryA("./img", 0);
	CreateDirectoryA("./html", 0);
}
Crawler::Crawler(const string& url)
	:m_url(url)
{
	// 创建文件夹，保存图片和网页文本文件  
	CreateDirectoryA("./img", 0);
	CreateDirectoryA("./html", 0);
}

Crawler::~Crawler()
{
}

bool Crawler::parseURL(const string& url, string& host, string& resource)
{
	if (strlen(url.c_str()) > 2000)
	{
		std::cout << "url to long!" << endl;
		return false;
	}
	const char* pos = strstr(url.c_str(), "http://");
	if (pos == NULL)
		pos = url.c_str();
	else
		pos += strlen("http://");
	if (strstr(pos, "/") == 0)
	{
		std::cout << "parseURL error" << endl;
		return false;
	}

	char pHost[100];
	char pResource[2000];
	sscanf(pos, "%[^/]%s", pHost, pResource);
	host = pHost;
	resource = pResource;
	return true;
}

bool Crawler::getHttpResponse(const string&url, string *&response)
{
	string host, resource;
	if (!parseURL(url, host, resource))
	{
		cout << "Can not parse the url!" << endl;
		return false;
	}
	string ret;
	/// 建立socket，建立连接，发送数据，并接收数据
	try
	{
		SocketClient s(host, 80);
		string request = "GET " + resource + " HTTP/1.1\r\nHost:" + host + "\r\nConnection:Close\r\n\r\n";
		s.sendLine(request);
		while (1)
		{
			string block = s.receiveBytes();
			if (block.empty())
				break;
			ret += block;
		}
	}
	catch (const char* s)
	{
		cerr << s << endl;
	}
	catch (std::string s)
	{
		cerr << s << endl;
	}
	catch (...)
	{
		cerr << "unhandled exception\n";
	}
	string::size_type pos = ret.find_last_of("</html>");
	ret = ret.substr(0, pos + 1);
	string *str = new string(ret);
	response = str;
	return true;
}


void Crawler::htmlParse(string& htmlResponse, vector<string>& imgurls)
{
	// 找到所有连接，加入到queue中
	const char *p = htmlResponse.c_str();
	char *tag = "href=\"";
	const char* pos = strstr(p, tag);
	ofstream ofile("url.txt", ios::app);
	while (pos)
	{
		pos += strlen(tag);
		const char* nextQ = strstr(pos, "\"");
		if (nextQ)
		{
			char *url = new char[nextQ - pos + 1];
			sscanf(pos, "%[^\"]", url);
			string surl = url;  // 转换为string类型，可以自动释放内存
			//if (surl.find_last_of(".css") != string::npos)
				//continue;
			if (m_visitedUrl.find(surl) == m_visitedUrl.end())
			{
				m_visitedUrl.insert(surl);
				ofile << surl << endl;
				m_hrefUrl.push(surl);
			}
			pos = strstr(pos, tag);
			delete[] url;  // 释放申请的内存
		}
	}
	ofile << endl << endl;
	ofile.close();

	tag = "<img ";
	const char* attr1 = "src=\"";
	const char* attr2 = "lazy-src=\"";
	pos = strstr(p, tag);
	while (pos)
	{
		pos += strlen(tag);
		const char* pos2 = strstr(pos, attr2);
		if (!pos2 || pos2 > strstr(pos, ">"))
		{
			const char* pos1 = strstr(pos, attr1);
			if (!pos1 || pos1 > strstr(pos, ">"))
			{
				pos = strstr(pos, ">");
				continue;
			}
			else
			{
				pos = pos1 + strlen(attr1);
			}
		}
		else
		{
			pos = pos2 + strlen(attr2);
		}

		const char* nextQ = strstr(pos, "\"");
		if (nextQ)
		{
			char* url = new char[nextQ - pos + 1];
			sscanf(pos, "%[^\"]", url);
			//cout << url << endl;
			string imgUrl = url;
			if (m_visitedImg.find(imgUrl) == m_visitedImg.end())
			{
				m_visitedImg.insert(imgUrl);
				imgurls.push_back(imgUrl);
			}
			pos = strstr(pos, tag);
			delete[] url;
		}
	}
	cout << "end of parse this theml" << endl;
}


string Crawler::toFileName(const string& url)
{
	string fileName;
	fileName.resize(url.size());
	int k = 0;
	for (int i = 0; i < (int)url.size(); ++i)
	{
		char ch = url[i];
		if (ch != '\\'&&ch != '/'&&ch != ':'&&ch != '*'&&ch != '?'&&ch != '"'&&ch != '<'&&ch != '>'&&ch != '|')
			fileName[k++] = ch;
	}
	return fileName.substr(0, k);
}

void Crawler::downLoadImg(vector<string>& imgurls, const string& url)
{
	// 生成保存该url下图片的文件夹
	string foldname = toFileName(url);
	foldname = "./img/" + foldname;
	if (!CreateDirectoryA(foldname.c_str(), NULL))
	{
		cout << "Can not create directory:" << foldname << endl;
	}

	for (int i = 0; i < (int)imgurls.size(); ++i)
	{
		/// 判断是否为图片，bmp，jpg，jpeg，gif
		string str = imgurls[i];
		int pos = str.find_last_of(".");
		if (pos == string::npos)
			continue;
		else
		{
			string ext = str.substr(pos + 1, str.size() - pos - 1);
			if (ext != "bmp" && ext != "jpg" && ext != "jpeg" && ext != "gif" && ext != "png")
				continue;
		}
		//下载其中的内容
		string *image;
		if (getHttpResponse(imgurls[i], image))
		{
			if (strlen((*image).c_str()) == 0)
				continue;

			const char *p = (*image).c_str();
			const char *pos = strstr(p, "\r\n\r\n") + strlen("\r\n\r\n");
			int temp = pos - p;
			int index = imgurls[i].find_last_of("/");
			if (index != string::npos)
			{
				string imgname = imgurls[i].substr(index, imgurls[i].size());
				ofstream ofile(foldname + imgname, ios::binary);
				if (!ofile.is_open())
					continue;
				ofile.write(pos, (*image).size() - (pos - p));
				ofile.close();
			}
			delete image;
			image = NULL;
		}
	}
}

void Crawler::bfs(const string& url)
{
	string* response;
	/// 获取网页的内容，放入response中
	if (!getHttpResponse(url, response))
	{
		cout << "The url is wrong! ignore." << endl;
		return;
	}
	string httpResponse = (*response);
	delete response;
	string filename = toFileName(url);
	ofstream ofile("./html/" + filename + ".html");
	if (ofile.is_open())
	{
		/// 保存该网页额文本内容
		ofile << httpResponse << endl;
		ofile.close();
	}
	vector<string> imgurls;
	/// 解析该网页的所有图片链接，放入imgurls里面
	htmlParse(httpResponse, imgurls);

	/// 下载所有图片资源
	downLoadImg(imgurls, url);
}

void Crawler::start()
{
	/// 提取网页中的超链接放入hrefUrl中，提取图片链接，下载图片
	bfs(m_url);

	/// 访问过的网址保存起来
	m_visitedUrl.insert(m_url);

	while (m_hrefUrl.size() != 0)
	{
		string url = m_hrefUrl.front();   /// 从队列的最开始取出一个网址
		cout << url << endl;
		bfs(url);
		m_hrefUrl.pop();  /// 遍历完之后，删除这个网址
	}
}