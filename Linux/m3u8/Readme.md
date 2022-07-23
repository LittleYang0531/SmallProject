# M3U8 Downloader

This small project can be run in any operator system.

System Requirement: 

- For Windows: 
  You need to compile `libcurl` which support `OpenSSL`, and perpare `MinGW` which support `C++17`
- For Linux: 
  You only need to install `libcurl`.
- For MacOS: 
  I don't have a MacOS device!

Compile Command: 

1. For Windows: `g++ main.cpp -o main.exe -std=c++17 -lpthread -lcurl`

2. For Linux: `g++ main.cpp -o main -std=c++17 -lpthread -lcurl`

Usage: 

1. For Windows: `./m3u8.exe [url] [file] [thread]`

2. For Linux: `./m3u8 [url] [file] [thread]`

Author: 

1. `main.cpp`: [@LittleYang0531](https://www.luogu.com.cn/user/185758)