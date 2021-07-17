#pragma once

#include "wxerr.h"
#include "wxmsg.h"
#include "wxurl.h"
#include "wxxml.h"
#include "wxthpool.h"
#include "wxcmd.h"
#include "czh-cpp/czh.h"

#include <ctime>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <map>
#include <functional>
//��ҵ΢��api��https://work.weixin.qq.com/api/doc/90001/90143/90372
namespace ws
{
  class WXserver
  {
  private:
    int port;
    WXmsg wxmsg;
    WXcmd wxcmd;
    std::map<std::string, std::string> tags;
    std::map<std::string, std::string> admin;
  public:
    WXserver(const std::string& path)
    {
      CZH::CZH config(path);

      std::string token = config.in("config")["Token"].get_value<std::string>();
      std::string encoding_aes_key = config.in("config")["EncodingAESKey"].get_value<std::string>();
      std::string corpid = config.in("config")["CorpID"].get_value<std::string>();
      std::string corpsecret = config.in("config")["CorpSecret"].get_value<std::string>();
      
      port = config.in("config")["Port"].get_value<int>();

      wxmsg = WXmsg(token, encoding_aes_key, corpid);
      wxcmd = WXcmd(corpid, corpsecret);

      tags = config.in("tags").value_map<std::string>();
      admin = config.in("admin").value_map<std::string>();
    }
    void run()
    {
      int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//�����׽��֣�ʧ�ܷ���-1
      sockaddr_in addr;
      addr.sin_family = AF_INET; //ָ����ַ��
      addr.sin_addr.s_addr = INADDR_ANY;//IP��ʼ��
      addr.sin_port = htons(port);//�˿ںų�ʼ��

      bind(sock, (sockaddr*)&addr, sizeof(addr));//����IP�Ͷ˿�
      listen(sock, 0);//���ü���

      //���ÿͻ���
      sockaddr_in clientAddr;
      int clientAddrSize = sizeof(clientAddr);
      int clientSock;
      //���ܿͻ�������
      std::cout << "Server started successfully\n";
      while (-1 != (clientSock = accept(sock, (sockaddr*)&clientAddr, (socklen_t*)&clientAddrSize)))
      {
        // ������
        std::string requestStr;
        int bufSize = 4096;
        requestStr.resize(bufSize);
        //��������
        recv(clientSock, &requestStr[0], bufSize, 0);

        //������Ӧͷ
        std::string response =
          "HTTP/1.1 200 OK\r\n"
          "Content-Type: text/html; charset=gbk\r\n"
          "Connection: close\r\n"
          "\r\n";
        send(clientSock, response.c_str(), response.length(), 0);

        WXthpool thpool(16);
        std::function<void(ws::WXserver*, const int,const std::string&)> func =
          [](ws::WXserver* pwxs, const int clientSock, const std::string& requestStr)
          {pwxs->url_router(clientSock, requestStr);};
        thpool.add_task(func, this, clientSock, requestStr);
      }
      close(sock);//�رշ������׽���
    }
    void url_router(const int clientSock, const std::string& requestStr)
    {
      std::string out = "";
      std::string firstLine = requestStr.substr(0, requestStr.find("\r\n"));
      firstLine = firstLine.substr(firstLine.find(" ") + 1);
      std::string url = firstLine.substr(0, firstLine.find(" "));

      WXurl req_url(url);
      req_url.deescape();
      std::string req_type = requestStr.substr(0, 4);
      if (req_type == "GET ")//Ӧ���пո�GET����֤URL�������api�ĵ�
      {

        std::string msg_sig = req_url.parse("msg_signature");
        std::string timestamp = req_url.parse("timestamp");
        std::string nonce = req_url.parse("nonce");
        std::string req_echostr = req_url.parse("echostr");
        std::string echostr = wxmsg.verify_url(msg_sig, timestamp, nonce, req_echostr);
        send(clientSock, echostr.c_str(), echostr.length(), 0);
        out += "verify url success\n";
      }
      if (req_type == "POST")//POST���յ��ظ��������api�ĵ�
      {
        auto a = requestStr.find("<xml>");
        auto b = requestStr.find("</xml>");
        std::string temp = requestStr.substr(a, b + 6);
        WXxml req_xml(temp);

        std::string msg_sig = req_url.parse("msg_signature");
        std::string timestamp = req_url.parse("timestamp");
        std::string nonce = req_url.parse("nonce");
       
        auto cut = [](const std::string& str)->std::string{return str.substr(9, str.size() - 3 - 9);};

        std::string msg_encrypt = cut(req_xml.parse("Encrypt"));
        std::string req_msg = wxmsg.decrypt_msg(msg_sig, timestamp, nonce, msg_encrypt);
        WXxml plain_xml(req_msg);

        std::string content = cut(plain_xml.parse("Content"));
        out += "content: " + content + "\n";

        std::string UserID = cut(plain_xml.parse("FromUserName"));
        out += "UserID: " + UserID + "\n";

        if (tags.find(content) != tags.end())
        {
          wxcmd.send("text", tags[content], UserID);
          out += "res: " + tags[content] += "\n";
        }
        else if (content[0] == '/')
        {
          if (check_user(UserID))
            wxcmd.command(content, UserID);
          else
          {
            wxcmd.send("text", "Permission denied", UserID);
            out += "res: Permission denied\n";
          }
        }
      }
      time_t now = time(0);
      char* dt = ctime(&now);
      out += "time: ";
      out += dt;
      out += "---------------completed---------------\n";
      std::cout << out;
      close(clientSock);
    }
    void add_cmd(const std::string& tag, const WXcmd_func& func)
    {
      wxcmd.add_cmd(tag, func);
    }
  private:
   inline bool check_user(const std::string& id)
    {
      if (admin.find(id) == admin.end())
        return false;
      else
      {
        if (admin[id] == "true")
          return true;
        else
          return false;
      }
    }
  };
}
