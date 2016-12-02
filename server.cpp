//Name: M.Ranjana
//Roll No : 20162014

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string>
#include <map>
#include <vector>
#include <utility>
#include <ctime>
using namespace std;

int handleSearchRequest(int newsockfd, char* cmds) {
	char num[10] ;
	int index = 0;
	string str = "";
	string str1(cmds);
	vector<pair<string,string > > v;
	map<string,vector<pair<string,string> > > repoData;
	
	char buffer[100], *fileName, *path, *ip;
	FILE *repoP, *logP;
	repoP = fopen("repo.txt", "a+");
	logP = fopen("log.txt", "a+");
	if (!repoP || !logP) {
		printf("Couldnt open data file\n");
		_exit(1);
	}
	while(fgets(buffer, sizeof(buffer),repoP)) {
		fileName = strtok(buffer, "\t");
		path = strtok(NULL, "\t");
		ip = strtok(NULL, "\t\n");
		string str1(fileName);
		string str2(path);
		string str3(ip);
		if (repoData.find(str1) != repoData.end()) {
			vector<pair<string,string > > v;
			v = repoData[str1];
			v.push_back(make_pair(str2, str3));
			repoData[fileName] = v;
		} else {
			vector<pair<string,string > > v;
			v.push_back(make_pair(str2, str3));
			repoData[str1] = v;
		}
	}
	fclose(repoP);
	fclose(logP);

	bzero(buffer, sizeof(buffer));
	if (repoData.find(cmds) != repoData.end()) {
		v = repoData[cmds];
		for (vector<pair<string,string > >::iterator it = v.begin(); it !=v.end();it++) {
			index++;
			sprintf(num, "%d", index);
			strcat(buffer, num);
			strcat(buffer, " ");
			strcat(buffer, str1.c_str());
			strcat(buffer, " ");
			strcat(buffer, it->first.c_str());
			strcat(buffer, " ");
			strcat(buffer, it->second.c_str());
			strcat(buffer, "\n");

		}
		write(newsockfd, buffer, strlen(buffer));
		return 1;
	} 
	str = "No such file\n";
	send(newsockfd, str.c_str(), str.size(), 0);
	return 0;
}
//make entry in repo data and repo.txt while share
int makeEntry(int sockFd, char *fileName, char *filePath, char *ip) {
	char buffer[100] = {'\0'}, ipPort[50]= {'\0'}, num[4];
	socklen_t len;
	struct sockaddr clientAdd1;
	string str;

	map<string,vector<pair<string,string> > > repoData;
	char buffer1[100], *fn, *path, *ip1;
	FILE *repoP, *logP;
	repoP = fopen("repo.txt", "a+");
	logP = fopen("log.txt", "a+");
	if (!repoP || !logP) {
		printf("Couldnt open data file\n");
		_exit(1);
	}
	while(fgets(buffer1, sizeof(buffer1),repoP)) {
		fn = strtok(buffer1, "\t");
		path = strtok(NULL, "\t");
		ip1 = strtok(NULL, "\t\n");
		string str1(fn);
		string str2(path);
		string str3(ip1);
		if (repoData.find(str1) != repoData.end()) {
			vector<pair<string,string > > v;
			v = repoData[str1];
			v.push_back(make_pair(str2, str3));
			repoData[fileName] = v;
		} else {
			vector<pair<string,string > > v;
			v.push_back(make_pair(str2, str3));
			repoData[str1] = v;
		}
	}
	fclose(repoP);
	fclose(logP);

	strcpy(ipPort, ip);
	if (repoData.find(fileName) != repoData.end()) {
		vector<pair<string,string > > v;
		v = repoData[fileName];
		v.push_back(make_pair(filePath, ipPort));
		repoData[fileName] = v;
	} else{
		vector<pair<string,string > > v;
		v.push_back(make_pair(filePath, ipPort));
		repoData[fileName] = v;
	}
	strcpy(buffer, fileName);
	strcat(buffer , "\t");
	strcat(buffer, filePath);
	strcat(buffer, "\t");
	strcat(buffer, ipPort);
	strcat(buffer, "\n\0");
	repoP = fopen("repo.txt", "a+");
	if (!repoP) {
		return -1;
	} else {
		fwrite (buffer , sizeof(char), strlen(buffer), repoP);
		fclose(repoP);
	}
	return 1;
}
void getDateTime(char buffer[]) {
	time_t rawtime;
  	struct tm * timeinfo;

  	time (&rawtime);
  	timeinfo = localtime(&rawtime);

  	strftime(buffer,80,"%d-%m-%Y %I:%M:%S",timeinfo);
}

