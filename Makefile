all: compile link
compile:
	g++ -I/opt/homebrew/Cellar/sfml/2.5.1_2/include -c main.cpp

link:
	g++ main.o -o main -L/opt/homebrew/Cellar/sfml/2.5.1_2/lib -lsfml-graphics -lsfml-window -lsfml-system