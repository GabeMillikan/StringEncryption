# StringEncryption
C++ way to hide strings in compiled binary. This makes it more difficult for users to modify the executable, since it's now impossible to search for a string like "company.com/product_registration.php" then modify the surrounding code.

# Example Output
Before

![before](https://i.imgur.com/Uv4UP8Z.png)

After

![after](https://i.imgur.com/2qFDdVf.png)

# Usage
1. Build `exampleProgram.exe`
```cpp
#include <iostream>
#include "string_encryption.hpp"

int main()
{
  std::cout << STRXOR("This string is not visible in the executable binary.") << std::endl;
}
```

2. Run the encryption engine. This step is only required in the final release. The program will work just fine without this step, but the strings will not be encrypted.
```sh
StringEncryption path/to/exampleProgram.exe
```

# Note
This will only work on string literals that do not contain a null character. Those that do contain `\0` should be split into multiple strings that do not contain it.

# The "Encryption" Method
This program does not use a cryptographic encoding, it is meerly a 4-byte XOR rolling across the string. This is sufficient for preventing strings fromw showing up in the binary, and is very computationally efficient (which is important, because the strings are decoded at runtime). 

# How it Works
1. `STRXOR` prepends this 'header' to your string `".@\x69\42\0"` and sets up a runtime call to `get_runtime` which executes on step 4.
2. `StringEncryption.exe` finds occurrences of that header in the program binary.
3. `StringEncryption.exe` replaces the header with `!{the 4 byte XOR key}` then applies the XOR to the program binary until the null terminator is found.
4. At runtime, in your program, `StringEncryption::get_runtime` allocates a new string on the heap and copies the (now encrypted) data from the program binary. 
5. `get_runtime` determines if the header starts with `.` or `!`, if the former, then the string was never encrypted at all, and the original string is simply returned. If the latter, then the XOR key is read from the header and the remainder of the string is decrypted by re-xor-ing it, and the new string is returned.
6. `::StringEncryption` automatically de-allocates all of the strings that it has put on the heap once the program terminates.