void logData(int opt, char addr[]) {
	FILE *logP;
	char dateTime[10000] = {'\0'}, buffer[1000] = {'\0'};
	getDateTime(dateTime);
	strcpy(buffer, dateTime);
	switch(opt) {
		case 1:	strcat(buffer, ": Search request from "); 
				break;	
		case 2: strcat(buffer, ": Search response to ");
				break;
		case 3: strcat(buffer, ": Share request from "); 
				break;
		case 4: strcat(buffer, ": Share ack to " );  
				break;	
		case 5: strcat(buffer, ": List request from " );  
				break;	
		case 6: strcat(buffer, ": List response to " );  
				break;											
	}
	strcat(buffer, addr);
	strcat(buffer, "\n");
	logP = fopen("log.txt", "a+");
	if (!logP) {
		cout << "Error while writing to log\n";
	} else {
		fwrite(buffer, 1, strlen(buffer), logP);
		fclose(logP);
	}
}

void handleListRequest(int newsockfd, char files[]) {
	char buffer1[100];
	FILE *repoP;
	repoP = fopen("repo.txt", "a+");
	if (!repoP) {
		printf("Couldnt open data file\n");
		_exit(1);
	}
	while(fgets(buffer1, sizeof(buffer1),repoP)) {
		strcat(files,buffer1);
	}
}
void handleRequest(int newsockfd, struct sockaddr_in clientAdd) {
	char  *cmds, *ip, buffer[100];
    vector<string> v;
    string msg;
    int port, status,n;
    while(1) {
    	bzero(buffer, sizeof(buffer));
       	if (read(newsockfd,buffer,100) <= 0) {
    		cout << "Error reading from socket\n";
    		break;
    	} 
    	if (buffer[0] != '\0') {
		    if (!strncmp(buffer, "exit", 4)) {
	            break;
	        }
		    cmds = strtok(buffer, "#@#");	
		    ip = inet_ntoa(clientAdd.sin_addr);
		    port =  ntohs(clientAdd.sin_port);
		    if (!strcmp(cmds,"SEARCH")) {	
		    	logData(1, ip);
		    	cmds = strtok(NULL, "#@#");
		    	handleSearchRequest(newsockfd, cmds);
		    	logData(2, ip);
		    } else if (!strcmp(cmds,"SHARE")) {
		    	cmds = strtok(NULL, "#@#");
		    	logData(3, ip);
		    	status = makeEntry(newsockfd, cmds, strtok(NULL, "#@#"), ip);
		    	logData(4, ip);
		    	if (status == -1) {
		    		msg = "Error while updating file\n";
		    		send(newsockfd, msg.c_str(), msg.size(), 0);
		    	} else {
		    		msg = "File updated successfully\n";
		    		send(newsockfd, msg.c_str(), msg.size(), 0);
		    	}
		    } else if (!strcmp(cmds, "LIST")) {
		    	logData(5, ip);
		    	bzero(buffer, sizeof(buffer));
		    	handleListRequest(newsockfd, buffer);
		    	send(newsockfd, buffer, sizeof(buffer), 0);
		    	logData(6,ip);
		    }
		}
    }
	
}

void createServer(char *data[]) {
	int socketId, port, isListening, newsockfd, n, pid;
	struct sockaddr_in socketAdd, clientAdd;
	socklen_t clilen;
	char buffer[10000];
	port = atoi(data[1]);
	socketId = socket(AF_INET, SOCK_STREAM, 0);
	bzero((char *) &socketAdd, sizeof(socketAdd));
	if (socketId < 0) {
		cout << "Error while creating socket\n";
	}
	socketAdd.sin_family = AF_INET;
	socketAdd.sin_addr.s_addr = INADDR_ANY;
	socketAdd.sin_port = htons(port);
	if (bind(socketId,(struct sockaddr *) &socketAdd, sizeof(socketAdd)) < 0) {
		cout << "Error while binding port\n";
	}
	if (listen(socketId, 5)) {
		cout << "Error while listening port\n";
	}
	clilen = sizeof(clientAdd);
	while(1) {
        newsockfd = accept(socketId, (struct sockaddr *) &clientAdd, &clilen);
        if (newsockfd < 0) {
             printf("ERROR on accept");
        }
        pid = fork();
       	if (pid == 0) {
        	handleRequest(newsockfd, clientAdd);
       	}
    }
	close(newsockfd);
    close(socketId);
}

int main(int argc, char *argv[]) {

	if (argc < 2) {
		cout << "Please provide port\n";
		return 0;
	}
	createServer(argv);
	return 0;
}