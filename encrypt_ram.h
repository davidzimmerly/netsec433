#include <string>
#include <iostream>
#include <cmath>
//#define
//#define
//#define

#ifndef AES192
#ifndef AES258
#define AES128
#endif
#endif
#ifndef LENGTH
#define LENGTH 64
#endif

#include <cstdio>
#include <stdint.h>
#include <wmmintrin.h>
#include <smmintrin.h>
#include <emmintrin.h>





#if !defined (ALIGN16)
# if defined (__GNUC__)
#   define ALIGN16 __attribute__ ( (aligned (16)))
# else
#   define ALIGN16 __declspec (align (16))
# endif
#endif


#define cpuid(func,ax,bx,cx,dx)\
  __asm__ __volatile__ ("cpuid":\
  "=a" (ax), "=b" (bx), "=c" (cx), "=d" (dx) : "a" (func));



typedef struct KEY_SCHEDULE{
  ALIGN16 unsigned char KEY[16*15];
  unsigned int nr;
}AES_KEY;






class encrypt_ram{
    private:

        std::string key = "94307803947898";//for initial encryption method XOR
        void leftShift(unsigned int &input);
        unsigned int function_s(int table,int row, int column );
        unsigned long long* K[16];
        void displayBinary(unsigned long long input);
        void displayBinary(unsigned int input);
        void displayBinaryX(unsigned int input,unsigned int bits);
        void displayBinaryX(unsigned long long input,unsigned int bits);
        void displayBinary(unsigned short input);
        void displayBinary(unsigned long input);

        unsigned int * function_f(unsigned int* data, unsigned long long* key);
        unsigned int * permutePbit(unsigned int &input);
        unsigned long long* permuteKey(unsigned long long &input);
        unsigned long long* permuteKey2(unsigned int &left, unsigned int &right);
        unsigned long long * permuteInitial(unsigned long long &message);
        unsigned long long * permuteEbit(unsigned int &message);
        unsigned long long * permuteFinal(unsigned long long &message);
        void bitwiseSplit(unsigned long long &input, unsigned int &leftDigits, unsigned int &rightDigits);
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
        const unsigned int s1 [64] = {14,  4,  13,  1,   2, 15,  11,  8,   3, 10,   6, 12,   5,  9,   0,  7,
                              0, 15,   7,  4,  14,  2,  13,  1,  10,  6,  12, 11,   9,  5,   3,  8,
                              4,  1,  14,  8,  13,  6,   2, 11,  15, 12,   9,  7,   3, 10,   5,  0,
                             15, 12,   8,  2,   4,  9,   1,  7,   5, 11,   3, 14,  10,  0,   6, 13};
        const unsigned int s2 [64] = {    15,  1,   8, 14,   6, 11,   3,  4,   9,  7,   2, 13,  12,  0,   5, 10,
                                  3, 13,   4,  7,  15,  2,   8, 14,  12,  0,   1, 10,   6,  9,  11,  5,
                                  0, 14,   7, 11,  10,  4,  13,  1,   5,  8,  12,  6,   9,  3,   2, 15,
                                 13,  8,  10,  1,   3, 15,   4,  2,  11,  6,   7, 12,   0,  5,  14,  9};
        const unsigned int s3 [64] = {    10,  0,   9, 14,   6,  3,  15,  5,   1, 13,  12,  7,  11,  4,   2,  8,
                                 13,  7,   0,  9,   3,  4,   6, 10,   2,  8,   5, 14,  12, 11,  15,  1,
                                 13,  6,   4,  9,   8, 15,   3,  0,  11,  1,   2, 12,   5, 10,  14,  7,
                                  1, 10,  13,  0,   6,  9,   8,  7,   4, 15,  14,  3,  11,  5,   2, 12};
        const unsigned int s4 [64] = {     7, 13,  14,  3,   0,  6,   9, 10,   1,  2,   8,  5,  11, 12,   4, 15,
                                 13,  8,  11,  5,   6, 15,   0,  3,   4,  7,   2, 12,   1, 10,  14,  9,
                                 10,  6,   9,  0,  12, 11,   7, 13,  15,  1,   3, 14,   5,  2,   8,  4,
                                  3, 15,   0,  6,  10,  1,  13,  8,   9,  4,   5, 11,  12,  7,   2, 14};
        const unsigned int s5 [64] = { 2, 12,   4,  1,   7, 10,  11,  6,   8,  5,   3, 15,  13,  0,  14,  9,
                             14, 11,   2, 12,   4,  7,  13,  1,   5,  0,  15, 10,   3,  9,   8,  6,
                              4,  2,   1, 11,  10, 13,   7,  8,  15,  9,  12,  5,   6,  3,   0, 14,
                             11,  8,  12,  7,   1, 14,   2, 13,   6, 15,   0,  9,  10,  4,   5,  3};
        const unsigned int s6 [64] = {    12,  1,  10, 15,   9,  2,   6,  8,   0, 13,   3,  4,  14,  7,   5, 11,
                                 10, 15,   4,  2,   7, 12,   9,  5,   6,  1,  13, 14,   0, 11,   3,  8,
                                  9, 14,  15,  5,   2,  8,  12,  3,   7,  0,   4, 10,   1, 13,  11,  6,
                                  4,  3,   2, 12,   9,  5,  15, 10,  11, 14,   1,  7,   6,  0,   8, 13};
        const unsigned int s7 [64] = { 4, 11,   2, 14,  15,  0,   8, 13,   3, 12,   9,  7,   5, 10,   6,  1,
                             13,  0,  11,  7,   4,  9,   1, 10,  14,  3,   5, 12,   2, 15,   8,  6,
                              1,  4,  11, 13,  12,  3,   7, 14,  10, 15,   6,  8,   0,  5,   9,  2,
                              6, 11,  13,  8,   1,  4,  10,  7,   9,  5,   0, 15,  14,  2,   3, 12};
        const unsigned int s8 [64] = { 13,  2,   8,  4,   6, 15,  11,  1,  10,  9,   3, 14,   5,  0,  12,  7,
                               1, 15,  13,  8,  10,  3,   7, 4,  12,  5,   6, 11,   0, 14 ,  9, 2,
                               7, 11,   4,  1,   9, 12,  14,  2,   0,  6,  10, 13,  15,  3,   5,  8,
                               2,  1,  14,  7,   4, 10,   8, 13,  15, 12,   9,  0,   3,  5,   6, 11};
        const int p [32] = { 16,   7,  20,  21,
                             29,  12,  28,  17,
                              1,  15,  23,  26,
                              5,  18,  31,  10,
                              2,   8,  24,  14,
                             32,  27,   3,   9,
                             19,  13,  30,   6,
                             22,  11,   4,  25,};
        const int ipInverse [64] = {40,     8,   48,    16,    56,   24,    64,   32,
                                    39,     7,   47,    15,    55,   23,    63,   31,
                                    38,     6,   46,    14,    54,   22,    62,   30,
                                    37,     5,   45,    13,    53,   21,    61,   29,
                                    36,     4,   44,    12,    52,   20,    60,   28,
                                    35,     3,   43,    11,    51,   19,    59,   27,
                                    34,     2,   42,    10,    50,   18,    58,   26,
                                    33,     1,   41,     9,    49,   17,    57,   25};

