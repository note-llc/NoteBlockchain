# BIP39 Mnemonic Implementation Plan

## Executive Summary
Add BIP39 mnemonic seed support to NoteBlockchain (Litecoin fork) with full backward compatibility.

## Requirements
1. Generate wallets from 12/24-word mnemonic phrases
2. Import/restore wallets using mnemonic phrases  
3. Maintain compatibility with existing HD and legacy wallets
4. Use BIP44 path (m/44'/2'/0'/0/x) for new mnemonic wallets
5. Keep existing path (m/0'/0'/x) for current HD wallets
6. Allow importprivkey alongside mnemonic wallets

## Implementation Phases

### Phase 1: BIP39 Library (2-3 days)
**New Files:**
- `src/bip39.h` - BIP39 interface
- `src/bip39.cpp` - Implementation
- `src/bip39_wordlist.h` - 2048 English words

**Key Functions:**
- `GenerateMnemonic(int strength)` - Create 12/24 word phrase
- `ValidateMnemonic(string)` - Validate phrase
- `MnemonicToSeed(string, passphrase)` - Convert to seed

**Build System:**
- Update `configure.ac` and `src/Makefile.am`

### Phase 2: Data Model Extensions (2-3 days)
**Modify:** `src/wallet/walletdb.h`

**Extend CHDChain:**
```cpp
enum DerivationPathType {
    DERIVATION_LEGACY = 0,  // m/0'/0'/x
    DERIVATION_BIP44 = 1    // m/44'/2'/0'/0/x
};
DerivationPathType pathType;
bool hasMnemonic;
static const int VERSION_HD_MNEMONIC = 3;
```

**Add CMnemonicData:**
```cpp
class CMnemonicData {
    std::vector<unsigned char> encryptedMnemonic;
    int64_t nCreateTime;
};
```

**Modify:** `src/wallet/wallet.h`
```cpp
enum WalletFeature {
    FEATURE_MNEMONIC = 160000,
    FEATURE_LATEST = FEATURE_MNEMONIC
};
```

### Phase 3: Key Derivation (3-4 days)
**Modify:** `src/wallet/wallet.cpp` - `DeriveNewChildKey()`

**Add BIP44 Path Support:**
- m/44' (purpose)
- m/44'/2' (Litecoin coin type)
- m/44'/2'/0' (account)
- m/44'/2'/0'/0 (external) or m/44'/2'/0'/1 (change)
- m/44'/2'/0'/0/x (address index)

**Keep Legacy Path:**
- m/0'/0'/x for existing wallets

### Phase 4: RPC Commands (3-4 days)
**Modify:** `src/wallet/rpcwallet.cpp`

**New Commands:**

1. **generatemnemonic [strength]**
   - Generate 12 or 24 word phrase
   - Returns: mnemonic, strength, word count

2. **importmnemonic "mnemonic" ["passphrase"] [rescan]**
   - Import mnemonic and create HD wallet
   - Uses BIP44 derivation
   - Optional passphrase support
   - Rescan blockchain

3. **getmnemonic**
   - Return wallet's mnemonic (if exists)
   - Requires unlocked wallet
   - Security warning in help text

**Register Commands:**
Add to RPC table in `src/wallet/rpcwallet.cpp`

### Phase 5: Wallet Initialization (2-3 days)
**Modify:** `src/wallet/wallet.cpp`

**CreateWalletFromFile:**
- Generate mnemonic by default for new wallets
- Initialize with BIP44 path
- Store encrypted mnemonic

**LoadWallet:**
- Detect wallet type (legacy/HD legacy/HD mnemonic)
- Load correct derivation path
- Maintain backward compatibility

### Phase 6: Testing (4-5 days)

**Unit Tests:** `src/test/bip39_tests.cpp`
- Mnemonic generation
- Validation
- Seed derivation
- BIP39 test vectors

**Functional Tests:** `test/functional/wallet_mnemonic.py`
- Generate and create wallet
- Import and restore
- BIP44 address derivation
- Backward compatibility
- importprivkey with mnemonic wallet

### Phase 7: Documentation (2-3 days)

**Files to Create/Update:**
- `doc/mnemonic-wallet.md` - User guide
- `doc/release-notes.md` - Feature announcement
- RPC help text for new commands

**Topics:**
- How to generate mnemonic
- Backup procedures
- Restore from mnemonic
- Wallet type differences
- Security best practices

## File Modification Summary

### New Files (3)
- `src/bip39.h`
- `src/bip39.cpp`
- `src/bip39_wordlist.h`

### Modified Files (6)
- `configure.ac` - Build config
- `src/Makefile.am` - Build system
- `src/wallet/walletdb.h` - Data structures
- `src/wallet/walletdb.cpp` - Database operations
- `src/wallet/wallet.h` - Wallet features
- `src/wallet/wallet.cpp` - Key derivation & init
- `src/wallet/rpcwallet.cpp` - RPC commands

### Test Files (2)
- `src/test/bip39_tests.cpp` - Unit tests
- `test/functional/wallet_mnemonic.py` - Functional tests

### Documentation (2)
- `doc/mnemonic-wallet.md`
- `doc/release-notes.md`

## Backward Compatibility

| Wallet Type | getnewaddress | importprivkey | Path | Mnemonic |
|-------------|---------------|---------------|------|----------|
| Legacy | ✅ | ✅ | N/A | ❌ |
| HD Legacy | ✅ | ✅ | m/0'/0'/x | ❌ |
| HD Mnemonic | ✅ | ✅ | m/44'/2'/0'/0/x | ✅ |

## Security Considerations

1. **Encryption:** Always encrypt mnemonic in database
2. **Memory:** Use secure allocators
3. **RPC:** Warn about getmnemonic exposure
4. **Backup:** Emphasize mnemonic backup importance
5. **Passphrase:** Support optional BIP39 passphrase

## Timeline
- Total: 18-25 days
- Phase 1: 2-3 days
- Phase 2: 2-3 days
- Phase 3: 3-4 days
- Phase 4: 3-4 days
- Phase 5: 2-3 days
- Phase 6: 4-5 days
- Phase 7: 2-3 days

## Success Criteria
1. ✅ Generate 12/24 word mnemonics
2. ✅ Import/restore from mnemonics
3. ✅ BIP44 derivation works
4. ✅ Existing wallets work
5. ✅ importprivkey works with mnemonic wallets
6. ✅ All tests pass
7. ✅ Documentation complete

## Next Steps
1. Review and approve plan
2. Begin Phase 1: BIP39 library integration
3. Progress reviews after each phase