#include "encrypt_ram.h"
unsigned int * encrypt_ram::function_f(unsigned int* data, unsigned long long* key){
    unsigned long long* e;
    e = permuteEbit(*data);
    unsigned long long xorOp;
    xorOp = (*key)^(*e);
    unsigned int position = 48;
    unsigned int b[8],col[8],row[8];
    for (int n=0; n<8; n++){ b[n]=col[n]=row[n]=0; }
    for (int n=0; n<8; n++){
        for (int i=0; i<6; i++){
            if (checkBit(xorOp, position-1-i))
                b[n] += (unsigned int)pow(2.0,6-i-1);
            if (i==0)
                row[n] = (int)encrypt_ram::checkBit(xorOp, position-1-i)*2+(int)encrypt_ram::checkBit(xorOp, position-i-6);
            else if (i<5)
                col[n] += (int)encrypt_ram::checkBit(xorOp, position-1-i)*(int)pow(2.0,5-i-1);
        }
        position -=6;
    }
    unsigned int f_result=0;
    int position2 = 32-1;
    for (int x=0; x<8; x++){
        unsigned int result = function_s(x+1,row[x],col[x]);
        for (int x=3; x>=0; x--){
            setBit(f_result,position2--,checkBit(result,x) );
        }
    }
    *e=0;
    delete e;
    return permutePbit(f_result);
}

encrypt_ram::encrypt_ram(){
    if (!Check_CPU_support_AES()){
        printf("Cpu does not support AES instruction set. Bailing out.\n");
        exit(1);
    }
    aesKey=NULL;
    aesKeySize = 0;
    for (int j=0; j<=16; j++){
        K[j]=NULL;
        K2[j]=NULL;
        K3[j]=NULL;
    }
    curlOn = false;//true; //toggles random api usage, need to hard code values in set key if set to false(there is default)
    
    //NOT IN REPO!!!!!!!!!!!!!!, put randomAPI key line 1 in file called config.txt
    if (curlOn){
        std::ifstream file("config.txt"); //not included in repo, first line should be your randomAPI key
        if (!std::getline(file, rk)){
            std::cerr << "cannot find configuration file values in config.txt" <<std::endl;
            exit(1);
        }
        file.close();
    }       
}

void encrypt_ram::setDESKey(){
//begin init
    aesKey=NULL;
    aesKeySize = 0;
    unsigned long long* key;
    unsigned long long* key2;
    unsigned long long* key3;
    if (curlOn){
        key=getNewLL();
        key2=getNewLL();
        key3=getNewLL();
    }
    else{
        key=new unsigned long long;
        key2=new unsigned long long;
        key3=new unsigned long long;
      
        *key=1234567321;
        *key2=7654321123;
        *key3=5512355123;
    }   
    unsigned long long* permutation1a;
    unsigned long long* permutation1b;
    unsigned long long* permutation1c;
    
    permutation1a = permuteKey(*key);
    permutation1b = permuteKey(*key2);
    permutation1c = permuteKey(*key3);
    unsigned int C[17], D[17]; //16+1 for the initial split
    unsigned int C2[17], D2[17]; //16+1 for the initial split
    unsigned int C3[17], D3[17]; //16+1 for the initial split
    bitwiseSplitDES(*permutation1a,C[0],D[0]);//this is the initial split
    bitwiseSplitDES(*permutation1b,C2[0],D2[0]);//this is the initial split
    bitwiseSplitDES(*permutation1c,C3[0],D3[0]);//this is the initial split

    //creates 16 48-bit subkeys (extra bits are cleared at left for easy c++)
    for (int x=1; x<=16; x++){
        //left shift
        C[x] = C[x-1];
        D[x] = D[x-1];
        C2[x] = C2[x-1];
        D2[x] = D2[x-1];
        C3[x] = C3[x-1];
        D3[x] = D3[x-1];
        

        leftShift(C[x]);
        leftShift(D[x]);
        leftShift(C2[x]);
        leftShift(D2[x]);
        leftShift(C3[x]);
        leftShift(D3[x]);
                

        if (x>=3 && x!=16 && x!=9){
            leftShift(C[x]);
            leftShift(D[x]);
            leftShift(C2[x]);
            leftShift(D2[x]);
            leftShift(C3[x]);
            leftShift(D3[x]);
        }
    }
    for (int x=1; x<=16; x++){
        K[x] = permuteKey2(C[x],D[x]);
        K2[x] = permuteKey2(C2[x],D2[x]);
        K3[x] = permuteKey2(C3[x],D3[x]);
    }
    *permutation1a=*permutation1b=*permutation1c=0;
    *key=*key2=*key3=0;
    delete permutation1a;
    delete permutation1b;
    delete permutation1c;
    delete key;
    delete key2;
    delete key3;
}

encrypt_ram::~encrypt_ram(){
    
    if (aesKey!=NULL)
        delete[] aesKey;
    for (int x=1; x<=16; x++){
        if (K[x]!=NULL){
            *K[x]=0;
            delete K[x];
        }
        if (K2[x]!=NULL){
            *K2[x]=0;
            delete K2[x];
        }
        if (K3[x]!=NULL){
            *K3[x]=0;
            delete K3[x];
        }
    }
}

void encrypt_ram::desEncryptSingleBlock(unsigned long long & message,uint8_t key){
    checkDESkeyChoice(key);
    unsigned long long* initialPermutation;
    initialPermutation = permuteInitial(message);
    unsigned int *l[17], *r[17];
    l[0] = new unsigned int;
    *l[0] = 0;
    r[0] = new unsigned int;
    *r[0] = 0;
    bitwiseSplit(*initialPermutation,*l[0],*r[0]);
    for (int n = 1 ; n<=16 ; n++){
        l[n] = new unsigned int;
        *l[n] = 0;
        *l[n] = *r[n-1];
        if (key==1)
            r[n] = function_f(r[n-1],K[n]);
        else if (key==2)
            r[n] = function_f(r[n-1],K2[n]);
        else //key==3
            r[n] = function_f(r[n-1],K3[n]);
        *r[n] = (*r[n])^(*l[n-1]);
    }
    unsigned long long val = (unsigned long long) *r[16] << 32 | *l[16]; //combine the two values, but reversed R[16]L[16]
    unsigned long long* finalPermutation = permuteFinal(val);
    //this may need to change, the behavior switchs the values 
    message =*finalPermutation;
    //sample output
    //displayBinary(*finalPermutation);
    //std::cerr << "this is encrypted value of input : "<<std::hex << *finalPermutation << std::dec<<std::endl;
    //std::cerr << "this is decimal equivalent: " << *finalPermutation << std::dec<<std::endl;
    //cleanup
    *initialPermutation=*finalPermutation=0;
    delete initialPermutation;
    delete finalPermutation;
    for (int n = 0 ; n<=16 ; n++){
        *l[n]= *r[n]=0;
        delete l[n];
        delete r[n];
    }

}

