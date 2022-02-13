TARGETS=connect

all: $(TARGETS)
clean:
	rm -f $(TARGETS)

connect: connect.cpp
	g++ -g -o $@ $<


test: Utility.cpp test.cpp test_server.cpp
	g++ -g -ggdb3 -o main_test Utility.cpp test.cpp
	g++ -g -ggdb3 -o main_server Utility.cpp test_server.cpp
