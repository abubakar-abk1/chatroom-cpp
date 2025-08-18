# C++ Chat Application 💬

This is a lightweight, multithreaded chat application built in **C++**.  
It supports both **Linux (POSIX sockets)** and **Windows (Winsock2)**, allowing multiple clients to connect to a server and exchange messages in real time.

## ✨ Features
- 🖧 **Client-Server architecture** using TCP sockets
- 🎨 **Colored messages** for better readability
- 🔄 **Multithreaded**: separate threads for sending and receiving
- 📡 Works on **Linux** (`<sys/socket.h>`) and **Windows** (`<winsock2.h>`)
- 👥 Supports multiple clients simultaneously
- `#exit` command to disconnect gracefully

## 🛠️ How to Run

### On Linux:
1. Run the following:
```bash
g++ server.cpp -o server -pthread
g++ client.cpp -o client -pthread
```
2. Open a terminal and run the following(server):
```bash
./server
```
4. For client open another terminal and run the following:
```bash
./client
```
5. for multiple users repeat step 4.

### On Windows:

- Compile with MinGW or MSVC:
```
g++ client.cpp -o client.exe -lws2_32
```


## 🚀 Future Improvements
- 🔐 User authentication (login system)  
- 💬 Private messages (DMs)  
- 📂 File sharing  
- 🖥️ GUI version (Qt / ImGui)

  
<img width="1366" height="768" alt="image" src="https://github.com/user-attachments/assets/d8018f5d-432a-40e5-b9ac-9a8d53a91b40" />


  
