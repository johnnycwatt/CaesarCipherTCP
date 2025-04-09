/*
 * File: caesar_cipher.h
 * Author: Johnny CW
 * Date: April 8, 2025
 */

#ifndef CAESAR_CIPHER_H
#define CAESAR_CIPHER_H

#include <string>

std::string caesar_encrypt(const std::string& text, int shift);
std::string caesar_decrypt(const std::string& text, int shift);


#endif //CAESAR_CIPHER_H
