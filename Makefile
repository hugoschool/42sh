##
## EPITECH PROJECT, 2024
## B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
## File description:
## Makefile
##

SRC	=	src/ast/command_parser.c	\
		src/ast/create_ast.c	\
		src/ast/execute_pipe.c	\
		src/ast/execute_ast.c	\
		src/ast/parser_ast.c	\
		src/ast/pipeline_parser.c	\
		src/ast/setup_redirection.c	\
		src/commands/env.c	\
		src/commands/executor.c	\
		src/commands/my_alias.c	\
		src/commands/my_cd.c	\
		src/commands/my_exit.c	\
		src/commands/my_getenv.c	\
		src/commands/my_setenv.c	\
		src/commands/my_unsetenv.c	\
		src/pipe/redirection.c	\
		src/pipe/tokenize_with_quotes.c	\
		src/pipe/tokenize.c	\
		src/utilities/char_utilities.c	\
		src/utilities/concat_args.c	\
		src/utilities/error_handling.c	\
		src/utilities/frees.c	\
		src/utilities/print_help.c	\
		src/args_parser.c	\
		src/command_struct.c	\
		src/path_handler.c


MAIN_SRC	= main.c

OBJ	=	$(SRC:.c=.o) $(MAIN_SRC:.c=.o)

NAME	=	42sh

CFLAGS += -Wall -Wextra -pedantic
CPPFLAGS	+= -Iinclude/
ifeq ($(ENV), dev)
	CFLAGS	+=	-g3
endif

all: $(NAME)

$(NAME): $(OBJ)
	gcc -o $(NAME) $(OBJ)

clean:
	$(RM) $(OBJ) $(LIB_OBJ)

fclean: clean
	$(RM) $(NAME)
	$(RM) unit_tests *.gcno *.gcda

re: fclean all

coding_style:
	coding-style . > /dev/null
	cat coding-style-reports.log
	$(RM) coding-style-reports.log

unit_tests: fclean all
	gcc -o unit_tests tests/test_mysh.c $(SRC) -Iinclude/ \
	--coverage -lcriterion
	./unit_tests

gcovr: unit_tests
	gcovr --exclude tests --gcov-ignore-parse-errors=negative_hits.warn
	$(RM) unit_tests *.gcno *.gcda

func_tests: fclean all
	cd tester && sh tester.sh && cd ..

tests: unit_tests func_tests

.PHONY: all clean fclean re coding_style unit_tests gcovr func_tests tests
