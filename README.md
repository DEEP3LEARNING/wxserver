# 企业微信回复消息服务器

## 功能
运行时可以通过 微信/企业微信 向应用发送消息，程序收到消息会提示。
如果收到config.czh中定义的自动回复，会回复指定内容。

## 依赖
```
openssl 
curl
czh-cpp
```

## 编译方法

```
gcc main.cpp -lstdc++ -lcrypto -lcurl -lpthread

```

## 参考
[企业微信API](https://work.weixin.qq.com/api/doc)  
[C++ 写的UrlEncode和UrlDecode实例](https://www.jb51.net/article/201855.htm)  
[C++用libcurl通过HTTP以表单的方式Post数据到服务器](https://blog.csdn.net/shaoyiju/article/details/78238336)  
[用C++写一个简单的服务器(Linux)](https://blog.csdn.net/qq_29695701/article/details/83830108)  
[C/C++中的__FUNCTION__，__FILE__和__LINE__](https://www.cnblogs.com/yooyoo/p/4717917.html)  
[czh-cpp](https://gitee.com/cmvy2020/czh-cpp)
[基于C++11的线程池(threadpool),简洁且可以带任意多的参数](https://www.cnblogs.com/lzpong/p/6397997.html)  
