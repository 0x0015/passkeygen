#include <iostream>
#include <termios.h>
#include <unistd.h>

#include "passCompute.hpp"

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
	std::cout<<"Enter passphrase: "<<std::flush;
	std::string pswd = getPasswordFromConsole();
	std::cout<<std::endl;
	if(pswd.empty()){
		std::cerr<<"No passphrase was entered!"<<std::endl;
		return 1;
	}
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

	auto passphrase = passCompute::computePassword(pswd, platform.empty() ? std::nullopt : (std::optional<std::string>)platform, passwordLen);

	if(!passphrase)
		return -1;

	std::cout<<"Got hash: "<<*passphrase<<std::endl;
	return 0;
}
