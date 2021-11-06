#include "string_encryption.hpp"
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>

uint32_t randint()
{
	static std::mt19937 engine(std::chrono::system_clock::now().time_since_epoch().count());
	static std::uniform_int_distribution<> distribution(-0x80000000i32, 0x7FFFFFFFi32);
	return (uint32_t)distribution(engine);
}

void encrypt(std::fstream& file)
{
	uint64_t start = file.tellg();
	uint32_t key = randint();

	char* keyparts = (char*)&key;

	char header[5];
	header[0] = '!';
	*(uint32_t*)(header + 1) = key;

	file.seekp(start);
	file.write(header, 5);

	file.seekg(file.tellp());

	bool stop = false;
	uint64_t offset = 0;
	char buffer[64];
	while (!stop)
	{
		file.seekg(start + 5 + offset);
		size_t bytesRead = file.read(buffer, 64).gcount();
		stop = bytesRead == 0;
		for (size_t o = 0; o < bytesRead; o++)
		{
			if (buffer[o] == '\0')
			{
				stop = true;
				bytesRead = o + 1;
			}
			buffer[o] ^= keyparts[o % 4];
		}

		file.seekp(start + 5 + offset);
		file.write(buffer, bytesRead);
		offset += bytesRead;
	}
}

int main(int argc, char* argv[])
{
	if (argc <= 1)
	{
		std::cout << "Must provide input file as command line argument." << std::endl;
		std::exit(1);
	}
	else if (argc > 2)
	{
		std::cout << "Must only provide input file. It will be modified in-place." << std::endl;
		std::exit(1);
	}

	std::fstream file(argv[1], std::ios::in | std::ios::out | std::ios::binary);
	if (!file.is_open())
	{
		std::cout << "Failed to open file: " << argv[1] << std::endl;
		std::exit(1);
	}

	int count = 0;
	char buffer[1024];
	while (true)
	{
		const uint64_t fposBefore = file.tellg();
		const size_t bytesRead = file.read(buffer, 1024).gcount();
		if (bytesRead < 6) break;


		bool found = false;
		for (size_t o = 0; o + 4 < bytesRead; o++)
		{
			if (!memcmp(buffer + o, ".@\x64\x42\0", 5))
			{
				file.seekg(fposBefore + o);
				std::cout << "Encrypting string starting at " << file.tellg() << std::endl;
				encrypt(file);
				found = true;
				break;
			}
		}

		if (!found)
			file.seekg((uint64_t)file.tellg() - 4);
		else
			count++;
	}

	file.close();
	std::cout << "Done! Encrypted a total of " << count << " strings!" << std::endl;
}