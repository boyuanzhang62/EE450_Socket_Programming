/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>

#define PORT "24200" // the port client will be connecting to
#define MAXDATASIZE 100 // max number of bytes we can get at once

#define LOCALHOST "127.0.0.1"
using namespace std;
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char blk[2]="@";
	char *zblk;
	zblk=blk;
	//string mg=" ";
	char s[INET6_ADDRSTRLEN];
	cout<<"The client is up and running"<<endl;
	if (argc != 5&& argc != 6) {
		fprintf(stderr,"usage: function input\n");
		
		exit(1);
	}
	
	if(!(strcmp(argv[1],"write")==0 || strcmp(argv[1],"compute")==0)){
		fprintf(stderr, "input function not valid!\n");
		exit(1);
	}
	if(strcmp(argv[1],"write")==0&&argc!=6 ){
		fprintf(stderr, "write function have 4 parameters!\n");
		exit(1);
	}
	if(strcmp(argv[1],"compute")==0&&argc!=5 ){
		fprintf(stderr, "compute function have 3 parameters!\n");
		exit(1);
	}
	
	if(strcmp(argv[1],"write")==0&&argc==6 ){
		cout<<"The client sent write operation to AWS"<<endl;
		
	}
	if(strcmp(argv[1],"compute")==0&&argc==5 ){
		cout<<"The client sent ID="<<argv[2]<<", size="<<argv[3]<<", and power="<<argv[4]<<" to AWS"<<endl;
		
	}


	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(LOCALHOST, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
		p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
	s, sizeof s);

	//printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure
	
	////////////////////////////////////////
	//insert space between input arguments//
	string temp = "";
    	for(int i = 1; i < argc-1; i++){
       		string str(argv[i]);
       		temp+=str+' ';
    	}
    	temp+=argv[argc-1];
    	strcpy(buf, temp.c_str());
    	buf[strlen(buf)]='\0';
	////////////////////////////////////////
	////////////////////////////////////////
		
	if ((numbytes = send(sockfd, buf, strlen(buf), 0)) == -1) {
		perror("send");
		exit(1);
	}
	
	
		//if ((numbytes = send(sockfd, zblk, strlen(zblk), 0)) == -1) {
		//	perror("send");
		//	exit(1);
		//}
		//memset(buf,0,sizeof(buf));
	
	
	
	/*
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
		perror("recv");
		exit(1);
	}
	
	buf[numbytes] = '\0';
	printf("client: received '%s'\n",buf);
	*/
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
			perror("recv");
			exit(1);
	}
	buf[numbytes] = '\0';
	
	//printf("client: received '%s'\n",buf);
	
	/////////////////////////////////
	//display the result of process//
	/////////////////////////////////
	string res(buf);
	string res1=res;
	int link_id;
	double delay;
	res.erase(3);
	if(res=="111")
	{
		res1.erase(0,4);
		int ps=res1.find(' ');
		string res2=res1;
		res2.erase(ps);
		link_id=stoi(res2);
		
		res1.erase(0,ps+1);
		for(int i=0;i<2;i++)
		{
			ps=res1.find(' ');
			res1.erase(0,ps+1);
			
		}
		delay=stod(res1);
		cout<<"The delay for link "<<link_id<<" is "<<delay<<" ms"<<endl;
		
	}
	else if(res=="222")
	{
		cout<<"Link ID not found"<<endl;
	}
	else if(res=="333")
	{
		cout<<"The write operation has been completed successfully"<<endl;
	}
		    	
	close(sockfd);

	return 0;
}
