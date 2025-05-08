CXX = g++
CXXFLAGS = -std=c++17 -pthread -O2

all:
	$(CXX) $(CXXFLAGS) -o generator generator.cpp
	$(CXX) $(CXXFLAGS) -o quick_sort quick_sort.cpp
	$(CXX) $(CXXFLAGS) -o verify verify.cpp
	./generator 1000000 > test.in
	./quick_sort test.in 1000 20 > out.sim
	./verify test.in out.sim

debug:
	$(CXX) $(CXXFLAGS) -DDEBUG -o generator generator.cpp
	$(CXX) $(CXXFLAGS) -DDEBUG -o quick_sort quick_sort.cpp
	./generator 30 > test.in
	./quick_sort test.in 5 4

clean:
	rm -f generator quick_sort verify *.o test.in out.sim