#include <iostream>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <mutex>
#define MAX_LEN 200
#define NUM_COLORS 6

struct terminal{
    int id;
    std::string name;
    int socket;
    std::thread th;
};

std::mutex cout_mutex,clients_mtx; 
std::vector<terminal> clients;
std::string def_col="\033[0m";
std::string colors[]={"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m","\033[36m"};
int seed = 0;

void handle_client(int client_fd, int id);
void shared_print(std::string str, bool endLine);
void broadcast_message(const std::string& name, int sender_id, const std::string& msg);
void set_name(char name[], int id);

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Socket creation failed.\n";
        return 1;
    }
    std::cout << "Socket created successfully.\n";

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    server_addr.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Binding failed.\n";
        close(server_fd);
        return 1;
    }
    std::cout << "Binding successful.\n";

    if (listen(server_fd, 5) < 0) {
        std::cerr << "Listen failed.\n";
        close(server_fd);
        return 1;
    }
    std::cout<<colors[NUM_COLORS-1]<<"\n\t  ====== Welcome to the chat-room ======   "<<std::endl<<def_col;

    while (true) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            std::cerr << "Accept failed.\n";
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "New client connected.\n";

        }
        seed++;
        std::lock_guard<std::mutex> lock(clients_mtx);
        std::thread t(handle_client, client_fd, seed);
        clients.push_back({seed,std::string("Anonymous"),client_fd, (move(t))});
    }

    for(int i=0; i<clients.size(); i++)
	{
		if(clients[i].th.joinable())
			clients[i].th.join();
	}

    close(server_fd);
    return 0;
}

void handle_client(int client_fd, int id) {
    char str[MAX_LEN], name[MAX_LEN];
    memset(str, 0, sizeof(str));

    recv(client_fd, name, sizeof(name), 0);
    set_name(name, id);

    std::string welcome_message = std::string(name)+std::string(" has joined");
    std::cout<<colors[id]<<welcome_message<<def_col<<std::endl;
    broadcast_message(std::string("#NULL"),id,std::string(welcome_message));
    shared_print(colors[id]+welcome_message+def_col,true);

    while (true) {

        memset(str, 0, sizeof(str));
        ssize_t bytes_received = recv(client_fd, str, sizeof(str) - 1, 0);

        if (strcmp(str, "#exit") == 0 || bytes_received <= 0) {
            std::string leave_msg = std::string(name) + " has left";
            std::cout<<colors[id]<<leave_msg<<def_col<<std::endl;
            broadcast_message(std::string("#NULL"),id,std::string(leave_msg));
            shared_print(colors[id]+welcome_message+def_col,true);

            close(client_fd);

            // Remove from clients vector
            {
                std::lock_guard<std::mutex> lock(clients_mtx);
                for (auto it = clients.begin(); it != clients.end(); ++it) {
                    if (it->id == id) {
                        if (it->th.joinable()) it->th.detach(); 
                        clients.erase(it);
                        break;
                    }
                }
            }
            return; // Exit the thread
        }


        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout <<colors[id]<< name << ": " << str <<def_col<< "\n";
        }
        
        broadcast_message(std::string(name),id,std::string(str));
        shared_print(colors[id]+welcome_message+def_col,true);
    }
    close(client_fd);
}

// Example structure: [name][color_code][message]
void broadcast_message(const std::string& name, int sender_id, const std::string& msg) {
    for (auto &c : clients) {
        if (c.id != sender_id) {
            send(c.socket, name.c_str(), MAX_LEN, 0);   // fixed size
            int color_code = sender_id % NUM_COLORS;
            send(c.socket, &color_code, sizeof(color_code), 0);
            send(c.socket, msg.c_str(), MAX_LEN, 0);
        }
    }
}


void set_name(char name[], int id) {
    std::lock_guard<std::mutex> lock(clients_mtx);
    for (int i = 0; i < clients.size(); i++) {
        if (clients[i].id == id) {
            clients[i].name = name;
        }
    }
}

void shared_print(std::string str, bool endLine)
{	
	std::lock_guard<std::mutex> guard(cout_mutex);
	std::cout<<str;
	if(endLine)
			std::cout<<std::endl;
}
