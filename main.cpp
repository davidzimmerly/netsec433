#include <string>
#include <iostream>
#include <cmath>

//i should start/change all the longs to ints for consistency for other than windows platforms ( since my longs need to be 32 bit)
bool anyKey();
void bitwiseSplit(unsigned long long &input, unsigned long &leftDigits, unsigned long &rightDigits);
void bitwiseSplit(unsigned int &input, unsigned short &leftDigits, unsigned short &rightDigits);
void bitwiseSplitDES(unsigned long long &input, unsigned int &leftDigits, unsigned int &rightDigits);
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
bool checkBit(unsigned long &input, int bit){ return (input>>bit)&1;}
void changeBit(unsigned long long &input, unsigned long long bit, bool value){ unsigned long long one = 1; input ^= (-value ^input) & (one << bit);}
void changeBit(unsigned int &input, int bit, bool value){ input ^= (-value ^input) & (1 << bit);}
void changeBit(unsigned short &input, int bit, bool value){ input ^= (-value ^input) & (1 << bit);}
unsigned long long* permuteKey(unsigned long long &input);
unsigned long long* permuteKey2(unsigned int &left, unsigned int &right);
unsigned long long * permuteInitial(unsigned long long &message);
unsigned long long * permuteEbit(unsigned long &message);
void displayBinary(unsigned long long input);
void displayBinary(unsigned int input);
void displayBinaryX(unsigned int input,unsigned int bits);
void displayBinaryX(unsigned long long input,unsigned int bits);
void displayBinary(unsigned short input);
void displayBinary(unsigned long input);
void leftShift(unsigned int &input);
int function_s(int table,int row, int column );

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
unsigned long long testMessage = 81985529216486895;//0000000100100011010001010110011110001001101010111100110111101111


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
    unsigned long long* permutation1;
    permutation1 = permuteKey(des_key);
    //displayBinary(des_key);
    //std::cout << std::endl;
    //displayBinary(*permutation1);
    //std::cout << std::endl;
    //std::cout << *permutation1 << std::endl;
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
    //subkey check
    /*for (int x=0; x<=16; x++){
        std::cout << "C["<<x<<"]=";
        unsigned int bits = 28;
        displayBinaryX(C[x],bits);
        std::cout<<std::endl;
        std::cout << "D["<<x<<"]=";
        displayBinaryX(D[x],bits);
        std::cout<<std::endl;
    }*/
    unsigned long long* K[16];
    for (int x=1; x<=16; x++){
        K[x] = permuteKey2(C[x],D[x]);
    }

    //key check
    /*for (int x=1; x<=16; x++){
        std::cout<<"K[";
        if (x+1<10)
            std::cout<<"0";
        std::cout<<x<<"]=";
        unsigned int bits = 48;
        //displayBinaryX(*K[x],bits);

        std::cout<<std::endl;
        //std::cout<<"K[x]="<<*K[x]<<std::endl;
    }*/
    //displayBinary(testMessage);
    unsigned long long* initialPermutation;
    initialPermutation = permuteInitial(testMessage);
    //displayBinary(*initialPermutation);
    //std::cout<<"Splitting initial Permutation:"<<std::endl;
    unsigned long left, right;
    bitwiseSplit(*initialPermutation,left,right);
    //displayBinary(left);
    //displayBinary(right);
    unsigned long long* e;
    e = permuteEbit(right);
    displayBinaryX(*e,48);//E(R_0)
    std::cout << std::endl;
    unsigned long long xorOp;
    xorOp = (*K[1])^(*e);
    displayBinaryX(xorOp,48);

    unsigned int position = 48;
    std::cout<<std::endl;
    unsigned int b[8];
    unsigned int col[8];
    unsigned int row[8];
    for (int n=0; n<8; n++){
        b[n]=0;
        col[n]=0;
        row[n]=0;
    }

    std::cout<<std::endl;

    for (int n=0; n<8; n++){
        for (int i=0; i<6; i++){
            //std::cout << "i="<<i<< " n="<<n<<std::endl;
    //        std::cout << "checking position:" << position -1-i<< std::endl;
            std::cout << checkBit(xorOp, position-1-i) ; // these are the b values, each
            //std::cout<<std::endl;
            if (checkBit(xorOp, position-1-i))
                b[n] += (unsigned int)pow(2.0,6-i-1);
            if (i==0)
                row[n] = (int)checkBit(xorOp, position-1-i)*2+(int)checkBit(xorOp, position-i-6);
            else if (i<5)
                col[n] += (int)checkBit(xorOp, position-1-i)*(int)pow(2.0,5-i-1);

        }
        std::cout<< " ";
        std::cout<<std::endl;
        std::cout<<"row["<<n<<"]="<<row[n]<<std::endl;
        std::cout<<"col["<<n<<"]="<<col[n]<<std::endl;
        //std::cout<<"b["<<n<<"]="<<b[n]<<std::endl;
        //std::cout<<"s011011="<<function_s(1,1,13);
        position -=6;
    }

    for (int x=0; x<8; x++){
        std::cout<<"s"<<x+1<<" ="<<function_s(x+1,row[x],col[x])<<std::endl;

    }



    for (int x=1; x<=16; x++){

        delete K[x];
    }


    delete er;
    delete permutation1;
    delete initialPermutation;
	return 0;
}


