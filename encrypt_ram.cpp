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
    delete e;
    //delete data;
    //delete key;
    return permutePbit(f_result);
}

encrypt_ram::encrypt_ram(unsigned long long & key){
//begin init
    unsigned long long* permutation1;
    permutation1 = permuteKey(key);
    unsigned int C[17], D[17]; //16+1 for the initial split
    bitwiseSplitDES(*permutation1,C[0],D[0]);//this is the initial split

    //creates 16 48-bit subkeys (extra bits are cleared at left for easy c++)
    for (int x=1; x<=16; x++){
        //left shift
        C[x] = C[x-1];
        D[x] = D[x-1];
        leftShift(C[x]);
        leftShift(D[x]);

        if (x>=3 && x!=16 && x!=9){
            leftShift(C[x]);
            leftShift(D[x]);

        }

    }
    for (int x=1; x<=16; x++){
        K[x] = permuteKey2(C[x],D[x]);
    }
    delete permutation1;
}

encrypt_ram::~encrypt_ram(){

    for (int x=1; x<=16; x++){
        delete K[x];
    }
}

void encrypt_ram::desEncrypt(unsigned long long & message){

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
        r[n] = function_f(r[n-1],K[n]);//^*l[0];
        *r[n] = (*r[n])^(*l[n-1]);
    }


    unsigned long long val = (unsigned long long) *r[16] << 32 | *l[16]; //combine the two values, but reversed R[16]L[16]
    unsigned long long* finalPermutation = permuteFinal(val);

    //sample output
    displayBinary(*finalPermutation);
    std::cout << std::hex << *finalPermutation << std::endl;

    //cleanup
    delete initialPermutation;
    delete finalPermutation;
    for (int n = 0 ; n<=16 ; n++){
        delete l[n];
        delete r[n];
    }

}


//we probably want some functionality to clear all variable values on exit??
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
    unsigned int bits = 28;//must be unsigned for displayBinaryX for bits above 31, cannot promote int constant
    //displayBinaryX(left,bits);
    //displayBinaryX(right,bits);
    //std::cout<<std::endl;
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
        std::cout << checkBit(input,x);
        if (x%4==0)
            std::cout<<" ";
    }
    std::cout << std::endl;
}
void encrypt_ram::displayBinary(unsigned int input){
    for (int x=31; x>=0; x--)
        std::cout << checkBit(input,x);
    std::cout << std::endl;
}
void encrypt_ram::displayBinaryX(unsigned int input,unsigned int bits){
    for (int x=bits-1; x>=0; x--){
        std::cout << checkBit(input,x);
        if (x%6==0)
            std::cout<<" ";
    }

}
void encrypt_ram::displayBinaryX(unsigned long long input,unsigned int bits){
    for (int x=bits-1; x>=0; x--){
        std::cout << checkBit(input,x);
        if (x%6==0)
            std::cout<<" ";
    }

}

unsigned int encrypt_ram::function_s(int table,int row, int column ){
   // std::cout << "starting function s with table="<<table<<" row="<<row<<" column="<<column<<std::endl;
   int address = (row) * 16 + (column);
   //std::cout <<"address= "<<address<<std::endl;
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
        std::cout << checkBit(input,x);
    std::cout << std::endl;
}
void encrypt_ram::displayBinary(unsigned long input){
    for (int x=15; x>=0; x--)
        std::cout << checkBit(input,x);
    std::cout << std::endl;
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

int encrypt_ram::Check_CPU_support_AES()
{
    unsigned int a,b,c,d;
    cpuid(1, a,b,c,d);
    return (c & 0x2000000);
}


void encrypt_ram::print_m128i_with_string(char const* string,__m128i data)
{
    unsigned char *pointer = (unsigned char*)&data;
    int i;
    printf("%-40s[0x",string);
    for (i=0; i<16; i++)
       printf("%02x",pointer[i]);
    printf("]\n");
}

__m128i encrypt_ram::AES_128_ASSIST (__m128i temp1, __m128i temp2)
{
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


void encrypt_ram::AES_128_Key_Expansion (const unsigned char *userkey,const unsigned char *key)
{
    

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
void encrypt_ram::AES_ECB_encrypt(const unsigned char *in,unsigned char *out,unsigned long length,const char *key,int number_of_rounds)
{
    __m128i tmp;
    int i,j;
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
void encrypt_ram::AES_ECB_decrypt(const unsigned char *in,unsigned char *out,unsigned long length,
const char *key,int number_of_rounds)
{
    __m128i tmp;
    int i,j;
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




/*****************************************************************************/




void encrypt_ram::KEY_192_ASSIST(__m128i* temp1, __m128i * temp2, __m128i * temp3)
{
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
void encrypt_ram::AES_192_Key_Expansion (const unsigned char *userkey,unsigned char *key)
{
    __m128i temp1, temp2, temp3;//  , temp4;
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
void encrypt_ram::KEY_256_ASSIST_1(__m128i* temp1, __m128i * temp2)
{
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
void encrypt_ram::KEY_256_ASSIST_2(__m128i* temp1, __m128i * temp3)
{
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
void encrypt_ram::AES_256_Key_Expansion (const unsigned char *userkey,const unsigned char *key)
{
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





void encrypt_ram::AES_CBC_encrypt(const unsigned char *in,unsigned char *out,unsigned char ivec[16],
unsigned long length,unsigned char *key,int number_of_rounds){
    __m128i feedback,data;
    int i,j;
    if (length%16)
        length = length/16+1;
    else length /=16;
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
void encrypt_ram::AES_CBC_decrypt(const unsigned char *in,unsigned char *out,unsigned char ivec[16],
unsigned long length,unsigned char *key,int number_of_rounds)
{
    __m128i data,feedback,last_in;
    int i,j;
    if (length%16)
    length = length/16+1;
    else length /=16;
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


void encrypt_ram::AES_CTR_encrypt (const unsigned char *in,unsigned char *out,const unsigned char ivec[8],
const unsigned char nonce[4],unsigned long length,const unsigned char *key,int number_of_rounds)
{
    __m128i ctr_block, tmp, ONE, BSWAP_EPI64;
    int i,j;
    if (length%16)
    length = length/16 + 1;
    else length/=16;
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
        };
        tmp = _mm_aesenclast_si128 (tmp, ((__m128i*)key)[j]);
        tmp = _mm_xor_si128(tmp,_mm_loadu_si128(&((__m128i*)in)[i]));
        _mm_storeu_si128 (&((__m128i*)out)[i],tmp);
    }
}
void encrypt_ram::print_m128i_with_string_short(char* string,__m128i data,int length)
{
    unsigned char *pointer = (unsigned char*)&data;
    int i;
    printf("%-40s[0x",string);
    for (i=0; i<length; i++)
    printf("%02x",pointer[i]);
    printf("]\n");
}

int encrypt_ram::AES_set_encrypt_key (const unsigned char *userKey,const int bits,AES_KEY *key)
{
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

int encrypt_ram::AES_set_decrypt_key (const unsigned char *userKey,const int bits,AES_KEY *key)
{
    int i,nr;;
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