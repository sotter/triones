# Define source files

SERVER = tcpserver
SERVER_SRCS = baseserver.cpp testmain.cpp
SERVER_OBJS = $(SERVER_SRCS:.cpp=.o)

BIN = $(SERVER)

DYNAMIC_LIB = 

STATIC_LIB = 

INSTALL_LIB_PATH = ../lib

CXXFLAGS  = -Wall -g -D__LINUX -fPIC -ggdb3 -Wno-deprecated

# Define header file paths
INCPATH = -I../../include/triones

CXX = g++

# Define the -L library path(s)
LDFLAGS = -L../../lib

# Define the -l library name(s)
LIBS = -lpthread -ltrioneslog -ltrionesthread -ltrionesutils -ltrionespack -ltriones

LIB_FLAGS = $(CXXFLAGS) $(LDFLAGS) $(LIBS) -shared -Wl,-soname,$(DYNAMIC_LIB)

.PHONY = all clean distclean

# Main entry point
#

AR       = ar
ARFLAGS  = -ruv
RANLIB	 = ranlib

all:  $(BIN) $(STATIC_LIB) $(DYNAMIC_LIB)

# For linking object file(s) to produce the executable

$(ASYN_CLIENT): $(ASYN_CLIENT_OBJS)
	@echo Linking $@
	$(CXX) $^ $(LDFLAGS) $(LIBS)  $(CXXFLAGS) $(INCPATH) -o $@
    
$(SYN_CLIENT): $(SYN_CLIENT_OBJS)
	@echo Linking $@
	$(CXX) $^ $(LDFLAGS) $(LIBS)  $(CXXFLAGS) $(INCPATH) -o $@
	
$(SERVER): $(SERVER_OBJS)
	@echo Linking $@
	$(CXX) $^ $(CXXFLAGS) -o $@ $(INCPATH) $(LDFLAGS) $(LIBS)

${STATIC_LIB}: ${OBJS}
	@if [ ! -d ${INSTALL_LIB_PATH} ]; then mkdir -p ${INSTALL_LIB_PATH}; fi
	$(AR) ${ARFLAGS} $@ $(OBJS)
	$(RANLIB) $@

${DYNAMIC_LIB} : ${OBJS}
	@if [ ! -d ${INSTALL_LIB_PATH} ]; then mkdir -p ${INSTALL_LIB_PATH}; fi
	$(CXX) $(LIB_FLAGS) -o $(DYNAMIC_LIB) $(OBJS)

# For compiling source file(s)
#
.cpp.o:
	@echo Compiling $<
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LDFLAGS)  $<

install:
	@if [ "x${STATIC_LIB}" != "x" ]; then mkdir -p ${INSTALL_LIB_PATH}; cp $(STATIC_LIB) $(INSTALL_LIB_PATH); fi
	@if [ "x${DYNAMIC_LIB}" != "x" ]; then mkdir -p ${INSTALL_LIB_PATH}; cp $(DYNAMIC_LIB) $(INSTALL_LIB_PATH); fi
# For cleaning up the project
#
clean:
	$(RM) $(SERVER_OBJS) core.* $(BIN) 

distclean: clean
	$(RM) $(BIN) 