void encrypt_ram::checkDESMode(std::string mode){
    if (!(mode=="single"|| mode=="triple")){
        std::cerr << "Invalid DES mode selection (single, triple) only.. Exiting"<<std::endl;
        exit(1);
    }
}


void encrypt_ram::checkDESkeyChoice(uint8_t key){
    if (!(key==1|| key==2 || key==3)){
        std::cerr << "Invalid DES key choice (1,2,3) only.. Exiting"<<std::endl;
        exit(1);
    }
}

desBlock* encrypt_ram::encrypt_DES(std::string &input,std::string mode){
    checkDESMode(mode);
    desBlock* newBlock=new desBlock;
    unsigned int desBlocks = input.length()/6;
    if (input.length()%6>0)//if leftovers after full blocks, fill partial block
        desBlocks++;
    newBlock->size = desBlocks;
    newBlock->data = new unsigned long long[desBlocks];
    int position=0;
    for (unsigned int j=0; j<desBlocks; j++){
        std::string temp = input.substr(position,6);
        std::string temp2 = string_to_nstring(temp);
        position +=6;
        unsigned long long * temp3 = nstring_to_ull(temp2);
        newBlock->data[j] = *temp3;
        *temp3=0;
        delete temp3;
        
        desEncryptSingleBlock(newBlock->data[j],1);
        if (mode=="triple"){
            desDecryptSingleBlock(newBlock->data[j],2);
            desEncryptSingleBlock(newBlock->data[j],3);
        }
        temp="";
        temp2="";

    }
    input="";
    return newBlock;
}

void encrypt_ram::decrypt_DES(desBlock* input,std::string mode){
    checkDESMode(mode);
    unsigned int desBlocks = input->size;
    for (unsigned int j=0; j<desBlocks; j++){
        if (mode=="triple"){
            desDecryptSingleBlock(input->data[j],3); 
            desEncryptSingleBlock(input->data[j],2);
        }
        desDecryptSingleBlock(input->data[j],1);
    }
}

void encrypt_ram::desDecryptSingleBlock(unsigned long long & message,uint8_t key){
    checkDESkeyChoice(key);
    unsigned long long* initialPermutation;
    initialPermutation = permuteInitial(message);
    unsigned int *l[17], *r[17];
    l[0] = new unsigned int;
    *l[0] = 0;
    r[0] = new unsigned int;
    *r[0] = 0;
    bitwiseSplit(*initialPermutation,*l[0],*r[0]);
    for (int n = 1 ; n<=16 ; n++){
        l[n] = new unsigned int;
        *l[n] = 0;
        *l[n] = *r[n-1];
        if (key==1)
            r[n] = function_f(r[n-1],K[17-n]);//^*l[0];
        else if (key==2)
            r[n] = function_f(r[n-1],K2[17-n]);//^*l[0];
        else//key==3
            r[n] = function_f(r[n-1],K3[17-n]);//^*l[0];
        *r[n] = (*r[n])^(*l[n-1]);
    }
    unsigned long long val = (unsigned long long) *r[16] << 32 | *l[16]; //combine the two values, but reversed R[16]L[16]
    unsigned long long* finalPermutation = permuteFinal(val);
    //this may need to change, the behavior switchs the values
    message =*finalPermutation;
    //cleanup
    *finalPermutation=*initialPermutation=0;
    delete initialPermutation;
    delete finalPermutation;
    for (int n = 0 ; n<=16 ; n++){
        *l[n]=*r[n]=0;
        delete l[n];
        delete r[n];
    }
}

void encrypt_ram::leftShift(unsigned int &input){
    bool bit = checkBit(input,27);//this is special function for 28 bit keys, but still should modularize it
    input= input<<1;
    setBit(input,0,bit);
    clearBit(input,28);
}

unsigned long long* encrypt_ram::permuteKey(unsigned long long &input){
    unsigned long long* permutation= new unsigned long long;
    *permutation = 0;
    for (int x=0; x<56; x++){//set 56 bit key, the upper bits should be blank
        changeBit(*permutation,55-x,checkBit(input,63-pc_1[x]+1));
    }
    input = 0;
    return permutation;
}
unsigned long long* encrypt_ram::permuteKey2(unsigned int &left, unsigned int &right){
    unsigned long long* permutation= new unsigned long long;
    *permutation = 0;
    for (int x=0; x<48; x++){//set 48 bit key, the upper bits should be blank
        int bit = pc_2[x];
        bool bitValue;
        if (56-bit>=28){
            bitValue = checkBit(left,28-bit); //27 to reverse sequence, -1 to decrement bit
        }
        else{
            bitValue = checkBit(right,56-bit);
        }
        changeBit(*permutation,47-x,bitValue);
    }
    left = right = 0;
    return permutation;
}

unsigned long long * encrypt_ram::permuteInitial(unsigned long long &message){
    unsigned long long* permutation= new unsigned long long;
    *permutation = 0;
    for (int x=0; x<64; x++){//set 56 bit key, the upper bits should be blank
        changeBit(*permutation,63-x,checkBit(message,64-ip[x]));
    }
    message = 0;
    return permutation;
}//should make this modular and combine permutation functions

unsigned long long * encrypt_ram::permuteEbit(unsigned int &message){
    unsigned long long* permutation= new unsigned long long;
    *permutation = 0;
    for (int x=0; x<48; x++){//set 48 bit key, the upper bits should be blank
        changeBit(*permutation,47-x,checkBit(message,32-ebit[x]));
    }
    message = 0;
    return permutation;
}
unsigned long long * encrypt_ram::permuteFinal(unsigned long long &message){
    unsigned long long* permutation= new unsigned long long;
    *permutation = 0;
    for (int x=0; x<64; x++){//set 48 bit key, the upper bits should be blank
        changeBit(*permutation,63-x,checkBit(message,64-ipInverse[x]));
    }
    message = 0;
    return permutation;
}

unsigned int * encrypt_ram::permutePbit(unsigned int &message){
    unsigned int* permutation= new unsigned int;
    *permutation = 0;
    for (int x=0; x<32; x++){//set 48 bit key, the upper bits should be blank
        changeBit(*permutation,31-x,checkBit(message,32-p[x]));
    }
    message = 0;
    return permutation;
}

void encrypt_ram::displayBinary(unsigned long long input){
    for (int x=63; x>=0; x--){
        std::cerr << checkBit(input,x);
        if (x%4==0)
            std::cerr<<" ";
    }
    std::cerr << std::endl;
}
void encrypt_ram::displayBinary(unsigned int input){
    for (int x=31; x>=0; x--)
        std::cerr << checkBit(input,x);
    std::cerr << std::endl;
}
void encrypt_ram::displayBinaryX(unsigned int input,unsigned int bits){
    for (int x=bits-1; x>=0; x--){
        std::cerr << checkBit(input,x);
        if (x%6==0)
            std::cerr<<" ";
    }

}
void encrypt_ram::displayBinaryX(unsigned long long input,unsigned int bits){
    for (int x=bits-1; x>=0; x--){
        std::cerr << checkBit(input,x);
        if (x%6==0)
            std::cerr<<" ";
    }
}

