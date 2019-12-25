#include "pch.h"
#include <iostream>
#include <winsock2.h>
#include <string>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable:4996) 

void serverSendMess(char name[]);
void serverRecvMess(SOCKET a);
void clientSendMess(SOCKET a, char name[]);
void clientRecvMess(SOCKET a);
void responeServer(SOCKET client, char name[]);
void CreateSever(SOCKET a);
void CreateClient(SOCKET a, char name[]);
void Server(char name[]);
void Client(char name[]);

int list[5] = { 0,0,0,0,0 };
SOCKET  client[5];

int main() {
	std::cout << "nhap ten cua ban:";
	char name[1024];
	std::cin.getline(name, 1024);
	std::cout << "1.tao phong chat\n2.tham gia phong chat\n";
	std::string choice;
	while (true) {
		std::cin >> choice;
		if ((choice.compare("1") == 0) || (choice.compare("2") == 0)) {
			break;
		}
		else {
			std::cout << "khong ton tai lua chon, moi nhap lai";
		}
	}
	if (choice.compare("1") == 0) {
		Server(name);
	}
	if (choice.compare("2") == 0) {
		Client(name);
	}
}
void Server(char name[]) {
	WSADATA WSAData;

	SOCKET server;

	SOCKADDR_IN clientAddr;

	WSAStartup(MAKEWORD(2, 0), &WSAData);

	server = socket(AF_INET, SOCK_STREAM, 0);

	CreateSever(server);

	int clientAddrSize = sizeof(clientAddr);
	std::thread recvList[5];
	std::thread sendThread(serverSendMess, name);
	for (int i = 0; i < 5; i++) {
		if (list[i] == 0) {
			client[i] = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize);
		}
		if (client[i] != INVALID_SOCKET) {
			recvList[i] = std::thread(serverRecvMess, client[i]);
			list[i] = 1;
		}
	}

	for (int i = 0; i < 5; i++) {
		if (list[i] == 1) {
			recvList[i].join();
		}
	}
	sendThread.join();
}

void Client(char name[]) {
	WSADATA WSAData;

	SOCKET server, client;

	WSAStartup(MAKEWORD(2, 0), &WSAData);

	server = socket(AF_INET, SOCK_STREAM, 0);

	CreateClient(server, name);

	std::thread rC1(responeServer, server, name);
	rC1.join();
}

void responeServer(SOCKET client, char name[]) {
	char buffer[1024];

	std::thread sendThread(clientSendMess, client, name);
	std::thread recvThread(clientRecvMess, client);

	sendThread.join();
	recvThread.join();

	closesocket(client);
	std::cout << "Client disconnected." << std::endl;

}
void CreateSever(SOCKET a) {
	SOCKADDR_IN serverAddr;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5555);
	bind(a, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	listen(a, 0);

	std::cout << "Listening for incoming connections..." << std::endl;
}
void CreateClient(SOCKET a, char name[]) {
	SOCKADDR_IN serverAddr;
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5555);

	connect(a, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	std::cout << "Connected to server!" << std::endl;

	char buffer[1024];
	strcpy(buffer, name);
	strcat(buffer, " has joined the chat !");
	send(a, buffer, sizeof(buffer), 0);
	std::cout << "you has joined the chat !" << std::endl;
}

void serverSendMess(char name[]) {
	char buffer[1024];
	char line[1024];

	while (true) {
		std::cin.getline(line, 1024);
		strcat(name, ":");
		strcpy(buffer, strcat(name, line));
		for (int i = 0; i < 2; i++) {
			if (list[i] == 1) {
				send(client[i], buffer, sizeof(buffer), 0);
			}
		}
		name = strtok(name, ":");
		memset(line, 0, sizeof(buffer));
	}
}
void serverRecvMess(SOCKET a) {
	char buffer[1024];
	while (true) {
		memset(buffer, 0, sizeof(buffer));
		if (recv(a, buffer, sizeof(buffer), 0) != -1) {
			std::cout << buffer << std::endl;
			for (int i = 0; i < 5; i++) {
				if (a != client[i] && client[i] != INVALID_SOCKET) {
					send(client[i], buffer, sizeof(buffer), 0);
				}
			}
		}
		else {
			closesocket(a);
			strcat(buffer, "a client has disconected");
			std::cout << buffer << std::endl;
			break;
		}
	}
}

void clientSendMess(SOCKET a, char name[]) {
	char buffer[1024];
	char line[1024];
	while (true) {
		std::cin.getline(line, 1024);
		strcat(name, ":");
		strcpy(buffer, strcat(name, line));
		send(a, buffer, sizeof(buffer), 0);
		name = strtok(name, ":");
		memset(buffer, 0, sizeof(buffer));
	}
}
void clientRecvMess(SOCKET a) {
	char buffer[1024];
	while (true) {
		memset(buffer, 0, sizeof(buffer));
		if (recv(a, buffer, sizeof(buffer), 0) != -1) {
			std::cout << buffer << std::endl;
		}
		else {
			closesocket(a);
			strcat(buffer, "server has disconected");
			std::cout << buffer << std::endl;
			break;
		}
	}
}