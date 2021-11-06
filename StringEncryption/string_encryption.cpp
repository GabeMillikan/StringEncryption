#include "string_encryption.hpp"

std::map<char*, std::unique_ptr<char>> StringEncryption::trackedStrings;
