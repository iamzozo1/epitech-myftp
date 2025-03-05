##
## EPITECH PROJECT, 2023
## B-PSU-200-PAR-2-1-minishell1-nicolas.nunney
## File description:
## Makefile
##

CC = g++

SRC	=	src/Server.cpp	\
		src/Core.cpp	\
		src/Error.cpp	\
		src/wrapped/Socket.cpp	\
		src/wrapped/Poll.cpp	\
		src/ClientData.cpp	\

MAIN =	src/main.cpp	\

UT_MAIN	=	tests/unit_tests.cpp	\

OBJ	=	$(SRC:.cpp=.o)

MAIN_OBJ = $(MAIN:.cpp=.o)

CFLAGS	=	-Wall -Wextra -std=c++17

CPPFLAGS	=	-I include/

LDFLAGS =

LDLIBS	=

LIBNAME	=

CS_CLEAN = *.log

UT_BIN	=	unit_tests

UT_FLAGS	=	--coverage -lcriterion

UT_CLEAN	=	*.gc*

VAL_CLEAN	=	vgcore*

CS_REPORT	=	coding-style-reports.log

NAME	=	myftp

RM	=	rm -f

all:	$(NAME)

$(NAME):	$(OBJ) $(MAIN_OBJ)
	$(CC) -o $(NAME) $(OBJ) $(MAIN_OBJ) $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) $(OBJ)
	$(RM) unit_test-*

fclean:	clean
	$(RM) $(NAME) $(TNAME)
	$(RM) $(NAME)
	$(RM) $(CS_CLEAN)
	$(RM) $(UT_CLEAN)
	$(RM) $(UT_BIN)
	$(RM) $(VAL_CLEAN)

re:	fclean all

valgrind: CFLAGS += -g3
valgrind: re

asan:	CC	=	clang -fsanitize=address
asan:	CFLAGS += -g3
asan:	re

gdb: valgrind
	gdb -ex "run" -ex "bt full" -ex "detach" -ex "quit" $(NAME)

coding_style:	fclean
	coding-style . . > /dev/null 2>&1
	cat $(CS_REPORT)
	make fclean  > /dev/null 2>&1

tests_run:
	$(CC) -I include/ -o $(UT_BIN) $(UT_MAIN) $(SRC) $(UT_FLAGS)
	./$(UT_BIN)

coverage:
	gcovr --exclude tests/ --exclude src/useful_funcs

.PHONY:
	all libb clean fclean re debug coding_style tests_run coverage

.SILENT :
	coding_style
