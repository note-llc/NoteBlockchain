// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams.h>
#include <consensus/merkle.h>

#include <tinyformat.h>
#include <util.h>
#include <utilstrencodings.h>

#include <assert.h>
#include <memory>

#include <chainparamsseeds.h>

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime = nTime;
    genesis.nBits = nBits;
    genesis.nNonce = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "Whats todays’ news.Need to read newspaper.";
    const CScript genesisOutputScript = CScript() << ParseHex("04DC507936C579A0913EEFE47BAFA969D2F83C76929D0CC690A4F56C1CC4A43EBEF612BA1596A898505FB4EFA44E2C87EDFED5975FD0A2B27AB2AC299C16FE7346") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

void CChainParams::UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    consensus.vDeployments[d].nStartTime = nStartTime;
    consensus.vDeployments[d].nTimeout = nTimeout;
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */
class CMainParams : public CChainParams
{


public:
    CMainParams()
    {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 1051200;
        consensus.BIP16Height = 160000; //6466572a3c572d7b935243cb0c3e7c2bc8cd764086b0b42dd317341b646e2b13
        consensus.BIP34Height = 165000;
        consensus.BIP34Hash = uint256S("909f8d5a1a4cb305c0b8d7a9fba5f598191730fbeb0f5da101a5816064cd2b38");// this hash need to be changed
        consensus.BIP65Height = 165000; //909f8d5a1a4cb305c0b8d7a9fba5f598191730fbeb0f5da101a5816064cd2b38
        consensus.BIP66Height = 165000; //909f8d5a1a4cb305c0b8d7a9fba5f598191730fbeb0f5da101a5816064cd2b38
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 5 * 60 * 60; // 5 Hours
        consensus.nPowTargetSpacing = 30;// 30 Seconds
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 450; // 75% of 600 to accept rule
        consensus.nMinerConfirmationWindow = 600;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999;

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1577750400; // 31 December 2019
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1609372800;   // 31 December 2020

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1577750400; // 31 December 2019
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 1609372800;   // 31 December 2020


        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00000000000000000000000000000000000000000000000000000d905d06346a");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0xf18022b25a767a6c2bca01276320ad3d2e6fffba28239df970f511a0b9e7d922"); //160578

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xf4;
        pchMessageStart[1] = 0xe5;
        pchMessageStart[2] = 0xed;
        pchMessageStart[3] = 0xe3;
        nDefaultPort = 16158;        
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(1542718651, 2085390519, 0x1e0ffff0, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x270f3e7b185c412d57ba913d10658df54f15201a67d736cb4071a4ec4eb54836"));
        assert(genesis.hashMerkleRoot == uint256S("0x7d2bef0e42d82d2b01af74ca942430e0373076ea00f5e8a366dfc1623bb17963"));

        // Note that of those with the service bits flag, most only support a subset of possible options
        vSeeds.emplace_back("52.52.160.103");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 53);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 5);
        base58Prefixes[SCRIPT_ADDRESS2] = std::vector<unsigned char>(1, 115);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 176);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x03, 0xBA, 0x30, 0x05};
        base58Prefixes[EXT_SECRET_KEY] = {0x03, 0xBA, 0x2f, 0x80};

        bech32_hrp = "note";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;

         checkpointData = {
            {
                {0, uint256S("0x270f3e7b185c412d57ba913d10658df54f15201a67d736cb4071a4ec4eb54836")},
                {400, uint256S("0x321f9f32e87e6e07cea4fd07758a4c53c712972c3e6524ba54de728089fca46b")},
                {960, uint256S("0x124f5a50d62c2971fe8a668982306b0f59ea98135fc18ca1434ff9a41764d347")},
                {1950, uint256S("0x521638fa8c54d86fb80d387ed251f6f963ff405a6e6de0880657dbd8704b3eb0")},
                {3050, uint256S("0x8608894c4e9d4303a2098506de895d89d2733152971114dc79c6a02b8a802b33")},
                {4200, uint256S("0x5a4ac7ad5dc97402d9d4e3cde6b40df287b050f690d9dbd2a2f4a198da4cbd54")},
                {9640, uint256S("0x4eec360c9193809be28d232078a0ff91bb669ce1aa018d18a1072be6b18cdd11")},                
                {16800, uint256S("0x02634b042f9a78882609ce0e319a846b5ce4a38433f07a52b48bec383f16c9e7")},
                {33532, uint256S("0xe9151fc49cecf5fac510ccc49934f943cd09c67582a335bf879568f6b559bbac")},                
                {61032, uint256S("0x2b5712fdb3a3d7860e63ce9f5102ec718d87ca52fd565da95c282b6170dca685")},
                {78000, uint256S("0x7477c6e0cd3bc79b88e8058c5ac1cb1545d5b59f3ed6fb70fb1feca8104d0ff5")},                
                {90032, uint256S("0x5c11bfe7b7230af9c5ba1b1e3a8af48409c290bdc40ede7f5b15392560ca8519")},
                {95032, uint256S("0x6e9fe9ba93565f16b00d62da8664ee2acd44671a5265e8ec5d19df5b705e7f28")},
                {121560, uint256S("0xc4d44d5e16d1263c99f66cdd5e8e733e84e246189512e0d534cc47bdb864f39b")},
                {132007, uint256S("0x051509d4ca0604e7b22e361055a09d98c460c461095224eba4fcc43aa7962b1e")},
                {139554, uint256S("0x95901afeee8be0955a6f1f0895a305ba1f73d2498eda3b103ee72805aabac40e")},
                {142797, uint256S("0x78b9aeae6b0ea7a906c597cc7a6f5bf403e28fc3febc1e6cdd1ef7d1fa2271ff")},
                {163508, uint256S("0xaef90a073e8b3e2f8c576b16b007148ee74220fc82b0136d617d94434aa9f4d2")},
                {188900, uint256S("0xc3ed00bfa1ff31381407ffbabb5281fa2ecf92a0d8cdb7db4ef6ae0a9395ef3f")},
                {245610, uint256S("0xf61c364f95a0495bce6c42e85dfff3bdb6ffa4535383c948141edcb33603715a")},
                {309935, uint256S("0x68936394b51d6b6189ec78cb2272cdc52e9961d5f37a745314a68058c0c76f35")},
                {501700, uint256S("0x2f2422c8ea485c7e697bef99ff23da8ca57d27473a47f117ed728930543f92fd")},
                {659250, uint256S("0x951e55cd7b842cdd4ba05624471069d4a50eb5b18a825d16ba21a9f300600cc7")},
                {746127, uint256S("0x0170f2440b23100c069f23688ad64ec0489aed18aae03a7e3e6b6d250d01b316")},

            }};

        chainTxData = ChainTxData{
            // Data from rpc: getchaintxstats 25376 (blockhash : 7ff5e20634ce399cf0405c4099ffa2c13ea8ea0f2bdff9a6e50624e794f782cc)
            /* nTime    */ 1551968032,
            /* nTxCount */ 171379,
            /* dTxRate  */ 0.033659470031163};

    }
};

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams
{
public:
    CTestNetParams()
    {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 1051200;
        consensus.BIP16Height = 0;
        consensus.BIP34Height = 76;
        consensus.BIP34Hash = uint256S("8075c771ed8b495ffd943980a95f702ab34fce3c8c54e379548bda33cc8c0573");
        consensus.BIP65Height = 76;
        consensus.BIP66Height = 76;
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 5 * 60 * 60; // 3.5 days
        consensus.nPowTargetSpacing = 30;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 450; // 75% for testchains
        consensus.nMinerConfirmationWindow = 600;       // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999;   // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1483228800; // January 1, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1517356801;   // January 31st, 2018

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1483228800; // January 1, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 1517356801;   // January 31st, 2018

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00"); //343833


        pchMessageStart[0] = 0xda;
        pchMessageStart[1] = 0xe7;
        pchMessageStart[2] = 0xaa;
        pchMessageStart[3] = 0x96;
        nDefaultPort = 15159;
        nPruneAfterHeight = 1000;


        genesis = CreateGenesisBlock(1542719217, 1118959, 0x1e0ffff0, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x6d4552054ee72a9e5be2a1e537a52b9761ea3d3e7b75a22af4931007379c6841"));
        assert(genesis.hashMerkleRoot == uint256S("0x7d2bef0e42d82d2b01af74ca942430e0373076ea00f5e8a366dfc1623bb17963"));

        vFixedSeeds.clear();
        vSeeds.clear();
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 196);
        base58Prefixes[SCRIPT_ADDRESS2] = std::vector<unsigned char>(1, 58);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};


        bech32_hrp = "tnote";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;

        checkpointData = (CCheckpointData){
            {
                {0, uint256S("6d4552054ee72a9e5be2a1e537a52b9761ea3d3e7b75a22af4931007379c6841")},
            }};
        chainTxData = ChainTxData{           
            /* nTime    */ 1542719217,
            /* nTxCount */ 0,
            /* dTxRate  */ 0.0};
    }
};