//we probably want some functionality to clear all variable values on exit??
void leftShift(unsigned int &input){
    bool bit = checkBit(input,27);//this is special function for 28 bit keys, but still should modularize it
    input= input<<1;
    setBit(input,0,bit);
    clearBit(input,28);
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
//permutation tables:
const int pc_1 [56] = { 57,   49,    41,   33,    25,    17,    9,
                        1,   58,    50,   42,    34,    26,   18,
                        10,    2,    59,   51,    43,    35,   27,
                        19,   11,     3,   60,    52,    44,   36,
                        63,   55,    47,   39,    31,    23,   15,
                        7,   62,    54 ,  46,    38,    30,   22,
                        14,    6,    61,   53,    45,    37,   29,
                        21,   13,     5,   28,    20,    12,    4 };
const int pc_2 [48] = { 14,    17,   11,    24,     1,    5,
                         3,    28,   15,     6,    21,   10,
                        23,    19,   12,     4,    26,    8,
                        16,     7,   27,    20,    13,    2,
                        41,    52,   31,    37,    47,   55,
                        30,    40,   51,    45,    33,   48,
                        44,    49,   39,    56,    34,   53,
                        46,    42,   50,    36,    29,   32 };
const int ip [64] = {   58,    50,   42,    34,    26,   18,    10,    2,
                        60,    52,   44,    36,    28,   20,    12,    4,
                        62,    54,   46,    38,    30,   22,    14,    6,
                        64,    56,   48,    40,    32,   24,    16,    8,
                        57,    49,   41,    33,    25,   17,     9,    1,
                        59,    51,   43,    35,    27,   19,    11,    3,
                        61,    53,   45,    37,    29,   21,    13,    5,
                        63,    55,   47,    39,    31,   23,    15,    7};
const int ebit [48] = { 32,     1,    2,     3,     4,    5,
                         4,     5,    6,     7,     8,    9,
                         8,     9,   10,    11,    12,   13,
                        12,    13,   14,    15,    16,   17,
                        16,    17,   18,    19,    20,   21,
                        20,    21,   22,    23,    24,   25,
                        24,    25,   26,    27,    28,   29,
                        28,    29,   30,    31,    32,    1};
const int s1 [64] = {14,  4,  13,  1,   2, 15,  11,  8,   3, 10,   6, 12,   5,  9,   0,  7,
                      0, 15,   7,  4,  14,  2,  13,  1,  10,  6,  12, 11,   9,  5,   3,  8,
                      4,  1,  14,  8,  13,  6,   2, 11,  15, 12,   9,  7,   3, 10,   5,  0,
                     15, 12,   8,  2,   4,  9,   1,  7,   5, 11,   3, 14,  10,  0,   6, 13};
const int s2 [64] = {    15,  1,   8, 14,   6, 11,   3,  4,   9,  7,   2, 13,  12,  0,   5, 10,
                          3, 13,   4,  7,  15,  2,   8, 14,  12,  0,   1, 10,   6,  9,  11,  5,
                          0, 14,   7, 11,  10,  4,  13,  1,   5,  8,  12,  6,   9,  3,   2, 15,
                         13,  8,  10,  1,   3, 15,   4,  2,  11,  6,   7, 12,   0,  5,  14,  9};
const int s3 [64] = {    10,  0,   9, 14,   6,  3,  15,  5,   1, 13,  12,  7,  11,  4,   2,  8,
                         13,  7,   0,  9,   3,  4,   6, 10,   2,  8,   5, 14,  12, 11,  15,  1,
                         13,  6,   4,  9,   8, 15,   3,  0,  11,  1,   2, 12,   5, 10,  14,  7,
                          1, 10,  13,  0,   6,  9,   8,  7,   4, 15,  14,  3,  11,  5,   2, 12};
const int s4 [64] = {     7, 13,  14,  3,   0,  6,   9, 10,   1,  2,   8,  5,  11, 12,   4, 15,
                         13,  8,  11,  5,   6, 15,   0,  3,   4,  7,   2, 12,   1, 10,  14,  9,
                         10,  6,   9,  0,  12, 11,   7, 13,  15,  1,   3, 14,   5,  2,   8,  4,
                          3, 15,   0,  6,  10,  1,  13,  8,   9,  4,   5, 11,  12,  7,   2, 14};
const int s5 [64] = { 2, 12,   4,  1,   7, 10,  11,  6,   8,  5,   3, 15,  13,  0,  14,  9,
                     14, 11,   2, 12,   4,  7,  13,  1,   5,  0,  15, 10,   3,  9,   8,  6,
                      4,  2,   1, 11,  10, 13,   7,  8,  15,  9,  12,  5,   6,  3,   0, 14,
                     11,  8,  12,  7,   1, 14,   2, 13,   6, 15,   0,  9,  10,  4,   5,  3};
const int s6 [64] = {    12,  1,  10, 15,   9,  2,   6,  8,   0, 13,   3,  4,  14,  7,   5, 11,
                         10, 15,   4,  2,   7, 12,   9,  5,   6,  1,  13, 14,   0, 11,   3,  8,
                          9, 14,  15,  5,   2,  8,  12,  3,   7,  0,   4, 10,   1, 13,  11,  6,
                          4,  3,   2, 12,   9,  5,  15, 10,  11, 14,   1,  7,   6,  0,   8, 13};
const int s7 [64] = { 4, 11,   2, 14,  15,  0,   8, 13,   3, 12,   9,  7,   5, 10,   6,  1,
                     13,  0,  11,  7,   4,  9,   1, 10,  14,  3,   5, 12,   2, 15,   8,  6,
                      1,  4,  11, 13,  12,  3,   7, 14,  10, 15,   6,  8,   0,  5,   9,  2,
                      6, 11,  13,  8,   1,  4,  10,  7,   9,  5,   0, 15,  14,  2,   3, 12};
const int s8 [64] = { 13,  2,   8,  4,   6, 15,  11,  1,  10,  9,   3, 14,   5,  0,  12,  7,
                       1, 15,  13,  8,  10,  3,   7, 4,  12,  5,   6, 11,   0, 14 ,  9, 2,
                       7, 11,   4,  1,   9, 12,  14,  2,   0,  6,  10, 13,  15,  3,   5,  8,
                       2,  1,  14,  7,   4, 10,   8, 13,  15, 12,   9,  0,   3,  5,   6, 11};



unsigned long long* permuteKey(unsigned long long &input){
    unsigned long long* permutation= new unsigned long long;
    *permutation = 0;
    for (int x=0; x<56; x++){//set 56 bit key, the upper bits should be blank
        changeBit(*permutation,55-x,checkBit(input,63-pc_1[x]+1));
    }
    return permutation;
}
unsigned long long* permuteKey2(unsigned int &left, unsigned int &right){
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

    return permutation;
}
unsigned long long * permuteInitial(unsigned long long &message){
    unsigned long long* permutation= new unsigned long long;
    *permutation = 0;
    for (int x=0; x<64; x++){//set 56 bit key, the upper bits should be blank
        changeBit(*permutation,63-x,checkBit(message,64-ip[x]));
    }
    return permutation;

}//should make this modular and combine permutation functions
unsigned long long * permuteEbit(unsigned long &message){
    unsigned long long* permutation= new unsigned long long;
    *permutation = 0;
    for (int x=0; x<48; x++){//set 48 bit key, the upper bits should be blank
        changeBit(*permutation,47-x,checkBit(message,32-ebit[x]));
    }
    return permutation;
}
void displayBinary(unsigned long long input){
    for (int x=63; x>=0; x--){
        std::cout << checkBit(input,x);
        if (x%4==0)
            std::cout<<" ";
    }
    std::cout << std::endl;
}
void displayBinary(unsigned int input){
    for (int x=31; x>=0; x--)
        std::cout << checkBit(input,x);
    std::cout << std::endl;
}
void displayBinaryX(unsigned int input,unsigned int bits){
    for (int x=bits-1; x>=0; x--){
        std::cout << checkBit(input,x);
        if (x%6==0)
            std::cout<<" ";
    }

}
void displayBinaryX(unsigned long long input,unsigned int bits){
    for (int x=bits-1; x>=0; x--){
        std::cout << checkBit(input,x);
        if (x%6==0)
            std::cout<<" ";
    }

}

int function_s(int table,int row, int column ){
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
void displayBinary(unsigned short input){
    for (int x=15; x>=0; x--)
        std::cout << checkBit(input,x);
    std::cout << std::endl;
}
void displayBinary(unsigned long input){
    for (int x=15; x>=0; x--)
        std::cout << checkBit(input,x);
    std::cout << std::endl;
}
void bitwiseSplit(unsigned long long &input, unsigned long &leftDigits, unsigned long &rightDigits){
    //64 bit  => 32 x 2 version
    //splits unsigned long long into two  bit shorts, the left and right containing respective bits
    //probably should eventually hard code these values for efficiency since function isn't modular
    int bits = 64;
    unsigned long long leftShift = bits/2;
    unsigned long long rightMask = (unsigned long long)pow(2.0,leftShift)-1;
    leftDigits = input >> leftShift;
    rightDigits = input & rightMask;
}
void bitwiseSplit(unsigned int &input, unsigned short &leftDigits, unsigned short &rightDigits){
    //32 bit  => 16 x 2 version
    //splits unsigned integer into two  bit shorts, the left and right containing respective bits
    int bits = 32;
    unsigned int leftShift = bits/2;
    unsigned int rightMask = (unsigned int)pow(2.0,leftShift)-1;
    leftDigits = input >> leftShift;
    rightDigits = input & rightMask;
}
void bitwiseSplitDES(unsigned long long &input, unsigned int &leftDigits, unsigned int &rightDigits){
    //56(64) bit  => 28(32) x 2 version
    leftDigits = input >> 28;
    unsigned long rightMask = 268435455;
    rightDigits = input & rightMask;
}
