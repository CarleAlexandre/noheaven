NAME		=	noHeaven

SERVER_NAME	=	server

BUILDDIR	=	build/

CC			=	gcc

CFLAGS		=	-Wall -Wextra -pedantic #-std=c++98 -O3

OBJ		=	$(SRC:%.cpp=%.o)

SERVER_OBJ	=	$(SERVER_SRC:%.cpp=%.o)

SRC		+=	source/engine.cpp

SERVER_SRC	+=	source/server.cpp

#$(wildcard ./source/*.cpp)

INCLUDE		=	-I C:/mingw64/include #-I C:/Users/emilia/Desktop/fat_std/include

ifeq ($(OS), Windows_NT)
LIBS		=	-lraylib -lopengl32 -lgdi32 -lwinmm -lstdc++
endif
ifeq ($(shell uname -s), Linux)
LIBS		=	-lraylib -lGL -lm -lpthread -ldl -lrt -lX11 lstdc++
endif

$(BUILDDIR)$(NAME)		:	$(OBJ)
		mkdir -p $(BUILDDIR)
		$(CC) $(OBJ) ${LIBS} -g -o $@

#add this above after lib to have no console: <-mwindows -Wl,--subsystem,windows>

$(OBJ)		:	%.o	:	%.cpp
		$(CC) $(CFLAGS) ${INCLUDE} -c $< -o $@

$(SERVER_NAME)	:	$(OBJ)
		$(CC) $(SERVER_OBJ) ${SERVER_LIBS} -o $@

$(SERVER_OBJ)	:	%.o	:	%.cpp
		$(CC) $(CFLAGS) ${INCLUDE} -c $< -o $@

all		:	$(BUILDDIR)$(NAME)

clean		:
		rm -rf $(OBJ)

fclean		:	clean
		rm -rf $(BUILDDIR)

re		:	fclean all