unsigned int encrypt_ram::function_s(uint8_t table,uint8_t row, uint8_t column ){
   uint8_t address = (row) * 16 + (column);
   switch(table){
    case 1:
        return s1[address];
    case 2:
        return s2[address];
    case 3:
        return s3[address];
    case 4:
        return s4[address];
    case 5:
        return s5[address];
    case 6:
        return s6[address];
    case 7:
        return s7[address];
    case 8:
        return s8[address];
    default:
        return 0;
   }
}
void encrypt_ram::displayBinary(unsigned short input){
    for (int x=15; x>=0; x--)
        std::cerr << checkBit(input,x);
    std::cerr << std::endl;
}
void encrypt_ram::displayBinary(unsigned long input){
    for (int x=15; x>=0; x--)
        std::cerr << checkBit(input,x);
    std::cerr << std::endl;
}
void encrypt_ram::bitwiseSplit(unsigned long long &input, unsigned int &leftDigits, unsigned int &rightDigits){
    //64 bit  => 32 x 2 version
    //splits unsigned long long into two  bit shorts, the left and right containing respective bits
    //probably should eventually hard code these values for efficiency since function isn't modular
    int bits = 64;
    unsigned long long leftShift = bits/2;
    unsigned long long rightMask = (unsigned long long)pow(2.0,leftShift)-1;
    leftDigits = input >> leftShift;
    rightDigits = input & rightMask;
}
void encrypt_ram::bitwiseSplit(unsigned int &input, unsigned short &leftDigits, unsigned short &rightDigits){
    //32 bit  => 16 x 2 version
    //splits unsigned integer into two  bit shorts, the left and right containing respective bits
    int bits = 32;
    unsigned int leftShift = bits/2;
    unsigned int rightMask = (unsigned int)pow(2.0,leftShift)-1;
    leftDigits = input >> leftShift;
    rightDigits = input & rightMask;
}
void encrypt_ram::bitwiseSplitDES(unsigned long long &input, unsigned int &leftDigits, unsigned int &rightDigits){
    //56(64) bit  => 28(32) x 2 version
    leftDigits = input >> 28;
    unsigned long rightMask = 268435455;
    rightDigits = input & rightMask;
}

int encrypt_ram::Check_CPU_support_AES(){
    //INTEL CODE
    unsigned int a,b,c,d;
    cpuid(1, a,b,c,d);
    return (c & 0x2000000);
}


__m128i encrypt_ram::AES_128_ASSIST (__m128i temp1, __m128i temp2){
    //INTEL CODE
    __m128i temp3;
    temp2 = _mm_shuffle_epi32 (temp2 ,0xff);
    temp3 = _mm_slli_si128 (temp1, 0x4);
    temp1 = _mm_xor_si128 (temp1, temp3);
    temp3 = _mm_slli_si128 (temp3, 0x4);
    temp1 = _mm_xor_si128 (temp1, temp3);
    temp3 = _mm_slli_si128 (temp3, 0x4);
    temp1 = _mm_xor_si128 (temp1, temp3);
    temp1 = _mm_xor_si128 (temp1, temp2);
    return temp1;
}

void encrypt_ram::AES_128_Key_Expansion (const unsigned char *userkey,const unsigned char *key){
    //INTEL CODE
    __m128i temp1, temp2;
    __m128i *Key_Schedule = (__m128i*)key;
    temp1 = _mm_loadu_si128((__m128i*)userkey);
    Key_Schedule[0] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1 ,0x1);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[1] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x2);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[2] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x4);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[3] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x8);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[4] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x10);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[5] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x20);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[6] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x40);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[7] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x80);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[8] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x1b);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[9] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x36);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[10] = temp1;
}

/* Note – the length of the output buffer is assumed to be a multiple of 16 bytes */
void encrypt_ram::AES_ECB_encrypt(const unsigned char *in,unsigned char *out,unsigned long length,const char *key,int number_of_rounds){
    //INTEL CODE
    __m128i tmp;
    unsigned int i;
    int j;
    //pointer to the PLAINTEXT
    //pointer to the CIPHERTEXT buffer
    //text length in bytes
    //pointer to the expanded key schedule
    //number of AES rounds 10,12 or 14
    if(length%16)
        length = length/16+1;
    else
        length = length/16;
    for(i=0; i < length; i++){
        tmp = _mm_loadu_si128 (&((__m128i*)in)[i]);
        tmp = _mm_xor_si128 (tmp,((__m128i*)key)[0]);
        for(j=1; j <number_of_rounds; j++){
            tmp = _mm_aesenc_si128 (tmp,((__m128i*)key)[j]);
        }
        tmp = _mm_aesenclast_si128 (tmp,((__m128i*)key)[j]);
        _mm_storeu_si128 (&((__m128i*)out)[i],tmp);
    }

}

void encrypt_ram::AES_ECB_decrypt(const unsigned char *in,unsigned char *out,unsigned long length,const char *key,int number_of_rounds){
    //INTEL CODE
    __m128i tmp;
    unsigned int i;
    int j;
    //pointer to the CIPHERTEXT
    //pointer to the DECRYPTED TEXT buffer
    //text length in bytes
    //pointer to the expanded key schedule
    //number of AES rounds 10,12 or 14
    if(length%16)
        length = length/16+1;
    else
        length = length/16;
    for(i=0; i < length; i++){
        tmp = _mm_loadu_si128 (&((__m128i*)in)[i]);
        tmp = _mm_xor_si128 (tmp,((__m128i*)key)[0]);
        for(j=1; j <number_of_rounds; j++){
            tmp = _mm_aesdec_si128 (tmp,((__m128i*)key)[j]);
        }
        tmp = _mm_aesdeclast_si128 (tmp,((__m128i*)key)[j]);
        _mm_storeu_si128 (&((__m128i*)out)[i],tmp);
    }
}

void encrypt_ram::KEY_192_ASSIST(__m128i* temp1, __m128i * temp2, __m128i * temp3){
    //INTEL CODE
    __m128i temp4;
    *temp2 = _mm_shuffle_epi32 (*temp2, 0x55);
    temp4 = _mm_slli_si128 (*temp1, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4);
    *temp1 = _mm_xor_si128 (*temp1, *temp2);
    *temp2 = _mm_shuffle_epi32(*temp1, 0xff);
    temp4 = _mm_slli_si128 (*temp3, 0x4);
    *temp3 = _mm_xor_si128 (*temp3, temp4);
    *temp3 = _mm_xor_si128 (*temp3, *temp2);
}

