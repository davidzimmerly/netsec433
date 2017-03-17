#include "encrypt_ram_rsa.h"
#include "encrypt_ram.h"

void AESTest(std::string &plainText, unsigned int key_length, int stress_iterations);
void DESTest(std::string &plainText, int stress_iterations);
void RSATest();

int main(int argc, char* argv[])
{
    int stress_iterations=1;
    unsigned int key_length = 256;
    std::string plainText = "78787878sdauhfiasduhfoaisdfjhlodsakijhfladskjfhladskjhflsadkjhfasldkjfhdslkjhfldskjhflsadkjfhldsakjfhdsalkjfhdsalkjhfdsalkjfhsdalkjfhdsalkjdsahlkdsajhfkladsjhfladskjhfalskdjfhladskjfhsakldjfhaldskjfhalsdkjfhsaldkjfhsadlkjfhalskdfjhaslkdjfhldskjfhalskdjfhlaskdjfhlaskdjfhlaksdjfhalskdjfhdsalkjfhdsalkasdfdsafasdkfjhaskdfjghaskdjfhsakldjhflaskdfjhaslkdfjhaklsdfjhladskjhffieu88f7dsyfidsufiewudshfuhfuewhuewhfuehfuehuhfuehufehuehuhfuehufheuhfeuhfuehufehuehfuheuehfuehufehufehuhfeuhfeuhufehufehuehufhuehfuhefuhfuheufhuehfuehufehufhufehufehuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu78786767677678787878787878787878787678987678987=z!";
    DESTest(plainText,stress_iterations);
    AESTest(plainText,key_length,stress_iterations);
    RSATest();
    return 0;
}

void DESTest(std::string &plainText, int stress_iterations){
    encrypt_ram* er = new encrypt_ram();
    er->setDESKey();
    for (int iterations=0; iterations<stress_iterations; iterations++){


        ;
        
        desBlock * newBlock = er->encrypt_DES(plainText,"triple");    
        std::string output="";
        std::string desMode="triple";
        er->decrypt_DES(newBlock,desMode);
        
        for (int x = 0; x<newBlock->size; x++){
            output+=er->ull_to_string(newBlock->data[x]);
            newBlock->data[x]=0;
        }
        //std::cerr<<"decrypted output:"<<output<<std::endl;
        for (unsigned int x=0; x<plainText.length(); x++)
            if (output[x]!=plainText[x]){
                std::cerr<<"DES check failure in mode: "<<desMode<<".  Exiting Program."<<std::endl;
                exit(1);
            }
                
        std::cerr<<"DES Mode: "<<desMode<<" TEST OK"<<std::endl;
        delete[] newBlock->data;
        delete newBlock;
    }
    delete er;
}

void AESTest(std::string &plainText, unsigned int key_length, int stress_iterations){

    encrypt_ram* er2; 
    er2 = new encrypt_ram();
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
    /*for(int stress=0; stress<stress_iterations; stress++){   
        std::string* output;
        std::string mode = "CBCP";
        aesBlock* block1 = er2->encrypt_AES(plainText,mode);
        output = er2->decrypt_AES(block1,mode);
        er2->checkStringMatch(&plainText,output);
        delete output;
        delete[] block1->data;
        delete block1;
    }
    std::cerr<<"/CBCP"; //doesn't work*/
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

    delete er2;
}

void RSATest(){
    std::string answer = "";
    bool rsa_true = false;
    while(true){
        std::cerr << "\nRSA Encryption? (Y/N) ";
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
            std::cerr << "Enter a string: ";
            std::cin >> rsa->strin;
            
            std::cerr << "Unencrypted string: " << rsa->strin << " at " << &rsa->strin << std::endl;
            std::cerr.flush();
            
            break;
        }
        
        while (true){
            rsa->generateValues();
            
            // Debug values
            
            std::cerr << "\nName: " << rsa->strin << std::endl;
            std::cerr << "\nPrime 1: " << rsa->p << std::endl;
            std::cerr << "Prime 2: " << rsa->q << std::endl;
            std::cerr << "N: " << rsa->n << std::endl;
            std::cerr << "Totient: " << rsa->totient << std::endl;
            std::cerr << "E: " << rsa->e << std::endl;
            std::cerr << "D: " << rsa->d << std::endl;
            
            if (rsa->testValues() == 1){
                rsa->encryptString();
                rsa->decryptString();
                break;
            }
        }
                
        delete rsa;

    }
}