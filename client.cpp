#include <stdio.h> 
#include <string.h> 
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <iostream>

using namespace std;

#define PORT 5000 
#define MAXLINE 1000 

int main() 
{ 
	char serverMsg[500];
	char message[100]; 
	int server_socket, n; 
	struct sockaddr_in servaddr; 
	
	// clear servaddr 
	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_family = AF_INET; 
	
	// create datagram socket 
	server_socket = socket(AF_INET, SOCK_DGRAM, 0); 
	
	// connect to server 
	if(connect(server_socket, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) 
	{ 
		printf("\n Error : Connect Failed \n"); 
		exit(0); 
	} 
	
	cout << "Enter your ID: ";
	fgets(message, 100, stdin);
	sendto(server_socket, message, MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
	n = sizeof(servaddr);
	
	// waiting for response 
	recvfrom(server_socket, serverMsg, sizeof(serverMsg), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n); 
	puts(serverMsg);

	while (1) {
		recvfrom(server_socket, serverMsg, sizeof(serverMsg), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n);

		int status1 = atoi(serverMsg);
		//cout << status1 << endl;

		if (status1 == 1) {
			string menuOption; 

			cout << "1. Reading a record\n2. Delete a record\n3. Add new record\n4. Request log of server access." << endl;
			cout << "Select an option: ";
			cin >> menuOption;
			int c = stoi(menuOption);

			sendto(server_socket, menuOption.c_str(), MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr)); 

			if (c == 1) {
				string inv;
				cout << "Enter invoice number: ";
				cin >> inv;

				sendto(server_socket, inv.c_str(), MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
				recvfrom(server_socket, serverMsg, sizeof(serverMsg), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n);

				cout << "Records obtained:\n" << serverMsg << endl;
			}

			else if (c == 2) {
				string inv;
				cout << "Enter invoice number: ";
				cin >> inv;

				sendto(server_socket, inv.c_str(), MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
				recvfrom(server_socket, serverMsg, sizeof(serverMsg), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n);

				cout << serverMsg << endl;
			}

			else if (c == 3) {
				string msg;

				cout << "Enter details of the new record:\n";

				cout << "Invoice no: ";
				cin >> msg;
				sendto(server_socket, msg.c_str(), MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr)); 

				cin.ignore();

				cout << "Description: ";
				getline(cin, msg);
				sendto(server_socket, msg.c_str(), MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr)); 

				cout << "Quantity: ";
				cin >> msg;
				sendto(server_socket, msg.c_str(), MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));

				cout << "Price in $: ";
				cin >> msg;
				sendto(server_socket, msg.c_str(), MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr)); 

				cout << "Customer id: ";
				cin >> msg;
				sendto(server_socket, msg.c_str(), MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr)); 

				cout << "Country: ";
				cin >> msg;
				sendto(server_socket, msg.c_str(), MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr)); 

				recvfrom(server_socket, serverMsg, sizeof(serverMsg), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n);
				cout << serverMsg << endl;
			}

			else if (c == 4) {
				cout << "\nLog of server access:\n";
				recvfrom(server_socket, serverMsg, sizeof(serverMsg), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n);
				cout << serverMsg << endl;
			}
		}

		else if (status1 == 2) {

			recvfrom(server_socket, serverMsg, sizeof(serverMsg), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n);
			int status2 = atoi(serverMsg);

			// don't do anything
			if (status2 == 0) {
				continue;
			}

			// authorise for delete
			else if (status2 == 1) {
				recvfrom(server_socket, serverMsg, sizeof(serverMsg), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n);
				cout << serverMsg << endl;

				string del;
				cout << "Do you authorise this deletion? Press 1 for yes and 0 for no: ";
				cin >> del;

				sendto(server_socket, del.c_str(), MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));

				recvfrom(server_socket, serverMsg, sizeof(serverMsg), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n);
				cout << serverMsg << endl;
			}

			// recieve update message
			else if (status2 == 3) {
				recvfrom(server_socket, serverMsg, sizeof(serverMsg), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n);
				cout << serverMsg << endl;
			}
		}

		if (status1 == 3) {
			cout << "Client exiting." << endl;
			break;
		}
	}

	// close the descriptor 
	close(server_socket);
}