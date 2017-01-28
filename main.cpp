#include <string>
#include <iostream>
//#include <bitset>
#include <climits>
#include <cmath>

bool anyKey();
void bitwiseSplit(unsigned long long input, unsigned long &leftDigits, unsigned long &rightDigits);

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

unsigned long long testInput = 17343092720046919925;//1111000010101111000010101111000011110101000011110101000011110101
//4038045445;//11110000101011111010111100000101


int main()
{
    bool done = false;
    encrypt_ram* er = new encrypt_ram();
    /*while(!done){

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
    */
    unsigned long ld,rd;
    bitwiseSplit(testInput,ld,rd);

    //std::cout << "llong size:" << sizeof(long long)<< std:: endl;

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





void bitwiseSplit(unsigned long long input, unsigned long &leftDigits, unsigned long &rightDigits){
    //splits unsigned integer into two  bit shorts, the left and right containing respective bits
    //tested on 16, 32 and 64 bit , but need particular inputs for each so make another function if you need 32 bit or less inputs(change input sizes and bits variable)
    int bits = 64;
    unsigned long long leftShift = bits/2;
    unsigned long long rightMask = (unsigned long long)pow(2.0,leftShift)-1;

    std::cout << "rightMask: " << rightMask << std::endl;
    // left 8 right 255 // left 16 right 65535
    leftDigits = input >> leftShift;  // this 8 is the split value of # of bits you have, we started with 16 so 16/2=8
    rightDigits = input & rightMask; // this is the max value for the lower end, so 2^split value-1=2^8-1=255
    std::cout << "leftDigits: " << leftDigits << std::endl;
    std::cout << "rightDigits: " << rightDigits << std::endl;
}

