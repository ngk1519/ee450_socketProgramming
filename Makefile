# C++ compiler
# Credit: Using make and writing Makefiles: https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html
CC=g++
CFLAGS=-Wall -g
# Must need "-std=c++0x" as flag above for linux

all: client awsO serverAO serverBO serverCO

client: client.o ServerFuncs.o
	$(CC) $(CFLAGS) -o client client.o ServerFuncs.o

aws:
	./awsO

serverA:
	./serverAO

serverB:
	./serverBO

serverC:
	./serverCO

awsO: aws.o ServerFuncs.o
	$(CC) $(CFLAGS) -o awsO aws.o ServerFuncs.o

serverAO: serverA.o WeightedGraph.o ServerFuncs.o
	$(CC) $(CFLAGS) -o serverAO serverA.o WeightedGraph.o ServerFuncs.o

serverBO: serverB.o WeightedGraph.o ServerFuncs.o
	$(CC) $(CFLAGS) -o serverBO serverB.o WeightedGraph.o ServerFuncs.o

serverCO: serverC.o WeightedGraph.o ServerFuncs.o
	$(CC) $(CFLAGS) -o serverCO serverC.o WeightedGraph.o ServerFuncs.o

aws.o: aws.cpp ServerFuncs.h
	$(CC) $(CFLAGS) -c aws.cpp ServerFuncs.cpp

client.o: client.cpp ServerFuncs.h
	$(CC) $(CFLAGS) -c client.cpp ServerFuncs.cpp

serverA.o: serverA.cpp WeightedGraph.h ServerFuncs.h
	$(CC) $(CFLAGS) -c serverA.cpp WeightedGraph.cpp ServerFuncs.cpp

serverB.o: serverB.cpp WeightedGraph.h ServerFuncs.h
	$(CC) $(CFLAGS) -c serverB.cpp WeightedGraph.cpp ServerFuncs.cpp

serverC.o: serverC.cpp WeightedGraph.h ServerFuncs.h
	$(CC) $(CFLAGS) -c serverC.cpp WeightedGraph.cpp ServerFuncs.cpp

WeightedGraph.o: WeightedGraph.cpp WeightedGraph.h
	$(CC) $(CFLAGS) -c WeightedGraph.cpp

ServerFuncs.o: ServerFuncs.cpp ServerFuncs.h
	$(CC) $(CFLAGS) -c ServerFuncs.cpp

clean:
	$(RM) -rf project *.o *~
