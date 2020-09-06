INC_PATH=/usr/local/include
BOOST_ROOT=/usr/local/boost_1_73_0
CXX = clang++
CXXFLAGS = -Wall -g -std=c++17 -I$(BOOST_ROOT) -pthread

main: main.cpp
	$(CXX) $(CXXFLAGS) -o main main.cpp \
		/usr/local/boost_1_73_0/stage/lib/libboost_thread.a
