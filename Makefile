EXE = console
CC = clang
CFLAGS = -Wall -c
LDFLAGS = -lcurl -ljson-c -lSDL2 -lSDL2_ttf -lSDL2_gfx
SRC = main.c oanda.c
OBJ = $(SRC:.c=.o)

all: $(SRC) $(EXE)

$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o && rm $(EXE)
