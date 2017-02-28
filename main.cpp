#include "encrypt_ram_rsa.h"
#include "encrypt_ram.h"
//i should start/change all the longs to ints for consistency for other than windows platforms ( since my longs need to be 32 bit) -dz

int main()
{
    int stress_iterations=1;
    unsigned int key_length = 256;
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
    }
    */
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
    
/***********************************************************************************************************/    

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
    
    // i want to use this string or version of it instead of the initialized char* below
    std::string plainTextNew1=   "837648asdtommytdsdskjtommytdsdskjhflsakdjhflkdsjhflkasdjhfklsadjhfdslakjhfdslkjhfldkjhfldksjahfldskjfhasldkjfhdslkjsafdhflkjdshlfkjdsahlfdskajhfldskjhatommytdsdskjhflsakdjhflkdsjhflkasdjhfklsadjhfdslakjhfdslkjhfldkjhfldksjahfldskjfhasldkjfhdslkjsafdhflkjdshlfkjdsahasdtommytdsdskjtommytdsdskjhflsakdjhflkdsjhflkasdjhfklsadjhfdslakjhfdslkjhfldkjhfldksjahfldskjfhasldkjfhdslkjsafdhflkjdshlfkjdsahlfdskajhfldskjhatommytdsdskjhflsakdjhflkdsjhflkasdjhfklsadjhfdslakjhfdslkjhfldkjhfldksjahfldskjfhasldkjfhdslkjsafdhflkjdshlfkjdsah";
    unsigned int len = plainTextNew1.length();//# characters in plainTextNew, must set input size for now?;
    
    
    
    unsigned int newSize=len;
    unsigned int total = len;
    newSize = key_length;
    
           
        while (total>key_length){
            newSize += key_length;
            total -= key_length;
        
        }
    
    //const unsigned char * plainTextNew = reinterpret_cast<const unsigned char *> (plainTextNew1.c_str());//errors
    char plainTextNew[newSize];
    for (unsigned int u=0; u<newSize;u++){
        plainTextNew[u]=0;
    }

    std::size_t length = plainTextNew1.copy(plainTextNew,plainTextNew1.length(),0);
    std::cerr << "plainTextNewsize: "<<length<< std::endl;//256
    

    std::cerr << "aes blocks to use: "<<newSize<< std::endl;//256
    ALIGN16 uint8_t* formattedNewPlainText = new ALIGN16 uint8_t[newSize];
    for (unsigned int j=0; j<newSize; j++){
        formattedNewPlainText[j] = 0;
    }
    
    //for each letter of string, encode the 8 bit integer equivalent to its char code/ascii value as entry in array
    for (unsigned int j=0; j<len; j++){
        formattedNewPlainText[j] = plainTextNew1[j];
    }
    //for (int j=0; j<plainTextNew.length(); j++)
      //  er2->print_m128i_with_string_short("",((__m128i*)formattedNewPlainText)[j],16);
    er2->getNewAESKey(key_length);
    std::cerr<<"testing "<<key_length<<"-bit key..";
    //need to change #DEFINE for test as well as key size and LENGTH =thisx2
    for(int stress=0; stress<stress_iterations; stress++){
        char* output;
        std::string mode = "CTR";
        aesBlock* block1 = er2->encrypt_AES((const  char*)plainTextNew,mode,newSize);
        output = ( char *)er2->decrypt_AES(block1,mode);
        er2->checkStringMatch(plainTextNew,output,len); 
        free(output);
        delete[] block1->data;
        delete block1;
    }    
    std::cerr<<"CTR";
    for(int stress=0; stress<stress_iterations; stress++){   
         char* output;
        std::string mode = "CBC";
        aesBlock* block1 = er2->encrypt_AES((const  char*)plainTextNew,mode,newSize);
        output = er2->decrypt_AES(block1,mode);
        er2->checkStringMatch(plainTextNew,output,len);
        free(output);
        delete[] block1->data;
        delete block1;
    }
    std::cerr<<"/CBC";
    for(int stress=0; stress<stress_iterations; stress++){
         char* output;
        std::string mode = "ECB";
        aesBlock* block1 = er2->encrypt_AES((const  char*)plainTextNew,mode,newSize);
        output = ( char *)er2->decrypt_AES(block1,mode);
        er2->checkStringMatch(plainTextNew,output,len); 
        free(output);
        delete[] block1->data;
        delete block1;
    }
    
    std::cerr<<"/ECB...OK"<<std::endl;
    
    
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
    //delete testMessage;
    
    delete[] formattedNewPlainText;
    delete er2;
    return 0;
}

