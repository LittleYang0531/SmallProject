#ifndef _SOCKET_H_
#define _SOCKET_H_
using namespace std;
const string ending="wedctfgvdhbewusaxolsnwjdjuhzksw";
struct Server {
	SOCKET ServerSocket;
	SOCKET ClientSocket;
	SOCKADDR_IN ServerAddress;
	SOCKADDR_IN ClientAddress;
	int recvtimes=0;
	string name="";
	ofstream fout;
	void SetEnvironment(string ip,int port,string connector_name);
	void Connect();
	void SendData(const char* data);
	string RecvData();
	void Clear();
};
struct Client {
	SOCKET ServerSocket;
	SOCKET ClientSocket;
	SOCKADDR_IN ServerAddress;
	SOCKADDR_IN ClientAddress;
	int recvtimes=0;
	string name;
	ofstream fout;
	void SetEnvironment(string ip,int port,string connector_name);
	void Connect();
	void SendData(const char* data);
	string RecvData();
	void Clear();
};

void return_error(const char* info) {
	cout<<"[Error] "<<info;
	getch();
	exit(3);
}

#define fout cout
void Server::SetEnvironment(string ip,int port,string connector_name)
{
//	fout.open("server.log",ios::app);
	name=connector_name;
	WORD w_req=MAKEWORD(2,2);
	WSADATA wsadata; int err;
	err=WSAStartup(w_req,&wsadata);
	if (err!=0) return_error("Failed to initialize SOCKET");
	if (LOBYTE(wsadata.wVersion)!=2||HIBYTE(wsadata.wHighVersion)!=2) 
		return_error("SOCKET version is not correct");
	this->ServerAddress.sin_family=AF_INET;
	this->ServerAddress.sin_addr.S_un.S_addr=inet_addr(ip.c_str());
	this->ServerAddress.sin_port=htons(port);
	this->ServerSocket=socket(AF_INET,SOCK_STREAM,0);
	if (bind(this->ServerSocket,(SOCKADDR*)&this->ServerAddress,sizeof(SOCKADDR))==SOCKET_ERROR)
		return_error("Failed to bind SOCKET");
	if (listen(this->ServerSocket,SOMAXCONN)<0) return_error("Failed to set listen status");
	fout<<"["<<this->name<<"] [info] Server::Connect(): Set Server Environment ("<<ip<<":"<<port<<")"<<endl;
}
void Server::Connect()
{
	int len=sizeof(SOCKADDR);
	this->ClientSocket=accept(this->ServerSocket,(SOCKADDR*)&this->ClientAddress,&len);
	if (this->ClientSocket==SOCKET_ERROR) return_error("Failed to connect to client");
	string ip=inet_ntoa(*((struct in_addr*)&this->ClientAddress.sin_addr));
	fout<<"["<<this->name<<"] [info] Server::Connect(): Connect to Client ("<<ip<<":"<<this->ClientAddress.sin_port<<")"<<endl;
}
void Server::SendData(const char* data)
{
	if (this->recvtimes) {
		char data2[1024*1024]="";
		int RecvLen=recv(ClientSocket,data2,sizeof data2,0);
		if (RecvLen<0) return_error("Recieve Signal error");
		fout<<"["<<this->name<<"] [info] Server::SendData(const char*): Recieve Signal form Client: "<<data2<<endl;
	} else this->recvtimes--,
		fout<<"["<<this->name<<"] [info] Server::SendData(const char*): Recieve Signal form Client: Recieve Signal"<<endl;
	Sleep(10);
	
	string tmp=data; tmp+=ending;
	int Length=string(tmp).size();
	int SendLen=send(ClientSocket,const_cast<char*>(tmp.c_str()),Length,0);
	if(SendLen<0) return_error("Send data error");
	fout<<"["<<this->name<<"] [info] Server::SendData(const char*): Send Data to Client: "<<data<<endl;
	fout<<"["<<this->name<<"] [info] Server::SendData(const char*): Send Byte: "<<SendLen<<"byte"<<endl;
	Sleep(10);
}
string Server::RecvData()
{
	string data2="recieve signal";
	int Length=data2.size();
	int SendLen=send(ClientSocket,const_cast<char*>(data2.c_str()),Length,0);
	if(SendLen<0) return_error("Send Signal error");
	fout<<"["<<this->name<<"] [info] Server::RecvData(): Send Signal to Client: "<<data2<<endl;
	Sleep(10);
	
	char data[1024*1024]="";
	int RecvLen=recv(ClientSocket,data,sizeof data,0);
	if (RecvLen<0) return_error("Recieve data error");
	string tmp=data;
	while (tmp.size()>=string("recieve signal").size()) {
		if (tmp.substr(0,14)=="recieve signal") {
			this->recvtimes++;
			tmp.erase(0,14);
		} else if (tmp.substr(tmp.size()-14)=="recieve signal") {
			this->recvtimes++;
			tmp.erase(tmp.size()-14,14);
		} else break;
	} if (tmp.size()>=ending.size()) tmp.erase(tmp.size()-ending.size(),ending.size());
	fout<<"["<<this->name<<"] [info] Server::RecvData(): Recieve Data form Client: "<<tmp<<endl;
	fout<<"["<<this->name<<"] [info] Server::RecvData(): Recieve Byte: "<<RecvLen<<"byte"<<endl;
	Sleep(10);
	return tmp;
}
void Server::Clear()
{
//	fout.close();
	closesocket(this->ServerSocket);
	closesocket(this->ClientSocket);
	WSACleanup();
}
#undef fout

