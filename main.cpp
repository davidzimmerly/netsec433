#include "encrypt_ram_rsa.h"
#include "encrypt_ram.h"

int main()
{
    int stress_iterations=1;
    unsigned int key_length = 256;
    


    encrypt_ram* er = new encrypt_ram();
    er->setDESKey();
    /**initial  memory search hex editor demo************************************************************************************/
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
    /***********************************************************************************************************/


    /*DES TEST*******************************************************************************************************/

    std::string convertMe = "davidzssdjkfhldskjfhlksdjhfldskjhfldksjhfklsdjhfjkdhfkdjfdmknbf,dmnbfdm,nbzzz";
    std::string convertedString= er->string_to_nstring(convertMe);
    unsigned long long* temp = er->nstring_to_ull(convertedString);
    
    desBlock * newBlock = er->encrypt_DES(convertMe,"triple");    
    std::string output="";
    std::string desMode="triple";
    er->decrypt_DES(newBlock,desMode);
    
    for (int x = 0; x<newBlock->size; x++){
        output+=er->ull_to_string(newBlock->data[x]);
        newBlock->data[x]=0;
    }
    //std::cerr<<"decrypted output:"<<output<<std::endl;
    for (unsigned int x=0; x<convertMe.length(); x++)
        if (output[x]!=convertMe[x]){
            std::cerr<<"DES check failure in mode: "<<desMode<<".  Exiting Program."<<std::endl;
        }
            
    std::cerr<<"DES Mode: "<<desMode<<" TEST OK"<<std::endl;
    delete[] newBlock->data;
    delete newBlock;
    *temp=0;
    delete temp;
    delete er;
    /***********************************************************************************************************/
        


    encrypt_ram* er2; 
    er2 = new encrypt_ram();
        

    //aes new key / new text example:***************************************************************************/
    
    std::string plainText="837648asdtommytdsdskjtommytdsdskjhflsakdjhflkdsjhflkasdjhfklsadjhfdslakjhfdslkjhfldkjhfldksjahfldskjfhasldkjfhdslkjsafdhflkjdshlfkjdsahlfdskajhfldskjhatommytdsdskjhflsakdjhflkdsjhflkasdjhfklsadjhfdslakjhfdslkjhfldkjhfldksjahfldskjfhasldkjfhdslkjsafdhflkjdshlfkjdsahasdtommytdsdskjtommytdsdskjhflsakdjhflkdsjhflkasdjhfklsadjhfdslakjhfdslkjhfldkjhfldksjahfldskjfhasldkjfhdslkjsafdhflkjdshlfkjdsahlfdskajhfldskjhatommytdsdskjhflsakdjhflkdsjhflkasdjhfklsadjhfdslakjhfdslkjhfldkjhfldksjahfldskjfhasldkjfhdslkjsafdhflkjdshlfkjdsah";
    er2->setAESKey(key_length);
    std::cerr<<"testing "<<key_length<<"-bit key..";
    for(int stress=0; stress<stress_iterations; stress++){
        std::string* output;
        std::string mode = "CTR";
        aesBlock* block1 = er2->encrypt_AES(plainText,mode);
        output = er2->decrypt_AES(block1,mode);
        er2->checkStringMatch(&plainText,output); 
        delete output;
        delete[] block1->data;
        delete block1;
    }    
    std::cerr<<"CTR";
    for(int stress=0; stress<stress_iterations; stress++){   
        std::string* output;
        std::string mode = "CBC";
        aesBlock* block1 = er2->encrypt_AES(plainText,mode);
        output = er2->decrypt_AES(block1,mode);
        er2->checkStringMatch(&plainText,output);
        delete output;
        delete[] block1->data;
        delete block1;
    }
    std::cerr<<"/CBC";
    for(int stress=0; stress<stress_iterations; stress++){
        std::string* output;
        std::string mode = "ECB";
        aesBlock* block1 = er2->encrypt_AES(plainText,mode);
        output = er2->decrypt_AES(block1,mode);
        er2->checkStringMatch(&plainText,output); 
        delete output;
        delete[] block1->data;
        delete block1;
    }
    
    std::cerr<<"/ECB...OK"<<std::endl;
    /***********************************************************************************************************/    
    
	/**RSA******************************************************************************************************/
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
        delete rsa;
	}*/
	/***********************************************************************************************************/
   
    delete er2;
    return 0;
}