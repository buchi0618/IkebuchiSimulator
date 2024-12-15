#Makefile
CXX = g++
TARGET = uavsim
CXXFLAGS = -O2 -std=c++14 -Wall -pipe
LDFLAGS = -lm
SRCS = uavsim-main.cpp common_knowledge.cpp god.cpp parameters.cpp uav.cpp uav-move.cpp uav-cellinfo.cpp cell.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS)
	
clean:
	$(RM) $(TARGET) $(OBJS)


TARGET2 = graphVari
SRCS2 = graphVari.cpp parameters.cpp
OBJS2 = $(SRCS2:.cpp=.o)

all: $(TARGET2)

$(TARGET2): $(OBJS2)
	$(CXX) -o $@ $(OBJS2)
	
clean:
	$(RM) $(TARGET2) $(OBJS2)


TARGET3 = graphAve
SRCS3 = graphAve.cpp parameters.cpp
OBJS3 = $(SRCS3:.cpp=.o)

all: $(TARGET3)

$(TARGET3): $(OBJS3)
	$(CXX) -o $@ $(OBJS3)
	
clean:
	$(RM) $(TARGET3) $(OBJS3)

