/*
** listener.c -- a datagram sockets "server" demo
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
//for writing txt file//
#include <fstream>    
#include <iostream>   
#include <string> 
#include <stdio.h>
#include <cmath>
#include <math.h>
#include <sstream>



typedef long long LL;

using namespace std;

#define PORT_A "21200"
#define PORT_B "22200"
#define PORT_AWS_UDP "23200"
#define MAXBUFLEN 100
#define LOCALHOST "127.0.0.1"
// get sockaddr, IPv4 or IPv6:

double round1(double num) 
{  
    double val = (int)(num * 100 + 0.50001); 
    //cout<<value<<endl;
    return (double)val / 100; 
} 

string doubleTranStr(double number) {
    stringstream ss;
    ss << number;
    return ss.str();
}

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(void)
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	
	char buf[MAXBUFLEN];
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];
	cout<<"The Server B is up and running using UDP on port <22200>"<<endl;
	while(1)
	{
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_flags = AI_PASSIVE; // use my IP
	
		if ((rv = getaddrinfo(NULL, PORT_B, &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			return 1;
		}
		// loop through all the results and bind to the first we can
		for(p = servinfo; p != NULL; p = p->ai_next) {
			if ((sockfd = socket(p->ai_family, p->ai_socktype,
			p->ai_protocol)) == -1) {
				perror("listener: socket");
				continue;
			}
			if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
				close(sockfd);
				perror("listener: bind");
				continue;
			}
		
			break;
		}
	
	
		if (p == NULL) {
			fprintf(stderr, "listener: failed to bind socket\n");
			return 2;
		}
		freeaddrinfo(servinfo);
	
		//printf("listener: waiting to recvfrom...\n");
		addr_len = sizeof their_addr;
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
		(struct sockaddr *)&their_addr, &addr_len)) == -1) {
			perror("recvfrom");
			exit(1);
		}
		//printf("listener: got packet from %s\n",
		//inet_ntop(their_addr.ss_family,
		//get_in_addr((struct sockaddr *)&their_addr),
		//s, sizeof s));
	
		//printf("listener: packet is %d bytes long\n", numbytes);
		buf[numbytes] = '\0';
	
		//printf("listener: packet contains \"%s\"\n", buf);
		
		close(sockfd);
	
		string str2(buf);
		int pp=str2.find(' ');
		string str1=str2;
		str2.erase(pp);
		str1.erase(0,pp+1);
		int link_id=stoi(str2);
		//cout<<"bw:"<<bw<<endl;
		
		
		pp=str1.find(' ');
		str2=str1;
		str1.erase(pp);
		str2.erase(0,pp+1);
		double bw=stod(str1);
		//cout<<"bw:"<<bw<<endl;
		
		pp=str2.find(' ');
		str1=str2;
		str2.erase(pp);
		str1.erase(0,pp+1);
		double length=stod(str2);
		//cout<<"length:"<<length<<endl;
		
		pp=str1.find(' ');
		str2=str1;
		str1.erase(pp);
		str2.erase(0,pp+1);
		double vel=stod(str1);
		//cout<<"vel:"<<vel<<endl;
		
		pp=str2.find(' ');
		str1=str2;
		str2.erase(pp);
		str1.erase(0,pp+1);
		double useless1=stod(str2);
		//cout<<useless1<<endl;
		pp=str1.find(' ');
		str2=str1;
		str1.erase(pp);
		str2.erase(0,pp+1);
		double size=stod(str1);
		//cout<<"size:"<<size<<endl;
		double power=stod(str2);
		
		cout<<"The Server B received link information: link <"<<link_id<<">, file size <"<<size<<">, and signal power <"<<power<<">"<<endl;
		double pow1=pow(10,(power/10));
		//cout<<pow1<<endl;
		double noise=pow(10,(useless1/10));
		//cout<<noise<<endl;
		double T_t=size/((bw*1000)*log(1+pow1/noise)/log(2));//size/(bw*1000);
		//cout<<"T_t:"<<T_t<<endl;
		double T_p=length*1000/vel;
		//cout<<"T_p:"<<T_p<<endl;
		double e=T_t+T_p;
		//cout<<"end to end:"<<e<<endl;
		
		double end2end=round1(e);
		
		//cout<<"end to end with 2 decimal:"<<end2end<<endl;
		//string stra=to_string(T_t);
		cout<<"The Server B finished the calculation for link <"<<link_id<<">"<<endl;
		
		string stra=doubleTranStr(T_t);
		string strb=doubleTranStr(T_p);
		string strc=doubleTranStr(end2end);
		
		string str=stra+' '+strb+' '+strc;
		//cout<<"the data sent to serverB is:"<<str<<endl;
		strcpy(buf, str.c_str());
	    	buf[strlen(buf)]='\0';
	
		///////////////////////////////////////////
		////////send result to AWS server//////////
		///////////////////////////////////////////


		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
		hints.ai_socktype = SOCK_DGRAM;
		//hints.ai_flags = AI_PASSIVE; // use my IP

		if ((rv = getaddrinfo(LOCALHOST, PORT_AWS_UDP, &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			return 1;
		}
		// loop through all the results and bind to the first we can
		for(p = servinfo; p != NULL; p = p->ai_next) {
			if ((sockfd = socket(p->ai_family, p->ai_socktype,
			p->ai_protocol)) == -1) {
				perror("listener: socket");
				continue;
			}
	
			break;
		}

		////////////////////////////////
		//write ack message in the buf//
		//temp = "111";
		//strcpy(buf, temp.c_str());
	    	//buf[strlen(buf)]='\0';
		////////////////////////////////
		////////////////////////////////

		if (p == NULL) {
			fprintf(stderr, "listener: failed to bind socket\n");
			return 2;
		}
		if ((numbytes = sendto(sockfd, buf, strlen(buf), 0,
		p->ai_addr, p->ai_addrlen)) == -1) {
			perror("talker: sendto");
			exit(1);
		}
		freeaddrinfo(servinfo);
		//printf("talker: sent %d bytes to %s\n", numbytes, buf);
		cout<<"The Server B finished sending the output to AWS"<<endl;
		close(sockfd);
	
	}
	return 0;
}
