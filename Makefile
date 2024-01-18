all:
	g++ -Ioqs_build/include -g -std=c++20 main.cpp passCompute.cpp -o passkeygen -lcrypto oqs_build/lib/liboqs.a -fsanitize=address

release:
	g++ -Ioqs_build/include -O3 -std=c++20 main.cpp passCompute.cpp -o passkeygen -lcrypto oqs_build/lib/liboqs.a

console_wasm:
	em++ -Ioqs_build/include -g -std=c++20 main.cpp passCompute.cpp -o passkeygen.wasm oqs_build/lib/liboqs.a WebCpp-Interaction-Lib/libInteraction.a -fsanitize=address

oqs:
	mkdir oqs_build;cd oqs_build;cmake -DOQS_USNSE_OPENSSL=OFF ../liboqs;make -j32

oqs_em:
	mkdir oqs_build;cd oqs_build;emcmake cmake -DOQS_USE_OPENSSL=OFF -DOQS_PERMIT_UNSUPPORTED_ARCHITECTURE=ON ../liboqs;make -j32

webcpp_em:
	cd WebCpp-Interaction-Lib;make -j32

all_em:
	em++ -Ioqs_build/include -g -std=c++20 em_main.cpp passCompute.cpp -o em_build/passkeygen.js oqs_build/lib/liboqs.a WebCpp-Interaction-Lib/libInteraction.a -s WASM=1 -s EXPORTED_RUNTIME_METHODS=ccall,cwrap -s NO_EXIT_RUNTIME -fsanitize=address

release_em:
	#SUPER JANK, but segfaults when address sanitizer is off
	em++ -Ioqs_build/include -O3 -std=c++20 em_main.cpp passCompute.cpp -o em_build/passkeygen.js oqs_build/lib/liboqs.a WebCpp-Interaction-Lib/libInteraction.a -s WASM=1 -s EXPORTED_RUNTIME_METHODS=ccall,cwrap -s NO_EXIT_RUNTIME -fsanitize=address

clean:
	rm -f passkeygen
	rm -rf -f oqs_build
	cd WebCpp-Interaction-Lib;make clean
	rm -f em_build/passkeygen.js em_build/passkeygen.wasm
	rm -f passkeygen.wasm
