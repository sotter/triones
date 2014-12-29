# Define source files

CLIENT = chatserver
CLIENT_SRCS = user.cpp iduser.cpp chatserver.cpp main.cpp
CLIENT_OBJS = $(CLIENT_SRCS:.cpp=.o)

BIN = $(CLIENT)

DYNAMIC_LIB = 

STATIC_LIB = 

INSTALL_LIB_PATH = ../lib

CXXFLAGS  = -Wall -g -D__LINUX -fPIC -ggdb3 -Wno-deprecated

# Define header file paths
INCPATH = -I../../include/triones

CXX = g++

# Define the -L library path(s)
LDFLAGS = -L../../lib -L../chatxml

# Define the -l library name(s)
LIBS = -lpthread -ltriones -lrt -ltrionesnet -ltrionesuser -lcomm -lchatxml -ltinyxml

LIB_FLAGS = $(CXXFLAGS) $(LDFLAGS) $(LIBS) -shared -Wl,-soname,$(DYNAMIC_LIB)

.PHONY = all clean distclean

# Main entry point
#

AR       = ar
ARFLAGS  = -ruv
RANLIB	 = ranlib

all:  $(BIN)

# For linking object file(s) to produce the executable	
$(CLIENT): $(CLIENT_OBJS)
	@echo Linking $@
	$(CXX) $^ $(CXXFLAGS) -o $@ $(INCPATH) $(LDFLAGS) $(LIBS)
# For compiling source file(s)
#
.cpp.o:
	@echo Compiling $<
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LDFLAGS) $<
# For cleaning up the project
#
clean:
	$(RM) $(CLIENT_OBJS) core.* $(BIN) 

distclean: clean
	$(RM) $(BIN) 


