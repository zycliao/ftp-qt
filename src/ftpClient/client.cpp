#include "client.h"
using namespace std;

Client::Client() {
    infoThread = new InfoThread;
}

Client::~Client() {
    closesocket(dataSocket);
    closesocket(controlSocket);
    WSACleanup();
    delete infoThread;
    delete buf;
    delete databuf;
}

int Client::login(QString ip_addr, QString username, QString password) {
    this->ip_addr = ip_addr.toStdString();
    this->username = username.toStdString();
    this->password = password.toStdString();
}

int Client::connectServer() {
    WSADATA dat;
    int ret;

    //初始化，很重要
    if (WSAStartup(MAKEWORD(2,2),&dat)!=0)  //Windows Sockets Asynchronous启动
    {
        cout<<"Init Failed: "<<GetLastError()<<endl;
        infoThread->sendInfo("Init Failed!\n");
        system("pause");
        return -1;
    }

    //创建Socket
    controlSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(controlSocket==INVALID_SOCKET)
    {
        cout<<"Creating Control Socket Failed: "<<GetLastError()<<endl;
        infoThread->sendInfo("Creating Control Socket Failed.\n");
        system("pause");
        return -1;
    }
    //构建服务器访问参数结构体
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_addr.S_un.S_addr=inet_addr(ip_addr.c_str()); //地址
    serverAddr.sin_port=htons(PORT);            //端口
    memset(serverAddr.sin_zero,0,sizeof(serverAddr.sin_zero));

    //连接
    ret=connect(controlSocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
    if(ret==SOCKET_ERROR)
    {
        cout<<"Control Socket Connecting Failed: "<<GetLastError()<<endl;
        infoThread->sendInfo("Control Socket Connecting Failed\n");
        system("pause");
        return -1;
    }
    cout<<"Control Socket connecting is success."<<endl;
    //接收返回状态信息
    Sleep(300);
    recvControl(220);

    //用户名
    executeCmd("USER " + username);
    recvControl(331);
    //executeFTPCmd(331, "USER", qstr2pch(username));                //331

    //密码
    executeCmd("PASS " + password);
    recvControl(230);
    //executeFTPCmd(230, "PASS", qstr2pch(password));            //230

    listPwd();
    return 0;
}

int Client::disconnect() {
    executeCmd("QUIT");
    recvControl(221);
    ip_addr, username, password, INFO = "";
    filelist.clear();
    memset(buf, 0, BUFLEN);
    memset(databuf, 0, DATABUFLEN);
    closesocket(dataSocket);
    closesocket(controlSocket);
    WSACleanup();
}

int Client::changeDir(string tardir) {
    memset(buf, 0, BUFLEN);
    executeCmd("CWD "+tardir);
    recvControl(250);
    intoPasv();
    listPwd();
    return 0;
}

int Client::downFile(string remoteName, string localDir){
    string localFile = localDir + "/" + remoteName;
    ofstream ofile;
    ofile.open(localFile);
    intoPasv();
    int fsize = getFileSize(remoteName);
    executeCmd("RETR "+remoteName);
    recvControl(150);
    memset(databuf, 0, DATABUFLEN);
    int recvNum;
    recvNum = min(DATABUFLEN, fsize);
    recv(dataSocket, databuf, recvNum, 0);
    fsize -= recvNum;
    for( ; fsize > 0;) {
        databuf[recvNum] = '\0';
        ofile<<databuf;
        recvNum = min(DATABUFLEN, fsize);
        recv(dataSocket, databuf, recvNum, 0);
        fsize -= recvNum;
    }
    databuf[recvNum] = '\0';
    ofile<<databuf;
    ofile.close();
    closesocket(dataSocket);
    recvControl(226);
}

//private function---------------------------------------------------------
int Client::executeCmd(string cmd) {
    cmd += "\r\n";
    int cmdlen = cmd.size();
    infoThread->sendInfo(cmd);
    cout << cmd;
    send(controlSocket, cmd.c_str(), cmdlen, 0);
    return 0;
}

// TODO:This function needs to be modified so that
// long information could be received.
int Client::recvControl(int stateCode, string errorInfo) {
    if(errorInfo.size()==1)
        errorInfo = "state code error!";
    if(nextInfo.size()==0) {
        int t;
        Sleep(50);
        memset(buf, 0, BUFLEN);
        recvInfo.clear();
        int infolen = recv(controlSocket, buf, BUFLEN, 0);
        if(infolen==BUFLEN) {
            cout << "ERROR! Too long information too receive!" << endl;
            infoThread->sendInfo("ERROR! Too long information too receive!\n");
            return -1;
        }
        buf[infolen] = '\0';
        t = getStateCode();
        recvInfo = buf;
        cout << recvInfo << endl;

        // JUNK
        int temp = recvInfo.find("\r\n226");
        if(temp>=0) {
            nextInfo = recvInfo.substr(temp+2);
        }
        // \JUNK
        infoThread->sendInfo(recvInfo);
        if(t == stateCode)
            return 0;
        else {
            cout << errorInfo << endl;
            infoThread->sendInfo(errorInfo);
            return -1;
        }
    }
    else {
        recvInfo = nextInfo;
        nextInfo.clear();
        return 0;
    }


}

//从返回信息中获取状态码
int Client::getStateCode()
{
    int num=0;
    char* p = buf;
    while(p != nullptr)
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
            if(*p<='9' && *p>='0')
                num1 = 10*num1+(*p)-'0';
        }
        if(cnt == 5)
        {
            if(*p<='9' && *p>='0')
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

int Client::listPwd() {
    intoPasv();
    executeCmd("LIST -al");
    recvControl(150);
    memset(databuf, 0, DATABUFLEN);
    string fulllist;
    int ret = recv(dataSocket, databuf, DATABUFLEN-1, 0);
    while(ret>0) {
        databuf[ret] = '\0';
        fulllist += databuf;
        ret = recv(dataSocket, databuf, DATABUFLEN-1, 0);
    }
    removeSpace(fulllist);

    int lastp, lastq, p, q;
    vector<string> eachrow;
    string rawrow;
    string item;
    filelist.clear();
    p = fulllist.find("\r\n");
    lastp = 0;
    while(p>=0) {
        eachrow.clear();
        rawrow = fulllist.substr(lastp, p-lastp);

        q = rawrow.find(' ');
        lastq = 0;
        for(int i=0; i<8; i++) {
            item = rawrow.substr(lastq, q-lastq);
            eachrow.push_back(item);
            lastq = q + 1;
            q = rawrow.find(' ', lastq);
        }
        item = rawrow.substr(lastq);
        eachrow.push_back(item);
        filelist.push_back(eachrow);

        lastp = p + 2;
        p = fulllist.find("\r\n", lastp);
    }
    closesocket(dataSocket);
    recvControl(226);
    return 0;
}

int Client::intoPasv() {
    int dataPort, ret;
    //切换到被动模式
    executeCmd("PASV");
    recvControl(227);
    //executeFTPCmd(227, "PASV");                //227

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
        return -1;
    }
    cout<<"Data Socket connecting is success."<<endl;
    return 0;
}

int Client::getFileSize(string fname) {
    executeCmd("SIZE " + fname);
    recvControl(213);
    char* p = buf;
    while(p != nullptr && *p != ' ') {
        p++;
    }
    p++;
    int num = 0;
    while(p != nullptr && *p != '\r') {
        num *= 10;
        num += (*p - '0');
        p++;
    }
    memset(buf, 0, BUFLEN);
    return num;

}

int Client::upFile(string localName) {
    // TODO:change to C++ style
    FILE* ifile = fopen(localName.c_str(), "rb");
    if(!ifile) {
        cout << "fail to open the file!\n";
        infoThread->sendInfo("fail to open the file!");
        return -1;
    }

    //get file name
    string localFileName;
    int p = localName.find_last_of("/");
    localFileName = localName.substr(p+1);

    intoPasv();
    executeCmd("STOR "+localFileName);
    recvControl(150, "Permission denied.");
    int count;
    while(!feof(ifile))
    {
        count = fread(databuf, 1, DATABUFLEN, ifile);
        send(dataSocket, databuf, count, 0);
    }
    memset(databuf, 0, DATABUFLEN);
    send(dataSocket, databuf, 1, 0);
    fclose(ifile);
    closesocket(dataSocket);
    recvControl(226);
    listPwd();
}

void Client::removeSpace(string & src) {
    int p, q;
    p = src.find(' ');
    while(p>=0) {
        for(q=p+1; src[q]==' '; q++);
        src.erase(p+1, q-p-1);
        p = src.find(' ', p+1);
    }
}

int Client::deleteFile(string fname) {
    executeCmd("DELE "+fname);
    recvControl(250);
    listPwd();
    return 0;
}

int Client::deleteDir(string dname) {
    executeCmd("RMD "+dname);
    recvControl(250);
    listPwd();
    return 0;
}

int Client::rename(string src, string dst) {
    executeCmd("RNFR "+src);
    recvControl(350);
    executeCmd("RNTO "+dst);
    recvControl(250);
    listPwd();
    return 0;
}

int Client::mkDir(string name) {
    executeCmd("MKD "+name);
    recvControl(250);
    listPwd();
    return 0;
}
