#Makefile
CXX = g++
TARGET = uavsim
CXXFLAGS = -O2 -std=c++14 -Wall -pipe
LDFLAGS = -lm
SRCS = uavsim-main.cpp common_knowledge.cpp god.cpp parameters.cpp uav.cpp uav-move.cpp uav-cellinfo.cpp cell.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -g -o $@ $(OBJS)
	
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

TARGET4 = mergegraph
SRCS4 = mergegraph.cpp parameters.cpp
OBJS4 = $(SRCS4:.cpp=.o)

all: $(TARGET4)

$(TARGET4): $(OBJS4)
	$(CXX) -o $@ $(OBJS4)
	
clean:
	$(RM) $(TARGET4) $(OBJS4)

TARGET5 = graphimpAve
SRCS5 = graphimpAve.cpp parameters.cpp
OBJS5 = $(SRCS5:.cpp=.o)

all: $(TARGET5)

$(TARGET5): $(OBJS5)
	$(CXX) -o $@ $(OBJS5)
	
clean:
	$(RM) $(TARGET5) $(OBJS5)

TARGET6 = vtime
SRCS6 = vtime.cpp parameters.cpp
OBJS6 = $(SRCS6:.cpp=.o)

all: $(TARGET6)

$(TARGET6): $(OBJS6)
	$(CXX) -o $@ $(OBJS6)
	
clean:
	$(RM) $(TARGET6) $(OBJS6)