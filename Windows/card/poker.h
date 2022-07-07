#ifndef _POKER_H_
#define _POKER_H_
using namespace std;
#define van long long
struct Cards{
	//cnt1 ���Ƹ��� 
	//isContinuous �Ƿ�Ϊ���� 
	//cnt2 С�Ƹ��� 
	string Str;
	van Id,cnt1,isContinuous,cnt2;
	vector<van> cards;
	
	Cards() { Id=-1; }
	bool operator ==(const Cards& oth)const {return Id==oth.Id;}
	void init(char* _Str,van _Id,van _cnt1,van _isContinuous,van _cnt2) {
		cnt1=_cnt1,cnt2=_cnt2,Str=_Str,Id=_Id;
		isContinuous=_isContinuous;
	}
	
	van value() {
		van i;//��������Ȩֵ
		if(Str=="����һ"||Str=="����һ��"||Str=="�ɻ�") {
			for(i=2; i<cards.size(); i++)
				if(cards[i]==cards[i-2])
					return cards[i];
		}
		if(Str=="�Ĵ���") {
			for(i=3; i<cards.size(); i++)
				if(cards[i]==cards[i-3])
					return cards[i];
		}
		return cards[0];
	}
	
	void calType() {
		van i,n=cards.size();
		if(n==0) {init("����",14,0,0,0);return;}
		if(n==2&&cards[0]==15&&cards[1]==14) {init("��ը",0,0,0,0);return;}
	
		//ͳ��ͬ�������ж�����
		van cntFlag[17]= {0};
		for(i=0; i<n; i++) cntFlag[cards[i]]++;
	
		//ͳ�Ƶ����������ٵ���
		van maxCnt=0,minCnt=4;
		for(i=0; i<17; i++) {
			if(maxCnt<cntFlag[i]) maxCnt=cntFlag[i];
			if(cntFlag[i]&&minCnt>cntFlag[i]) minCnt=cntFlag[i];
		}
	
		if(n==4&&maxCnt==4) {init("ը��",1,4,0,0);return;}
		if(n==1) {init("����",2,1,0,0);return;}
		if(n==2&&maxCnt==2) {init("����",3,2,0,0);return;}
		if(n==3&&maxCnt==3) {init("���� ",4,3,0,0);return;}
		if(n==4&&maxCnt==3) {init("����һ",5,3,0,1);return;}
		if(n==5&&maxCnt==3&&minCnt==2) {init("����һ��",6,3,0,2);return;}
		if(n==6&&maxCnt==4) {init("�Ĵ���",7,4,0,1);return;}
		if(n==8&&maxCnt==4&&minCnt==2) {init("�Ĵ���",8,4,0,2);	return;}
		if(n>=5&&maxCnt==1&&cards[0]==cards[n-1]+n-1) {init("˳��",9,1,1,0);return;}
		if(n>=6&&maxCnt==2&&minCnt==2&&cards[0]==cards[n-1]+n/2-1) {init("����",10,2,1,0);return;}
	
		van fjCnt;//ͳ�������Ҵ���3���ŵ���
		for(i=0; i<17 &&cntFlag[i]<3; i++);
		for(fjCnt=0; i<17 &&cntFlag[i]>=3; i++,fjCnt++);
		if(fjCnt>1) {
			if(n==fjCnt*3) init("�ɻ�",11,3,1,0);
			else if(n==fjCnt*4)	init("�ɻ�",12,3,1,1);
			else if(n==fjCnt*5&&minCnt==2) init("�ɻ�",13,3,1,2);
		}
	}

	void arrange() {//�������򡢼�������
		sort(cards.begin(),cards.end());
		calType();
	}
	
	bool cmp(Cards &y) {
		if(y.Str=="��ը") return 0;
		else if(Str=="��ը") return 1;
		else if(Str=="ը��"&&y.Str=="ը��") return value()>y.value();
		else if(y.Str=="ը��") return 0;
		else if(Str=="ը��") return 1;
		else if(Id==y.Id) return value()>y.value();
		else return 0;
	}
	
	bool Init(vector<van> input, vector<van> Holds, Cards lst) {
		cards.clear();
		
		van i,j,visitFlag[20]={0};//����������û����Щ��
		for(i=0; i<input.size(); i++) {
			van find=0;
			for(j=0; j<Holds.size(); j++) {
				if(!visitFlag[j]&&Holds[j]==input[i]) {
					visitFlag[j]=1,find=Holds[j];
					break;
				}
			}
			if(find) cards.push_back(find);
			else {
				cout<<input[i]+2<<"û���ҵ�\t";
				cards.clear();
				return 0;
			}
		}
		arrange();
		
		if(lst.Id==-1) return 1;
		if(!cmp(lst)) return 0;
		return 1;
	}
	
	void Refresh(vector<van> &Holds){
		bool vis[20]={0};
		for(van i=0;i<cards.size();i++)
			for(van j=0;j<Holds.size();j++)
				if(cards[i]==Holds[j]&&!vis[j])
					{vis[j]=1;break;}
					
		vector<van> nxt;nxt.clear();
		for(van i=0;i<Holds.size();i++)
			if(!vis[i]) nxt.push_back(Holds[i]);
		Holds=nxt;
	}

};
#endif
