#include "encrypt_ram_rsa.h"
#include "encrypt_ram.h"

int main()
{
    encrypt_ram* er = new encrypt_ram();
    er->setDESKey();
    
    bool done = false;
    std::vector<std::string> v;
    std::vector<desBlock*> e;
    std::string output="";
    while(!done){
        if (v.size()>0){
            std::cerr<<"plaintext items: "<<std::endl;
            
            for (unsigned int i=0; i<v.size(); i++ )
                if (v[i]!="")
                    std::cerr << v[i] << " ";
            std::cerr<<std::endl;
        }
        if (e.size()>0){
            std::cerr<<"[encrypted items]: "<<std::endl;
            for (unsigned int o=0; o<e.size(); o++){
                output="";
                er->decrypt_DES(e[o],"triple");
                for (unsigned int i = 0; i<e[o]->size; i++){    
                    //std::cerr<<output;
                    output+=er->ull_to_string(e[o]->data[i]);
                    e[o]->data[i] = 0;
                }
                std::cerr<<output<<" ";
                
                delete[] e[o]->data;
                delete e[o];

                e[o]=er->encrypt_DES(output,"triple");
                output="";
            }
            

            std::cerr<<std::endl;
        }
                  //  output+=er->ull_to_string(e[x]);
                    //newBlock->data[x]=0;
        //}
        std::string input="";
        std::cerr<<std::endl<<"Enter new plaintext (one word at a time): ( or E/D/X): ";
        std::cin>>input;
        if (input=="e" || input=="encrypt" || input=="E"){
            for (unsigned int i=0; i<v.size(); i++ ){
                desBlock * newBlock = er->encrypt_DES(v[i],"triple");    
                e.push_back(newBlock);
                v[i]="";

                
            }
            while (v.size()>0){
                v[0]="";
                v.erase(v.begin());
                
            }

        }
        else if (input=="d" || input=="decrypt" || input=="D"){
            for (unsigned int o=0; o<e.size(); o++ ){
                er->decrypt_DES(e[o],"triple")   ;    
                output="";
                for (unsigned int i = 0; i<e[o]->size; i++){    
                        output+=er->ull_to_string(e[o]->data[i]);
                }
                delete[] e[o]->data;
                delete e[o];

                v.push_back(output);
                output="";
            }

            while (e.size()>0){
                e.erase(e.begin());
            }

        }        
        else{
            if (input=="x" || input=="exit" || input=="X"){
                input="";
                std::cerr<<std::endl<<"Exiting Program.."<<std::endl;
                for (unsigned int i=0; i<e.size(); i++){
                    e[i]->data = 0;
                    delete[] e[i]->data;
                    delete e[i];
                }
                 while (e.size()>0){
                    e.erase(e.begin());
                }
                done=true;
            }
            else{
                v.push_back(input);
                input="";
            }
        }
        input="";
    }
    for (unsigned int i =0; i<v.size(); i++){
        v[i]="";
    }
    while (v.size()>0){
        v.pop_back();
    }
         
//    std::cerr<<"DES TEST"<<std::endl;
    /***********************************************************************************************************/
    er->anyKey();
    delete er;
    return 0;
}