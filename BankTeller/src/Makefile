CXX = g++
CXXFLAGS = -std=c++17 -pthread -O2

all:
	$(CXX) $(CXXFLAGS) -o generator generator.cpp
	$(CXX) $(CXXFLAGS) -o bank_teller bank_teller.cpp
	$(CXX) $(CXXFLAGS) -o judge judge.cpp
	./bank_teller 2 test0.in > out.sim && ./judge out.sim
	./generator 100 1 20 1 5 > test.in  && ./bank_teller 4 test.in > out.sim && ./judge out.sim
	./generator 100 1 100 1 5 > test.in  && ./bank_teller 20 test.in > out.sim && ./judge out.sim

debug:
	$(CXX) $(CXXFLAGS) -DDEBUG -o generator generator.cpp
	$(CXX) $(CXXFLAGS) -DDEBUG -o bank_teller bank_teller.cpp
	./bank_teller 2 test0.in

timer:
	$(CXX) $(CXXFLAGS) -o generator generator.cpp
	$(CXX) $(CXXFLAGS) -o bank_teller bank_teller.cpp
	./generator 100 1 20 1 5 > test.in  && ./bank_teller 1 test.in > out.sim
	./generator 100 1 20 1 5 > test.in  && ./bank_teller 2 test.in >> out.sim
	./generator 100 1 20 1 5 > test.in  && ./bank_teller 4 test.in >> out.sim
	./generator 100 1 20 1 5 > test.in  && ./bank_teller 8 test.in >> out.sim
	./generator 100 1 20 1 5 > test.in  && ./bank_teller 16 test.in >> out.sim
	./generator 100 1 20 1 5 > test.in  && ./bank_teller 32 test.in >> out.sim
	./generator 100 1 20 1 5 > test.in  && ./bank_teller 64 test.in >> out.sim
	./generator 100 1 20 1 5 > test.in  && ./bank_teller 128 test.in >> out.sim
	./generator 4 1 20 1 5 > test.in  && ./bank_teller 10 test.in >> out.sim
	./generator 8 1 20 1 5 > test.in  && ./bank_teller 10 test.in >> out.sim
	./generator 16 1 20 1 5 > test.in  && ./bank_teller 10 test.in >> out.sim
	./generator 32 1 20 1 5 > test.in  && ./bank_teller 10 test.in >> out.sim
	./generator 64 1 20 1 5 > test.in  && ./bank_teller 10 test.in >> out.sim
	./generator 128 1 20 1 5 > test.in  && ./bank_teller 10 test.in >> out.sim
	./generator 256 1 20 1 5 > test.in  && ./bank_teller 10 test.in >> out.sim
	./generator 512 1 20 1 5 > test.in  && ./bank_teller 10 test.in >> out.sim


clean:
	rm -f generator bank_teller judge *.o test.in out.sim