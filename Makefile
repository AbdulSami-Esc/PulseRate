all: pulseroute test_array test_hash test_heap test_graph test_loader test_routing test_fleet

test_fleet: tests/test_fleet.cpp include/RobotFleet.h include/ReverseIndex.h include/Robot.h include/Dijkstra.h include/Graph.h include/HashTable.h include/MinHeap.h include/DynamicArray.h
	$(CXX) $(CXXFLAGS) tests/test_fleet.cpp -o test_fleet

run-tests: test_array test_hash test_heap test_graph test_loader test_routing test_fleet
	./test_array
	./test_hash
	./test_heap
	./test_graph
	cd tests && ../test_loader
	./test_routing
	./test_fleet