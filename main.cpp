#include <string>
#include <iostream>
#include <cmath>

bool anyKey();
void bitwiseSplit(unsigned long long &input, unsigned long &leftDigits, unsigned long &rightDigits);
void bitwiseSplit(unsigned int &input, unsigned short &leftDigits, unsigned short &rightDigits);
void setBit(unsigned long long &input, int bit, bool value){ input |= value << bit; }
void setBit(unsigned int &input, int bit, bool value){ input |= value << bit; }
void setBit(unsigned short &input, int bit, bool value){ input |= value << bit; }
void clearBit(unsigned long long &input, int bit){ input &= ~(1 << bit ); }
void clearBit(unsigned int &input, int bit){ input &= ~(1 << bit ); }
void clearBit(unsigned short &input, int bit){ input &= ~(1 << bit ); }
void toggleBit(unsigned long long &input, int bit){ input ^= 1 << bit ; }
void toggleBit(unsigned int &input, int bit){ input ^= 1 << bit ; }
void toggleBit(unsigned short &input, int bit){ input ^= 1 << bit ; }
bool checkBit(unsigned long long &input, int bit){ return (input>>bit)&1;}
bool checkBit(unsigned int &input, int bit){ return (input>>bit)&1;}
bool checkBit(unsigned short &input, int bit){ return (input>>bit)&1;}
void changeBit(unsigned long long &input, unsigned long long bit, bool value){ unsigned long long one = 1; input ^= (-value ^input) & (one << bit);}
/*void changeBit(unsigned int &input, int bit, bool value){ input ^= (-value ^input) & (1 << bit);}
void changeBit(unsigned short &input, int bit, bool value){ input ^= (-value ^input) & (1 << bit);}*/
unsigned long long* permuteKey(unsigned long long &input);
void displayBinary(unsigned long long input);
void displayBinary(unsigned int input);
void displayBinary(unsigned short input);


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

unsigned long long testInput = 17343092720046919924;//1111000010101111000010101111000011110101000011110101000011110101



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
    unsigned long long des_key = 1383827165325090801;
    unsigned long long* permutation1 = new unsigned long long;
    permutation1 = permuteKey(des_key);
    //setBit()
    displayBinary(*permutation1);
    std::cout << std::endl;
    std::cout << *permutation1 << std::endl;
    //changeBit(*permutation1, 2, 1);
    //std::cout << *permutation1 << std::endl;
    delete er;
    delete permutation1;
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

const int pc_1 [56] = { 57,   49,    41,   33,    25,    17,    9,
                        1,   58,    50,   42,    34,    26,   18,
                        10,    2,    59,   51,    43,    35,   27,
                        19,   11,     3,   60,    52,    44,   36,
                        63,   55,    47,   39,    31,    23,   15,
                        7,   62,    54 ,  46,    38,    30,   22,
                        14,    6,    61,   53,    45,    37,   29,
                        21,   13,     5,   28,    20,    12,    4 };

unsigned long long* permuteKey(unsigned long long &input){
    unsigned long long* permutation= new unsigned long long;
    *permutation = 0;
    for (int x=0; x<56; x++){//set 56 bit key, the upper bits should be blank
        changeBit(*permutation,55-x,checkBit(input,63-pc_1[x]+1));
    }
    return permutation;
}

void displayBinary(unsigned long long input){
    for (int x=63; x>=0; x--)
        std::cout << checkBit(input,x);
    std::cout << std::endl;
}
void displayBinary(unsigned int input){
    for (int x=31; x>=0; x--)
        std::cout << checkBit(input,x);
    std::cout << std::endl;
}
void displayBinary(unsigned short input){
    for (int x=15; x>=0; x--)
        std::cout << checkBit(input,x);
    std::cout << std::endl;
}


void bitwiseSplit(unsigned long long input, unsigned long &leftDigits, unsigned long &rightDigits){
    //64 bit  => 32 x 2 version
    //splits unsigned long long into two  bit shorts, the left and right containing respective bits
    int bits = 64;
    unsigned long long leftShift = bits/2;
    unsigned long long rightMask = (unsigned long long)pow(2.0,leftShift)-1;
    leftDigits = input >> leftShift;
    rightDigits = input & rightMask;
}

void bitwiseSplit(unsigned int input, unsigned short &leftDigits, unsigned short &rightDigits){
    //32 bit  => 16 x 2 version
    //splits unsigned integer into two  bit shorts, the left and right containing respective bits
    int bits = 32;
    unsigned int leftShift = bits/2;
    unsigned int rightMask = (unsigned int)pow(2.0,leftShift)-1;
    leftDigits = input >> leftShift;
    rightDigits = input & rightMask;
}


