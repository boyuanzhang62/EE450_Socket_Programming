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
////////////////////////
using namespace std;

#define PORT_A "21200" 
#define PORT_AWS_UDP "23200"
#define MAXBUFLEN 100
#define LOCALHOST "127.0.0.1"
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
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	
	char buf[MAXBUFLEN];
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];
	cout<<"The Server A is up and running using UDP on port <21200>."<<endl;
	
	fstream afile;
	afile.open("database.txt",ios::in);
	
	if(!afile){
		ofstream inaFile; 
		inaFile.open("database.txt");

		if(inaFile.is_open())
		{
			inaFile << "Link_ID Bandwidth(MHz) Length(km) Velocity(km/s) NoisePower(dBm)" <<endl;   
		}
	
		inaFile.close();   
		//cout<<"database.txt"<<"txt write complete!\n";
	}
	afile.close();
	
	while(1){
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_flags = AI_PASSIVE; // use my IP
	
		if ((rv = getaddrinfo(NULL, PORT_A, &hints, &servinfo)) != 0) {
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
	
		int w_c=0;
		string str(buf);
		str.erase(1);
		
		if(str=="c") w_c=1;
		if(str=="w") w_c=2;
		
		if(w_c==2)
		{
			cout<<"The Server A received input for writing"<<endl;
			////////////////////////////////////////////
			//delete the "write" in the front of input//
			string temp = "";
			string str(buf);
			str.erase(0,5);
			strcpy(buf, str.c_str());
		    	buf[strlen(buf)]='\0';
			////////////////////////////////////////////
			////////////////////////////////////////////
	
	
			//char buffer[256];
			string buffer;
			string data;
			fstream _file;
			_file.open("database.txt",ios::in);
			if(!_file){
				ofstream inFile; 
				inFile.open("database.txt");
		
				string temp = "";
				string str(buf);
				str="1"+str;
				strcpy(buf, str.c_str());
			    	buf[strlen(buf)]='\0';  
			    	  
				if(inFile.is_open())
				{
					inFile << "Link_ID Bandwidth(MHz) Length(km) Velocity(km/s) NoisePower(dBm)" <<endl;  
					 
				}
			
				inFile.close();   
				//cout<<"database.txt"<<"txt write complete!\n";
			}
			else
			{
				//cout<<"database.txt"<<" exist!\n";
		
		
				    
				while(getline(_file,data))
				{
					//_file.getline(buffer,256,'\n');
					//getline(_file,data);
					//cout<<data<<endl;
					buffer=data;
				}
				//cout<<buffer<<endl;
				int link_id;
		
				int pos=buffer.find(' ');
				buffer.erase(pos);
				ofstream inFile;
				//cout<<pos<<endl;
		
				if(buffer!="Link_ID")
				{
					link_id=stoi(buffer)+1;
					cout<<"The Server A wrote link <"<<link_id<<"> to database"<<endl;
					//cout<<link_id<<endl;
		
					inFile.open("database.txt",ios::app);

					string str(buf);
					str=to_string(link_id)+str;
					strcpy(buf, str.c_str());
				    	buf[strlen(buf)]='\0';  
				    	  
					if(inFile.is_open())
					{ 
						inFile << buf <<endl;  
					}
				}
				else if(buffer=="Link_ID")
				{
					link_id=1;
					cout<<"The Server A wrote link <"<<link_id<<"> to database"<<endl;
					//cout<<link_id<<endl;
		
					 
					inFile.open("database.txt",ios::app);
		
					string str(buf);
					str=to_string(link_id)+str;
					strcpy(buf, str.c_str());
				    	buf[strlen(buf)]='\0';  
				    	  
					if(inFile.is_open())
					{ 
						inFile << buf <<endl;  
					} 
				}
				inFile.close();   
				_file.close();
				
		
			}
	
			close(sockfd);
	
	
	
			////////////////////////////////////////
			////////send ack to AWS server//////////
			////////////////////////////////////////
	
	
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
	
			memset(buf,0,sizeof(buf));
	
			////////////////////////////////
			//write ack message in the buf//
			temp = "111";
			strcpy(buf, temp.c_str());
		    	buf[strlen(buf)]='\0';
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
			close(sockfd);
		}
		else if(w_c==1)
		{
			//cout<<"compute command read from server"<<endl;
			////////////////////////////////////////////
			//find the link_id in the compute cammand//
			
			string str(buf);
			str.erase(0,8);
			int pos=str.find(' ');
			str.erase(pos);
			int link_id_in=stoi(str);
			int m=0;
			////////////////////////////////////////////
			////////////////////////////////////////////
			cout<<"The Server A received input <"<<link_id_in<<"> for computing"<<endl;
	
			//char buffer[256];
			string buffer;
			string data;
			fstream _file;
			_file.open("database.txt",ios::in);
			if(!_file){
				  
				cout<<"You need to establish a database.txt before you use the command <compute>!"<<endl;
			}
			else
			{
				//cout<<"database.txt"<<" exist!\n";
		
		
				getline(_file,data);    
				while(getline(_file,data))
				{
					//_file.getline(buffer,256,'\n');
					//getline(_file,data);
					//cout<<data<<endl;
					buffer=data;
					
					int pos=buffer.find(' ');
					buffer.erase(pos);
					//cout<<"this is link_id:"<<buffer<<endl;
					int link_id=stoi(buffer);
					if(link_id==link_id_in){
						strcpy(buf, data.c_str());
						m=1;
						break;
					}
					//cout<<link_id<<endl;
				}
				//cout<<buffer<<endl;
				_file.close();
			
				
			}
	
			close(sockfd);
			
			////////////////////////////////
			//insert m in the front of buf//
		       	string strr(buf);
		        strr=to_string(m)+' '+strr;
		    	strcpy(buf, strr.c_str());
		    	buf[strlen(buf)]='\0';
			////////////////////////////////
			////////////////////////////////
			
			
			///////////////////////////////////////////
			////////send result to AWS server//////////
			///////////////////////////////////////////
			
			if(m==0)
			{
				cout<<"Link ID not found"<<endl;
			}
			else if(m==1)
			{
				cout<<"The Server A finished sending the search result to AWS"<<endl;
			}
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
			close(sockfd);
		
		
		}
	}
	return 0;
}
