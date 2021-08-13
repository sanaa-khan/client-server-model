#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <string>
#include <string.h>
#include <cstring>

using namespace std;

#define PORT 5000 
#define MAXLINE 1000 

string charPtrToString(char* toConvert) {
	string str;
	for (int i = 0; toConvert[i]; i++)
		str += toConvert[i];

	return str;
}

string getRecord(string invoice) {
	ifstream in("Assignment 1.txt");
	string word, line, record;

	if (in) {
		// discard header lines
		getline(in, line);
		getline(in, line);

		while (!in.eof()) {
			getline(in, line);

			word = line.substr(0, line.find('\t'));

			if (word == invoice) {
				record += line;
				record += "\n";
			}
		}

		in.close();
		return record;
	}

	return "Record not found.";
}

void deleteRecords(string invoice) {
	ifstream in("Assignment 1.txt");
	string word, line, record;

	if (in) {
		// discard header lines
		getline(in, line);
		record += line;
		record += "\n";
		getline(in, line);
		record += line;
		record += "\n";

		while (!in.eof()) {
			getline(in, line);

			word = line.substr(0, line.find('\t'));

			if (word != invoice) {
				record += line;
				record += "\n";
			}
		}

		in.close();

		ofstream out("Assignment 1.txt");
		if (out) {
			out << record;
			out.close();
		}
	}
}

string getDate() {
	char date[30], day[2], month[2], year[4];
    string Date;

	time_t t = time(NULL);
    tm* tPtr = localtime(&t);

    Date = to_string((tPtr->tm_mday));
    Date.append("/");
    Date.append(to_string((tPtr->tm_mon)+1));
    Date.append("/");
    Date.append(to_string((tPtr->tm_year)+1900));

    return Date;
}

string getTime() {
    string Time;

    time_t t = time(NULL);
    tm* tPtr = localtime(&t);

    Time  = to_string(tPtr->tm_hour);
    Time.append(":");
    Time.append(to_string(tPtr->tm_min));
    Time.append(":");
    Time.append(to_string(tPtr->tm_sec));

    return Time;
}

void updateServerLog(string name) {
	ofstream out("Server Log.txt", ios::app);

	if (out) {
		out << name << "\t" << getDate() << "\t" << getTime() << endl;
		out.close();
	}
}

