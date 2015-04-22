.PHONY: all

all: stttrandom stttfirst stttrecur viewcompetition

clean:
	rm stttrandom stttfirst stttrecur viewcompetition


stttrandom: stttrandom.cpp
	g++ -Wall -O2 -std=c++11 -o stttrandom stttrandom.cpp

stttfirst: stttfirst.cpp
	g++ -Wall -O2 -std=c++11 -o stttfirst stttfirst.cpp

stttrecur: stttrecur.cpp
	g++ -Wall -O2 -std=c++11 -o stttrecur stttrecur.cpp

viewcompetition: viewcompetition.cpp
	g++ -Wall -O2 -std=c++11 -o viewcompetition viewcompetition.cpp
