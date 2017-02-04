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

