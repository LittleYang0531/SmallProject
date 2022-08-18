#ifndef _SOCKET_H_
#define _SOCKET_H_

#include<sys/socket.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<termio.h>

using namespace std;

int getch() {
    struct termios tm, tm_old;
    int fd = 0, ch;
    if (tcgetattr(fd, &tm) < 0) return -1;
    tm_old = tm;
    cfmakeraw(&tm);
    if (tcsetattr(fd, TCSANOW, &tm) < 0) return -1;
    ch = getchar();
    if (tcsetattr(fd, TCSANOW, &tm_old) < 0) return -1;
    return ch;
}

const string ending="wedctfgvdhbewusaxolsnwjdjuhzksw";
struct Server {
	int ServerSocket;
	int ClientSocket;
	sockaddr_in ServerAddress;
	sockaddr_in ClientAddress;
	int recvtimes=0;
	string name="";
	ofstream fout;
	void SetEnvironment(string ip, int port, string namae = "") {
        fout.open("server.log", ios::app);
        name = namae; // namae => なまえ(名前) => 名字

        /** 初始化服务端socket */
        bzero(&ServerAddress, sizeof(ServerAddress));
        ServerAddress.sin_family = AF_INET;
        ServerAddress.sin_addr.s_addr = inet_addr(ip.c_str());
        ServerAddress.sin_port = htons(port);
        ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (ServerSocket < 0) {
            cout << "[" << name << "] [Error] Server::SetEnvironment(string ip, int port, string namae): Failed to initialize socket!" << endl;
            fout << "[" << name << "] [Error] Server::SetEnvironment(string ip, int port, string namae): Failed to initialize socket!" << endl;
            exit(3);
        }

        /** 绑定服务端socket */
        int opt = 1;
        setsockopt(ServerSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        int ret = bind(ServerSocket, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress));
        if (ret == -1) {
            cout << "[" << name << "] [Error] Server::SetEnvironment(string ip, int port, string namae): Failed to bind socket!" << endl;
            fout << "[" << name << "] [Error] Server::SetEnvironment(string ip, int port, string namae): Failed to bind socket!" << endl;
            exit(3);
        }

        /** 设置服务端监听态 */
        ret = listen(ServerSocket,1);
        if (ret == -1) {
            cout << "[" << name << "] [Error] Server::SetEnvironment(string ip, int port, string namae): Failed to listen to client!" << endl;
            fout << "[" << name << "] [Error] Server::SetEnvironment(string ip, int port, string namae): Failed to listen to client!" << endl;
            exit(3);
        }

        fout << "[" << name << "] [Info] Server::SetEnvironment(string ip, int port, string namae): Listening..." << endl;
    }
	void Connect() {
        socklen_t len = sizeof(sockaddr);
        ClientSocket = accept(ServerSocket, (struct sockaddr*)&ClientAddress, &len);
        if (ClientSocket < 0) {
            cout << "[" << name << "] [Error] Server::Connect(): Failed to accept request!" << endl;
            fout << "[" << name << "] [Error] Server::Connect(): Failed to accept request!" << endl;
            pthread_exit(NULL);
        }
        string ip = inet_ntoa(*((struct in_addr*)&ClientAddress.sin_addr));
        fout << "[" << name << "] [info] Server::Connect(): Connect to Client (" << ip << ":" << ClientAddress.sin_port << ")" << endl;
    }
    int send2(const char* data) {
        return send(ClientSocket, data, strlen(data), 0);
    }
    int recv2(string& ret) {
        const int length = 1024 * 1024;
        char __buf[length] = "";
        memset(__buf, '\0', sizeof __buf);
        int s = recv(ClientSocket, __buf, sizeof __buf, 0);
        ret = __buf;
        return s;
    }
	void SendData(const char* data) {
        if (recvtimes == 0) {
            string ret = "";
            int s = recv2(ret);
            if (s < 0) {
                cout << "[" << name << "] [Error] Server::SendData(const char*): Recieve Signal error" << endl;
                fout << "[" << name << "] [Error] Server::SendData(const char*): Recieve Signal error" << endl;
                exit(3);
            }
            fout << "[" << name << "] [Info] Server::SendData(const char*): Recieve Signal form Client: " << ret << endl;
        } else {
            recvtimes--;
            string ret = "Recieve Signal";
            fout << "[" << name << "] [Info] Server::SendData(const char*): Recieve Signal form Client: " << ret << endl;
        }
        // usleep(10000);

        string d = string(data) + ending;
        int s = send2(d.c_str());
        if (s < 0) {
            cout << "[" << name << "] [Error] Server::SendData(const char*): Send data error" << endl;
            fout << "[" << name << "] [Error] Server::SendData(const char*): Send data error" << endl;
            exit(3);
        }
        fout << "[" << name << "] [Info] Server::SendData(const char*): Send Data to Client: " << data << endl;
        fout << "[" << name << "] [Info] Server::SendData(const char*): Send Byte: " << d.size() << "byte" << endl;
        // usleep(10000);
    }
	string RecvData() {
        string data2 = "Recieve Signal";
        int s = send2(data2.c_str());
        if (s < 0) {
            cout << "[" << name << "] [Error] Server::RecvData(): Send Signal Error" << endl;
            fout << "[" << name << "] [Error] Server::RecvData(): Send Signal Error" << endl;
            exit(3);
        }
        fout << "[" << name << "] [Info] Server::RecvData(): Send Signal to Client: " << data2 << endl;
        // usleep(10000);

        string data;
        s = recv2(data);
        if (s < 0) {
            cout << "[" << name << "] [Error] Server::RecvData(): Send Signal Error" << endl;
            fout << "[" << name << "] [Error] Server::RecvData(): Send Signal Error" << endl;
            exit(3);
        }
        string tmp = data;
        while(tmp.size() >= data2.size()) {
            if (tmp.substr(0, data2.size()) == data2) {
                recvtimes++;
                tmp.erase(0, data2.size());
            } else if (tmp.substr(tmp.size() - data2.size()) == data2) {
                recvtimes++;
                tmp.erase(tmp.size() - data2.size(), data2.size());
            } else break;
        } if (tmp.size() >= ending.size()) tmp.erase(tmp.size() - ending.size(), ending.size());
        fout << "[" << name << "] [Info] Server::RecvData(): Recieve Data form Client: " << data << endl;
        fout << "[" << name << "] [Info] Server::RecvData(): Recieve Byte: " << s << "byte" << endl;
        // usleep(10000);
        return tmp;
    }
	void Clear() {
        fout.close();
        close(ServerSocket);
        close(ClientSocket);
    }
};

