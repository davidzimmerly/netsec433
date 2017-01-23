#include <string>
#include <iostream>

int key = 123456789;

class encrypt_ram {
	private:
		std::string key = "94307803947898";
	public:
		std::string name = "sammy";
		std::string encrypt(std::string value) {  //simple XOR stream cipher, key needs to be as long as value (need to pad if not)
			for (int x = 0; x < value.size(); x++) {
				value[x] ^= key[x];
			}
			return value;
		}
		std::string decrypt(std::string value) {
			return encrypt(value);
		}
};

void anyKey();

int main()
{

	//int* newFindMe = new int(94875);
	encrypt_ram* er = new encrypt_ram();
	anyKey();
	while (1) {
		//so this repeatedly encrypts/decrypts, and shows the problem of multiple instances of "sammy" in memory after multiple iterations.

		std::string temp = "not set";
		std::cout << "Value: " << er->name<< std::endl;
		er->name  = er->encrypt(er->name);
		std::cout << "Encrypted Value: " << er->name<< std::endl;



		anyKey();


	}
	delete er;
	return 0;
}


void anyKey()
{
    std::cout << "Press any key + enter to continue..." << std::endl;
    std::string c;
    std::cin >> c;
    std::getline(std::cin, c);
}

