EXE = console

CXX = clang
CXXFLAGS = -Wall -c
LDFLAGS = -lcurl -ljson-c -lSDL2 -lSDL2_ttf -lSDL2_gfx

all: $(EXE)

$(EXE): main.o
	$(CXX) $(LDFLAGS) $< -o $@

main.o: main.c
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm *.o && rm $(EXE)
