#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include "crawler.h"
using namespace std;

int main()
{
	/// 下载一个页面图片
	Crawler cr1;
	cr1.bfs("http://tieba.baidu.com/p/3927954592");

	/// 广度遍历下载网站图片
	//Crawler cr2("http://tieba.baidu.com/p/3927954592");
	//cr2.start();
	
	system("pause");
	return 0;
}