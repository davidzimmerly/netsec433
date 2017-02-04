#include <string>
#include <iostream>
#include <cmath>
#include "encrypt_ram.h"

//i should start/change all the longs to ints for consistency for other than windows platforms ( since my longs need to be 32 bit) -dz
bool anyKey();








//unsigned long long testInput = 17343092720046919924;//1111000010101111000010101111000011110101000011110101000011110101
//unsigned long long testMessage = 81985529216486895;//0000000100100011010001010110011110001001101010111100110111101111


int main()
{
    bool done = false;

    unsigned long long* des_key = new unsigned long long;
    *des_key = 1383827165325090801;
    encrypt_ram* er = new encrypt_ram(*des_key);
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
    */

    unsigned long long* testMessage = new unsigned long long;
    *testMessage = 81985529216486895;
    er->desEncrypt(*testMessage);
    delete des_key;
    delete testMessage;
    delete er;
    return 0;
}



bool anyKey(){
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
