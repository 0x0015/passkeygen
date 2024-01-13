#include <iostream>
#include <vector>
#include <optional>
#include <cstring>
#include <termios.h>
#include <unistd.h>

#include <oqs/oqs.h>

void cleanup_stack(uint8_t *secret_key, size_t secret_key_len) {
	OQS_MEM_cleanse(secret_key, secret_key_len);
}

void cleanup_heap(uint8_t *public_key, uint8_t *secret_key,
                  uint8_t *message, uint8_t *signature,
                  OQS_SIG *sig) {
	if (sig != NULL) {
		OQS_MEM_secure_free(secret_key, sig->length_secret_key);
	}
	OQS_MEM_insecure_free(public_key);
	OQS_MEM_insecure_free(message);
	OQS_MEM_insecure_free(signature);
	OQS_SIG_free(sig);
}

std::optional<std::vector<uint8_t>> secureHash(const std::vector<uint8_t>& message) {

	OQS_SIG *sig = nullptr;
	uint8_t *public_key = nullptr;
	uint8_t *secret_key = nullptr;
	uint8_t *signature = nullptr;
	uint8_t *message_raw = nullptr;
	size_t signature_len;
	OQS_STATUS rc;

	sig = OQS_SIG_new(OQS_SIG_alg_dilithium_3);
	if (sig == NULL) {
		printf("[example_heap]  OQS_SIG_alg_dilithium_3 was not enabled at compile-time.\n");
		return std::nullopt;
	}

	public_key = (uint8_t*) malloc(sig->length_public_key);
	secret_key = (uint8_t*) malloc(sig->length_secret_key);
	signature = (uint8_t*) malloc(sig->length_signature);
	message_raw = (uint8_t*) malloc(message.size());
	if ((public_key == nullptr) || (secret_key == nullptr) || (signature == nullptr) || (message_raw == nullptr)) {
		std::cerr<<"ERROR: malloc failed!"<<std::endl;
		cleanup_heap(public_key, secret_key, message_raw, signature, sig);
		return std::nullopt;
	}

	std::memcpy(message_raw, message.data(), message.size());
	// let's not create a random test message to sign
	//OQS_randombytes(message, message_len);

	//static const std::vector<uint8_t>* mes_static = &message;
	//OQS_randombytes_custom_algorithm([](uint8_t* buf, size_t len){
	//	for(unsigned int i=0;i<len;i++)
	//		buf[i] = (*mes_static)[i % mes_static->size()];
	//});
	rc = OQS_SIG_keypair(sig, public_key, secret_key);
	
	if (rc != OQS_SUCCESS) {
		std::cerr<<"ERROR: OQS_SIG_keypair failed!"<<std::endl;
		cleanup_heap(public_key, secret_key, message_raw, signature, sig);
		return std::nullopt;
	}
	rc = OQS_SIG_sign(sig, signature, &signature_len, message_raw, message.size(), secret_key);
	if (rc != OQS_SUCCESS) {
		std::cerr<<"ERROR: OQS_SIG_sign failed!"<<std::endl;
		cleanup_heap(public_key, secret_key, message_raw, signature, sig);
		return std::nullopt;
	}
	rc = OQS_SIG_verify(sig, message_raw, message.size(), signature, signature_len, public_key);
	if (rc != OQS_SUCCESS) {
		std::cerr<<"ERROR: OQS_SIG_verify failed!"<<std::endl;
		cleanup_heap(public_key, secret_key, message_raw, signature, sig);
		return std::nullopt;
	}

	printf("OQS_SIG_dilithium_3 operations completed.\n");
	cleanup_heap(public_key, secret_key, message_raw, signature, sig);

	//OQS_randombytes_switch_algorithm(OQS_RAND_alg_system);
	return std::vector<uint8_t>(signature, signature+signature_len); // success
}

std::string constructAskiiChars(bool lower = true, bool capital = true, bool numbers = true, bool numberShiftSyms = true, bool extraCommon = true, const std::string& extra = "()[]{}/"){
	std::string allowedChars;
	if(lower)
		allowedChars += "abcdefghijklmnopqrstuvwxyz";
	if(capital)
		allowedChars += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if(numbers)
		allowedChars += "1234567890";
	if(numberShiftSyms)
		allowedChars += "!@#$%^&*";
	if(extraCommon)
		allowedChars += "+-=?_";
	
	allowedChars += extra;

	return allowedChars;
}



uint64_t generateRandomU64(){
	union u64vec{
		uint64_t num;
		uint8_t vec[8];
	};
	u64vec vec;
	OQS_randombytes(vec.vec, 8);
	return vec.num;
}

