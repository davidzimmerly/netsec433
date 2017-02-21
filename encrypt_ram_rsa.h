#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <math.h>
#include <vector>
#include "BigInteger.h"

class encrypt_ram_rsa{
	public:
		long int generatePrime();
		int gcd(long int x, int y);
		long int coprime(long int totient);
		int modInverse(int a, int m);
		void generateValues();
		void encryptString();
		void decryptString();
		void encryptPIN();
		void decryptPIN();
		
		long long int p, q, n, totient, e;
		long long int d = 0;
		vector<long long int> fn;
		
		std::string name = "";
		int pin = 0;
		
		BigInteger encrypted_pin;
		BigInteger decrypted_pin;
		BigInteger dec_mult_pin;
};
