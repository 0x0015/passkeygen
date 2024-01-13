all:
	g++ -g -std=c++20 main.cpp -o main -loqs -lcrypto

release:
	g++ -O3 -std=c++20 main.cpp -o main -loqs -lcrypto

clean:
	rm -f main
