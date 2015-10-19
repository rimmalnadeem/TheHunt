CC = gcc
CFLAGS = -Wall -Werror -g -std=c99
BINS = testGameView testHunterView testDracView

all : $(BINS)

testGameView : testGameView.o GameView.o Map.o Places.o queue.o
testGameView.o : testGameView.c Globals.h Game.h

testHunterView : testHunterView.o HunterView.o GameView.o Map.o Places.o queue.o
testHunterView.o : testHunterView.c GameView.c Map.c Places.h queue.h

testDracView : testDracView.o DracView.o GameView.o Map.o Places.o queue.o
testDracView.o : testDracView.c GameView.c Map.c Places.h queue.h

queue.o : queue.c queue.h
Places.o : Places.c Places.h
Map.o : Map.c Map.h Places.h
GameView.o : GameView.c GameView.h
HunterView.o : HunterView.c HunterView.h
DracView.o : DracView.c DracView.h

clean :
        rm -f $(BINS) *.o core