void encrypt_ram::AES_192_Key_Expansion (const unsigned char *userkey,unsigned char *key){
    //INTEL CODE
    __m128i temp1, temp2, temp3;
    __m128i *Key_Schedule = (__m128i*)key;
    temp1 = _mm_loadu_si128((__m128i*)userkey);
    temp3 = _mm_loadu_si128((__m128i*)(userkey+16));
    Key_Schedule[0]=temp1;
    Key_Schedule[1]=temp3;
    temp2=_mm_aeskeygenassist_si128 (temp3,0x1);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    Key_Schedule[1] = (__m128i)_mm_shuffle_pd((__m128d)Key_Schedule[1],(__m128d)temp1,0);
    Key_Schedule[2] = (__m128i)_mm_shuffle_pd((__m128d)temp1,(__m128d)temp3,1);
    temp2=_mm_aeskeygenassist_si128 (temp3,0x2);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    Key_Schedule[3]=temp1;
    Key_Schedule[4]=temp3;
    temp2=_mm_aeskeygenassist_si128 (temp3,0x4);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    Key_Schedule[4] = (__m128i)_mm_shuffle_pd((__m128d)Key_Schedule[4],
    (__m128d)temp1,0);
    Key_Schedule[5] = (__m128i)_mm_shuffle_pd((__m128d)temp1,(__m128d)temp3,1);
    temp2=_mm_aeskeygenassist_si128 (temp3,0x8);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    Key_Schedule[6]=temp1;
    Key_Schedule[7]=temp3;
    temp2=_mm_aeskeygenassist_si128 (temp3,0x10);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    Key_Schedule[7] = (__m128i)_mm_shuffle_pd((__m128d)Key_Schedule[7],
    (__m128d)temp1,0);
    Key_Schedule[8] = (__m128i)_mm_shuffle_pd((__m128d)temp1,(__m128d)temp3,1);
    temp2=_mm_aeskeygenassist_si128 (temp3,0x20);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    Key_Schedule[9]=temp1;
    Key_Schedule[10]=temp3;
    temp2=_mm_aeskeygenassist_si128 (temp3,0x40);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    Key_Schedule[10] = (__m128i)_mm_shuffle_pd((__m128d)Key_Schedule[10],
    (__m128d)temp1,0);
    Key_Schedule[11] = (__m128i)_mm_shuffle_pd((__m128d)temp1,(__m128d)temp3,1);
    temp2=_mm_aeskeygenassist_si128 (temp3,0x80);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    Key_Schedule[12]=temp1;
}

void encrypt_ram::KEY_256_ASSIST_1(__m128i* temp1, __m128i * temp2){
    //INTEL CODE
    __m128i temp4;
    *temp2 = _mm_shuffle_epi32(*temp2, 0xff);
    temp4 = _mm_slli_si128 (*temp1, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4);
    *temp1 = _mm_xor_si128 (*temp1, *temp2);
}

