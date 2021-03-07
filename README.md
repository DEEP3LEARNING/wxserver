# ��ҵ΢�Żظ���Ϣ������

## ����
2021.3.7

����ʱ����ͨ�� ΢��/��ҵ΢�� ��Ӧ�÷�����Ϣ�������յ���Ϣ����ʾ��
����յ�config.yaml�ж�����Զ��ظ�����ظ�ָ�����ݡ�
����յ�config.yaml�ж���Ĺ���Ա���͵� pic/file ���ᷢ��ָ��ͼƬ���ļ���
��(����ҵ΢��Ӧ�÷���):
```
image /home/example_user/example.jpg
file /home/example_user/example.cpp
```
��Ӧ��������config.yaml��command�У�Ŀǰֻ������file(�ļ�)��image(ͼƬ)

## ����
ʹ��ʱ��Ҫconfig.yaml(wxsend.cpp��server.cpp��ʹ��)����wxsend��server����ͬһĿ¼  

�⣺
	tinyxml2: tinyxml2-2.1.0
	openssl: openssl-1.0.1h
	yaml-cpp : yaml-cpp 0.6.3-2

## ��֪����
�����wxsendд�ɳ�Ա������libcurl��segmentation fault���ݲ�֪��ν��������wxsend��������  
��ˣ�Ҳ�������������е�������
```	
./wxsend [type] [item] [UserID]
```
����
```	
./wxsend text example @all
./wxsend image example.jpg @all
./wxsend file example.cpp @all
```
## ���뷽��

### wxsend 
```
clang++ wxsend.cpp /usr/lib/libyaml-cpp.so.0.6 -lcurl -o wxsend
```
### Others (vs2019 or clang++)

#### 1. vs2019
��������ļ�����Ŀ��������wxsend.cpp  
��Ŀ����-������-����-���������(Ҫ����·��)  
```	
libcrypto.a
libtinyxml2.so.7
libyaml-cpp.so.0.6
libcurl.so
```
#### 2. clang++��Arch Linux ʾ����
```
clang++ server.cpp WXBizMsgCrypt.cpp /usr/lib/libtinyxml2.so.7 /usr/local/openssl/lib/libcrypto.a /usr/lib/libyaml-cpp.so.0.6 -o server

```