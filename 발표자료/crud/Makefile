NAME = test

CFLAGS = -Wall -Wextra -Werror -w

INCS = -I/usr/include/libmongoc-1.0 -I/usr/include/libbson-1.0 -lmongoc-1.0 -lbson-1.0

SRCS = crud.c cJSON.c
OBJS = $(SRCS:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS)  -c $< -o $@ $(INCS)

all: $(NAME)

$(NAME): $(OBJS) 
	gcc $(CFLAGS)  $(OBJS) -o $@ $(INCS) 

run: $(NAME)
	./test

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean
	make

.phony: all clean fclean re