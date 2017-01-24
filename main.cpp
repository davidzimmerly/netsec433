#include <string>
#include <iostream>

bool anyKey();
void bitwiseSplit(unsigned int input, unsigned short &leftDigits, unsigned short &rightDigits);

class encrypt_ram{
    private:
        std::string key = "94307803947898";
    public:
        void encrypt(std::string &message){
            for (int x = 0; x < message.size(); x++) { message[x] ^= key[x]; }//simple XOR encrypt with key (key should be as long as input, ensure key size on update)
        }
        void decrypt(std::string &message){
           encrypt(message);
        }

};

unsigned int testInput = 43522;//10101010 00000010

int main()
{
    bool done = false;
    encrypt_ram* er = new encrypt_ram();
    while(!done){

        std::string message =  "Sammy"; //value is exposed in memory
        er->encrypt(message);

        //value is encrypted in memory

        std::cout << "Encrypted: " << message<< std::endl;
        done = anyKey();

        if (!done){
            er->decrypt(message);

            //value is exposed in memory

            std::cout << "Decrypted: " << message<< std::endl;
            done = anyKey();
        }

        //need to re-decrypt to access again, needs to be cleared to get out of memory
        if (!done){
            std::cout<< "re encrypt decrypted value"<< std::endl;
            er->encrypt(message);
            done = anyKey();
        }
    }
    //unsigned short ld, rd;
    //bitwiseSplit(testInput,ld,rd);
    delete er;
	return 0;
}


//we probably want some functionality to clear all variable values on exit??

bool anyKey()
{
    std::cout << "Press any key + enter to continue...x/exit to exit" << std::endl;
    std::string c;
    std::cin >> c;

    if (c=="x" || c=="exit"){
        std::getline(std::cin, c);
        return true;
    }
    else{
        std::getline(std::cin, c);
        return false;
    }

}


void bitwiseSplit(unsigned int input, unsigned short &leftDigits, unsigned short &rightDigits){
    //splits 16 bit unsigned integer into two 8 bit shorts, the left and right containing respective bits
    leftDigits = input >> 8;  // this 8 is the split value of # of bits you have, we started with 16 so 16/2=8
    rightDigits = input & 255; // this is the max value for the lower end, so 2^split value-1=2^8-1=255
    //std::cout << "leftDigits: " << leftDigits << std::endl;
    //std::cout << "rightDigits: " << rightDigits << std::endl;
}

