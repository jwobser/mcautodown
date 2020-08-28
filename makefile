CXX = clang++
CXXFLAGS = -Wall -g -std=c++17

main: main.o rcon.o
	$(CXX) $(CXXFLAGS) -o main main.o rcon.o
main.o: main.cpp
rcon.o:
	$(CXX) $(CXXFLAGS) -c ./SourceRCON/src/srcon.cpp -o rcon.o

clean:
	rm main.o main rcon.o