struct Client {
	int ServerSocket;
	int ClientSocket;
	sockaddr_in ServerAddress;
	sockaddr_in ClientAddress;
	int recvtimes=0;
	string name;
	ofstream fout;
	void SetEnvironment(string ip, int port, string namae = "") {
        fout.open("client.log", ios::app);
        name = namae; 

        ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
        if(ServerSocket < 0){
            cout << "[" << name << "] [Error] Client::SetEnvironment(string ip, int port, string namae): Failed to initialize socket!" << endl;
            fout << "[" << name << "] [Error] Client::SetEnvironment(string ip, int port, string namae): Failed to initialize socket!" << endl;
            exit(3);
        }

        memset(&ServerAddress, 0, sizeof(ServerAddress));
        ServerAddress.sin_family = AF_INET;
        ServerAddress.sin_port = htons(port);
        if(inet_pton(AF_INET, ip.c_str(), &ServerAddress.sin_addr) <= 0){
            cout << "[" << name << "] [Error] Client::SetEnvironment(string ip, int port, string namae): inet_pton error!" << endl;
            fout << "[" << name << "] [Error] Client::SetEnvironment(string ip, int port, string namae): inet_pton error!" << endl;
            exit(3);
        }
    }
	void Connect() {
        socklen_t len = sizeof(sockaddr);
        int ret = connect(ServerSocket, (struct sockaddr*)&ServerAddress, len);
        if (ret < 0) {
            cout << "[" << name << "] [Error] Client::Connect(string ip, int port, string namae): Connect error!" << endl;
            fout << "[" << name << "] [Error] Client::Connect(string ip, int port, string namae): Connect error!" << endl;
            exit(3);
        }
    }
    int send2(const char* data) {
        return send(ServerSocket, data, strlen(data), 0);
    }
    int recv2(string& ret) {
        const int length = 1024 * 1024;
        char __buf[length] = "";
        memset(__buf, '\0', sizeof __buf);
        int s = recv(ServerSocket, __buf, sizeof __buf, 0);
        ret = __buf;
        return s;
    }
	void SendData(const char* data) {
        if (recvtimes == 0) {
            string data2 = "";
            int s = recv2(data2);
            if (s < 0) {
                cout << "[" << name << "] [Error] Client::SendData(const char*): Recieve Signal error" << endl;
                fout << "[" << name << "] [Error] Client::SendData(const char*): Recieve Signal error" << endl;
                exit(3);
            }
            fout << "[" << name << "] [Info] Client::SendData(const char*): Recieve Signal form Server: " << data2 << endl;
        } else {
            recvtimes--;
            fout << "[" << name << "] [Info] Client::SendData(const char*): Recieve Signal form Server: Recieve Signal" << endl;
        }
        // usleep(10000);

        string tmp = data; tmp += ending;
        int s = send2(tmp.c_str());
        if (s < 0) {
            cout << "[" << name << "] [Error] Client::SendData(const char*): Send data error" << endl;
            fout << "[" << name << "] [Error] Client::SendData(const char*): Send data error" << endl;
            exit(3);
        }
        fout << "[" << name << "] [Info] Client::SendData(const char*): Send Data to Server: " << data << endl;
        fout << "[" << name << "] [Info] Client::SendData(const char*): Send Byte: " << s << "byte" << endl;
        // usleep(10000);
    }
	string RecvData() {
        string data2 = "Recieve Signal";
        int s = send2(data2.c_str());
        if (s < 0) {
            cout << "[" << name << "] [Error] Client::RecvData(): Send Signal error" << endl;
            fout << "[" << name << "] [Error] Client::RecvData(): Send Signal error" << endl;
            exit(3);
        }
        // usleep(10000);

        string data = "";
        s = recv2(data);
        if (s < 0) {
            cout << "[" << name << "] [Error] Client::RecvData(): Recieve data error" << endl;
            fout << "[" << name << "] [Error] Client::RecvData(): Recieve data error" << endl;
            exit(3);
        }
        string tmp = data;
        while (tmp.size() >= data2.size()) {
            if (tmp.substr(0, data2.size()) == data2) {
                recvtimes++;
                tmp.erase(0, data2.size());
            } else if (tmp.substr(tmp.size() - data2.size()) == data2) {
                recvtimes++;
                tmp.erase(tmp.size() - data2.size(), data2.size());
            } else break;
        } if (tmp.size() >= ending.size()) tmp.erase(tmp.size() - ending.size(), ending.size());
        fout << "[" << name << "] [Info] Client::RecvData(): Recieve Data from Server: " << tmp << endl;
        fout << "[" << name << "] [Info] Client::RecvData(): Recieve Byte: " << s << "byte" << endl;
        // usleep(10000);
        return tmp;
    }
	void Clear() {
        fout.close();
        close(ServerSocket);
        // close(ClientSocket);
    }
};

#endif