void encrypt_ram::KEY_256_ASSIST_2(__m128i* temp1, __m128i * temp3){
    //INTEL CODE
    __m128i temp2,temp4;
    temp4 = _mm_aeskeygenassist_si128 (*temp1, 0x0);
    temp2 = _mm_shuffle_epi32(temp4, 0xaa);
    temp4 = _mm_slli_si128 (*temp3, 0x4);
    *temp3 = _mm_xor_si128 (*temp3, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp3 = _mm_xor_si128 (*temp3, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp3 = _mm_xor_si128 (*temp3, temp4);
    *temp3 = _mm_xor_si128 (*temp3, temp2);
}

void encrypt_ram::AES_256_Key_Expansion (const unsigned char *userkey,const unsigned char *key){
    //INTEL CODE
    __m128i temp1, temp2, temp3;
    __m128i *Key_Schedule = (__m128i*)key;
    temp1 = _mm_loadu_si128((__m128i*)userkey);
    temp3 = _mm_loadu_si128((__m128i*)(userkey+16));
    Key_Schedule[0] = temp1;
    Key_Schedule[1] = temp3;
    temp2 = _mm_aeskeygenassist_si128 (temp3,0x01);
    KEY_256_ASSIST_1(&temp1, &temp2);
    Key_Schedule[2]=temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    Key_Schedule[3]=temp3;
    temp2 = _mm_aeskeygenassist_si128 (temp3,0x02);
    KEY_256_ASSIST_1(&temp1, &temp2);
    Key_Schedule[4]=temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    Key_Schedule[5]=temp3;
    temp2 = _mm_aeskeygenassist_si128 (temp3,0x04);
    KEY_256_ASSIST_1(&temp1, &temp2);
    Key_Schedule[6]=temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    Key_Schedule[7]=temp3;
    temp2 = _mm_aeskeygenassist_si128 (temp3,0x08);
    KEY_256_ASSIST_1(&temp1, &temp2);
    Key_Schedule[8]=temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    Key_Schedule[9]=temp3;
    temp2 = _mm_aeskeygenassist_si128 (temp3,0x10);
    KEY_256_ASSIST_1(&temp1, &temp2);
    Key_Schedule[10]=temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    Key_Schedule[11]=temp3;
    temp2 = _mm_aeskeygenassist_si128 (temp3,0x20);
    KEY_256_ASSIST_1(&temp1, &temp2);
    Key_Schedule[12]=temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    Key_Schedule[13]=temp3;
    temp2 = _mm_aeskeygenassist_si128 (temp3,0x40);
    KEY_256_ASSIST_1(&temp1, &temp2);
    Key_Schedule[14]=temp1;
}

void encrypt_ram::AES_CBC_encrypt(const unsigned char *in,unsigned char *out,unsigned char ivec[16],unsigned long length,unsigned char *key,int number_of_rounds){
    //INTEL CODE
    __m128i feedback,data;
    unsigned int i;
    int j;
    if (length%16)
        length = length/16+1;
    else 
        length /=16;
    feedback=_mm_loadu_si128 ((__m128i*)ivec);
    for(i=0; i < length; i++){
        data = _mm_loadu_si128 (&((__m128i*)in)[i]);
        feedback = _mm_xor_si128 (data,feedback);
        feedback = _mm_xor_si128 (feedback,((__m128i*)key)[0]);
    for(j=1; j <number_of_rounds; j++)
        feedback = _mm_aesenc_si128 (feedback,((__m128i*)key)[j]);
        feedback = _mm_aesenclast_si128 (feedback,((__m128i*)key)[j]);
        _mm_storeu_si128 (&((__m128i*)out)[i],feedback);
    }
}

void encrypt_ram::AES_CBC_decrypt(const unsigned char *in,unsigned char *out,unsigned char ivec[16],unsigned long length,unsigned char *key,int number_of_rounds){
    //INTEL CODE
    __m128i data,feedback,last_in;
    unsigned int i;
    int j;
    if (length%16)
        length = length/16+1;
    else
        length /=16;
    feedback=_mm_loadu_si128 ((__m128i*)ivec);
    for(i=0; i < length; i++){
        last_in=_mm_loadu_si128 (&((__m128i*)in)[i]);
        data = _mm_xor_si128 (last_in,((__m128i*)key)[0]);
        for(j=1; j <number_of_rounds; j++){
            data = _mm_aesdec_si128 (data,((__m128i*)key)[j]);
        }
        data = _mm_aesdeclast_si128 (data,((__m128i*)key)[j]);
        data = _mm_xor_si128 (data,feedback);
        _mm_storeu_si128 (&((__m128i*)out)[i],data);
        feedback=last_in;
    }
}

void encrypt_ram::AES_CTR_encrypt (const unsigned char *in,unsigned char *out,const unsigned char ivec[8],const unsigned char nonce[4],unsigned long length,const unsigned char *key,int number_of_rounds){
    //INTEL CODE
    __m128i ctr_block, tmp, ONE, BSWAP_EPI64;
    unsigned int i;
    int j;
    if (length%16)
        length = length/16 + 1;
    else 
        length/=16;
    ONE = _mm_set_epi32(0,1,0,0);
    BSWAP_EPI64 = _mm_setr_epi8(7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8);
    ctr_block = _mm_setzero_si128();
    ctr_block = _mm_insert_epi64(ctr_block, *(long long*)ivec, 1);
    ctr_block = _mm_insert_epi32(ctr_block, *(long*)nonce, 1);
    ctr_block = _mm_srli_si128(ctr_block, 4);
    ctr_block = _mm_shuffle_epi8(ctr_block, BSWAP_EPI64);
    ctr_block = _mm_add_epi64(ctr_block, ONE);
    for(i=0; i < length; i++){
        tmp = _mm_shuffle_epi8(ctr_block, BSWAP_EPI64);
        ctr_block = _mm_add_epi64(ctr_block, ONE);
        tmp = _mm_xor_si128(tmp, ((__m128i*)key)[0]);
        for(j=1; j <number_of_rounds; j++) {
            tmp = _mm_aesenc_si128 (tmp, ((__m128i*)key)[j]);
        }
        tmp = _mm_aesenclast_si128 (tmp, ((__m128i*)key)[j]);
        tmp = _mm_xor_si128(tmp,_mm_loadu_si128(&((__m128i*)in)[i]));
        _mm_storeu_si128 (&((__m128i*)out)[i],tmp);
    }
}

int encrypt_ram::AES_set_encrypt_key (const unsigned char *userKey,const int bits,AES_KEY *key){
    if (!userKey || !key)
        return -1;
    if (bits == 128)
    {
        AES_128_Key_Expansion (userKey,reinterpret_cast<const unsigned char*>(key));
        key->nr = 10;
        return 0;
    }
    else if (bits == 192)
    {
        AES_192_Key_Expansion (userKey,reinterpret_cast<unsigned char*>(key));
        key->nr = 12;
        return 0;
    }
    else if (bits == 256)
    {
        AES_256_Key_Expansion (userKey,reinterpret_cast<const unsigned char*>(key));
        key->nr = 14;
        return 0;
    }
    return -2;
}

int encrypt_ram::AES_set_decrypt_key (const unsigned char *userKey,const int bits,AES_KEY *key){
    int nr;
    AES_KEY temp_key;
    __m128i *Key_Schedule = (__m128i*)key->KEY;
    __m128i *Temp_Key_Schedule = (__m128i*)temp_key.KEY;
    if (!userKey || !key)
        return -1;
    if (AES_set_encrypt_key(userKey,bits,&temp_key) == -2)
        return -2;
    nr = temp_key.nr;
    key->nr = nr;
    Key_Schedule[nr] = Temp_Key_Schedule[0];
    Key_Schedule[nr-1] = _mm_aesimc_si128(Temp_Key_Schedule[1]);
    Key_Schedule[nr-2] = _mm_aesimc_si128(Temp_Key_Schedule[2]);
    Key_Schedule[nr-3] = _mm_aesimc_si128(Temp_Key_Schedule[3]);
    Key_Schedule[nr-4] = _mm_aesimc_si128(Temp_Key_Schedule[4]);
    Key_Schedule[nr-5] = _mm_aesimc_si128(Temp_Key_Schedule[5]);
    Key_Schedule[nr-6] = _mm_aesimc_si128(Temp_Key_Schedule[6]);
    Key_Schedule[nr-7] = _mm_aesimc_si128(Temp_Key_Schedule[7]);
    Key_Schedule[nr-8] = _mm_aesimc_si128(Temp_Key_Schedule[8]);
    Key_Schedule[nr-9] = _mm_aesimc_si128(Temp_Key_Schedule[9]);
    if(nr>10){
        Key_Schedule[nr-10] = _mm_aesimc_si128(Temp_Key_Schedule[10]);
        Key_Schedule[nr-11] = _mm_aesimc_si128(Temp_Key_Schedule[11]);
    }
    if(nr>12){
        Key_Schedule[nr-12] = _mm_aesimc_si128(Temp_Key_Schedule[12]);
        Key_Schedule[nr-13] = _mm_aesimc_si128(Temp_Key_Schedule[13]);
    }
    Key_Schedule[0] = Temp_Key_Schedule[nr];
    return 0;
}

void encrypt_ram::setAESKey(int size){
    int keys=0;
    std::string resultString="";
    keys=size/8;
    if (!(size==128 || size==256)){
        std::cerr<<"unsupported key size "<<size<<", exiting."<<std::endl;
    }

    if (aesKey!=NULL)
        delete[] aesKey;
    aesKeySize = size;
    aesKey = new ALIGN16 uint8_t[keys];
    int* results;
    if (curlOn)
        results=secureRequest(std::to_string(keys),rk);
    
    
    for(int k=0;k<keys;k++){
        if (curlOn)
            aesKey[k]=results[k];
        else
            aesKey[k]=128+k;
    }
    if (curlOn){
        for (int i=0; i<10; i++){
            results[i]=0;
        }
        delete[] results;
    }
    AES_set_encrypt_key(aesKey, size, &key);
    AES_set_decrypt_key(aesKey, size, &decrypt_key);
}

//converts NUMERICAL string..
unsigned long long* encrypt_ram::nstring_to_ull(std::string input){//wrapper for stoull call
    std::string str = input;
    for(unsigned int i=0; i<8&&str.length()!=0; i++)
        if(str[i] == ' '){
            str.erase(i--,1);            
        }
    std::string::size_type sz = 0;   // alias of size_t
    unsigned long long* ll=new unsigned long long;
    *ll=0;
    *ll = std::stoull (str,&sz,0);
    return ll;
}

std::string encrypt_ram::string_to_nstring(std::string &input){//wrapper for stoull call
    std::string convertedString="";
    for (uint8_t f=0; f<6; f++){//can fit 18x3 digit combos inside 64 bit long long
        if (f<input.length()){
            std::string addMe= std::to_string((int)input[f]);
            if (f>0 && addMe.length()<2){
                addMe.insert(0,"0");
                addMe.insert(0,"0");
            }
            else if (f>0 && addMe.length()<3)
                addMe.insert(0,"0");
            convertedString+=addMe;
        }
        else{//string was short characters, pad 0s at end for null 
            convertedString+="000";
        }

    }
    return convertedString;
}

std::string encrypt_ram::ull_to_string(unsigned long long &input){
    //input should be 18 digits
    std::string output="asdfgh";//initalize 6 digits so can access each position
    unsigned long long mod =1000000000000000; //since input should be 18 digit will use this:
                                              //basically to "decimal shift" the number by groups of 3 digits
    unsigned int temp;
    for (int j=0; j<6; j++){
        temp = input/mod;
        output[j]=(char)temp;   
        input -=mod*temp;
        mod/=1000;
    }
    
    
    
    return output;
}

//need input checking on values here! arguments, set to NONE for single line call
std::string encrypt_ram::call_curl(std::string address, std::string arguments){
    CURL *curl;
    CURLcode responseFromRequest;
    std::string resultString="";
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, address.c_str());//request this data
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        if (arguments!="NONE"){
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, arguments.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, arguments.length());

        }
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, encrypt_ram::WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resultString);
        responseFromRequest = curl_easy_perform(curl);
        if (responseFromRequest != CURLE_OK){
            fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(responseFromRequest));
            exit(1);
        }
    }
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return resultString;
}