void Client::SetEnvironment(string ip,int port,string connector_name)
{
	fout.open("client.log",ios::app);
	name=connector_name;
	WORD w_req=MAKEWORD(2,2);
	WSADATA wsadata; int err;
	err=WSAStartup(w_req,&wsadata);
	if (err!=0) return_error("Failed to initialize SOCKET");
	if (LOBYTE(wsadata.wVersion)!=2||HIBYTE(wsadata.wHighVersion)!=2)
		return_error("SOCKET version is not correct");
	this->ServerAddress.sin_family=AF_INET;
	this->ServerAddress.sin_addr.S_un.S_addr=inet_addr(ip.c_str());
	this->ServerAddress.sin_port=htons(port);
}
void Client::Connect()
{
	this->ServerSocket=socket(AF_INET,SOCK_STREAM,0);
	if (connect(this->ServerSocket,(SOCKADDR*)&this->ServerAddress,sizeof(SOCKADDR))==SOCKET_ERROR) 
		return_error("Failed to connect to server");
	string ip=inet_ntoa(*((struct in_addr*)&this->ServerAddress.sin_addr));
	fout<<"["<<this->name<<"] [info] Client::Connect(): Connect to Server ("<<ip<<":"<<this->ServerAddress.sin_port<<")"<<endl;
}
void Client::SendData(const char* data)
{
	if (this->recvtimes==0) {
		char data2[1024*1024]="";
		int RecvLen=recv(ServerSocket,data2,sizeof data2,0);
		if (RecvLen<0) return_error("Recieve Signal error");
		fout<<"["<<this->name<<"] [info] Client::SendData(const char*): Recieve Signal form Server: "<<data2<<endl;
	} else this->recvtimes--,
		fout<<"["<<this->name<<"] [info] Client::SendData(const char*): Recieve Signal form Server: Recieve Signal"<<endl;
	Sleep(10);
	
	string tmp=data; tmp+=ending;
	int Length=string(tmp).size();
	int SendLen=send(ServerSocket,const_cast<char*>(tmp.c_str()),Length,0);
	if (SendLen<0) return_error("Send data error");
	fout<<"["<<this->name<<"] [info] Client::SendData(const char*): Send Data to Server: "<<data<<endl;
	fout<<"["<<this->name<<"] [info] Client::SendData(const char*): Send Byte: "<<SendLen<<"byte"<<endl;
	Sleep(10);
}
string Client::RecvData()
{
	string data2="recieve signal";
	int Length=data2.size();
	int SendLen=send(ServerSocket,const_cast<char*>(data2.c_str()),Length,0);
	if (SendLen<0) return_error("Send Signal error");
	fout<<"["<<this->name<<"] [info] Client::RecvData(): Send Signal to Server: "<<data2<<endl;
	Sleep(10);
	
	char data[1024*1024]="";
	int RecvLen=recv(ServerSocket,data,sizeof data,0);
	if (RecvLen<0) return_error("Recieve data error");
	string tmp=data;
	while (tmp.size()>=string("recieve signal").size()) {
		if (tmp.substr(0,14)=="recieve signal") {
			this->recvtimes++;
			tmp.erase(0,14);
		} else if (tmp.substr(tmp.size()-14)=="recieve signal") {
			this->recvtimes++;
			tmp.erase(tmp.size()-14,14);
		} else break;
	} if (tmp.size()>=ending.size()) tmp.erase(tmp.size()-ending.size(),ending.size());
	fout<<"["<<this->name<<"] [info] Client::RecvData(): Recieve Data from Server: "<<tmp<<endl;
	fout<<"["<<this->name<<"] [info] Client::RecvData(): Recieve Byte: "<<RecvLen<<"byte"<<endl;
	Sleep(10);
	return tmp;
}
void Client::Clear()
{
	fout.close();
	closesocket(this->ServerSocket);
	closesocket(this->ClientSocket);
	WSACleanup();
}


bool GetLocalIp(char* ip)
{
    WSADATA wsaData;  
    int ret=WSAStartup(MAKEWORD(2,2),&wsaData);  
    if (ret!=0)  return false;  
    char hostname[256];  
    ret=gethostname(hostname,sizeof(hostname));  
    if (ret==SOCKET_ERROR) return false;  
    HOSTENT* host=gethostbyname(hostname);  
    if (host==NULL) return false;  
    strcpy(ip,inet_ntoa(*(in_addr*)*host->h_addr_list));  
    return true;
}  
int StringToInt(std::string a)
{
	int ans=0;
	for (int i=0;i<a.size();i++) ans*=10,ans+=a[i]-48;
	return ans;
}
std::string IntToString(int a)
{
	char ans[101]="",k=-1;
	while (a!=0)
	{
		ans[++k]=a%10+48;
		a/=10;
	}
	for (int i=0;i<(k+1)/2;i++) std::swap(ans[i],ans[k-i]);
	return (char*)(((std::string)ans).data()); 
}
#endif