/**
 * Regression test
 */
class CRegTestParams : public CChainParams
{
public:
    CRegTestParams()
    {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        consensus.BIP16Height = 0;         // always enforce P2SH BIP16 on regtest
        consensus.BIP34Height = 100000000; // BIP34 has not activated on regtest (far in the future so block v1 are not rejected in tests)
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1351; // BIP65 activated on regtest (Used in rpc activation tests)
        consensus.BIP66Height = 1251; // BIP66 activated on regtest (Used in rpc activation tests)
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 5 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 30;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 450; // 75% for testchains
        consensus.nMinerConfirmationWindow = 600;       // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0xad;
        pchMessageStart[1] = 0xe2;
        pchMessageStart[2] = 0x86;
        pchMessageStart[3] = 0xf0;
        nDefaultPort = 14160;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1542719500, 0, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x4313560f2705c5af5f4ca1e85abbba079dc3863451096d40072d08ab56ff3b50"));
        assert(genesis.hashMerkleRoot == uint256S("0x7d2bef0e42d82d2b01af74ca942430e0373076ea00f5e8a366dfc1623bb17963"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;

        checkpointData = {
            {
                {0, uint256S("4313560f2705c5af5f4ca1e85abbba079dc3863451096d40072d08ab56ff3b50")},
            }};


        chainTxData = ChainTxData{
            0,
            0,
            0};

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 196);
        base58Prefixes[SCRIPT_ADDRESS2] = std::vector<unsigned char>(1, 58);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "rnote";
    }
};

static std::unique_ptr<CChainParams> globalChainParams;

const CChainParams& Params()
{
    assert(globalChainParams);
    return *globalChainParams;
}

std::unique_ptr<CChainParams> CreateChainParams(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
        return std::unique_ptr<CChainParams>(new CMainParams());
    else if (chain == CBaseChainParams::TESTNET)
        return std::unique_ptr<CChainParams>(new CTestNetParams());
    else if (chain == CBaseChainParams::REGTEST)
        return std::unique_ptr<CChainParams>(new CRegTestParams());
    throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    globalChainParams = CreateChainParams(network);
}

void UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    globalChainParams->UpdateVersionBitsParameters(d, nStartTime, nTimeout);
}
