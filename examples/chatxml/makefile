# Define source files

TARGET = libchatxml.a
TARGET_SRCS = chatxml.cpp
TARGET_OBJS = $(TARGET_SRCS:.cpp=.o)

SLIB = $(TARGET)

DYNAMIC_LIB = 

STATIC_LIB = 

INSTALL_LIB_PATH = 

CXXFLAGS  = -Wall -g -D__LINUX -fPIC -ggdb3 -Wno-deprecated

# Define header file paths
INCPATH = 

CXX = g++

# Define the -L library path(s)
LDFLAGS = -L../../lib

# Define the -l library name(s)
LIBS = -ltinyxml

LIB_FLAGS = $(CXXFLAGS) $(LDFLAGS) $(LIBS) -shared -Wl,-soname,$(DYNAMIC_LIB)

.PHONY = all clean distclean

# Main entry point
#

AR       = ar
ARFLAGS  = -ruv
RANLIB	 = ranlib

all:  $(SLIB)

# For linking object file(s) to produce the executable	
$(TARGET): $(TARGET_OBJS)
	@echo Linking $@
	${AR} ${ARFLAGS} $@ $^
	${RANLIB} $@
# For compiling source file(s)
#
.cpp.o:
	@echo Compiling $<
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LDFLAGS)  $<
# For cleaning up the project
#
clean:
	$(RM) $(TARGET_OBJS) core.* $(SLIB) 

distclean: clean
	$(RM) $(SLIB) 


