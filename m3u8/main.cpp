#include<bits/stdc++.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<curl/curl.h>
#include<pthread.h>
#include<filesystem>
using namespace std;
using namespace filesystem;
typedef long long van;
template<typename T> inline
void read(T& x) {
    T f=1,b=0; char ch=getchar();
    while (!isdigit(ch)) {
        if (ch=='-') f=-1;
        ch=getchar();
    } while (isdigit(ch))
        b*=10,b+=ch-'0',ch=getchar();
    x=f*b; return;
}
template<typename T> inline
void print(T x) {
    if (x<0) putchar('-'),x=-x;
    if (x==0) return putchar('0'),void();
    van st[51]={0},k=0;
    while (x) st[++k]=x%10,x/=10;
    for (int i=k;i;i--) putchar(st[i]+'0');
} 
size_t dl_req_reply(void *buffer, size_t size, size_t nmemb, void *user_p) {
	FILE *fp=(FILE*)user_p;
	size_t return_size=fwrite(buffer,size,nmemb,fp);
	return return_size;
}
void curl_get_download(const std::string &url, std::string filename) {
	const char* file_name=filename.c_str();
	char* pc=new char[1024];
	strcpy(pc,file_name);
	FILE *fp=fopen(pc,"wb");
	CURL *curl=curl_easy_init();
	CURLcode res;
	if (curl) {
		struct curl_slist* header_list=NULL;
		header_list=curl_slist_append(header_list,"User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko");
		curl_easy_setopt(curl,CURLOPT_HTTPHEADER,header_list);
		curl_easy_setopt(curl,CURLOPT_HEADER,0);
		curl_easy_setopt(curl,CURLOPT_URL,url.c_str());
		curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER,false);
		curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST,false);
		curl_easy_setopt(curl,CURLOPT_VERBOSE,0);
		curl_easy_setopt(curl,CURLOPT_READFUNCTION,NULL);
		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,&dl_req_reply);
		curl_easy_setopt(curl,CURLOPT_WRITEDATA,fp);
		curl_easy_setopt(curl,CURLOPT_NOSIGNAL,1);
		curl_easy_setopt(curl,CURLOPT_CONNECTTIMEOUT,30);
		curl_easy_setopt(curl,CURLOPT_TIMEOUT,30);
		res=curl_easy_perform(curl);
	} curl_easy_cleanup(curl);
	fclose(fp);
}
bool isFileExists_stat(string name) {
    struct stat buffer;   
    return (stat(name.c_str(), &buffer) == 0); 
}
time_t clock2() {
	return chrono::duration_cast<chrono::milliseconds>
	(chrono::system_clock::now().time_since_epoch()).count();
}
vector<string> url;
static pthread_mutex_t g_mutex_lock;
int now=0,used=0,failed=0; double sum=0;
int thread_num=0; double sumt=0,sumt_merge=0;
pthread_t pt[1010];
string repair_num(int x,int len) {
    int beflen=to_string(x).size();
    string res=""; 
    for (int i=beflen+1;i<=len;i++) res+="0";
    res+=to_string(x); return res;
}
struct dsu {
    van fa[100100];
    void init(){for (int i=0;i<=1e5;i++) fa[i]=i;}
    van getfa(van x){return fa[x]==x?x:fa[x]=getfa(fa[x]);}
    void merge(van a,van b){a=getfa(a),b=getfa(b);fa[a]=b;}
}S;
int getid() {
    pthread_mutex_lock(&g_mutex_lock);
    int res=now++; if (res>=url.size()) res=-1;
    pthread_mutex_unlock(&g_mutex_lock);
    return res;
}
int getnum() {
    pthread_mutex_lock(&g_mutex_lock);
    used++;
    pthread_mutex_unlock(&g_mutex_lock);
    return used;
}
void output(int thread_id,string info) {
    pthread_mutex_lock(&g_mutex_lock);
    cout<<"[Thread #"<<repair_num(thread_id,to_string(thread_num).size())<<"] "<<info<<endl;
    pthread_mutex_unlock(&g_mutex_lock);
}
void* download_thread(void* arg) {
    int thread_id=*(int*)arg;
    int id=0; while(1) {
        id=getid(); if (id==-1) pthread_exit(0);
        string x=url[id];
        double st=clock2();
        curl_get_download(x,"/tmp/curl/"+to_string(id+1)+".ts");
        double t=clock2()-st; t/=1000.0; sumt+=t;
        double siz=file_size(("/tmp/curl/"+to_string(id+1)+".ts").c_str());
        siz/=1024.0; siz/=1024.0;
        siz*=100; siz=round(siz); siz/=100.0; sum+=siz;
        if (siz<=1e-10) failed++;
        double speed=t<=1e-10?0:siz/t;
        speed*=100; speed=round(speed); speed/=100.0;
        stringstream buffer;
        buffer<<"["<<repair_num(getnum(),to_string(url.size()).size())<<"/"<<url.size()<<"] "<<"Downloading Video Part "<<repair_num(id+1,to_string(url.size()).size())<<", "<<siz<<"MB|"<<speed<<"MB/s";
        output(thread_id,buffer.str()); S.merge(id,id+1);
    } return (void*)NULL;
}
string save_to="";
void* merge_thread(void* arg) {
    int thread_id=*(int*)arg;
    int i=1; ofstream fout(save_to.c_str(), ios::binary); double now=0;
    while (i<=url.size()) {
        van to=S.getfa(0);
        for (i;i<=to;i++) {
            double siz=file_size(("/tmp/curl/"+to_string(i)+".ts").c_str());
            siz/=1024.0; siz/=1024.0;
            siz*=100; siz=round(siz); siz/=100.0; now+=siz;
            double st=clock2();
            ifstream in(("/tmp/curl/"+to_string(i)+".ts").c_str(), ios::binary);
            stringstream buffer,buffer2;
            buffer<<in.rdbuf();
            fout<<buffer.str();
            double t=clock2()-st; t/=1000.0; sumt_merge+=t;
            double speed=t<=1e-10?0:siz/t;
            speed*=100; speed=round(speed); speed/=100.0;
            buffer2<<"["<<repair_num(i,to_string(url.size()).size())<<"/"<<url.size()<<"] "<<"Merging data... ";
            buffer2<<now<<"MB/"<<sum<<"MB|"<<speed<<"MB/s";
            output(thread_id,buffer2.str());
            in.close();
        }
    } fout.close();
    return (void*)NULL;
}
string getpath(const char* path) {
	char res[100010]="";
	#ifdef __linux__
	if(realpath(path,res)) return res;
	#elif _WIN32
	if (_fullpath(res,path,100010)) return res;
	#endif
	else return "/";
}
int main(int argc,char** argv) {
    if (argc<4) {
        cout<<"Usage: "<<argv[0]<<" [url] [file] [thread]"<<endl;
        return 0;
    } save_to=argv[2]; thread_num=atoi(argv[3])+1;
    if (!isFileExists_stat("/tmp")) {
        #ifdef __linux__
            mkdir("/tmp",0777);
        #else 
            mkdir("/tmp");
        #endif
    }
    if (isFileExists_stat("/tmp/curl")) {
        cout<<"Clearing Cache Data..."<<endl;
        system("rm /tmp/curl -r");
    } 
    #ifdef __linux__
        mkdir("/tmp/curl",0777);
    #else 
        mkdir("/tmp/curl");
    #endif
    cout<<"Downloading index.m3u8..."<<endl;
    curl_get_download(argv[1],"/tmp/curl/index.m3u8");
    ifstream fin("/tmp/curl/index.m3u8"); S.init();
    while (!fin.eof()) {
        string x; getline(fin,x);
        if (x=="") continue;
        if (x[0]=='#') continue;
        url.push_back(x);
    } fin.close(); curl_global_init(CURL_GLOBAL_ALL);
    for (int i=1;i<=atoi(argv[3]);i++) {
        pthread_create(&pt[i],NULL,download_thread,&i);
        usleep(10000);
    } int merge_id=atoi(argv[3])+1;
    for (int i=1;i<=atoi(argv[3]);i++) {
        pthread_join(pt[i],(void**)NULL);
    } pthread_create(&pt[merge_id],NULL,merge_thread,&merge_id);
    pthread_join(pt[merge_id],(void**)NULL);
    cout<<"---------------------"<<endl;
    cout<<"Download Finished!"<<endl;
    cout<<"Video have saved to "<<getpath(argv[2])<<endl;
    cout<<"Have "<<failed<<" video part cannot download"<<endl;
    cout<<"---------------------"<<endl;
}