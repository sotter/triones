# Define executable name
BIN = test

# Define source files
#CPP_SRCS = packet.cpp packetqueue.cpp tprotocol.cpp main.cpp
CPP_SRCS = udptest.cpp

# Define header file paths

CTFOPRJ_HOME = /home/liubo/workspace/ctfo

#INCPATH = -I${CTFOPRJ_HOME}/dblibs/mongo/include -I${CTFOPRJ_HOME}/dblibs/mongo/include/boost

CXX = g++

# Define the -L library path(s)
#LDFLAGS = -L/home/liubo/workspace/ctfo/dblibs/mongo/lib
#LDFLAGS = -L./ -L${CTFOPRJ_HOME}/dblibs/mongo/lib

# Define the -l library name(s)
#LIBS = -lmongoclient -lboost_filesystem-mt -lboost_system-mt -lboost_thread-mt 
#
# Only in special cases should anything be edited below this line
OBJS      = $(CPP_SRCS:.cpp=.o)
CXXFLAGS  = -Wall -g -lpthread 

.PHONY = all clean distclean

# Main entry point
#

all:  $(BIN)

# For linking object file(s) to produce the executable

$(BIN): $(OBJS)
	@echo Linking $@
	$(CXX) $^ $(CXXFLAGS) $(LDFLAGS) $(LIBS)  -o $@


# For compiling source file(s)
#
.cpp.o:
	@echo Compiling $<
	$(CXX) -c $(CXXFLAGS) $(INCPATH)  $<

# For cleaning up the project
#
clean:
	$(RM) $(OBJS) core.* $(BIN)

distclean: clean
	$(RM) $(BIN) 


