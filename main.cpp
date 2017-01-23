#include <string>
#include <iostream>

void anyKey();

std::string key = "94307803947898";

int main()
{

        std::string message =  "Sammy"; //value is exposed in memory

        for (int x = 0; x < message.size(); x++) { message[x] ^= key[x]; }//simple XOR encrypt with key (key should be as long as input, ensure key size on update)
        }

        //value is encrypted in memory

        std::cout << "Encrypted: " << message<< std::endl;
        anyKey();


        for (int x = 0; x < message.size(); x++) { message[x] ^= key[x]; }//simple XOR encrypt with key (key should be as long as input, ensure key size on update)

        std::cout << "Decrypted: " << message<< std::endl;//value is exposed in memory
        anyKey();

        std::cout<< "cleared decrypted value"; //need to re-decrypt to access again, needs to be cleared to get out of memory
        message = "";
        anyKey();

	}
	return 0;
}


void anyKey()
{
    std::cout << "Press any key + enter to continue..." << std::endl;
    std::string c;
    std::cin >> c;
    std::getline(std::cin, c);
}

