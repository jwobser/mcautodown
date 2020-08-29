CXX = clang++
CXXFLAGS = -Wall -g -std=c++17

main: main.o ./SourceRCON/bin/rcon.o
	$(CXX) $(CXXFLAGS) -o main main.o ./SourceRCON/bin/rcon.o
main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

SourceRCON/bin/rcon.o: SourceRCON/include/srcon.h SourceRCON/src/srcon.cpp
	$(CXX) $(CXXFLAGS) -c ./SourceRCON/src/srcon.cpp -o ./SourceRCON/bin/rcon.o

clean:
	rm main.o main rcon.o
