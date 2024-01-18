#include <iostream>

#include "WebCpp-Interaction-Lib/DOM.hpp"
#include "passCompute.hpp"

std::optional<std::string> computeAndSetOutput(){
	std::shared_ptr<Element> platform = Element::getById("platformInput");
	std::shared_ptr<Element> passphrase = Element::getById("passphraseInput");
	std::shared_ptr<Element> output = Element::getById("hashOutput");

	if(passphrase->get_dom_value().empty()){
		output->dom_innerHTML = "Error: no passphrase was entered";
		return std::nullopt;
	}
	//probably don't print the password to console
	//std::cout<<"computing passhash: platform: "<<platform->get_dom_value()<<", passphrase: "<<passphrase->get_dom_value()<<std::endl;
	auto passHash = passCompute::computePassword(passphrase->dom_value, (std::optional<std::string>) (((std::string)platform->dom_value).empty() ? std::nullopt : (std::optional<std::string>) platform->dom_value));

	if(passHash){
		output->dom_innerHTML = (std::string) *passHash;
		return passHash;
	}else{
		output->dom_innerHTML = "Error: unable to compute passphrase";
		return std::nullopt;
	}
}

void computePassButton(){
	computeAndSetOutput();
}

void computeAndCopyButton(){
	auto hash = computeAndSetOutput();
	if(hash){
		JS_Man jsCaller;
		std::string toCall = "navigator.clipboard.writeText(\"" + *hash + "\");";
		jsCaller.evalJS(toCall);
	}
}

int main(int argc, char** argv){
	std::shared_ptr<Element> computeButton = Element::getById("computeButton");
	computeButton->dom_onclick = computePassButton;
	std::shared_ptr<Element> computeCopyButton = Element::getById("computeCopyButton");
	computeCopyButton->dom_onclick = computeAndCopyButton;
	{
		//I hate this so much, but couldn't figure out a better way to do it
		//C++ interaction lib just doesn't have onkeypress events implemented, which makes using the rest of the systems a lot harder (as it only really wants to be used with itself)
		JS_Man jsCaller;
		std::string toCall = "document.getElementById('passphraseInput').onkeypress = ((event) => {if(event.key == 'Enter'){document.getElementById('computeButton').click()}});";
		jsCaller.evalJS(toCall);
	}

	std::cout<<"passwordGenerator Initialized"<<std::endl;
}