    public:
        int Check_CPU_support_AES();

        void print_m128i_with_string(char const* string,__m128i data);

        encrypt_ram(unsigned long long & key);
        ~encrypt_ram();
        void encrypt(std::string &message){
            for (int x = 0; x < message.size(); x++) { message[x] ^= key[x]; }//simple XOR encrypt with key (key should be as long as input, ensure key size on update)
        }
        void decrypt(std::string &message){
           encrypt(message);
        }
        void desEncrypt(unsigned long long & message);
        void desDecrypt(unsigned long long & message);
        __m128i AES_128_ASSIST (__m128i temp1, __m128i temp2);
        void AES_128_Key_Expansion (const unsigned char *userkey,const unsigned char *key);
        void AES_ECB_encrypt(const unsigned char *in,unsigned char *out,unsigned long length,const char *key,int number_of_rounds);
        void AES_ECB_decrypt(const unsigned char *in,unsigned char *out,unsigned long length,const char *key,int number_of_rounds);
        void KEY_192_ASSIST(__m128i* temp1, __m128i * temp2, __m128i * temp3);
        void AES_192_Key_Expansion (const unsigned char *userkey,unsigned char *key);
        void KEY_256_ASSIST_1(__m128i* temp1, __m128i * temp2);
        void KEY_256_ASSIST_2(__m128i* temp1, __m128i * temp3);
        void AES_256_Key_Expansion (const unsigned char *userkey,const unsigned char *key);
        void AES_CBC_encrypt(const unsigned char *in,unsigned char *out,unsigned char ivec[16],unsigned long length,unsigned char *key,int number_of_rounds);
        void AES_CBC_decrypt(const unsigned char *in,unsigned char *out,unsigned char ivec[16],unsigned long length,unsigned char *key,int number_of_rounds);
        void AES_CTR_encrypt (const unsigned char *in,unsigned char *out,const unsigned char ivec[8],
        const unsigned char nonce[4],unsigned long length,const unsigned char *key,int number_of_rounds);
        void print_m128i_with_string_short(char* string,__m128i data,int length);
        int AES_set_encrypt_key (const unsigned char *userKey,const int bits,AES_KEY *key);
        int AES_set_decrypt_key (const unsigned char *userKey,const int bits,AES_KEY *key);
        
};

