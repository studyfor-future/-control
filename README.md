# -control
远程服务控制，利用流数据，UDP协议实现实例

自身实验TCP协议 的远程连接 

参考代码：https://blog.csdn.net/Z_Stand/article/details/102597100

关于解决多客户端通信的方法：
多个客户端同时与服务端通信，可以考虑一下多线程/多进程，因为建立连接是需要server端处于accept阻塞阶段，而接收消息是需要server处于recv-send阶段，accept、recv和send都是阻塞的，因此在单线程单进程程序中server只能处于其中的一个阶段。当处于accept阶段，可以建立连接，但不能接收发送消息；当处于send-recv阶段，不能建立连接。而多线程/多进程，能够使得主线程/进程一直在等待新连接，然后再分出一些线程/进程处理与客户通信的recv-send。