bool encrypt_ram::anyKey(){
    std::cerr << "Press any key + enter to continue...x/exit to exit" << std::endl;
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

size_t encrypt_ram::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp){
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

unsigned long long* encrypt_ram::getNewLL(){
    int* results=secureRequest("6",rk);
    unsigned long long* returnMe = new unsigned long long;
    *returnMe=0;
    int position=15;
    for(int k=0;k<6;k++){
        *returnMe += results[k]*pow(10,position);
        position-=3;
    }
    for (int i=0; i<6; i++){
        results[i]=0;
    }
    delete[] results;
    return returnMe;




//    return encrypt_ram::nstring_to_ull(encrypt_ram::call_curl("https://www.random.org/cgi-bin/randbyte?nbytes=7%26format=d","NONE"));
}

void encrypt_ram::AES_CBC_encrypt_parallelize_4_blocks(const unsigned char *in,unsigned char *out,unsigned char ivec1[16],unsigned char ivec2[16],unsigned char ivec3[16],unsigned char ivec4[16],unsigned long length,const unsigned char *key,int nr){
    //INTEL CODE
    __m128i feedback1,feedback2,feedback3,feedback4;
    __m128i data1,data2,data3,data4;
    unsigned int i;
    int j;
    feedback1=_mm_loadu_si128((__m128i*)ivec1);
    feedback2=_mm_loadu_si128((__m128i*)ivec2);
    feedback3=_mm_loadu_si128((__m128i*)ivec3);
    feedback4=_mm_loadu_si128((__m128i*)ivec4);
    for(i=0; i < length/16/4; i++){
        data1 = _mm_loadu_si128 (&((__m128i*)in)[i*4+0]);
        data2 = _mm_loadu_si128 (&((__m128i*)in)[i*4+1]);
        data3 = _mm_loadu_si128 (&((__m128i*)in)[i*4+2]);
        data4 = _mm_loadu_si128 (&((__m128i*)in)[i*4+3]);
        feedback1 = _mm_xor_si128 (data1,feedback1);
        feedback2 = _mm_xor_si128 (data2,feedback2);
        feedback3 = _mm_xor_si128 (data3,feedback3);
        feedback4 = _mm_xor_si128 (data4,feedback4);
        feedback1 = _mm_xor_si128 (feedback1,((__m128i*)key)[0]);
        feedback2 = _mm_xor_si128 (feedback2,((__m128i*)key)[0]);
        feedback3 = _mm_xor_si128 (feedback3,((__m128i*)key)[0]);
        feedback4 = _mm_xor_si128 (feedback4,((__m128i*)key)[0]);
        for(j=1; j <nr;j++){
            feedback1 = _mm_aesenc_si128 (feedback1,((__m128i*)key)[j]);
            feedback2 = _mm_aesenc_si128 (feedback2,((__m128i*)key)[j]);
            feedback3 = _mm_aesenc_si128 (feedback3,((__m128i*)key)[j]);
            feedback4 = _mm_aesenc_si128 (feedback4,((__m128i*)key)[j]);
        }
        feedback1 = _mm_aesenclast_si128 (feedback1,((__m128i*)key)[j]);
        feedback2 = _mm_aesenclast_si128 (feedback2,((__m128i*)key)[j]);
        feedback3 = _mm_aesenclast_si128 (feedback3,((__m128i*)key)[j]);
        feedback4 = _mm_aesenclast_si128 (feedback4,((__m128i*)key)[j]);


        _mm_storeu_si128(&((__m128i*)out)[i*4+0],feedback1);
        _mm_storeu_si128(&((__m128i*)out)[i*4+1],feedback2);
        _mm_storeu_si128(&((__m128i*)out)[i*4+2],feedback3);
        _mm_storeu_si128(&((__m128i*)out)[i*4+3],feedback4);
    }
}

void encrypt_ram::AES_CBC_encrypt_parallelize_4_blocks(const unsigned char *in,unsigned char *out,unsigned char ivec[16],unsigned long length,const unsigned char *key,int nr){
    //INTEL CODE
    /*__m128i feedback1,feedback2,feedback3,feedback4;
    __m128i data1,data2,data3,data4,last_in;
    unsigned int i;
    int j;
    feedback1=_mm_loadu_si128((__m128i*)ivec);
    for(i=0; i < length/16/4; i++){
        data1 = _mm_loadu_si128 (&((__m128i*)in)[i*4+0]);
        data2 = _mm_loadu_si128 (&((__m128i*)in)[i*4+1]);
        data3 = _mm_loadu_si128 (&((__m128i*)in)[i*4+2]);
        data4 = _mm_loadu_si128 (&((__m128i*)in)[i*4+3]);
        
        feedback2 = data1;
        feedback3 = data2;
        feedback4 = data3;
        last_in = data4;
        feedback1 = _mm_xor_si128 (feedback1,((__m128i*)key)[0]);
        feedback2 = _mm_xor_si128 (feedback2,((__m128i*)key)[0]);
        feedback3 = _mm_xor_si128 (feedback3,((__m128i*)key)[0]);
        feedback4 = _mm_xor_si128 (feedback4,((__m128i*)key)[0]);
        for(j=1; j <nr;j++){
            feedback1 = _mm_aesenc_si128 (feedback1,((__m128i*)key)[j]);
            feedback2 = _mm_aesenc_si128 (feedback2,((__m128i*)key)[j]);
            feedback3 = _mm_aesenc_si128 (feedback3,((__m128i*)key)[j]);
            feedback4 = _mm_aesenc_si128 (feedback4,((__m128i*)key)[j]);
        }
        feedback1 = _mm_aesenclast_si128 (feedback1,((__m128i*)key)[j]);
        feedback2 = _mm_aesenclast_si128 (feedback2,((__m128i*)key)[j]);
        feedback3 = _mm_aesenclast_si128 (feedback3,((__m128i*)key)[j]);
        feedback4 = _mm_aesenclast_si128 (feedback4,((__m128i*)key)[j]);


        _mm_storeu_si128(&((__m128i*)out)[i*4+0],feedback1);
        _mm_storeu_si128(&((__m128i*)out)[i*4+1],feedback2);
        _mm_storeu_si128(&((__m128i*)out)[i*4+2],feedback3);
        _mm_storeu_si128(&((__m128i*)out)[i*4+3],feedback4);
    }*/
}
void encrypt_ram::AES_CBC_decrypt_parallelize_4_blocks(const unsigned char *in,unsigned char *out,unsigned char ivec1[16],unsigned char ivec2[16],unsigned char ivec3[16],unsigned char ivec4[16],unsigned long length,const unsigned char *key,int nr){
    /*__m128i feedback1,feedback2,feedback3,feedback4,last_in;
    __m128i data1,data2,data3,data4;
    unsigned int i;
    int j;
    if (length%16) 
        length = length/16 + 1; 
    else length/=16; 
    feedback1=_mm_loadu_si128((__m128i*)ivec);
    for(i=0; i < length/4; i++){
        data1=_mm_loadu_si128 (&((__m128i*)in)[i*4+0]);
        data2=_mm_loadu_si128 (&((__m128i*)in)[i*4+1]);
        data3=_mm_loadu_si128 (&((__m128i*)in)[i*4+2]);
        data4=_mm_loadu_si128 (&((__m128i*)in)[i*4+3]);
        
        feedback2 = data1;
        feedback3 = data2;
        feedback4 = data3;
        last_in = data4;
        data1 = _mm_xor_si128 (data1,((__m128i*)key)[0]);
        data2 = _mm_xor_si128 (data2,((__m128i*)key)[0]);
        data3 = _mm_xor_si128 (data3,((__m128i*)key)[0]);
        data4 = _mm_xor_si128 (data4,((__m128i*)key)[0]);
        for(j=1; j <nr;j++){
            data1 = _mm_aesdec_si128 (data1,((__m128i*)key)[j]);
            data2 = _mm_aesdec_si128 (data2,((__m128i*)key)[j]);
            data3 = _mm_aesdec_si128 (data3,((__m128i*)key)[j]);
            data4 = _mm_aesdec_si128 (data4,((__m128i*)key)[j]);
            
        }
        data1 = _mm_aesdeclast_si128 (data1,((__m128i*)key)[j]);
        data2 = _mm_aesdeclast_si128 (data2,((__m128i*)key)[j]);
        data3 = _mm_aesdeclast_si128 (data3,((__m128i*)key)[j]);
        data4 = _mm_aesdeclast_si128 (data4,((__m128i*)key)[j]);
        
        data1 = _mm_xor_si128 (data1,feedback1);
        data2 = _mm_xor_si128 (data2,feedback2);
        data3 = _mm_xor_si128 (data3,feedback3);
        data4 = _mm_xor_si128 (data4,feedback4);
        
        _mm_storeu_si128(&((__m128i*)out)[i*4+0],data1);
        _mm_storeu_si128(&((__m128i*)out)[i*4+1],data2);
        _mm_storeu_si128(&((__m128i*)out)[i*4+2],data3);
        _mm_storeu_si128(&((__m128i*)out)[i*4+3],data4);
        feedback1=last_in;
        
    }
    unsigned int j2=j;
    for(j2=i*4; j2 < length; j2++){ 
        data1=_mm_loadu_si128 (&((__m128i*)in)[j2]); 
        last_in=data1; 
        data1 = _mm_xor_si128 (data1,((__m128i*)key)[0]);    
        for(uint8_t i=1; i < nr; i++){ 
            data1 = _mm_aesdec_si128 (data1,((__m128i*)key)[i]); 
        } 
        data1 = _mm_aesdeclast_si128 (data1,((__m128i*)key)[i]); 
        data1 = _mm_xor_si128 (data1,feedback1); 
        _mm_storeu_si128 (&((__m128i*)out)[j2],data1); 
        feedback1=last_in; 
         
    } */

}
void encrypt_ram::AES_CBC_decrypt_parallelize_4_blocks(const unsigned char *in,unsigned char *out,unsigned char ivec[16],unsigned long length,const unsigned char *key,int nr){
    //INTEL CODE
    __m128i feedback1,feedback2,feedback3,feedback4,last_in;
    __m128i data1,data2,data3,data4;
    unsigned int i;
    int j;
    if (length%16) 
        length = length/16 + 1; 
    else length/=16; 
    feedback1=_mm_loadu_si128((__m128i*)ivec);
    for(i=0; i < length/4; i++){
        data1=_mm_loadu_si128 (&((__m128i*)in)[i*4+0]);
        data2=_mm_loadu_si128 (&((__m128i*)in)[i*4+1]);
        data3=_mm_loadu_si128 (&((__m128i*)in)[i*4+2]);
        data4=_mm_loadu_si128 (&((__m128i*)in)[i*4+3]);
        
        feedback2 = data1;
        feedback3 = data2;
        feedback4 = data3;
        last_in = data4;
        data1 = _mm_xor_si128 (data1,((__m128i*)key)[0]);
        data2 = _mm_xor_si128 (data2,((__m128i*)key)[0]);
        data3 = _mm_xor_si128 (data3,((__m128i*)key)[0]);
        data4 = _mm_xor_si128 (data4,((__m128i*)key)[0]);
        for(j=1; j <nr;j++){
            data1 = _mm_aesdec_si128 (data1,((__m128i*)key)[j]);
            data2 = _mm_aesdec_si128 (data2,((__m128i*)key)[j]);
            data3 = _mm_aesdec_si128 (data3,((__m128i*)key)[j]);
            data4 = _mm_aesdec_si128 (data4,((__m128i*)key)[j]);
            
        }
        data1 = _mm_aesdeclast_si128 (data1,((__m128i*)key)[j]);
        data2 = _mm_aesdeclast_si128 (data2,((__m128i*)key)[j]);
        data3 = _mm_aesdeclast_si128 (data3,((__m128i*)key)[j]);
        data4 = _mm_aesdeclast_si128 (data4,((__m128i*)key)[j]);
        
        data1 = _mm_xor_si128 (data1,feedback1);
        data2 = _mm_xor_si128 (data2,feedback2);
        data3 = _mm_xor_si128 (data3,feedback3);
        data4 = _mm_xor_si128 (data4,feedback4);
        
        _mm_storeu_si128(&((__m128i*)out)[i*4+0],data1);
        _mm_storeu_si128(&((__m128i*)out)[i*4+1],data2);
        _mm_storeu_si128(&((__m128i*)out)[i*4+2],data3);
        _mm_storeu_si128(&((__m128i*)out)[i*4+3],data4);
        feedback1=last_in;
        
    }
    unsigned int j2=j;
    for(j2=i*4; j2 < length; j2++){ 
        data1=_mm_loadu_si128 (&((__m128i*)in)[j2]); 
        last_in=data1; 
        data1 = _mm_xor_si128 (data1,((__m128i*)key)[0]);    
        for(uint8_t i=1; i < nr; i++){ 
            data1 = _mm_aesdec_si128 (data1,((__m128i*)key)[i]); 
        } 
        data1 = _mm_aesdeclast_si128 (data1,((__m128i*)key)[i]); 
        data1 = _mm_xor_si128 (data1,feedback1); 
        _mm_storeu_si128 (&((__m128i*)out)[j2],data1); 
        feedback1=last_in; 
         
    } 



}

aesBlock* encrypt_ram::encrypt_AES(std::string &input, std::string mode){
    //length here is padded size of block to encode, len is actual unpadded size of string input
    unsigned int len = input.length();//# characters in plainTextNew, must set input size for now?;
    unsigned int length=aesKeySize;
    int total = len;
        while (total>aesKeySize){
        length += aesKeySize;
        total -= aesKeySize;
    
    }
    char plainTextNew[length];
    for (unsigned int u=0; u<length;u++){
        plainTextNew[u]=0;
    }

    input.copy(plainTextNew,input.length(),0);
    ALIGN16 uint8_t* formattedNewPlainText = new ALIGN16 uint8_t[length];
    for (unsigned int j=0; j<length; j++){
        formattedNewPlainText[j] = 0;
    }
    
    //for each letter of string, encode the 8 bit integer equivalent to its char code/ascii value as entry in array
    for (unsigned int j=0; j<len; j++){
        formattedNewPlainText[j] = input[j];
    }
    ALIGN16 uint8_t* CIPHERTEXT = new ALIGN16 uint8_t[length];
    for (unsigned int j=0; j<length; j++){
        CIPHERTEXT[j] = 0;
    }
    
    aesBlock* returnMe = new aesBlock;
    returnMe->size = length;
    for (unsigned int j=0; j<len; j++){
        formattedNewPlainText[j] = input[j];
    }
    if (mode=="CTR"){
        AES_CTR_encrypt(formattedNewPlainText,CIPHERTEXT,CTR128_IV,CTR128_NONCE,length,key.KEY,key.nr);
    }
    else if (mode=="CBC"){
        AES_CBC_encrypt(formattedNewPlainText,CIPHERTEXT,CBC_IV,length,key.KEY,key.nr);
    }        
    else if (mode=="ECB"){
        AES_ECB_encrypt(formattedNewPlainText,CIPHERTEXT,length,(const char*)key.KEY,key.nr);
    }
    /*else if (mode=="CBCP"){
        AES_CBC_encrypt_parallelize_4_blocks(formattedNewPlainText,CIPHERTEXT,CBC_IV,CBC_IVb,CBC_IVc,CBC_IVd,length,key.KEY,key.nr);
    }*/
    else{
        std::cerr <<"Invalid mode for encrypt_AES"<<std::endl;
        exit(1);
    } 
    
    for (unsigned int x=0; x<length; x++)
        formattedNewPlainText[x]=0;

    delete[] formattedNewPlainText;
    
    returnMe->data=CIPHERTEXT;
    return returnMe;
}
std::string* encrypt_ram::decrypt_AES(aesBlock* input, std::string mode){
    
     char* DECRYPTEDTEXT = ( char *)malloc(sizeof(char)*input->size);
    
    if (mode=="CTR"){
        AES_CTR_encrypt(input->data,( unsigned char *)DECRYPTEDTEXT,CTR128_IV,CTR128_NONCE,input->size,key.KEY,key.nr);
    }
    else if (mode=="CBC"){
        AES_CBC_decrypt(input->data,( unsigned char *)DECRYPTEDTEXT,  CBC_IV,  input->size, decrypt_key.KEY, key.nr);
    }        
    else if (mode=="ECB"){
        AES_ECB_decrypt(input->data,( unsigned char *)DECRYPTEDTEXT, input->size, (const char*)decrypt_key.KEY, decrypt_key.nr);
    }
    /*else if (mode=="CBCP"){
        AES_CBC_decrypt_parallelize_4_blocks(input->data,( unsigned char *)DECRYPTEDTEXT,CBC_IV,input->size,key.KEY,key.nr);
    }*/
    else{
        std::cerr <<"Invalid mode for encrypt_AES (CTR,CBC,ECB)"<<std::endl;
        exit(1);
    } 
    std::string* output = new std::string(DECRYPTEDTEXT);
    free(DECRYPTEDTEXT);
    return output;
}

void encrypt_ram::checkStringMatch( std::string* string1,  std::string* string2){
    //for (unsigned int x=0; x<string1->length(); x++){
    if (*string2 != *string1){
        std::cerr<<"strings unequal in AES  mode"<<std::endl;
        exit(1);
    }
    //}
}

int* encrypt_ram::secureRequest(std::string elements, std::string& apiKey){
    std::string* returnMe = new std::string;
    std::string jsonArguments="{\"jsonrpc\":\"2.0\",\"method\":\"generateSignedIntegers\",\"params\":{\"apiKey\":\""+apiKey+"\",\"n\":"+elements+",\"min\":0,\"max\":255,\"replacement\":true,\"base\":10},\"id\":13527}";
    std::string resultString;
    resultString = call_curl("https://api.random.org/json-rpc/1/invoke", jsonArguments);
    if(resultString.find("error")!= std::string::npos){
        std::cerr << "ERROR in Random API result, exiting." << std::endl;    
        exit(1);
    }
    std::string signature="";
    std::string random="";
    int randomStartPosition = resultString.find("random")+8;
    int signatureStartPosition = resultString.find("signature")+12;
    if(resultString.find("random")!= std::string::npos){
        int length = resultString.find("}")-randomStartPosition+1;
        random = resultString.substr(randomStartPosition,length);
    }
    if(resultString.find("signature")!= std::string::npos){
        int length = resultString.find("\"",signatureStartPosition)-signatureStartPosition;
        signature = resultString.substr(signatureStartPosition,length);
    }
    jsonArguments="{\"jsonrpc\":\"2.0\",\"method\":\"verifySignature\",\"params\":{\"random\":"+random+",\"signature\":\""+signature+"\"},\"id\":198633}";
    int returnMeStart = resultString.find("[")+1;

    if(resultString.find("[")!= std::string::npos){
        int length = resultString.find("]")-returnMeStart;
        *returnMe = resultString.substr(returnMeStart,length);
    }
    
    resultString = call_curl("https://api.random.org/json-rpc/1/invoke", jsonArguments);
    if(resultString.find("true")== std::string::npos){
        std::cerr<<"Authenticity or Integrity of RandomAPI data not verified, exiting."<<std::endl;
        exit(1);
    }
    std::stringstream ss( *returnMe);
    std::string::size_type sz;  
    int* results;
    results = new int[std::stoi (elements,&sz)];
    uint8_t i = 0;
    while( ss.good() )
    {
        std::string substr;
        getline( ss, substr, ',' );
        results[i++]= std::stoi (substr,&sz);
    }    
    ss.flush();
    resultString = jsonArguments = random = signature="";
    *returnMe="";
    delete returnMe;
    return results;
}

void encrypt_ram::removeCharFromString(char c,std::string &str){
    for(unsigned int i=0; i<str.length(); i++)
        if(str[i] == c){
            str.erase(i,1);
            i=0;    
        }
}