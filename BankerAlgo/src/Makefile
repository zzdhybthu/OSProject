CXX = g++
CXXFLAGS = -std=c++17 -pthread -O2

ARGS = "3,3,2" "0,1,0;2,0,0;3,0,2;2,1,1;0,0,2" "7,4,3;1,2,2;6,0,0;0,1,1;4,3,1" "1,0,2" "1"  # 成功分配
ARGS1 = "3,3,2" "0,1,0;2,0,0;3,0,2;2,1,1;0,0,2" "7,4,3;1,2,2;6,0,0;0,1,1;4,3,1" "0,0,2" "3"  # 请求资源超出最大需求
ARGS2 = "3,3,2" "0,1,0;2,0,0;3,0,2;2,1,1;0,0,2" "7,4,3;1,2,2;6,0,0;0,1,1;4,3,1" "4,3,1" "4"  # 请求资源超出可用资源
ARGS3 = "3,3,2" "0,1,0;2,0,0;3,0,2;2,0,0;0,3,0" "7,4,3;1,2,2;6,0,0;0,1,1;4,3,1" "0,0,1" "0"  # 请求资源后导致死锁

all:
	$(CXX) $(CXXFLAGS) banker_algo.cpp -o banker_algo
	./banker_algo $(ARGS)
	./banker_algo $(ARGS1)
	./banker_algo $(ARGS2)
	./banker_algo $(ARGS3)

debug:
	$(CXX) $(CXXFLAGS) -DDEBUG banker_algo.cpp -o banker_algo
	./banker_algo $(ARGS)
	./banker_algo $(ARGS1)
	./banker_algo $(ARGS2)
	./banker_algo $(ARGS3)

clean:
	rm -f banker_algo *.o test.in out.sim