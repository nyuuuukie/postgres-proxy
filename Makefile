.PHONY: clean fclean re all makedir

NAME = pg_proxy

###################################################################################
#                               Compiler & Flags                                  #
###################################################################################

CXX       =   clang++
CPPFLAGS  =   -Wall -Wextra -Werror -std=c++17

ifeq ($(shell uname), Linux)
	ifeq ($(DEBUG), 1)
		CPPFLAGS += -fstandalone-debug -g
	endif
else
	ifeq ($(DEBUG), 1)
		CPPFLAGS += -g
	endif
endif

###################################################################################
#                              Directories & Files                                #
###################################################################################

SRCS_DIR     = src
OBJS_DIR     = .obj
DEPS_DIR     = .deps
INCLUDE_DIR  = include

SRCS     =  $(shell ls ${SRCS_DIR})

OBJS = $(addprefix $(OBJS_DIR)/, $(SRCS:.cpp=.o))
DEPS = $(addprefix $(DEPS_DIR)/, $(SRCS:.cpp=.d))

###################################################################################
#                                   Commands                                      #
###################################################################################

all: makedir $(NAME)

makedir:
	@if ! [ -d ${OBJS_DIR} ] ; then mkdir ${OBJS_DIR} ; fi
	@if ! [ -d ${DEPS_DIR} ] ; then mkdir ${DEPS_DIR} ; fi


$(NAME): $(OBJS)
	$(CXX) $(CPPFLAGS) $^ -o $@ $(LDFLAGS)

-include $(DEPS)
$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp	
	$(CXX) $(CPPFLAGS) -c $< -o $@ -I $(INCLUDE_DIR) -MMD -MF $(patsubst ${OBJS_DIR}/%.o, ${DEPS_DIR}/%.d, $@)

clean:
	rm -rf ${DEPS_DIR} ${OBJS_DIR} ${LOGS_DIR}

fclean: clean
	rm -rf $(NAME)

re: fclean all
