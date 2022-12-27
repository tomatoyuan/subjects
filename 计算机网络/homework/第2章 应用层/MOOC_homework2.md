[TOC]

# 作业2

## T1

> <font color=green>假设你在浏览某网页时点击了一个超链接，URL为“https://www.kicker.com.cn/index.html”，且该URL对应的IP地址在你的计算机上没有缓存；文件index.html引用了8个小图像。域名解析过程中，无等待的一次DNS解析请求与响应时间记为RTTd，HTTP请求传输Web对象过程的一次往返时间记为RTTh。请回答下列问题： </font>



> <font color=green>1）你的浏览器解析到URL对应的IP地址的最短时间是多少？最长时间是多少？</font>

**解：**

如果**local DNS server**上有缓存，只需一次查询即可，最短时间为 **1**RTTh

如果**local DNS server**没有缓存，向**local DNS server**查询一次，向**root DNS server**查询一次，向**TLD DNS server**查询一次，再分别查询**.com**和**.cn**对应的服务器，最长时间为 **5**RTTh



><font color=green>2）若浏览器没有配置并行TCP连接，则基于HTTP1.0获取URL链接Web页完整内容（包括引用的图像，下同）需要多长时间（不包括域名解析时间，下同）？</font>

**解：**

HTTP1.0默认非持久性连接，每个对象的发送都要建立一次TCP连接。

建立一个TCP连接，需要1个RTTh，发送一个对象1RTTh，加上发送Web对象请求共计发送9个对象

需要 2 * 9 = 18 RTTh的时间



> <font color=green>3）若浏览器配置5个并行TCP连接，则基于HTTP1.0获取URL链接Web页完整内容需要多长时间？</font>

**解：**

首先获取Web的html网页需要打开TCP并发送请求一次，2RTTh

8张图片，分两次发送，每次并行发送花费2RTTh

共计 6 RTTh



> <font color=green>‌4）若浏览器没有配置并行TCP连接，则基于非流水模式的HTTP1.1获取URL链接Web页完整内容需要多长时间？基于流水模式的HTTP1.1获取URL链接Web页完整内容需要多长时间？</font>

**解：**

(1) 没有配置并行TCP连接，则基于非流水模式的HTTP1.1
$$
t = 1 RTTh(建立TCP) + 1 RTTh(获取html) + 8 RTTh(获取8张图片) = 10 RTTh
$$
(2) 基于流水模式的HTTP1.1
$$
t = 1 RTTh(建立TCP) + 1 RTTh(获取html) + 1 RTTh(获取八张图片)
$$
