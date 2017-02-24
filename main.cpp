#include <string>//std::string
#include <iostream>//std::cerr, std::endl
#include "encrypt_ram_rsa.h"
#include "encrypt_ram.h"
#include <stdlib.h>//for printf support (remove eventually when done with AES drivers)
#include <fstream>//std::ifstream,file i/o
//i should start/change all the longs to ints for consistency for other than windows platforms ( since my longs need to be 32 bit) -dz

void checkTextMatch(std::string originalS, uint8_t* decrypted,uint8_t* original );
uint8_t* encrypt_AES(encrypt_ram* er, std::string &input, std::string mode, unsigned int length);
std::string decrypt_AES(encrypt_ram* er, uint8_t* input, std::string mode, unsigned int length);


ALIGN16 uint8_t CBC_IV[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
/*ALIGN16 uint8_t CBC128_EXPECTED[] = {0x76,0x49,0xab,0xac,0x81,0x19,0xb2,0x46,0xce,0xe9,0x8e,0x9b,0x12,0xe9,0x19,0x7d,0x50,0x86,0xcb,0x9b,0x50,0x72,0x19,0xee,0x95,0xdb,0x11,0x3a,0x91,0x76,0x78,0xb2,0x73,0xbe,0xd6,0xb8,0xe3,0xc1,0x74,0x3b,0x71,0x16,0xe6,0x9e,0x22,0x22,0x95,0x16,0x3f,0xf1,0xca,0xa1,0x68,0x1f,0xac,0x09,0x12,0x0e,0xca,0x30,0x75,0x86,0xe1,0xa7};
ALIGN16 uint8_t CBC192_EXPECTED[] = {0x4f,0x02,0x1d,0xb2,0x43,0xbc,0x63,0x3d,0x71,0x78,0x18,0x3a,0x9f,0xa0,0x71,0xe8,0xb4,0xd9,0xad,0xa9,0xad,0x7d,0xed,0xf4,0xe5,0xe7,0x38,0x76,0x3f,0x69,0x14,0x5a,0x57,0x1b,0x24,0x20,0x12,0xfb,0x7a,0xe0,0x7f,0xa9,0xba,0xac,0x3d,0xf1,0x02,0xe0,0x08,0xb0,0xe2,0x79,0x88,0x59,0x88,0x81,0xd9,0x20,0xa9,0xe6,0x4f,0x56,0x15,0xcd};
ALIGN16 uint8_t CBC256_EXPECTED[] = {0xf5,0x8c,0x4c,0x04,0xd6,0xe5,0xf1,0xba,0x77,0x9e,0xab,0xfb,0x5f,0x7b,0xfb,0xd6,0x9c,0xfc,0x4e,0x96,0x7e,0xdb,0x80,0x8d,0x67,0x9f,0x77,0x7b,0xc6,0x70,0x2c,0x7d,0x39,0xf2,0x33,0x69,0xa9,0xd9,0xba,0xcf,0xa5,0x30,0xe2,0x63,0x04,0x23,0x14,0x61,0xb2,0xeb,0x05,0xe2,0xc3,0x9b,0xe9,0xfc,0xda,0x6c,0x19,0x07,0x8c,0x6a,0x9d,0x1b};*/
ALIGN16 uint8_t CTR128_IV[] = {0xC0,0x54,0x3B,0x59,0xDA,0x48,0xD9,0x0B};
//ALIGN16 uint8_t CTR192_IV[] = {0x02,0x0C,0x6E,0xAD,0xC2,0xCB,0x50,0x0D};
//ALIGN16 uint8_t CTR256_IV[] = {0xC1,0x58,0x5E,0xF1,0x5A,0x43,0xD8,0x75};
ALIGN16 uint8_t CTR128_NONCE[] = {0x00,0x6C,0xB6,0xDB};
//ALIGN16 uint8_t CTR192_NONCE[] = {0x00,0x96,0xB0,0x3B};
//ALIGN16 uint8_t CTR256_NONCE[] = {0x00,0xFA,0xAC,0x24};
/*ALIGN16 uint8_t CTR128_EXPECTED[] = {0x51,0x04,0xA1,0x06,0x16,0x8A,0x72,0xD9,0x79,0x0D,0x41,0xEE,0x8E,0xDA,0xD3,0x88,0xEB,0x2E,0x1E,0xFC,0x46,0xDA,0x57,0xC8,0xFC,0xE6,0x30,0xDF,0x91,0x41,0xBE,0x28};
ALIGN16 uint8_t CTR192_EXPECTED[] = {0x45,0x32,0x43,0xFC,0x60,0x9B,0x23,0x32,0x7E,0xDF,0xAA,0xFA,0x71,0x31,0xCD,0x9F,0x84,0x90,0x70,0x1C,0x5A,0xD4,0xA7,0x9C,0xFC,0x1F,0xE0,0xFF,0x42,0xF4,0xFB,0x00};
ALIGN16 uint8_t CTR256_EXPECTED[] = {0xF0,0x5E,0x23,0x1B,0x38,0x94,0x61,0x2C,0x49,0xEE,0x00,0x0B,0x80,0x4E,0xB2,0xA9,0xB8,0x30,0x6B,0x50,0x8F,0x83,0x9D,0x6A,0x55,0x30,0x83,0x1D,0x93,0x44,0xAF,0x1C};*/
//for cbc and ecb modes: not ctr:
/*ALIGN16 uint8_t AES128_TEST_KEY[] = {0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c};
ALIGN16 uint8_t AES192_TEST_KEY[] = {0x8e,0x73,0xb0,0xf7,0xda,0x0e,0x64,0x52,0xc8,0x10,0xf3,0x2b,0x80,0x90,0x79,0xe5,0x62,0xf8,0xea,0xd2,0x52,0x2c,0x6b,0x7b};
ALIGN16 uint8_t AES256_TEST_KEY[] = {0x60,0x3d,0xeb,0x10,0x15,0xca,0x71,0xbe,0x2b,0x73,0xae,0xf0,0x85,0x7d,0x77,0x81,0x1f,0x35,0x2c,0x07,0x3b,0x61,0x08,0xd7,0x2d,0x98,0x10,0xa3,0x09,0x14,0xdf,0xf4};
ALIGN16 uint8_t AES_TEST_VECTOR[] = {0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a,0xae,0x2d,0x8a,0x57,0x1e,0x03,0xac,0x9c,0x9e,0xb7,0x6f,0xac,0x45,0xaf,0x8e,0x51,0x30,0xc8,0x1c,0x46,0xa3,0x5c,0xe4,0x11,0xe5,0xfb,0xc1,0x19,0x1a,0x0a,0x52,0xef,0xf6,0x9f,0x24,0x45,0xdf,0x4f,0x9b,0x17,0xad,0x2b,0x41,0x7b,0xe6,0x6c,0x37,0x10};
*/
//for ctr test mode use:
/*ALIGN16 uint8_t AES128_TEST_KEY[] = {0x7E,0x24,0x06,0x78,0x17,0xFA,0xE0,0xD7,0x43,0xD6,0xCE,0x1F,0x32,0x53,0x91,0x63};
ALIGN16 uint8_t AES192_TEST_KEY[] = {0x7C,0x5C,0xB2,0x40,0x1B,0x3D,0xC3,0x3C,0x19,0xE7,0x34,0x08,0x19,0xE0,0xF6,0x9C,0x67,0x8C,0x3D,0xB8,0xE6,0xF6,0xA9,0x1A};
ALIGN16 uint8_t AES256_TEST_KEY[] = {0xF6,0xD6,0x6D,0x6B,0xD5,0x2D,0x59,0xBB,0x07,0x96,0x36,0x58,0x79,0xEF,0xF8,0x86,0xC6,0x6D,0xD5,0x1A,0x5B,0x6A,0x99,0x74,0x4B,0x50,0x59,0x0C,0x87,0xA2,0x38,0x84};
ALIGN16 uint8_t AES_TEST_VECTOR[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F};
*/////
/*ALIGN16 uint8_t ECB128_EXPECTED[] = {0x3a,0xd7,0x7b,0xb4,0x0d,0x7a,0x36,0x60,0xa8,0x9e,0xca,0xf3,0x24,0x66,0xef,0x97,0xf5,0xd3,0xd5,0x85,0x03,0xb9,0x69,0x9d,0xe7,0x85,0x89,0x5a,0x96,0xfd,0xba,0xaf,0x43,0xb1,0xcd,0x7f,0x59,0x8e,0xce,0x23,0x88,0x1b,0x00,0xe3,0xed,0x03,0x06,0x88,0x7b,0x0c,0x78,0x5e,0x27,0xe8,0xad,0x3f,0x82,0x23,0x20,0x71,0x04,0x72,0x5d,0xd4};
ALIGN16 uint8_t ECB192_EXPECTED[] = {0xbd,0x33,0x4f,0x1d,0x6e,0x45,0xf2,0x5f,0xf7,0x12,0xa2,0x14,0x57,0x1f,0xa5,0xcc,0x97,0x41,0x04,0x84,0x6d,0x0a,0xd3,0xad,0x77,0x34,0xec,0xb3,0xec,0xee,0x4e,0xef,0xef,0x7a,0xfd,0x22,0x70,0xe2,0xe6,0x0a,0xdc,0xe0,0xba,0x2f,0xac,0xe6,0x44,0x4e,0x9a,0x4b,0x41,0xba,0x73,0x8d,0x6c,0x72,0xfb,0x16,0x69,0x16,0x03,0xc1,0x8e,0x0e};
ALIGN16 uint8_t ECB256_EXPECTED[] = {0xf3,0xee,0xd1,0xbd,0xb5,0xd2,0xa0,0x3c,0x06,0x4b,0x5a,0x7e,0x3d,0xb1,0x81,0xf8,0x59,0x1c,0xcb,0x10,0xd4,0x10,0xed,0x26,0xdc,0x5b,0xa7,0x4a,0x31,0x36,0x28,0x70,0xb6,0xed,0x21,0xb9,0x9c,0xa6,0xf4,0xf9,0xf1,0x53,0xe7,0xb1,0xbe,0xaf,0xed,0x1d,0x23,0x30,0x4b,0x7a,0x39,0xf9,0xf3,0xff,0x06,0x7d,0x8d,0x8f,0x9e,0x24,0xec,0xc7};
*/
int main()
{
    int stress_iterations=1;
    //configuration options (for json test) 
    /***********************************************************************************************************/
    //NOT IN REPO!!!!!!!!!!!!!!, put randomAPI key line 1 in file called config.txt
    /*std::ifstream file("config.txt"); //not included in repo currently, first line should be randomAPI key
    std::string rk;
    if (!std::getline(file, rk)){
        std::cerr << "cannot find configuration file values in config.txt" <<std::endl;
        exit(1);
    }
    file.close();*/
    /***********************************************************************************************************/

    /*unsigned long long* des_key = new unsigned long long;
    *des_key = 1383827165325090801;
    encrypt_ram* er = new encrypt_ram(*des_key);
    if (!er->Check_CPU_support_AES()){
        printf("Cpu does not support AES instruction set. Bailing out.\n");
        return 1;
    }*/
    //initial  memory search hex editor demo:*/
    /*
    bool done = false;
    while(!done){
        std::string message =  "Sammy"; //value is exposed in memory
        er->encrypt(message);
        //value is encrypted in memory
        std::cerr << "Encrypted: " << message<< std::endl;
        done = er->anyKey();
        if (!done){
            er->decrypt(message);
            //value is exposed in memory
            std::cerr << "Decrypted: " << message<< std::endl;
            done = er->anyKey();
        }
        //need to re-decrypt to access again, needs to be cleared to get out of memory
        if (!done){
            std::cerr<< "re encrypt decrypted value"<< std::endl;
            er->encrypt(message);
            done = er->anyKey();
        }
    }*/
    

    /*unsigned long long* testMessage = new unsigned long long;
    *testMessage = 81985529216486895;
    
    er->desEncrypt(*testMessage);
    std::cerr <<"Test Message= "<< *testMessage << std::endl;
    er->desDecrypt(*testMessage);
    std::cerr <<"Test Message= "<< *testMessage << std::endl;
    
    delete des_key;
    
    delete er;*/
    encrypt_ram* er2; 
    er2 = new encrypt_ram();
    if (!er2->Check_CPU_support_AES()){
        printf("Cpu does not support AES instruction set. Bailing out.\n");
        return 1;
    }
    //unsigned long long* des_key2=new unsigned long long;
    
    //*des_key2 = er2->getNewLL();  //think bug is here 
    //std::cerr <<"new key: "<<*des_key2<<std::endl;


/***********************************************************************************************************/    
    //json request example:
    /*std::string jsonArguments="{\"jsonrpc\":\"2.0\",\"method\":\"generateIntegers\",\"params\":{\"apiKey\":\""+rk+"\",\"n\":10,\"min\":1,\"max\":10,\"replacement\":true,\"base\":16},\"id\":13527}";
    std::string resultString = er2->call_curl("https://api.random.org/json-rpc/1/invoke", jsonArguments);
    std::cerr << resultString << std::endl;*/
/***********************************************************************************************************/

    //aes new key / new text example:
    std::string plainTextNew = "jfifdoifudifodifidufidofidoappodijjjjjjjjfifdoifudifodifidufidofidoappodijjjjjjjjfifdoifudifodifidufidofidoappodijjjjjjjjfifdoifudifodifidufidofidoappodijjjjjjjjfifdoifudifodifidufidofidoappodijjjjjjjjfifdoifudifodifidufidofidoapp89347hfjy8732y4ed7yruedfyf";
    //std::cerr << "plainTextNewsize: "<<plainTextNew.length()<< std::endl;//256
    ALIGN16 uint8_t* formattedNewPlainText = new ALIGN16 uint8_t[256];

    //for each letter of string, encode the 8 bit integer equivalent to its char code/ascii value as entry in array
    for (unsigned int j=0; j<LENGTH; j++){
        formattedNewPlainText[j] = plainTextNew[j];
    }
    //for (int j=0; j<plainTextNew.length(); j++)
      //  er2->print_m128i_with_string_short("",((__m128i*)formattedNewPlainText)[j],16);
    uint8_t *CIPHERTEXT,*DECRYPTEDTEXT;//,*CIPHER_KEY;
    
        //CIPHERTEXT = new ALIGN16 uint8_t[256];//(uint8_t*)malloc(LENGTH);
        DECRYPTEDTEXT = new ALIGN16 uint8_t[256];//(uint8_t*)malloc(LENGTH);
    
    er2->getNewAESKey(256);
    for (int key_length=256; key_length<=256; key_length+=64){
     
        std::string output="";
        std::cerr<<"testing "<<key_length<<"-bit key..";
        //need to change #DEFINE for test as well as key size and LENGTH =thisx2
        
//        CIPHER_KEY = er2->aesKey;
        //AES_KEY key, decrypt_key;
        //er2->AES_set_encrypt_key(CIPHER_KEY, key_length, &key);
        //er2->AES_set_decrypt_key(CIPHER_KEY, key_length, &decrypt_key);
            
        for(int stress=0; stress<stress_iterations; stress++){
            //ctr mode
            CIPHERTEXT = encrypt_AES(er2,plainTextNew,"CTR",256);
            
            //er2->AES_CTR_encrypt(formattedNewPlainText,CIPHERTEXT,CTR128_IV,CTR128_NONCE,LENGTH,er2->key.KEY,er2->key.nr);
            er2->AES_CTR_encrypt(CIPHERTEXT,DECRYPTEDTEXT,   CTR128_IV, CTR128_NONCE,  LENGTH, er2->key.KEY, er2->key.nr);
      
            //note: these text comparisons give valgrind linking trouble on subsequent attempts but appear to work
            for (int j=0; j<LENGTH; j++){
                //std::cerr << DECRYPTEDTEXT[j];//error on 2nd attempt
                //std::cerr << formattedNewPlainText[j];//no error
                //std::cerr << CIPHERTEXT[j];//errors
            }
            checkTextMatch(plainTextNew,DECRYPTEDTEXT, formattedNewPlainText);
            
            
        }    
        std::cerr<<"CTR";
        for(int stress=0; stress<stress_iterations; stress++){   
            delete[] CIPHERTEXT;
            //cbc mode
            CIPHERTEXT = encrypt_AES(er2,plainTextNew,"CBC",256);
            //er2->AES_CBC_encrypt(formattedNewPlainText,CIPHERTEXT,CBC_IV,LENGTH,er2->key.KEY,er2->key.nr);
            er2->AES_CBC_decrypt(CIPHERTEXT,DECRYPTEDTEXT,  CBC_IV,  LENGTH, er2->decrypt_key.KEY, er2->key.nr);
            /*std::string output = decrypt_AES(er2,CIPHERTEXT,"CBC",256);
            
            if (output!=plainTextNew){
                std::cerr<<"strings unequal in AES CTR mode"<<std::endl;;
                exit(1);
            }*/
            checkTextMatch(plainTextNew,DECRYPTEDTEXT, formattedNewPlainText);

            
        }
        std::cerr<<"/CBC";
        for(int stress=0; stress<stress_iterations; stress++){
            //ecb mode
            delete[] CIPHERTEXT;
            CIPHERTEXT = encrypt_AES(er2,plainTextNew,"ECB",256);
            //er2->AES_ECB_encrypt(formattedNewPlainText,CIPHERTEXT,LENGTH,(const char*)er2->key.KEY,er2->key.nr);
            er2->AES_ECB_decrypt(CIPHERTEXT,DECRYPTEDTEXT, LENGTH, (const char*)er2->decrypt_key.KEY, er2->decrypt_key.nr);
            
            checkTextMatch(plainTextNew,DECRYPTEDTEXT, formattedNewPlainText);
            
            
        }
        
        std::cerr<<"/ECB...OK"<<std::endl;
        
    }
        delete[] CIPHERTEXT;

        delete[] DECRYPTEDTEXT;
    


    
	/*
	// RSA - Because this takes an inordinate amount of time to run,
	// it asks the user if they'd like to opt out
	std::string answer = "";
	bool rsa_true = false;
	while(true){
		std::cerr << "\nAttempt RSA encryption? (May take a while) Y/N ";
		std::cin >> answer;
		if (answer == "n" || answer == "no" || answer == "No" || answer == "N"){
			std::cerr << "No RSA encryption then..." << std::endl;
			break;
		} else if (answer == "y" || answer == "yes" || answer == "Yes" || answer == "Y"){
			rsa_true = true;
			std::cerr << "RSA encryption continues..." << std::endl;
			break;
		}
		std::cerr << "Not a valid choice - try again" << std::endl;
	}

	if (rsa_true){
		encrypt_ram_rsa * rsa = new encrypt_ram_rsa;
		
		srand(time(0));
		
		// Gets string and int from user
		while (true) {
			std::cerr << "Enter your name: ";
			std::cin >> rsa->name;
			
			std::cerr << "Enter your PIN: ";
			std::cin >> rsa->pin;

			std::cerr << std::endl;
			std::cerr << "First name: " << rsa->name << " at " << &rsa->name << std::endl;
			std::cerr << "PIN: " << rsa->pin << " at " << &rsa->pin << std::endl;

			break;
		}
		
		rsa->generateValues();
		
		std::cerr << "\nPrime 1: " << rsa->p << std::endl;
		std::cerr << "Prime 2: " << rsa->q << std::endl;
		std::cerr << "N: " << rsa->n << std::endl;
		std::cerr << "Totient: " << rsa->totient << std::endl;
		std::cerr << "E: " << rsa->e << std::endl;
		std::cerr << "D: " << rsa->d << std::endl;
		
		std::cerr << "\nEncrypting name (this will take a while)..." << std::endl;
		
		rsa->encryptString();
		rsa->decryptString();
		
		std::cerr << "Encrypting PIN (this will take a while)..." << std::endl;
		
		rsa->encryptPIN();
		rsa->decryptPIN();
	}*/
	
	//delete des_key2;
  //  delete testMessage;
    delete[] formattedNewPlainText;
    delete er2;
    return 0;
}


void checkTextMatch(std::string originalS, uint8_t* decrypted,uint8_t* original ){
    for(unsigned int i=0; i<LENGTH; i++){
        if (decrypted[i] != original[i]){
            printf("The DECRYPTED TEXT is not equal to the original PLAINTEXT.\n\n");
            exit(1);
        }
    }
    std::string output="";
    for (unsigned int j=0; j<LENGTH; j++){
        output+=(char)decrypted[j];
    }
    if (!output.compare(originalS)==0){
        std::cerr << "string is not same as original" << std::endl;
        exit(1);
    }
}

uint8_t* encrypt_AES(encrypt_ram* er, std::string &input, std::string mode, unsigned int length){
    
    //need to code support for length>256
    ALIGN16 uint8_t* formattedNewPlainText = new ALIGN16 uint8_t[length];
    ALIGN16 uint8_t* CIPHERTEXT = new ALIGN16 uint8_t[length];
 //   ALIGN16 uint8_t* DECRYPTEDTEXT = new ALIGN16 uint8_t[length];
    
    for (unsigned int j=0; j<LENGTH; j++){
        formattedNewPlainText[j] = input[j];
    }
        //for (int j=0; j<plainTextNew.length(); j++)
      //  er2->print_m128i_with_string_short("",((__m128i*)formattedNewPlainText)[j],16);

    //CIPHERTEXT = new ALIGN16 uint8_t[256];//(uint8_t*)malloc(LENGTH);
    //DECRYPTEDTEXT = new ALIGN16 uint8_t[256];//(uint8_t*)malloc(LENGTH);
    
    
    if (mode=="CTR"){
        er->AES_CTR_encrypt(formattedNewPlainText,CIPHERTEXT,CTR128_IV,CTR128_NONCE,LENGTH,er->key.KEY,er->key.nr);

    }
    else if (mode=="CBC"){
        er->AES_CBC_encrypt(formattedNewPlainText,CIPHERTEXT,CBC_IV,LENGTH,er->key.KEY,er->key.nr);
       
    }        
    else if (mode=="ECB"){
        er->AES_ECB_encrypt(formattedNewPlainText,CIPHERTEXT,LENGTH,(const char*)er->key.KEY,er->key.nr);

    }
    else{
        std::cerr <<"Invalid mode for encrypt_AES"<<std::endl;
        exit(1);
    } 
    
    for (int x=0; x<256; x++)
        formattedNewPlainText[x]=0;

    delete[] formattedNewPlainText;
    
    return CIPHERTEXT;
}
std::string decrypt_AES(encrypt_ram* er, uint8_t* input, std::string mode, unsigned int length){
    
    //need to code support for length>256
    std::string newPlainText;
    //newPlainText = new std::string;
    newPlainText.reserve(length);
    
    ALIGN16 uint8_t* DECRYPTEDTEXT = new ALIGN16 uint8_t[length];
    
        //for (int j=0; j<plainTextNew.length(); j++)
      //  er2->print_m128i_with_string_short("",((__m128i*)formattedNewPlainText)[j],16);

    //CIPHERTEXT = new ALIGN16 uint8_t[256];//(uint8_t*)malloc(LENGTH);
    //DECRYPTEDTEXT = new ALIGN16 uint8_t[256];//(uint8_t*)malloc(LENGTH);
    
    
    if (mode=="CTR"){
        er->AES_CTR_encrypt(input,DECRYPTEDTEXT,CTR128_IV,CTR128_NONCE,LENGTH,er->key.KEY,er->key.nr);

    }
    else if (mode=="CBC"){
        er->AES_CBC_decrypt(input,DECRYPTEDTEXT,  CBC_IV,  LENGTH, er->decrypt_key.KEY, er->key.nr);
               
    }        
    else if (mode=="ECB"){
        er->AES_ECB_decrypt(input,DECRYPTEDTEXT, LENGTH, (const char*)er->decrypt_key.KEY, er->decrypt_key.nr);
    
    }
    else{
        std::cerr <<"Invalid mode for encrypt_AES"<<std::endl;
        exit(1);
    } 
    
    for (int x=0; x<256; x++)
        newPlainText[x]=(char)DECRYPTEDTEXT[x];
    checkTextMatch(newPlainText,DECRYPTEDTEXT, input);
    
    delete[] DECRYPTEDTEXT;
    
    return newPlainText;
}

