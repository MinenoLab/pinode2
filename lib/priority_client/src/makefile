CXX = g++
CXXFLAGS = -w -std=c++11 -march=native

TARGET_S = pri_server_test
LIBS_S = -pthread
INCLUDE_S = -I./source/server -I./source/server/agent -I./source/prioritypacket -I./conf/
SRCDIR_S = ./source/server
SRCS_S =  $(wildcard $(SRCDIR_S)/*.cpp)
AGENTDIR_S = ./source/server/agent
AGENTSRCS_S = $(wildcard $(AGENTDIR_S)/*.cpp)

TARGET_C = pri_client_test
LIBS_C = 
INCLUDE_C = -I./source/client -I./source/client/agent -I./source/prioritypacket -I./conf/
SRCDIR_C = ./source/client
SRCS_C =  $(wildcard $(SRCDIR_C)/*.cpp)
AGENTDIR_C = ./source/client/agent
AGENTSRCS_C = $(wildcard $(AGENTDIR_C)/*.cpp)


PPDIR = ./source/prioritypacket
PPSRCS = $(wildcard $(PPDIR)/*.cpp)


server: $(SRCS_S) $(AGENTSRCS_S) $(PPSRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_S) -o $(TARGET_S) $^ $(LIBS_S)


client: $(SRCS_C) $(AGENTSRCS_C) $(PPSRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_C) -o $(TARGET_C) $^ $(LIBS_C)


all: $(SRCS_S) $(AGENTSRCS) $(PPSRCS) $(SRCS_C)
	make server
	make client


clean:
	rm -f $(TARGET_S) $(TARGET_C) $(OBJS)
