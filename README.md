# crawler
下载网页图片和html页面，会在当前目录下创建html和img文件夹，用于存放html页面和图片，会创建url.txt,用于存放所有的url地址。

#下载一个页面和下载当前页面的所有图片
	Crawler cr1;
	cr1.bfs("http://tieba.baidu.com/p/3927954592");
#广度遍历下载所有连接的网页以及图片
	Crawler cr2("http://tieba.baidu.com/p/3927954592");
	cr2.start();
