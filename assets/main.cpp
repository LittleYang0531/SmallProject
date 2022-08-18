#include<bits/stdc++.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<curl/curl.h>
#include<jsoncpp/json/json.h>
#include<pthread.h>
using namespace std;

const string list_url = "https://download.mcbbs.net/mc/game/version_manifest.json";
const string asset_url = "http://resources.download.minecraft.net/";

typedef Json::Value Array;
string json_encode(Array val) {
    Json::FastWriter writer;
    return writer.write(val);
}
string json_pretty_encode(Array val) {
    Json::StyledWriter writer;
    return writer.write(val);
}
Array json_decode(string json) {
    Json::Reader reader;
    Array val;
	if (!reader.parse(json, val, false))
        cerr << "Invalid JSON String!" << endl;
    return val;
}
string readFile(string path) {
    ifstream t;
    t.open(path.c_str());
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

struct node {
    string p = "";
    string h = "";
    int s = 0;
    node(string p, string h, int s): p(p), h(h), s(s){};
};
vector<node> q;
static pthread_mutex_t g_mutex_lock;
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
int filesize(string path) {
    ifstream fin(path.c_str());
	if(fin.is_open()) {
		fin.seekg(0, ios::end);
		int size = fin.tellg();
		fin.close();
        return size;
	} return 0;
}
time_t clock2() {
	return chrono::duration_cast<chrono::milliseconds>
	(chrono::system_clock::now().time_since_epoch()).count();
}
pthread_t pt[1010];
int thread_num = 0;
long long size_now = 0;
long long size_sum = 0;
long long finished = 0;
int getid() {
    pthread_mutex_lock(&g_mutex_lock);
    int res=++thread_num; 
    pthread_mutex_unlock(&g_mutex_lock);
    return res;
}
int now = 0;
int getnum() {
    pthread_mutex_lock(&g_mutex_lock);
    int res=now++; if (res >= q.size()) res = -1; 
    pthread_mutex_unlock(&g_mutex_lock);
    return res;
}
string repair_num(int x,int len) {
    int beflen=to_string(x).size();
    string res=""; 
    for (int i=beflen+1;i<=len;i++) res+="0";
    res+=to_string(x); return res;
}
void output(int thread_id,string info) {
    pthread_mutex_lock(&g_mutex_lock);
    cout<<"[Thread #"<<repair_num(thread_id,to_string(thread_num).size())<<"] "<<info<<endl;
    pthread_mutex_unlock(&g_mutex_lock);
}
long long add_sum(long long siz) {
    pthread_mutex_lock(&g_mutex_lock);
    size_now += siz; long long ret = size_now;
    pthread_mutex_unlock(&g_mutex_lock);
    return ret;
}
int finished_num() {
    pthread_mutex_lock(&g_mutex_lock);
    int res=++finished;
    pthread_mutex_unlock(&g_mutex_lock);
    return res;
}
void* work_thread(void* arg) {
    int thread_id = getid();
    int work_id = getnum(); 
    while (work_id != -1) {
        node x = q[work_id];
        bool cached = false;
        if (filesize("./assets/" + x.h.substr(0, 2) + "/" + x.h) != x.s) {
            mkdir(("./assets/" + x.h.substr(0, 2)).c_str(), 0777);
            curl_get_download(asset_url + x.h.substr(0, 2) + "/" + x.h, "./assets/" + x.h.substr(0, 2) + "/" + x.h);
        } else cached = true;
        stringstream buffer;
        buffer << "[" << repair_num(finished_num(), to_string(q.size()).size()) << "/" << q.size() << "] " 
               << "[" << fixed << setprecision(2) << add_sum(x.s) / 1024.0 / 1024.0 << "MB/" << size_sum / 1024.0 / 1024.0 << "MB] "
               << "Downloaded " << x.p << (cached ? " [Cached]" : "");
        output(thread_id, buffer.str());
        work_id = getnum();
    }
    return (void*)NULL;
}
int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " [id]" << endl;
        return 3;
    }
    cout << "[Main Thread] Downloading version_manifest.json..." << endl;
    curl_get_download(list_url, "./version_manifest.json");
    string json = readFile("./version_manifest.json");
    Array arr = json_decode(json)["versions"];

    int goal_id = -1;
    for (int i = 0; i < arr.size(); i++) 
        if (arr[i]["id"].asString() == argv[1]) goal_id = i;
    if (goal_id == -1) {
        cout << "[Main Thread] Invalid version id!" << endl;
        return 3;
    }

    cout << "[Main Thread] Downloading " << arr[goal_id]["id"].asString() << ".json..." << endl;
    curl_get_download(arr[goal_id]["url"].asString(), "./" + arr[goal_id]["id"].asString() + ".json");
    json = readFile("./" + arr[goal_id]["id"].asString() + ".json");
    arr = json_decode(json);
    string url = arr["assetIndex"]["url"].asString();
    string id = arr["assetIndex"]["id"].asString();

    cout << "[Main Thread] Downloading " << id << ".json..." << endl;
    curl_get_download(url, "./" + id + ".json");
    json = readFile("./" + id + ".json");
    arr = json_decode(json)["objects"];

    mkdir("./assets", 0777);
    auto mem = arr.getMemberNames();
    for (auto key : mem) {
        q.push_back(node(key, arr[key]["hash"].asString(), arr[key]["size"].asInt()));
        size_sum += arr[key]["size"].asInt();
    }
    for (int i = 1; i <= 32; i++)
        pthread_create(&pt[i], NULL, work_thread, NULL);
    for (int i = 1; i <= 32; i++) 
        pthread_join(pt[i], NULL);

    int failed = 0;
    for (int i = 0; i < q.size(); i++) {
        node x = q[i];
        string path = "./assets/" + x.h.substr(0, 2) + "/" + x.h;
        if (filesize(path) != x.s) failed++;
    } 
    cout << "Downloaded " << q.size() << " files." << endl;
    cout << "Have " << failed << " was wrong!" << endl;
}