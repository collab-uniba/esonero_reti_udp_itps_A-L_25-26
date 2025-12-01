/*
 * main.c
 *
 * UDP Server - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a UDP server
 * portable across Windows, Linux and macOS.
 */

#if defined WIN32
#include <winsock.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <stdio.h>
#include <stdlib.h>
#include "protocol.h"

#define NO_ERROR 0

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

int main(int argc, char *argv[]) {

	// TODO: Implement server logic

#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif

	int my_socket;

	// TODO: Create UDP socket
	// my_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// TODO: Configure server address
	// struct sockaddr_in server_addr;
	// memset(&server_addr, 0, sizeof(server_addr));
	// server_addr.sin_family = AF_INET;
	// server_addr.sin_port = htons(SERVER_PORT);
	// server_addr.sin_addr.s_addr = INADDR_ANY;

	// TODO: Bind socket
	// bind(my_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

	// TODO: Implement datagram reception loop (no listen/accept for UDP)
	// struct sockaddr_in client_addr;
	// int client_addr_len = sizeof(client_addr);
	// while (1) {
	//     // Receive datagram from client
	//     int bytes_received = recvfrom(my_socket, buffer, BUFFER_SIZE, 0,
	//                                   (struct sockaddr*)&client_addr, &client_addr_len);
	//     // Process and send response
	//     sendto(my_socket, response, strlen(response), 0,
	//            (struct sockaddr*)&client_addr, client_addr_len);
	// }

	printf("Server terminated.\n");

	closesocket(my_socket);
	clearwinsock();
	return 0;
} // main end
