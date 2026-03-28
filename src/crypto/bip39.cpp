// Copyright (c) 2020 The NoteBlockchain developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <crypto/bip39.h>
#include <crypto/bip39_wordlist.h>
#include <crypto/sha256.h>
#include <crypto/sha512.h>
#include <random.h>
#include <utilstrencodings.h>
#include <crypto/hmac_sha512.h>
#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace BIP39 {

// PBKDF2-HMAC-SHA512 implementation for BIP39
static void PBKDF2_HMAC_SHA512(const std::vector<unsigned char>& password,
                                const std::vector<unsigned char>& salt,
                                int iterations,
                                std::vector<unsigned char>& output)
{
    output.resize(64); // SHA512 output is 64 bytes
    
    // Simple PBKDF2 implementation
    // U1 = HMAC(password, salt || INT(1))
    std::vector<unsigned char> salt_block = salt;
    salt_block.push_back(0);
    salt_block.push_back(0);
    salt_block.push_back(0);
    salt_block.push_back(1);
    
    CHMAC_SHA512 hmac(password.data(), password.size());
    std::vector<unsigned char> U(64);
    hmac.Write(salt_block.data(), salt_block.size());
    hmac.Finalize(U.data());
    
    output = U;
    
    // Iterate
    for (int i = 1; i < iterations; i++) {
        CHMAC_SHA512 hmac_iter(password.data(), password.size());
        hmac_iter.Write(U.data(), U.size());
        hmac_iter.Finalize(U.data());
        
        for (size_t j = 0; j < output.size(); j++) {
            output[j] ^= U[j];
        }
    }
}

std::string GenerateMnemonic(int strength)
{
    // Validate strength
    if (strength != 128 && strength != 256) {
        throw std::runtime_error("Invalid strength. Must be 128 or 256 bits.");
    }
    
    // Generate random entropy
    int entropy_bytes = strength / 8;
    std::vector<unsigned char> entropy(entropy_bytes);
    GetStrongRandBytes(entropy.data(), entropy_bytes);
    
    // Calculate checksum
    CSHA256 hasher;
    unsigned char hash[CSHA256::OUTPUT_SIZE];
    hasher.Write(entropy.data(), entropy.size()).Finalize(hash);
    
    int checksum_bits = strength / 32;
    
    // Combine entropy and checksum into bit array
    std::vector<bool> bits;
    bits.reserve(strength + checksum_bits);
    
    // Add entropy bits
    for (size_t i = 0; i < entropy.size(); i++) {
        for (int j = 7; j >= 0; j--) {
            bits.push_back((entropy[i] >> j) & 1);
        }
    }
    
    // Add checksum bits
    for (int i = 0; i < checksum_bits; i++) {
        bits.push_back((hash[0] >> (7 - i)) & 1);
    }
    
    // Convert bits to words
    const std::vector<std::string>& wordlist = GetWordList();
    std::vector<std::string> words;
    
    for (size_t i = 0; i < bits.size(); i += 11) {
        int index = 0;
        for (int j = 0; j < 11; j++) {
            index = (index << 1) | (bits[i + j] ? 1 : 0);
        }
        words.push_back(wordlist[index]);
    }
    
    // Join words with spaces
    std::ostringstream oss;
    for (size_t i = 0; i < words.size(); i++) {
        if (i > 0) oss << " ";
        oss << words[i];
    }
    
    return oss.str();
}

bool ValidateMnemonic(const std::string& mnemonic)
{
    // Normalize and split into words
    std::string normalized = NormalizeMnemonic(mnemonic);
    std::istringstream iss(normalized);
    std::vector<std::string> words;
    std::string word;
    
    while (iss >> word) {
        words.push_back(word);
    }
    
    // Check word count (12, 15, 18, 21, or 24 words)
    if (words.size() != 12 && words.size() != 15 && words.size() != 18 && 
        words.size() != 21 && words.size() != 24) {
        return false;
    }
    
    // Check all words are in wordlist
    const std::vector<std::string>& wordlist = GetWordList();
    std::vector<int> indices;
    
    for (const auto& w : words) {
        int index = GetWordIndex(w);
        if (index < 0) {
            return false;
        }
        indices.push_back(index);
    }
    
    // Convert indices to bits
    std::vector<bool> bits;
    for (int index : indices) {
        for (int i = 10; i >= 0; i--) {
            bits.push_back((index >> i) & 1);
        }
    }
    
    // Calculate entropy and checksum lengths
    int total_bits = static_cast<int>(bits.size());
    int checksum_bits = total_bits / 33;
    int entropy_bits = total_bits - checksum_bits;
    
    // Extract entropy
    std::vector<unsigned char> entropy;
    for (int i = 0; i < entropy_bits; i += 8) {
        unsigned char byte = 0;
        for (int j = 0; j < 8; j++) {
            byte = (byte << 1) | (bits[i + j] ? 1 : 0);
        }
        entropy.push_back(byte);
    }
    
    // Calculate expected checksum
    CSHA256 hasher;
    unsigned char hash[CSHA256::OUTPUT_SIZE];
    hasher.Write(entropy.data(), entropy.size()).Finalize(hash);
    
    // Extract actual checksum from bits
    unsigned char actual_checksum = 0;
    for (int i = 0; i < checksum_bits; i++) {
        actual_checksum = (actual_checksum << 1) | (bits[entropy_bits + i] ? 1 : 0);
    }
    
    // Compare checksums
    unsigned char expected_checksum = hash[0] >> (8 - checksum_bits);
    
    return actual_checksum == expected_checksum;
}

std::vector<unsigned char> MnemonicToSeed(const std::string& mnemonic, const std::string& passphrase)
{
    // Validate mnemonic first
    if (!ValidateMnemonic(mnemonic)) {
        throw std::runtime_error("Invalid mnemonic phrase");
    }
    
    // Normalize mnemonic
    std::string normalized_mnemonic = NormalizeMnemonic(mnemonic);
    
    // Prepare password (mnemonic as UTF-8)
    std::vector<unsigned char> password(normalized_mnemonic.begin(), normalized_mnemonic.end());
    
    // Prepare salt ("mnemonic" + passphrase)
    std::string salt_str = "mnemonic" + passphrase;
    std::vector<unsigned char> salt(salt_str.begin(), salt_str.end());
    
    // Apply PBKDF2-HMAC-SHA512 with 2048 iterations
    std::vector<unsigned char> seed;
    PBKDF2_HMAC_SHA512(password, salt, 2048, seed);
    
    return seed;
}

const std::vector<std::string>& GetWordList()
{
    return BIP39_WORDLIST_ENGLISH;
}

int GetWordIndex(const std::string& word)
{
    const std::vector<std::string>& wordlist = GetWordList();
    auto it = std::find(wordlist.begin(), wordlist.end(), word);
    
    if (it != wordlist.end()) {
        return static_cast<int>(std::distance(wordlist.begin(), it));
    }
    
    return -1;
}

std::string NormalizeMnemonic(const std::string& mnemonic)
{
    std::string result = mnemonic;
    
    // Trim leading/trailing whitespace
    size_t start = result.find_first_not_of(" \t\n\r");
    size_t end = result.find_last_not_of(" \t\n\r");
    
    if (start == std::string::npos) {
        return "";
    }
    
    result = result.substr(start, end - start + 1);
    
    // Normalize internal whitespace to single spaces
    std::string normalized;
    bool in_space = false;
    
    for (char c : result) {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            if (!in_space) {
                normalized += ' ';
                in_space = true;
            }
        } else {
            normalized += c;
            in_space = false;
        }
    }
    
    return normalized;
}

} // namespace BIP39

// Made with Bob
