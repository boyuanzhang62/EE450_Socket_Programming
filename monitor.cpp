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
#include <string>     // std::string, std::stod

#define PORT_MONITOR "25200" // the port client will be connecting to
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

int main(void)
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
	cout<<"The monitor is up and running"<<endl;

	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(LOCALHOST, PORT_MONITOR, &hints, &servinfo)) != 0) {
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
	
	while(1)
	{
		if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
				perror("recv");
				exit(1);
		}
		buf[numbytes] = '\0';
		//printf("client: received '%s'\n",buf);
		//cout<<"received from monitor"<<buf<<endl;
		int w_c=0;
		string str(buf);
		str.erase(1);
		if(str=="c") w_c=1;
		if(str=="w") w_c=2;

		if(w_c==2)
		{
			string str1(buf);
			str1.erase(0,6);
			int pos=str1.find(' ');
			string str2=str1;
			str2.erase(pos);
			str1.erase(0,pos+1);
			double bw=stod(str2);
			//cout<<"bandwidth!!!1"<<endl;
			pos=str1.find(' ');
			str2=str1;
			str2.erase(pos);
			str1.erase(0,pos+1);
			double length=stod(str2);
		
			pos=str1.find(' ');
			str2=str1;
			str2.erase(pos);
			str1.erase(0,pos+1);
			double vel=stod(str2);
			
			double noise_pow=stod(str1);
		
			cout<<"The monitor received BW = "<<bw<<", L = "<<length<<", V = "<<vel<<" and P = "<<noise_pow<<" from the AWS"<<endl;

		}
		else if(w_c==1)
		{
			string str1(buf);
			str1.erase(0,8);
			int pos=str1.find(' ');
			string str2=str1;
			str2.erase(pos);
			str1.erase(0,pos+1);
			double link_id=stod(str2);
		
			pos=str1.find(' ');
			str2=str1;
			str2.erase(pos);
			str1.erase(0,pos+1);
			double size=stod(str2);
			
			double pow=stod(str1);
		
			cout<<"The monitor received link ID = "<<link_id<<", size = "<<size<<" and power = "<<pow<<" from the AWS"<<endl;
		}
	
	
	
	
		memset(buf,0,sizeof(buf));
	
		if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
				perror("recv");
				exit(1);
		}
		buf[numbytes] = '\0';
	
	
		/////////////////////////////////
		//display the result of process//
		/////////////////////////////////
		string res3;
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
			double T[2];
			for(int i=0;i<2;i++)
			{
				ps=res1.find(' ');
				res3=res1;
				res3.erase(ps);
				res1.erase(0,ps+1);
				T[i]=stod(res3);
			
			}
			delay=stod(res1);
			cout<<"The delay for link "<<link_id<<" : "<<endl<<"Tt = "<<T[0]<<"ms,"<<endl <<"Tp = "<<T[1]<<"ms,"<<endl<<"Delay ="<<delay<<" ms"<<endl;
		
		}
		else if(res=="222")
		{
			cout<<"Link ID not found"<<endl;
		}
		else if(res=="333")
		{
			cout<<"The write operation has been completed successfully"<<endl;
		}
				    	
		
	}
	close(sockfd);
	return 0;
}
