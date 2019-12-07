PROGRAM = main
CC = gcc
FLAGS = -Wall
LIBS = -lGL -lGLU -lglut -lm

$(PROGRAM): main.o	
	$(CC) $(FLAGS) -o $(PROGRAM) main.o $(LIBS)	

.PHONY:
	clean

clean:
	-rm *.o $(PROGRAM)
