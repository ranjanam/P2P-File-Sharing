//Name: M.Ranjana
//Roll No : 20162014

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
using namespace std;

#define MYPORT 5696

void error(const char *msg)
{
    perror(msg);
    exit(0);
}
int createFile(char *fileName) {
    int fp;
    fp = open(fileName, O_WRONLY|O_CREAT | O_TRUNC, 0666);
    return fp;
}

void connectToDownloadServer(char *fileName, char *path, char *ip) {
    int bytesRead, fileNotCreated = 1;
    char input[100];
    int  portno, n, opt, sockfd, fp;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    unsigned char ch;
    char buffer[2000] = {'\0'}, str[2000] = {'\0'}, *result;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(ip);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(MYPORT);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        return;
    }
    
    strcpy(buffer, path);
    strcat(buffer, fileName);
    strcat(buffer, "#@#");
    send(sockfd, buffer, sizeof(buffer), 0);
    if (recv(sockfd, buffer, sizeof(buffer), 0)>0) {
        result=strtok(buffer, "#");
        if (!strcmp(result, "ERROR")) {
            result = strtok(NULL, "#");
            result = strtok(NULL, "\n");
            cout << result << endl;
            return;
        } else {
            result = strtok(NULL, "@");
            result = strtok(NULL, "\n");
            // ch='1';
            // send(sockfd, &ch, 1, 0);
            cout << "Downloading file..\n";
            while(1) {
                if (recv(sockfd, &ch, 1, 0)> 0) {
                    if (fileNotCreated) {
                        fp = createFile(fileName);
                        if (!fp) {
                            cout << "Unable to download file";
                            break;
                        }
                        fileNotCreated = 0;
                    }
                    write(fp, &ch, 1);
                } else {
                    break;
                }
            }
            cout << "File downloaded successfully\n";
        }
    }
    
    close(fp);
    return;
}
void createClient(char **argv) {
    int sockfd, portno, n, opt, selectedOpt, i;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[2000] = {'\0'}, str[500] = {'\0'}, *fileName, *path, *ip, *port, option[100];
    string str1;

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    while(1) {
        printf("\n1.Search\n2.Share\n3.List Files\n4.Exit\n>> ");
        cin >> option;
        if (option[0] < '1' && option[0] > '4') {
            opt = 0;
        } else {
            opt = option[0] - '0';
        }
        bzero(option, sizeof(option));
        switch(opt) {
            case 1: printf("Please enter search pattern: ");
                    cin >> str1;
                    str1 = "SEARCH#" + str1 + "#";
                    bzero(buffer, sizeof(buffer));
                    send(sockfd, str1.c_str(), 100, 0);
                    recv(sockfd, buffer, 2000, 0);
                    cout << buffer;
                    if (!strcmp(buffer, "No such file\n")) {
                        break;
                    }
                    cout << "Select Mirror : ";
                    cin >> option;
                    if (option[0] < '1') {
                        cout << "Invalid input\n";
                    } else {
                        opt = option[0] - '0';
                    }
                    bzero(option, sizeof(option));
                    opt--;
                    i=0;
                    while(opt--) {
                        while(buffer[i]!='\n'){i++;};
                        i++;
                    }
                    if (buffer[i] == '\0') {
                        cout << "No such mirror\n";
                        break;
                    }
                    str1 = "";
                    char *fileName,*ip;
                    while(buffer[i] != '\n') {
                        str1 +=buffer[i];
                        i++;
                    }
                    str1+="\n";
                    strcat(option, str1.c_str());
                    strtok(option, " ");
                    fileName = strtok(NULL, " ");
                    path = strtok(NULL, " ");
                    ip = strtok(NULL, "\n");
                    connectToDownloadServer(fileName, path, ip);
                    break;
            case 2: printf("Enter file name and path: ");
                    scanf("%s", buffer);
                    scanf("%s", str);
                    strcat(buffer, "#@#");
                    strcat(buffer, str);
                    bzero(str, sizeof(str));
                    strcpy(str, "SHARE#@#");
                    strcat(str, buffer);
                    strcat(str, "#@#");
                    send(sockfd, str, sizeof(str), 0);
                    bzero(buffer, sizeof(buffer));
                    read(sockfd, buffer, sizeof(buffer));
                    cout << buffer;
                    break;
            case 3: bzero(buffer, sizeof(buffer));
                    strcpy(buffer, "LIST");
                    strcat(buffer, "#@#");
                    send(sockfd, buffer, sizeof(buffer), 0);
                    bzero(buffer, sizeof(buffer));
                    read(sockfd, buffer, sizeof(buffer));
                    cout << buffer << endl;
                    break;
            case 4: strcpy(str ,"exit");
                    send(sockfd, str, sizeof(str), 0);
                    cout << "closed client\n";
                    close(sockfd);
                    exit(0);        
                    break;
            default : cout << "Invalid option\n";   
                    break;     
        }
    }
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
        case 1: strcat(buffer, ": Download request from "); 
                break;  
        case 2: strcat(buffer, ": File sent to ");
                break;                     
    }
    strcat(buffer, addr);
    strcat(buffer, "\n");
    logP = fopen("clientlog.txt", "a+");
    if (!logP) {
        cout << "Error while writing to log\n";
    } else {
        fwrite(buffer, 1, strlen(buffer), logP);
        fclose(logP);
    }
}

void handleRequest(int newsockfd, struct sockaddr_in clientAdd) {
    char buffer[1000] = {'\0'}, *fileName, *ip;
    char input[10];
    int fp;
    unsigned char ch;
   
    if (!recv(newsockfd,buffer,1000, 0)) {
        printf("ERROR reading from socket");
        return;
    }
    ip = inet_ntoa(clientAdd.sin_addr);
    logData(1, ip);
    fileName = strtok(buffer, "#@#");
    fp = open(fileName, O_RDONLY,0666);
    if (fp < 0) {
        bzero(buffer, sizeof(buffer));
        strcpy(buffer, "ERROR#@#File does not exist\n");
        send(newsockfd, buffer, sizeof(buffer), 0);
    } else {
        //check size and then error for half file
        bzero(buffer, sizeof(buffer));
        strcpy(buffer, "SUCCESS#@#File exists\n");
        send(newsockfd, buffer, sizeof(buffer), 0);
        while(read(fp, &ch, 1)>0) {
            send(newsockfd, &ch, 1, 0);
        }
        shutdown(newsockfd, SHUT_RDWR);
        logData(2, ip);
        close(fp);
    }
    exit(0);
}

void createDownLoadServer() {
    int socketId, isListening, newsockfd, n, portno, pid;
    struct sockaddr_in socketAdd, clientAdd;
    socklen_t clilen;
    char buffer[10000];

    socketId = socket(AF_INET, SOCK_STREAM, 0);
    bzero((char *) &socketAdd, sizeof(socketAdd));
    if (socketId < 0) {
        cout << "Error while creating socket\n";
    }
    socketAdd.sin_family = AF_INET;
    socketAdd.sin_addr.s_addr = INADDR_ANY;
    socketAdd.sin_port = htons(MYPORT);
    if (bind(socketId,(struct sockaddr *) &socketAdd, sizeof(socketAdd)) < 0) {
        perror("ERROR");
        return;
    }
    if (listen(socketId, 5)) {
        cout << "Error while listening port\n";
         return;
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
int main(int argc, char *argv[])
{
    int pid;
    if (argc < 3) {
        cout << "Invalid inputs\n";
        exit(0);
    }

    pid = fork();
    if (pid != 0) {
        createDownLoadServer();
    } else {
        createClient(argv);
    }    
    return 0;
}
