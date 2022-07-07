#include<bits/stdc++.h>
#include<Windows.h>
#include<conio.h>
#include"socket.h"
#include"poker.h"
#define van long long
using namespace std;
van ren=3;
Server s[4]; Client c[4];
van lesscard[20]; string name[4];
vector<van> pai[4]; Cards C,lst;
string toname[16]={"","3","4","5","6","7","8","9","0","J","Q","K","A","2","S","B"};
van getpai() {
	van x=rand()%15+1;
	while (lesscard[x]==0) {
		x=rand()%15+1;
	} lesscard[x]--;
	return x;
}
vector<van> getdz() {
	vector<van> res;
	for (int i=1;i<=15;i++) {
		while (lesscard[i]) res.push_back(i),lesscard[i]--;
	} return res;
}
string tostring(vector<van> x) {
	string ret="";
	for (int i=0;i<x.size();i++) ret+=toname[x[i]];
	return ret;
}
vector<van> tovec(string x) {
	vector<van> ret;
	for (int i=0;i<x.size();i++) {
		for (int j=1;j<=15;j++) {
			if (x[i]==toname[j][0]) ret.push_back(j);
		}
	} return ret;
} string id="1.0.1";
int main(int argc,char** argv) {
	if (argc<3) {
		cout<<"Usage: ./server.exe [ host ] [ port ]"<<endl;
		cout<<"Example: ./server.exe 127.0.0.1 10086"<<endl;
		return 0;
	}
	srand(time(0));
	for (int i=1;i<=13;i++) lesscard[i]=4;
	lesscard[14]=lesscard[15]=1;
	s[0].SetEnvironment(argv[1],atoi(argv[2]),"Main Connector");
	for (int i=1;i<=ren;i++) {
		s[0].Connect(); van port=rand()%10000+40000;
		string getid=s[0].RecvData();
		if (getid!=id) {
			s[0].SendData("fail");
			i--; continue;
		} s[0].SendData("ok");
		s[0].SendData(to_string(ren).c_str());
		s[0].SendData(to_string(port).c_str());
		while(name[i]=="") {
			name[i]=s[0].RecvData(); 
			if (name[i]=="") s[0].SendData("R");
			else s[0].SendData("O");
		} s[i].SetEnvironment(argv[1],port,"Client #"+to_string(i)); s[i].Connect();
		s[0].Clear(); s[0].SetEnvironment(argv[1],atoi(argv[2]),"Main Connector");
		for (int j=1;j<i;j++) s[j].SendData(name[i].c_str());
		for (int j=1;j<=i;j++) {
			s[i].SendData(name[j].c_str());
		}
	} 
	
	// 发牌系统 
	for (int j=1;j<=17;j++) for (int i=1;i<=3;i++) pai[i].push_back(getpai());
	for (int i=1;i<=ren;i++) s[i].SendData(tostring(pai[i]).c_str());
	
	// 抢地主系统
	vector<van> dz=getdz(); bool isget[4]={0},have=0;
	for (int i=1;i<=ren;i++) {
		isget[i]=atoi(s[i].RecvData().c_str());
		have|=isget[i];
	} if (!have) for (int i=1;i<=ren;i++) isget[i]=1;
	van give=0; while (isget[give]==0) give=rand()%ren+1;
	for (int i=0;i<dz.size();i++) pai[give].push_back(dz[i]);
	for (int i=1;i<=ren;i++) {
		if (give==i) s[give].SendData("get");
		else s[i].SendData(name[give].c_str());
	}
	
	// 正式 game
	van now=give,num=1; string nowname[4];
	while (1) {
		for (int i=1;i<=ren;i++) s[i].SendData(tostring(pai[i]).c_str());
		for (int i=1;i<=ren;i++) nowname[i]=name[i];
		nowname[now]+="[Turn]";
		for (int i=1;i<=ren;i++) {
			string info="";
			for (int j=1;j<i;j++) info+=nowname[j]+": "+to_string(pai[j].size())+(j!=ren?" | ":"");
			info+=nowname[i]+"[You]: "+to_string(pai[i].size())+(i!=ren?" | ":"");
			for (int j=i+1;j<=ren;j++) info+=nowname[j]+": "+to_string(pai[j].size())+(j!=ren?" | ":"");
			s[i].SendData(info.c_str());
		}	
			
		/*
		1. 给三个人发送指令 ("wait"/"pai")
		2. 接受出牌人的出牌信息
		3. 判断出牌是否合法
		4. 不合法给出牌人发送指令 "fail"，返回到第 2 步
		   若合法给出牌人发送指令 "ok"，执行接下来的步骤
		5. 给所有人发送出牌信息
		6. 给所有人发送当前牌信息 
		*/
		
		for (int i=1;i<=ren;i++) {
			if (i==now) s[i].SendData("pai");
			else s[i].SendData("wait");
		}
		if (num==ren) lst=Cards(),num=1;
		string put="";
		while(1) {
			string input=s[now].RecvData();
			if (input=="N") {
				s[now].SendData("ok"); num++;
				break;
			} 
			if (!C.Init(tovec(input),pai[now],lst)) {
				s[now].SendData("fail");
			} else {
				vector<van> in=tovec(input);
				sort(in.begin(),in.end());
				for (int i=0;i<in.size();i++) {
					for (auto it=pai[now].begin();it!=pai[now].end();it++) {
						if ((*it)==in[i]) {
							pai[now].erase(it);
							break;
						}
					}	
				} s[now].SendData("ok");
				lst=C; put=tostring(in);
				num=1; break;
			}
		} string message=name[now]+": "+put;
		cout<<"[info] User '"<<name[now]<<"' put "<<put<<endl;
		for (int i=1;i<=ren;i++) s[i].SendData(message.c_str());
		if (pai[now].size()==0) {
			for (int i=1;i<=ren;i++) {
				s[i].SendData("stop");
				s[i].SendData(name[now].c_str());
			} break;
		} else for (int i=1;i<=ren;i++) s[i].SendData("continue");
		
		now++; if (now>ren) now-=ren;
	}
	
	
	getch();
}
