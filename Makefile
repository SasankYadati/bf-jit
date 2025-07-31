CXX = g++
CXXFLAGS = -std=c++11 -Wall -O3 -g
DEFINES =
LDFLAGS =

EXECUTABLES = simpleinterp optinterp

all: $(EXECUTABLES)

COMMONFILES = utils.cpp utils.h parser.cpp parser.h

simpleinterp: $(COMMONFILES) simpleinterp.cpp
		$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

optinterp: $(COMMONFILES) optinterp.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f core $(EXECUTABLES) *.o *.gch

test:
	python3 tests/test_bf.py

.PHONY: clean test