std::string toAskii(const std::vector<uint8_t>& vec, int wantedSize){
	const uint64_t* vec_64 = (const uint64_t*)vec.data();
	int vec64_size = vec.size() / 8;
	std::string generalMap = constructAskiiChars();

	std::string lowerMap = constructAskiiChars(true, false, false, false, false, "");
	std::string upperMap = constructAskiiChars(false, true, false, false, false, "");
	std::string specialMap = constructAskiiChars(false, false, false, true, true, "");
	std::string numberMap = constructAskiiChars(false, false, true, false, false, "");

	std::string output;
	output.resize(std::min(vec64_size, wantedSize));

	uint64_t requiredUpper = generateRandomU64() % output.size();
	uint64_t requiredLower = generateRandomU64() % output.size();
	while(requiredLower == requiredUpper) requiredLower = generateRandomU64() % output.size();
	uint64_t requiredSpecial = generateRandomU64() % output.size();
	while(requiredSpecial == requiredUpper || requiredSpecial == requiredLower) requiredSpecial = generateRandomU64() % output.size();
	uint64_t requiredNumber = generateRandomU64() % output.size();
	while(requiredNumber == requiredUpper || requiredNumber == requiredLower || requiredNumber == requiredSpecial) requiredNumber = generateRandomU64() % output.size();


	for(unsigned int i=0;i<output.size();i++){
		if(i == requiredUpper){
			output[i] = upperMap[vec_64[i] % upperMap.size()];
		}else if(i == requiredLower){
			output[i] = lowerMap[vec_64[i] % lowerMap.size()];
		}else if(i == requiredSpecial){
			output[i] = specialMap[vec_64[i] % specialMap.size()];
		}else if(i == requiredNumber){
			output[i] = numberMap[vec_64[i] % numberMap.size()];
		}else{
			output[i] = generalMap[vec_64[i] % generalMap.size()];
		}
	}

	return output;
}

int getch() {
    int ch;
    // struct to hold the terminal settings
    struct termios old_settings, new_settings;
    // take default setting in old_settings
    tcgetattr(STDIN_FILENO, &old_settings);
    // make of copy of it (Read my previous blog to know 
    // more about how to copy struct)
    new_settings = old_settings;
    // change the settings for by disabling ECHO mode
    // read man page of termios.h for more settings info
    new_settings.c_lflag &= ~(ICANON | ECHO);
    // apply these new settings
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
    // now take the input in this mode
    ch = getchar();
    // reset back to default settings
    tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);
    return ch;
}

std::string getPasswordFromConsole(){	
	int ch;
	std::string password;
	while ((ch = getch()) != '\n') {
	        if (ch == 127 || ch == 8) { // handle backspace
	            if (password.size() != 0) {
	                password.pop_back();
	                printf("\b \b");
	            }
	        } else {
	            password += (char) ch;
	            // echo the '*' to get feel of taking password 
	            printf("*");
	        }
	    }
	return password;
}

int main(int argc, char** argv){
	std::cout<<"Enter platform/program: "<<std::flush;
	std::string platform;
	std::getline(std::cin, platform);
	std::cout<<"Enter keyphrase: "<<std::flush;
	std::string pswd = getPasswordFromConsole();
	std::cout<<std::endl;
	std::cout<<"Enter password length [default=32]: "<<std::flush;
	std::string passwordLenStr;
	std::getline(std::cin, passwordLenStr);
	int passwordLen = 32;
	if(!passwordLenStr.empty()){
		try{
			passwordLen = std::stoi(passwordLenStr);
		}catch(std::exception& e){
			std::cerr<<"Unable to convert \""<<passwordLenStr<<"\" to number ("<<e.what()<<")"<<std::endl;
			return 1;
		}
	}

	std::string myMessage = pswd;
	if(!platform.empty())
		pswd += "_" + platform;

	static const uint8_t* mes_static = (uint8_t*) myMessage.data();
	static const auto mes_size = myMessage.size();
	OQS_randombytes_custom_algorithm([](uint8_t* buf, size_t len){
		for(unsigned int i=0;i<len;i++)
			buf[i] = mes_static[i % mes_size];
	});
	unsigned char entropy[48];
	unsigned char personalization[128];
	OQS_randombytes(entropy, 48);
	OQS_randombytes(personalization, 127);
	personalization[127] = '\0';
	OQS_randombytes_nist_kat_init_256bit(entropy, personalization);
	OQS_randombytes_switch_algorithm(OQS_RAND_alg_nist_kat);

	std::vector<uint8_t> message_vec(myMessage.begin(), myMessage.end());
	auto ret = secureHash(message_vec);
	if(!ret){
		std::cerr<<"An error occurred while hashing password"<<std::endl;
		return 1;
	}else{
		std::string output = toAskii(*ret, passwordLen);
		std::cout<<"Got hash: "<<output<<std::endl;
		return 0;
	}
}
