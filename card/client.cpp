#include<bits/stdc++.h>
#ifdef __linux__
#include"socket_linux.h"
#else
#include<Windows.h>
#include<conio.h>
#include"socket.h"
#endif
#define van long long
using namespace std;
string name[16]={"","3","4","5","6","7","8","9","0","J","Q","K","A","2","S","B"};
string tostring(vector<van> x) {
	string ret="";
	for (int i=0;i<x.size();i++) ret+=name[x[i]];
	return ret;
}
vector<van> tovec(string x) {
	vector<van> ret;
	for (int i=0;i<x.size();i++) {
		for (int j=1;j<=15;j++) {
			if (x[i]==name[j][0]) ret.push_back(j);
		}
	} return ret;
} string id="1.0.2";
int main() {
	string ip = "127.0.0.1"; int p = 10086;
	// cout<<"Server IP Address: "; cin>>ip;
	// cout<<"Server Port: "; cin>>p;
	string x;
	cout<<"Input your name here: ";
	cin>>x; Client c;
	c.SetEnvironment(ip,p,"Connector");
	c.Connect(); 
	c.SendData(id.c_str());
	string cmd=c.RecvData();
	if (cmd!="ok") {
		cout<<"Server didn't accept id '"<<id<<"'";
		return 0;
	} van ren=atoi(c.RecvData().c_str());
	int port=atoi(c.RecvData().c_str());
	string ret="R";
	while (ret=="R") {
		c.SendData(x.c_str());
		ret=c.RecvData();
	} 
	c.Clear();
	c.SetEnvironment(ip,port,"Connector");
	c.Connect();
	for (int i=1;i<=ren;i++) {
		x=c.RecvData();
		cout<<"User '"<<x<<"' add the game!"<<endl;
	} cout<<"Game Start!"<<endl;
	
	// ����ϵͳ 
	x=c.RecvData();
	vector<van> pai=tovec(x); 
	sort(pai.begin(),pai.end());
	for (int i=0;i<17;i++) cout<<name[pai[i]]<<" "; cout<<endl;
	
	// ������ϵͳ 
	bool isget=false;
	cout<<"Rob the landlord?: [y/n,default: n]"; char get = 0;
	cin >> get; if (get=='y'||get=='Y') isget=true;
	c.SendData(isget?"1":"0");
	string command=c.RecvData();
	if (command=="get") cout<<"You rob the landlord!"<<endl;
	else cout<<"'"<<command<<"' rob the landlord."<<endl;
	
	// ��ʽ game
	while (1) {
		cout<<endl;
		pai=tovec(c.RecvData());
		sort(pai.begin(),pai.end());
		for (int i=0;i<pai.size();i++) cout<<name[pai[i]]<<" "; cout<<endl;
		string info; info=c.RecvData();
		for (int i=0;i<info.size();i++) {
			if (info[i]=='|') cout<<endl,i++;
			else cout<<info[i];
		} cout<<endl;
		
		string command=c.RecvData();
		if (command=="pai") {
			while(1) {
				cout<<"Input card: ";
				string input; cin >> input;
				if (input!="N") for (int i=0;i<input.size();i++) {
					bool ok=false;
					for (int j=1;j<=15;j++) {
						if (name[j][0]==input[i]) ok=true;
					} if (!ok) goto ji; 
				} if (false) {
					ji: cout<<"Invalid Card!"<<endl;
					continue;
				} c.SendData(input.c_str());
				command=c.RecvData();
				if (command=="ok") break;
				else {
					cout<<"Invalid Card2!"<<endl;
					continue;
				}
			} 
		} info=c.RecvData();
		cout<<info<<endl;
		command=c.RecvData();
		if (command=="stop") {
			string name=c.RecvData();
			cout<<"Game Over!"<<endl;
			cout<<"'"<<name<<"' won the game!"<<endl;
			getchar();
			return 0;
		}
	} 
	
	getchar();
}
