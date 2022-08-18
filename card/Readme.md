# Poker
System Requirement: none

Compile Command: 
- For Windows: 
  1. Server Platform: `g++ server.cpp -o server.exe -lwsock32`
  2. Client Platform: `g++ client.cpp -o client.exe -lwsock32`
- For Linux: 
  1. Server Platform: `g++ server.cpp -o server`
  2. Client Platform: `g++ client.cpp -o client`

Usage: 
1. Server Platform: `./server(.exe) [ host ] [ ip ]`
2. Client Platform: `./client(.exe)`

Author: 
1. `socket.h`/`socket-linux.h`: [@LittleYang0531](https://github.com/LittleYang0531)
2. `poker.h`: [@Figo](https://www.luogu.com.cn/user/230141)
3. `server.cpp` & `client.cpp`: [@LittleYang0531](https://github.com/LittleYang0531)

Thanks [@Figo](https://www.luogu.com.cn/user/230141) for provide support for me!