# Minecraft Assets Downloader

This small project can be run in any operator system.

System Requirement: 

- For Windows: 
  You need to compile `libcurl` which support `OpenSSL`, and perpare `MinGW` which support `C++17`.
  You also need to compile `libjsoncpp`.
- For Linux: 
  You only need to install `libcurl` & `libjsoncpp-dev`.
- For MacOS: 
  I don't have a MacOS device!

Compile Command: 

1. For Windows: `g++ main.cpp -o main.exe -std=c++17 -lpthread -lcurl -ljson`
2. For Linux: `g++ main.cpp -o main -std=c++17 -lpthread -lcurl -ljsoncpp`

Usage: 

1. For Windows: `./main.exe [id]`
2. For Linux: `./main [id]`

Author: 

1. `main.cpp`: [@LittleYang0531](https://github.com/LittleYang0531)