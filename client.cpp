#include <iostream>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#define MAX_LEN 200
#define NUM_COLORS 6

bool exit_flag = false;
std::string def_col="\033[0m";
std::string colors[]={"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m", "\033[36m"};

void send_message(int client_socket);
void recv_message(int client_socket);

int main() {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Connect to localhost

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed\n";
        return 1;
    }
    char name[MAX_LEN];
    std::cout<<"enter your name: ";
    std::cin.getline(name,MAX_LEN);
    send(client_socket,name,strlen(name),0);

    std::thread t1(send_message, client_socket); 
    std::thread t2(recv_message, client_socket);
    

	if(t1.joinable())
		t1.join();
	if(t2.joinable())
		t2.join();
        
    return 0;
}

void eraseText(int cnt)
{
	char back_space=8;
	for(int i=0; i<cnt; i++)
	{
		std::cout<<back_space;
	}	
}

void send_message(int client_socket){
    char str[MAX_LEN];

    while(true){
        std::cout<<colors[1]<<"You: "<<def_col;
        std::cin.getline(str,MAX_LEN);
        send(client_socket,str,strlen(str),0);

        if(strcmp(str,"#exit") == 0){
            exit_flag = true;
            close(client_socket);
            return;
        }
    }
}

void recv_message(int client_socket){
    
    while(true)
    {   
        if(exit_flag)
            return;
        char name[MAX_LEN], str[MAX_LEN];
        int color_code;
        int byte_recieved = recv(client_socket,name,sizeof(name),0);
        if(byte_recieved <= 0)
            continue;

        recv(client_socket,&color_code,sizeof(color_code),0);
        recv(client_socket,str,sizeof(str),0);
        eraseText(6);

        if(strcmp(name, "#NULL") == 0){
            std::cout<<colors[color_code]<<str<<def_col<<std::endl;
        }else{
            std::cout<<colors[color_code]<<name<<": "<<def_col<<str<<std::endl;
        }

        std::cout<<colors[1]<<"You: "<<def_col;
		fflush(stdout);
        
    }
}
