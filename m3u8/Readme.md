# M3U8 Downloader

Download Link => [Click Me](https://github.com/LittleYang0531/SmallProject)

This small project can be run in any operator system.

New Features:

1. Add anime website source "omofun.tv(dead)" and "malimali6.com" | Usage: php search.php [keywords]

System Requirement: 

- For Windows: 
  You need to compile `libcurl` which support `OpenSSL`, and perpare `MinGW` which support `C++17`
- For Linux: 
  You only need to install `libcurl`.
- For MacOS: 
  I don't have a MacOS device!

Compile Command: 

1. For Windows: `g++ main.cpp -o main.exe -std=c++17 -lpthread -lcurl -lcurldll`
2. For Linux: `g++ main.cpp -o main -std=c++17 -lpthread -lcurl`

Usage: 

1. For Windows: `./main.exe [url] [file] [thread]`
2. For Linux: `./main [url] [file] [thread]`

Author: 

1. `main.cpp`: [@LittleYang0531](https://github.com/LittleYang0531)
