/*
 * File: caesar_cipher.cpp
 * Author: Johnny CW
 * Date: April 8, 2025
 * Description: Implements a simple Caesar Cipher encrypt and decrypt that preserves case and skips non-letters.
 */

#include "caesar_cipher.h"
#include <cctype>

using namespace std;

string caesar_encrypt(const string& text, int shift) {
    string result = text;
    shift = shift % 26; // Normalize shift
    for(char& c : result){
        if(isupper(c)){
            int temp =(c - 'A' + shift);
            if (temp < 0) temp += 26;
            c = (temp % 26) + 'A';
        }else if (islower(c)){
            int temp =(c - 'a' + shift);
            if (temp < 0) temp += 26;
            c =(temp % 26) + 'a';
        }
    }
    return result;
}

std::string caesar_decrypt(const std::string& text, int shift) {
    return caesar_encrypt(text, -shift);
}