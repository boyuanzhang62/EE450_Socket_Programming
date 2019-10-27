/*
** server.c -- a stream socket server demo
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <unistd.h>


#define PORT "24200" // the port users will be connecting to
#define PORT_MONITOR "25200"
#define PORT_A "21200"
#define PORT_B "22200"
#define PORT_AWS_UDP "23200"

#define BACKLOG 10 // how many pending connections queue will hold
#define MAXDATASIZE 100 // max number of bytes we can get at once
#define MAXBUFLEN 100

#define LOCALHOST "127.0.0.1"

using namespace std;

void sigchld_handler(int s)
{
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;
	while(waitpid(-1, NULL, WNOHANG) > 0);
	errno = saved_errno;
}

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
	
	//int link_id;
	//float bw,length,welocity,noisepower,size,signalpower;
	
	char bw[MAXDATASIZE];
	int numbytes;

	char buf[MAXDATASIZE];
	char c_com[MAXDATASIZE];
	int sockfd, new_fd,sockfd_monitor,new_fd_monitor; 
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;
	socklen_t addr_len;
	cout<<"The AWS is up and running."<<endl;
	////////////////////////////////////////////////////////
	///////////////connect to monitor///////////////////////
	////////////////////////////////////////////////////////
	
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT_MONITOR, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd_monitor = socket(p->ai_family, p->ai_socktype,
		p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}
		if (setsockopt(sockfd_monitor, SOL_SOCKET, SO_REUSEADDR, &yes,
		sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}
		if (bind(sockfd_monitor, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd_monitor);
			perror("server: bind");
			continue;
		}
		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}
	
	
	

	if (listen(sockfd_monitor, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}
	sa.sa_handler = sigchld_handler; // reap all dead processes

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	//printf("server: waiting for connections...\n");

	
	sin_size = sizeof their_addr;
	new_fd_monitor = accept(sockfd_monitor, (struct sockaddr *)&their_addr, &sin_size);
	if (new_fd_monitor == -1) {
		perror("accept");
		
	}

	inet_ntop(their_addr.ss_family,
	get_in_addr((struct sockaddr *)&their_addr),
	s, sizeof s);
	
	while(1){
	
		////////////////////////////////////////////////////////
		///////////////connect to client////////////////////////
		////////////////////////////////////////////////////////
		
		
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE; // use my IP
	
		if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			return 1;
		}
		// loop through all the results and bind to the first we can
		for(p = servinfo; p != NULL; p = p->ai_next) {
			if ((sockfd = socket(p->ai_family, p->ai_socktype,
			p->ai_protocol)) == -1) {
				perror("server: socket");
				continue;
			}
			if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
			sizeof(int)) == -1) {
				perror("setsockopt");
				exit(1);
			}
			if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
				close(sockfd);
				perror("server: bind");
				continue;
			}
			break;
		}
	
		freeaddrinfo(servinfo); // all done with this structure
	
		if (p == NULL) {
			fprintf(stderr, "server: failed to bind\n");
			exit(1);
		}
		
		
		
	
		if (listen(sockfd, BACKLOG) == -1) {
			perror("listen");
			exit(1);
		}
		sa.sa_handler = sigchld_handler; // reap all dead processes
	
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART;
	
		if (sigaction(SIGCHLD, &sa, NULL) == -1) {
			perror("sigaction");
			exit(1);
		}
	
		//printf("server: waiting for connections...\n");

		//while(1) { // main accept() loop
			sin_size = sizeof their_addr;
			new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
			if (new_fd == -1) {
				perror("accept");
				//continue;
			}

			inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);

			//printf("server: got connection from %s\n", s);

			//if (!fork()) { // this is the child process
				//close(sockfd); // child doesn't need the listener
				//if (send(new_fd, "Hello, world!", 13, 0) == -1)
				//	perror("send");

			
				if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
						perror("recv");
						exit(1);
				}
				buf[numbytes] = '\0';
				string strq(buf);
				strcpy(c_com, strq.c_str());
			
				//printf("client: received '%s'\n",buf);
			


				//close(new_fd);
				//exit(0);
			//}
			//close(new_fd); // parent doesn't need this
		//}
		
		
		
		
		
		
		
		
		
		
		

		
			
		if ((numbytes = send(new_fd_monitor, buf, strlen(buf), 0)) == -1) {
			perror("send");
			exit(1);
		}

				
		
		
		int w_c=0;
		string str(buf);
		str.erase(1);
		if(str=="c") w_c=1;
		if(str=="w") w_c=2;
	
		if(w_c==2){
			cout<<"The AWS received operation <write> from the client using TCP over <24200>"<<endl;
			cout<<"The AWS sent operation <write> and arguments to the monitor using TCP over port <25200>"<<endl;

			////////////////////////////////////////////////////////
			///////////////connect to serverA////////////////////////
			////////////////////////////////////////////////////////
			
			cout<<"The AWS sent operation <write> to Backend-Server A using UDP over port <23200>"<<endl;
	
			int sockfd_UDP;

			memset(&hints, 0, sizeof hints);
			//memset(buf,0,sizeof(buf));
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_DGRAM;
	
			if ((rv = getaddrinfo(LOCALHOST, PORT_A, &hints, &servinfo)) != 0) {
				fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
				return 1;
			}
			// loop through all the results and make a socket
			for(p = servinfo; p != NULL; p = p->ai_next) {
				if ((sockfd_UDP = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
					perror("talker: socket");
					continue;
				}
		
				break;
			}
	
			if (p == NULL) {
				fprintf(stderr, "talker: failed to create socket\n");
				return 2;
			}
			if ((numbytes = sendto(sockfd_UDP, buf, strlen(buf), 0,
			p->ai_addr, p->ai_addrlen)) == -1) {
				perror("talker: sendto");
				exit(1);
			}
	
			freeaddrinfo(servinfo);
	
			//printf("talker: sent %d bytes to %s\n", numbytes, buf);
			close(sockfd_UDP);
	
			/////////////////////////////////////////////////////
			////////////receive ack from serverA/////////////////
			/////////////////////////////////////////////////////
	
			int sockfd_UDP_recv;
	
			memset(&hints, 0, sizeof hints);
			hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
			hints.ai_socktype = SOCK_DGRAM;
			hints.ai_flags = AI_PASSIVE; // use my IP
	
			if ((rv = getaddrinfo(NULL, PORT_AWS_UDP, &hints, &servinfo)) != 0) {
				fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
				return 1;
			}
			// loop through all the results and bind to the first we can
			for(p = servinfo; p != NULL; p = p->ai_next) {
				if ((sockfd_UDP_recv = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
					perror("listener: socket");
					continue;
				}
				if (bind(sockfd_UDP_recv, p->ai_addr, p->ai_addrlen) == -1) {
					close(sockfd_UDP_recv);
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
			if ((numbytes = recvfrom(sockfd_UDP_recv, buf, MAXBUFLEN-1 , 0,
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
			cout<<"The AWS received response from Backend-Server A for writing using UDP over port <23200>"<<endl;
		
		
			if ((numbytes = send(new_fd, "333", strlen("333"), 0)) == -1) {
				perror("send");
				exit(1);
			}
			sleep(1);
			if ((numbytes = send(new_fd_monitor, "333", strlen("333"), 0)) == -1) {
				perror("send");
				exit(1);
			}
			cout<<"The AWS sent result to client for operation <write> using TCP over port <24200>"<<endl;
			cout<<"The AWS sent write response to the monitor using TCP over port <25200>"<<endl;
	
	
			close(sockfd_UDP_recv);
		}
		else if(w_c==1){
			cout<<"The AWS received operation <compute> from the client using TCP over <24200>"<<endl;
			cout<<"The AWS sent operation <compute> and arguments to the monitor using TCP over port <25200>"<<endl;
			/////////////////////////////////////////////////
			///////////////connect to serverA////////////////
			/////////////////////////////////////////////////
			//cout<<"compute command read from aws"<<endl;
			cout<<"The AWS sent operation <compute> to Backend-Server A using UDP over port <23200>"<<endl;
			int sockfd_UDP;

			memset(&hints, 0, sizeof hints);
			//memset(buf,0,sizeof(buf));
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_DGRAM;
	
			if ((rv = getaddrinfo(LOCALHOST, PORT_A, &hints, &servinfo)) != 0) {
				fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
				return 1;
			}
			// loop through all the results and make a socket
			for(p = servinfo; p != NULL; p = p->ai_next) {
				if ((sockfd_UDP = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
					perror("talker: socket");
					continue;
				}
		
				break;
			}	
	
			if (p == NULL) {
				fprintf(stderr, "talker: failed to create socket\n");
				return 2;
			}
			if ((numbytes = sendto(sockfd_UDP, buf, strlen(buf), 0,
			p->ai_addr, p->ai_addrlen)) == -1) {
				perror("talker: sendto");
				exit(1);
			}
	
			freeaddrinfo(servinfo);
	
			//printf("talker: sent %d bytes to %s\n", numbytes, buf);
			close(sockfd_UDP);
		
			////////////////////////////////////////////////////////
			////////////receive result from serverA/////////////////
			////////////////////////////////////////////////////////
	
			int sockfd_UDP_recv;
	
			memset(&hints, 0, sizeof hints);
			hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
			hints.ai_socktype = SOCK_DGRAM;
			hints.ai_flags = AI_PASSIVE; // use my IP
	
			if ((rv = getaddrinfo(NULL, PORT_AWS_UDP, &hints, &servinfo)) != 0) {
				fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
				return 1;
			}
			// loop through all the results and bind to the first we can
			for(p = servinfo; p != NULL; p = p->ai_next) {
				if ((sockfd_UDP_recv = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
					perror("listener: socket");
					continue;
				}
				if (bind(sockfd_UDP_recv, p->ai_addr, p->ai_addrlen) == -1) {
					close(sockfd_UDP_recv);
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
			if ((numbytes = recvfrom(sockfd_UDP_recv, buf, MAXBUFLEN-1 , 0,
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
	
	
			close(sockfd_UDP_recv);
		
			int m=0;
			string strs(buf);
			strs.erase(1);
			if(strs=="0") m=0;
			if(strs=="1") m=1;
			if(m==0)
			{
				cout<<"Link ID not found"<<endl;
				cout<<"The AWS sent result to client for operation <compute> using TCP over port <24200>"<<endl;
				cout<<"The AWS sent compute results to the monitor using TCP over port <25200>"<<endl;
				if ((numbytes = send(new_fd, "222", strlen("222"), 0)) == -1) {
					perror("send");
					exit(1);
				}
				if ((numbytes = send(new_fd_monitor, "222", strlen("222"), 0)) == -1) {
					perror("send");
					exit(1);
				}
				
			}
			else if(m==1)
			{
				cout<<"The AWS received link information from Backend-Server A using UDP over port <23200>"<<endl;
				/////////////////////////////////////////////////
				///////////////connect to serverB////////////////
				/////////////////////////////////////////////////
	
				int sockfd_UDP;

				memset(&hints, 0, sizeof hints);
				//memset(buf,0,sizeof(buf));
				hints.ai_family = AF_UNSPEC;
				hints.ai_socktype = SOCK_DGRAM;
	
				if ((rv = getaddrinfo(LOCALHOST, PORT_B, &hints, &servinfo)) != 0) {
					fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
					return 1;
				}
				// loop through all the results and make a socket
				for(p = servinfo; p != NULL; p = p->ai_next) {
					if ((sockfd_UDP = socket(p->ai_family, p->ai_socktype,
					p->ai_protocol)) == -1) {
						perror("talker: socket");
						continue;
					}
		
					break;
				}	
	
				if (p == NULL) {
					fprintf(stderr, "talker: failed to create socket\n");
					return 2;
				}
			
				////////////////////////////////////////////
				//delete the "1 link_id " in the front of input//
			
				string str3(buf);
				string str4(c_com);
				
				str4.erase(0,8);
				str3.erase(0,2);
				int p1=str3.find(' ');
				int p2=str4.find(' ');
				string str5=str3;
				string str6=str3;
				str6.erase(p1);
				str5.erase(p1);
				str3.erase(0,p1+1);
				str4.erase(0,p2);
				str3=str6+" "+str3+str4;
				
				strcpy(buf, str3.c_str());
			    	buf[strlen(buf)]='\0';
				////////////////////////////////////////////
				////////////////////////////////////////////
				//int fuc=str6.find(' ');
				//str6.erase(fuc);
				
				string str1(buf);
				int pp=str1.find(' ');
				string str2=str1;
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
		
				pp=str1.find(' ');
				str2=str1;
				str1.erase(pp);
				str2.erase(0,pp+1);
				double size=stod(str1);
				//cout<<"size:"<<size<<endl;
				double useless2=stod(str2);
		
				cout<<"The AWS sent link ID= "<<str6<<" , size= "<<size<<" , power= "<<useless2<<" , and link information to Backend-Server B using UDP over port <23200>"<<endl;
			
			
				if ((numbytes = sendto(sockfd_UDP, buf, strlen(buf), 0,
				p->ai_addr, p->ai_addrlen)) == -1) {
					perror("talker: sendto");
					exit(1);
				}
	
				freeaddrinfo(servinfo);
	
				//printf("talker: sent %d bytes to %s\n", numbytes, buf);
				close(sockfd_UDP);
		
				////////////////////////////////////////////////////////
				////////////receive result from serverB/////////////////
				////////////////////////////////////////////////////////
	
				int sockfd_UDP_recv;
	
				memset(&hints, 0, sizeof hints);
				hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
				hints.ai_socktype = SOCK_DGRAM;
				hints.ai_flags = AI_PASSIVE; // use my IP
	
				if ((rv = getaddrinfo(NULL, PORT_AWS_UDP, &hints, &servinfo)) != 0) {
					fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
					return 1;
				}
				// loop through all the results and bind to the first we can
				for(p = servinfo; p != NULL; p = p->ai_next) {
					if ((sockfd_UDP_recv = socket(p->ai_family, p->ai_socktype,
					p->ai_protocol)) == -1) {
						perror("listener: socket");
						continue;
					}
					if (bind(sockfd_UDP_recv, p->ai_addr, p->ai_addrlen) == -1) {
						close(sockfd_UDP_recv);
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
				if ((numbytes = recvfrom(sockfd_UDP_recv, buf, MAXBUFLEN-1 , 0,
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
				cout<<"The AWS received outputs from Backend-Server B using UDP over port <23200>"<<endl;
				//printf("listener: packet contains \"%s\"\n", buf);
	
				close(sockfd_UDP_recv);
			
				/////////////////////////////////////////
				//send the result to client and monitor//
				/////////////////////////////////////////
				string res1(buf);
				res1="111 "+str5+" "+res1;
				strcpy(buf, res1.c_str());
			    	buf[strlen(buf)]='\0';
			
				if ((numbytes = send(new_fd, buf, strlen(buf), 0)) == -1) {
					perror("send");
					exit(1);
				}
				sleep(1);
				
				if ((numbytes = send(new_fd_monitor, buf, strlen(buf), 0)) == -1) {
					perror("send");
					exit(1);
				}
				cout<<"The AWS sent result to client for operation compute using TCP over port <24200>"<<endl;
				cout<<"The AWS sent compute results to the monitor using TCP over port <25200>"<<endl;
			}
		}
		
		close(new_fd);
		close(sockfd);
	}
	close(new_fd_monitor);
	close(sockfd_monitor);
	return 0;
}
