CXX = clang++
CXXFLAGS = -Wall -g -std=c++17

main: main.o
	$(CXX) $(CXXFLAGS) -o main main.o 
main.o: main.cpp

clean:
	rm main.o main