// Copyright (c) 2020 The NoteBlockchain developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_BIP39_H
#define BITCOIN_BIP39_H

#include <string>
#include <vector>

/**
 * BIP39 Mnemonic Implementation
 * 
 * This implements the BIP39 specification for generating deterministic keys
 * from mnemonic sentences (seed phrases).
 * 
 * Reference: https://github.com/bitcoin/bips/blob/master/bip-0039.mediawiki
 */

namespace BIP39 {

/**
 * Generate a new mnemonic phrase from random entropy
 * 
 * @param strength Entropy strength in bits (128 for 12 words, 256 for 24 words)
 * @return Mnemonic phrase as space-separated words
 * @throws std::runtime_error if strength is invalid
 */
std::string GenerateMnemonic(int strength = 128);

/**
 * Validate a mnemonic phrase
 * 
 * @param mnemonic Space-separated mnemonic words
 * @return true if valid, false otherwise
 */
bool ValidateMnemonic(const std::string& mnemonic);

/**
 * Convert mnemonic phrase to seed bytes (512 bits)
 * 
 * @param mnemonic Space-separated mnemonic words
 * @param passphrase Optional passphrase for additional security (default: empty)
 * @return 64-byte seed derived from mnemonic
 * @throws std::runtime_error if mnemonic is invalid
 */
std::vector<unsigned char> MnemonicToSeed(const std::string& mnemonic, const std::string& passphrase = "");

/**
 * Get the BIP39 English wordlist
 * 
 * @return Reference to the 2048-word list
 */
const std::vector<std::string>& GetWordList();

/**
 * Find word index in wordlist
 * 
 * @param word Word to find
 * @return Index (0-2047) or -1 if not found
 */
int GetWordIndex(const std::string& word);

/**
 * Normalize mnemonic string (trim, lowercase, normalize spaces)
 * 
 * @param mnemonic Input mnemonic
 * @return Normalized mnemonic
 */
std::string NormalizeMnemonic(const std::string& mnemonic);

} // namespace BIP39

#endif // BITCOIN_BIP39_H

// Made with Bob
