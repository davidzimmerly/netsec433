// RSA Encryption Example by Jacob Brown
// Utilizes BigInteger to store very large
// numbers and manipulate them. Very slow
// at the moment.
#include "encrypt_ram_rsa.h"

long int encrypt_ram_rsa::generatePrime(){
	long int p = (rand() % 1000) + 1;
	for (int i = 2; i <= p / 2; i++) {
		if (!(p % i)) {
			p++;
			i = 2;
		}
	}

	return p;
}

int encrypt_ram_rsa::gcd(long int x, int y) {
	while (y) {
		int temp = x;
		x = y;
		y = temp % y;
	}
	return x;
}

long int encrypt_ram_rsa::coprime(long int totient) {
	long int b = (rand() % totient) + 1;

	while (gcd(totient, b) != 1 || b == 0) {
		b = (rand() % totient) + 1;
	}

	return b;
}

int encrypt_ram_rsa::modInverse(int a, int m) {
	a = a % m;
	for (int x = 1; x < m; x++) {
		if ((a*x) % m == 1) {
			return x;
		}
	}
	return 0;
}

void encrypt_ram_rsa::generateValues(){
	while(d == 0){
		p = generatePrime();
		q = generatePrime();
		n = p * q;
		totient = (p - 1) * (q - 1);
		e = coprime(totient);
		d = modInverse(e, totient);
	}
}

void encrypt_ram_rsa::encryptString(){
	vector<long long int> k;
	std::string x;
	int y;
	//long long int test;
	BigInteger z;

	// Encrypts string
	for (uint i = 0; i < name.size(); i++) {
		x = to_string((int)name.at(i));
		y = (int)name.at(i);
		z = BigInteger(x);
		name.at(i) = 'x';
		for (int j = 1; j < e; j++) {
			z *= y;
		}
		z %= n;
		fn.push_back(stoll(z.getNumber()));
	}
	std::cout << z.getNumber() << std::endl;
}

void encrypt_ram_rsa::decryptString(){
	BigInteger z2;
	long long int y2;
	string s;

	// Decrypts string
	for (uint i = 0; i < name.size(); i++) {
		z2 = BigInteger(fn.at(i));
		y2 = fn.at(i);
		for (int j = 1; j < d; j++) {
			z2 *= y2;
		}
		z2 %= n;
		int guy = stoi(z2.getNumber());
		s.push_back(static_cast<char>(guy));
	}
	std::cout << s;
}

void encrypt_ram_rsa::encryptPIN(){
	BigInteger pin_temp = BigInteger(pin);

	// Encrypts int
	for (int i = 1; i < e; i++) {
		pin_temp *= pin;
	}

	encrypted_pin = BigInteger(pin_temp % n);

	std::cout << "Public Key PIN: " << pin_temp.getNumber() << std::endl;
	std::cout << "Encrypted PIN: " << encrypted_pin.getNumber() << std::endl;
}

void encrypt_ram_rsa::decryptPIN(){
	decrypted_pin = BigInteger(encrypted_pin);
	dec_mult_pin = BigInteger(encrypted_pin);
	encrypted_pin = BigInteger("");
	
	// Decrypts Int
	for (int i = 1; i < d; i++) {
		decrypted_pin *= dec_mult_pin;
	}

	decrypted_pin %= n;

	std::cout << "Decrypted PIN: " << decrypted_pin.getNumber() << std::endl;

}
