TARGETS=connect

all: $(TARGETS)
clean:
	rm -f $(TARGETS)

connect: connect.cpp
	g++ -g -o $@ $<


test: Utility.cpp test.cpp
	g++ -o main_test Utility.cpp test.cpp