int main() 
{ 
	char buffer[100], msg[100] = "You are connected to the server.";
	
	int server_socket, len; 
	int clients = 3;
	string clientIDs[clients];
	struct sockaddr_in servaddr, clientArr[clients];
	bzero(&servaddr, sizeof(servaddr)); 

	// Create a UDP Socket 
	server_socket = socket(AF_INET, SOCK_DGRAM, 0);		 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_family = AF_INET; 

	// bind server address to socket descriptor 
	bind(server_socket, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
	
	cout << "Waiting for all " << clients << " clients to connect." << endl;

	for (int i = 0; i < clients; i++) {
		updateServerLog(clientIDs[i]);

		len = sizeof(clientArr[i]);
		recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientArr[i],(socklen_t*)&len); 
		clientIDs[i] = charPtrToString(buffer);
		clientIDs[i].erase(clientIDs[i].length() - 1);
		cout << "Connected: " << buffer;

		sendto(server_socket, msg, MAXLINE, 0, (struct sockaddr*)&clientArr[i], sizeof(clientArr[i]));
	}

	cout << "All clients are connected." << endl;

	ofstream out("Server Log.txt");
	if (out) {
		out << "Name\tDate\tTime\n";
		out.close();
	}

	while (1) {
		for (int i = 0; i < clients; i++) {

			updateServerLog(clientIDs[i]);

			int currentClient = i;
			int menuSelection;
			string signal;

			for (int j = 0; j < clients; j++) {
				if (j == currentClient)
					signal = "1";
				else
					signal = "2";

				sendto(server_socket, signal.c_str(), MAXLINE, 0, (struct sockaddr*)&clientArr[j], sizeof(clientArr[j]));
			}

			recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientArr[i],(socklen_t*)&len); 
			cout << "Option selected: " << buffer << endl;
			menuSelection = atoi(buffer);

			// read
			if (menuSelection == 1) {
				recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientArr[i],(socklen_t*)&len);

				string inv = charPtrToString(buffer);
				cout << "Selected invoice number: " << inv << endl;

				string record = getRecord(inv);
				sendto(server_socket, record.c_str(), MAXLINE, 0, (struct sockaddr*)&clientArr[i], sizeof(clientArr[i]));

				cout << "The specified records have been sent." << endl;

				for (int j = 0; j < clients; j++) {
					if (j != currentClient)
						sendto(server_socket, "0", MAXLINE, 0, (struct sockaddr*)&clientArr[j], sizeof(clientArr[j]));
				}
			}

			// delete
			else if (menuSelection == 2) {

				recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientArr[i],(socklen_t*)&len);

				string inv = charPtrToString(buffer);
				cout << "Selected invoice number: " << inv << endl;

				int permission = 1;

				for (int j = 0; j < clients; j++) {
					if (j != currentClient) {
						sendto(server_socket, "1", MAXLINE, 0, (struct sockaddr*)&clientArr[j], sizeof(clientArr[j]));

						string msg = "Client ";
						msg += clientIDs[i];
						msg += " wants to delete all records of invoice ";
						msg += inv;

						sendto(server_socket, msg.c_str(), MAXLINE, 0, (struct sockaddr*)&clientArr[j], sizeof(clientArr[j]));
						recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientArr[j],(socklen_t*)&len);

						permission *= atoi(buffer);
					}
				}

				if (permission) {
					cout << "All clients have authorised the deletion." << endl;

					for (int j = 0; j < clients; j++)
							sendto(server_socket, "Records have been deleted.", MAXLINE, 0, (struct sockaddr*)&clientArr[j], sizeof(clientArr[j]));

					deleteRecords(inv);
				}

				else {
					cout << "Deletion not authorised." << endl;
					sendto(server_socket, "NOT ALLOWED", MAXLINE, 0, (struct sockaddr*)&clientArr[i], sizeof(clientArr[i]));

					for (int j = 0; j < clients; j++) {
						if (j != currentClient)
							sendto(server_socket, "Records were not deleted.", MAXLINE, 0, (struct sockaddr*)&clientArr[j], sizeof(clientArr[j]));
					}
				}
				
			}

			// add
			else if (menuSelection == 3) {
				string newRecord;

				recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientArr[i],(socklen_t*)&len); // invoice
				newRecord = charPtrToString(buffer);
				newRecord += "\t";

				recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientArr[i],(socklen_t*)&len); // description
				newRecord += charPtrToString(buffer);
				newRecord += "\t";

				recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientArr[i],(socklen_t*)&len); // quantity
				newRecord += charPtrToString(buffer);
				newRecord += "\t";

				recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientArr[i],(socklen_t*)&len); // price
				newRecord += charPtrToString(buffer);
				newRecord += "\t";

				recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientArr[i],(socklen_t*)&len); // customer id
				newRecord += charPtrToString(buffer);
				newRecord += "\t";

				recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientArr[i],(socklen_t*)&len); // country
				newRecord += charPtrToString(buffer);
				newRecord += "\t";

				cout << "\nRecord to be added: " << newRecord << endl;

				ofstream out("Assignment 1.txt", ios::app);
				if (out) {
					out << newRecord << endl;
					out.close();
				}

				for (int j = 0; j < clients; j++) {
					if (j != currentClient)
						sendto(server_socket, "3", MAXLINE, 0, (struct sockaddr*)&clientArr[j], sizeof(clientArr[j]));
				}

				for (int j = 0; j < clients; j++) {
						sendto(server_socket, "A new record has been added.", MAXLINE, 0, (struct sockaddr*)&clientArr[j], sizeof(clientArr[j]));
				}
			}

			// access log
			else if (menuSelection == 4) {
				ifstream in("Server Log.txt");
				string file;
				char c;

				if (in) {
					while (!in.eof()) {
						c = in.get();
						file += c;
					}

					in.close();
				}

				sendto(server_socket, file.c_str(), MAXLINE, 0, (struct sockaddr*)&clientArr[i], sizeof(clientArr[i]));

				for (int j = 0; j < clients; j++) {
					if (j != currentClient)
						sendto(server_socket, "0", MAXLINE, 0, (struct sockaddr*)&clientArr[j], sizeof(clientArr[j]));
				}
			}
		}

		int choice;
		cout << "Press 0 to exit the program or 1 to continue: ";
		cin >> choice;

		if (choice == 0) {
			for (int j = 0; j < clients; j++) {
				sendto(server_socket, "3", MAXLINE, 0, (struct sockaddr*)&clientArr[j], sizeof(clientArr[j]));
			}

			break;
		}
	}
		
	close(server_socket);
} 
