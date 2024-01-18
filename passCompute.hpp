#pragma once
#include <iostream>
#include <optional>
#include <vector>
#include <cstdint>

namespace passCompute{
	std::string toAskii(const std::vector<uint8_t>& vec, int wantedSize);
	std::optional<std::vector<uint8_t>> secureHash(const std::vector<uint8_t>& message);
	std::optional<std::string> computePassword(const std::string& passphrase, const std::optional<std::string>& platform, int passwordLength = 32);
}
