all:
	g++ -std=c++11 client.cpp -o client
	g++ -std=c++11 monitor.cpp -o monitor
	g++ -std=c++11 serverA.cpp -o serverA
	g++ -std=c++11 serverB.cpp -o serverB
	g++ -std=c++11 aws.cpp -o aws
	
serverA:
	./serverA

serverB:
	./serverB

aws:
	./aws

monitor:
	./monitor

.PHONY: serverA serverB aws monitor
