#include "client.h"

Client::Client() {

}

Client::~Client() {
    delete buf;
    delete databuf;
}

int Client::login(QString ip_addr, QString username, QString password) {
    this->ip_addr = ip_addr;
    this->username = username;
    this->password = password;
}

int Client::connectServer() {
    using namespace std;
    WSADATA dat;
    SOCKADDR_IN serverAddr;
    int dataPort, ret;

    //初始化，很重要
    if (WSAStartup(MAKEWORD(2,2),&dat)!=0)  //Windows Sockets Asynchronous启动
    {
        cout<<"Init Falied: "<<GetLastError()<<endl;
        system("pause");
        return -1;
    }

    //创建Socket
    controlSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(controlSocket==INVALID_SOCKET)
    {
        cout<<"Creating Control Socket Failed: "<<GetLastError()<<endl;
        system("pause");
        return -1;
    }
    //构建服务器访问参数结构体
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_addr.S_un.S_addr=inet_addr(ip_addr.toStdString().c_str()); //地址
    serverAddr.sin_port=htons(PORT);            //端口
    memset(serverAddr.sin_zero,0,sizeof(serverAddr.sin_zero));

    //连接
    ret=connect(controlSocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
    if(ret==SOCKET_ERROR)
    {
        cout<<"Control Socket connecting Failed: "<<GetLastError()<<endl;
        system("pause");
        return -1;
    }
    cout<<"Control Socket connecting is success."<<endl;
    //接收返回状态信息
    recv(controlSocket,buf,100,0);
    cout<<buf;       //220

    //根据返回信息提取状态码，进行判断
    if(getStateCode() != 220)
    {
        cout<<"Error: Control Socket connecting Failed"<<endl;
        system("pause");
        exit(-1);
    }

    //用户名
    executeFTPCmd(331, "USER", qstr2pch(username));                //331

    //密码
    executeFTPCmd(230, "PASS", qstr2pch(password));            //230

    //切换到被动模式
    executeFTPCmd(227, "PASV");                //227

    //返回的信息格式为---h1,h2,h3,h4,p1,p2
    //其中h1,h2,h3,h4为服务器的地址，p1*256+p2为数据端口
    dataPort=getPortNum();
    //客户端数据传输socket
    dataSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    serverAddr.sin_port=htons(dataPort);    //更改连接参数中的port值
    ret=connect(dataSocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
    if(ret==SOCKET_ERROR)
    {
        cout<<"Data Socket connecting Failed: "<<GetLastError()<<endl;
        system("pause");
        return -1;
    }
    cout<<"Data Socket connecting is success."<<endl;
    listPwd();
    return 0;
}

//通过控制socket执行FTP命令
int Client::executeFTPCmd(int stateCode, char* cmd, char* arg)
{
    memset(buf, 0, BUFLEN);
    if(arg)
        sprintf(buf, "%s %s\r\n", cmd, arg);
    else
        sprintf(buf, "%s\r\n", cmd);
    int cmdlen = (int)strlen(buf);
    send(controlSocket, buf, cmdlen, 0);
    memset(buf, 0, BUFLEN);
    recv(controlSocket, buf, BUFLEN, 0);
    std::cout << buf << std::endl;
    if(getStateCode() == stateCode)
    {
        return 0;
    }
    else
    {
        std::cout << "The StateCode is Error!" << std::endl;
        return -1;
    }
}
//从返回信息中获取状态码
int Client::getStateCode()
{
    int num=0;
    char* p = buf;
    while(p != NULL)
    {
        num=10*num+(*p)-'0';
        p++;
        if(*p==' ' || *p=='-')
        {
            break;
        }
    }

    return num;
}

//从返回信息“227 Entering Passive Mode (182,18,8,37,10,25).”中
//获取数据端口
int Client::getPortNum()
{
    int num1=0,num2=0;

    char* p=buf;
    int cnt=0;
    while( 1 )
    {
        if(cnt == 4 && (*p) != ',')
        {
            num1 = 10*num1+(*p)-'0';
        }
        if(cnt == 5)
        {
            num2 = 10*num2+(*p)-'0';
        }
        if((*p) == ',')
        {
            cnt++;
        }
        p++;
        if((*p) == ')')
        {
            break;
        }
    }
    std::cout<<"The data port number is "<<num1*256+num2<<std::endl;
    return num1*256+num2;
}

void Client::listPwd() {
    executeFTPCmd(150, "NLST", ".");
    memset(databuf, 0, DATABUFLEN);
    recv(dataSocket, databuf, DATABUFLEN, 0);
    std::vector<char*> result;
    char* one_result = nullptr;
    one_result = strtok(databuf, DELIMITER);
    while(one_result) {
        result.push_back(one_result);
        one_result = strtok(nullptr, DELIMITER);
    }
    pwdFiles = result;
}

