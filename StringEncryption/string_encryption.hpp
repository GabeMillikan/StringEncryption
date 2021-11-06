#pragma once
#include <cstdint>
#include <map>
#include <memory>

namespace StringEncryption {
	extern std::map<char*, std::unique_ptr<char>> trackedStrings;

	template< size_t N_STR_BYTES >
	const char* get_runtime(char const (&stringLiteral)[N_STR_BYTES])
	{
		volatile char* vpLiteral = (volatile char*)stringLiteral;
		char* pLiteral = (char*)stringLiteral;

		const auto& alreadyDecoded = trackedStrings.find(pLiteral);
		if (alreadyDecoded != trackedStrings.end())
		{
			// we've already decoded this string!
			return alreadyDecoded->second.get();
		}
		else
		{
			// we still need to decode this one
			char* decoded = new char[N_STR_BYTES - 5];

			// was the string even encrypted?
			if (vpLiteral[0] == '.') // nope
			{
				memcpy(decoded, stringLiteral + 5, N_STR_BYTES - 5);
			}
			else // yep
			{
				const uint32_t key = *(volatile uint32_t*)(vpLiteral + 1);
				char* keyparts = (char*)&key;
			
				for (size_t i = 0; i < N_STR_BYTES - 5; i++)
				{
					decoded[i] = vpLiteral[i + 5] ^ keyparts[i % 4];
				}
			}

			trackedStrings.insert(std::make_pair(pLiteral, decoded));
			return decoded;
		}
	}
}

#define STRXOR(str) (StringEncryption::get_runtime(".@\x64\x42\0" str))