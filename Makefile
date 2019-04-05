UNAME := $(shell uname -s)

SRC_DIR		= ./src
TARGET_DIR	= ./bin
OBJ_DIR		= ./obj


CXXFLAGS    = -g -Wall -std=c++14 -MMD -MP 
LDLIBS		= -lpthread -lm -lboost_program_options -lboost_system
OPENCV_LIBS	= `pkg-config --libs opencv`
NDNCXX_LIBS	= `pkg-config --libs libndn-cxx`

PRODUCER	= $(TARGET_DIR)/producer
PR_SRC_DIR	= $(SRC_DIR)/producer
PR_OBJ_DIR = $(OBJ_DIR)/producer
PR_CXXFLAGS	= `pkg-config --cflags opencv libndn-cxx`
PR_SRCS		= $(wildcard $(PR_SRC_DIR)/*.cpp)
PR_OBJS		= $(addprefix $(PR_OBJ_DIR)/, $(notdir $(PR_SRCS:.cpp=.o)))
PR_DEPS		= $(PR_OBJS:.o=.d)
PR_LIBS		= $(LOADLIBES) $(NDNCXX_LIBS) $(OPENCV_LIBS) $(LDLIBS)

SRCS		= $(PR_SRCS) 

.PHONY: all clean depend

all: producer

producer: $(PRODUCER)


$(PRODUCER): $(PR_OBJS)
	@-mkdir -p $(TARGET_DIR)
	$(LINK.cc) $^ $(PR_LIBS) -o $@

$(PR_OBJ_DIR)/%.o: $(PR_SRC_DIR)/%.cpp	
	@-mkdir -p $(OBJ_DIR)
	@-mkdir -p $(PR_OBJ_DIR)
	$(COMPILE.cc) $< $(PR_CXXFLAGS) -o $@ 


clean:
	-$(RM) $(CONSUMER) $(PRODUCER)  \
			$(PR_OBJS) $(PR_DEPS) \
			*~ .*~ core

-include  $(PR_DEPS)
