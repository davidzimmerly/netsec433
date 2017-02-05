// RSA Encryption Example by Jacob Brown
// Utilizes BigInteger to store very large
// numbers and manipulate them. Very slow
// at the moment. Also includes "main" function
// for immediate testing

#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <ctime>
#include <math.h>
#include <vector>
#include "BigInteger.h"

using namespace std;

long long int p, q, n, totient, e;
long long int d = 0;

class Account {
public:
	string name;
	int pin;
};

// Generates and returns a prime number
long int generatePrime() {
	long int p = (rand() % 1000) + 1;
	for (int i = 2; i <= p / 2; i++) {
		if (!(p % i)) {
			p++;
			i = 2;
		}
	}

	return p;
}

// Checks two numbers for divisor
int gcd(long int x, int y) {
	while (y) {
		int temp = x;
		x = y;
		y = temp % y;
	}
	return x;
}

// Finds a coprime based on totient
long int coprime(long int totient) {
	long int b = (rand() % totient) + 1;

	while (gcd(totient, b) != 1 || b == 0) {
		b = (rand() % totient) + 1;
	}

	return b;
}

// Finds modular multiplicative inverse with
// two integers
int modInverse(int a, int m) {
	a = a % m;
	for (int x = 1; x < m; x++) {
		if ((a*x) % m == 1) {
			return x;
		}
	}
	return 0;
}

// TODO - create a function that access a string's address
// and encrypts the string
string encryptString(long long int n, long long int e, string s) {

	return "\0";
}

int main() {
	cout << "***Test Application***\n" << endl;

	Account * new_acct = new Account;

	srand(time(0));

	// Gets string and int from user
	while (true) {
		cout << "Enter your name: ";
		getline(cin, new_acct->name);

		cout << "Enter your PIN: ";
		cin >> new_acct->pin;

		cout << endl;
		cout << "First name: " << new_acct->name << " at " << &new_acct->name << endl;
		cout << "PIN: " << new_acct->pin << " at " << &new_acct->pin << endl;

		break;
	}

	// Generates various values to use with encryption/decryption
	// If it fails to find a suitable modular multiplicative
	// inverse based on e and totient, it tries a new set of numbers
	while (d == 0) {
		p = generatePrime();
		q = generatePrime();
		n = p * q;
		totient = (p - 1) * (q - 1);
		e = coprime(totient);
		d = modInverse(e, totient);
	}

	cout << "Prime 1: " << p << endl;
	cout << "Prime 2: " << q << endl;
	cout << "N: " << n << endl;
	cout << "Totient: " << totient << endl;
	cout << "E: " << e << endl;
	cout << "D: " << d << endl;

	cout << endl;

	cout << "Working (Trust Me): " << endl;

	vector<long long int> fn, k;
	string x;
	int y;
	long long int test;
	BigInteger z;

	// Encrypts string
	for (int i = 0; i < new_acct->name.size(); i++) {
		x = to_string((int)new_acct->name.at(i));
		y = (int)new_acct->name.at(i);
		z = BigInteger(x);
		new_acct->name.at(i) = 'x';
		for (int j = 1; j < e; j++) {
			z *= y;
		}
		z %= n;
		fn.push_back(stoll(z.getNumber()));
	}

	BigInteger z2;
	long long int y2;
	string s;

	// Decrypts string
	for (int i = 0; i < new_acct->name.size(); i++) {
		z2 = BigInteger(fn.at(i));
		y2 = fn.at(i);
		for (int j = 1; j < d; j++) {
			z2 *= y2;
		}
		z2 %= n;
		int guy = stoi(z2.getNumber());
		s.push_back(static_cast<char>(guy));
	}
	cout << s;

	BigInteger pin_temp = BigInteger(new_acct->pin);

	// Encrypts int
	for (int i = 1; i < e; i++) {
		pin_temp *= new_acct->pin;
	}

	BigInteger encrypted_pin = BigInteger(pin_temp % n);

	cout << "Public Key PIN: " << pin_temp.getNumber() << endl;
	cout << "Encrypted PIN: " << encrypted_pin.getNumber() << endl;

	cout << endl;

	BigInteger decrypted_pin = BigInteger(encrypted_pin);
	BigInteger dec_mult_pin = BigInteger(encrypted_pin);

	// Decrypts Int
	for (int i = 1; i < d; i++) {
		decrypted_pin *= dec_mult_pin;
	}

	decrypted_pin %= n;

	cout << "Decrypted PIN: " << decrypted_pin.getNumber() << endl;

	while (true) {
	}

	return 0;
}
