#Makefile
CXX = g++
TARGET = uavsim
CXXFLAGS = -O2 -std=c++11 -Wall -pipe
LDFLAGS = -lm
SRCS = uavsim-main.cpp common_knowledge.cpp god.cpp parameters.cpp uav.cpp uav-move.cpp uav-cellinfo.cpp cell.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS)
	
clean:
	$(RM) $(TARGET) $(OBJS)