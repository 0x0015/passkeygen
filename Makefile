all:
	g++ -g -std=c++20 main.cpp -o main -lcrypto oqs_build/lib/liboqs.a

release:
	g++ -O3 -std=c++20 main.cpp -o main -lcrypto oqs_build/lib/liboqs.a

oqs:
	mkdir oqs_build;cd oqs_build;cmake ../liboqs -DOQS_USNSE_OPENSSL=OFF;make -j

clean:
	rm -f main
	rm -rf -f oqs_build
