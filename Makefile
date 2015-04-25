.PHONY: all

all: stttrandom stttfirst stttrecur stttswag viewcompetition

clean:
	rm stttrandom stttfirst stttrecur stttswag viewcompetition


stttrandom: stttrandom.cpp
	g++ -Wall -O2 -std=c++11 -o stttrandom stttrandom.cpp

stttfirst: stttfirst.cpp
	g++ -Wall -O2 -std=c++11 -o stttfirst stttfirst.cpp

stttrecur: stttrecur.cpp
	g++ -Wall -O2 -std=c++11 -o stttrecur stttrecur.cpp

stttswag: stttswag.cc
	g++ -Wall -O2 -std=c++11 -o stttswag stttswag.cc

viewcompetition: viewcompetition.cpp
	g++ -Wall -O2 -std=c++11 -o viewcompetition viewcompetition.cpp
