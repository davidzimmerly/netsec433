#include "encrypt_ram_rsa.h"

// Determines if two values are coprimes by GCD
int encrypt_ram_rsa::gcd(long int x, int y) {
	while (y) {
		int temp = x;
		x = y;
		y = temp % y;
	}
	return x;
}

// Returns totient's coprime
long int encrypt_ram_rsa::coprime(long int totient) {
	long int b = (rand() % totient) + 1;

	while (gcd(totient, b) != 1 || b == 0) {
		b = (rand() % totient) + 1;
	}

	return b;
}

// Determines modular multiplicative inverse
int encrypt_ram_rsa::modInverse(int a, int m) {
	a = a % m;
	for (int x = 1; x < m; x++) {
		if ((a * x) % m == 1) {
			return x;
		}
	}
	return 0;
}

// Generates values used in encryption/decryption.
// If there isn't a modular multiplicative inverse
// for value e, it rolls again. Primes below 1000
// are used.
void encrypt_ram_rsa::generateValues(){	
	d = 0;
	while (d == 0){
		p = rand_primes[(rand() % 167)];
		q = rand_primes[(rand() % 167)];
		n = p * q;
		totient = (p - 1) * (q - 1);
		e = coprime(totient);
		d = modInverse(e, totient);
	}
}

// Tests values before soldiering on
int encrypt_ram_rsa::testValues(){
	long long int z1, y1, z2, y2;
	std::vector<long long int> vec;
	std::string test1 = "TESTER";
	std::string test2 = "";
	
	for (uint i = 0; i < test1.size(); i++){
		z1 = static_cast<int>(test1.at(i));
		y1 = z1;
		for (uint j = 1; j < e; j++){
			z1 *= y1;
			z1 %= n;
		}
		vec.push_back(z1);
	}
	
	for (uint i = 0; i < vec.size(); i++){
		z2 = vec.at(i);
		y2 = z2;
		for (uint j = 1; j < d; j++){
			z2 *= y2;
			z2 %= n;
		}
		test2.push_back(static_cast<char>(z2));
	}
	
	vec.clear();
	
	if (test1 == test2){
		std::cout << "\nApproved values" << std::endl;
		return 1;
	} else {
		std::cout << "\nUnusable values - trying again..." << std::endl;
		return -1;
	}
}

// Encrypts string character-by-character and places
// it in vector fn
void encrypt_ram_rsa::encryptString(){
	long long int z, y;
	
	std::cerr << "\nEncrypted string: ";
	
	// Encrypts string
	for (uint i = 0; i < strin.size(); i++) {
		z = static_cast<int>(strin.at(i));
		y = z;
		strin.at(i) = 'x';
		for (uint j = 1; j < e; j++) {
			z *= y;
			z %= n;
		}
		fn.push_back(z);
		std::cerr << fn.at(i) << " ";
	}
	std::cerr << std::endl;
}

// Decrypts string character-by-character and displays
// string if values were usable
void encrypt_ram_rsa::decryptString(){
	long long int z2, y2;
	strin = "";

	std::cerr << "\nDecrypted string: ";
	
	for (uint i = 0; i < fn.size(); i++) {
		z2 = fn.at(i);
		y2 = z2;
		for (uint j = 1; j < d; j++) {
			z2 *= y2;
			z2 %= n;
		}
		std::cerr << static_cast<char>(z2);
	}
	
	// Clears vector in case bad values were used
	fn.clear();
	
	std::cerr << std::endl;
	std::cerr.flush();
}
