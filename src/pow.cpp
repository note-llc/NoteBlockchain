// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow.h>

#include <arith_uint256.h>
#include <chain.h>
#include <primitives/block.h>
#include <uint256.h>
#include <util.h>

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    //Get next height and compare to forkheight
    if (pindexLast->nHeight + 1 > params.nDigiSheildHFHeight){
        return DigiShieldV4(pindexLast,params);
    }
    return GetNextWorkRequiredLegacy(pindexLast,pblock,params);
}

unsigned int DigiShieldV4(const CBlockIndex* pindexLast, const Consensus::Params& params)
{
    // find first block in averaging interval
    // Go back by what we want to be nAveragingInterval blocks per algo
    const CBlockIndex* pindexFirst = pindexLast;
    const arith_uint256 powLimit = UintToArith256(params.powLimit);
    for (int i = 0; pindexFirst && i < params.nAveragingInterval; i++)
    {
        pindexFirst = pindexFirst->pprev;
    }

    const CBlockIndex* pindexPrev = pindexLast->pprev;
    if (pindexPrev == nullptr || pindexFirst == nullptr)
    {
        //return UintToArith256(params.powLimit).GetCompact();
        return powLimit.GetCompact();
    }

    // Limit adjustment step
    // Use medians to prevent time-warp attacks
    int64_t nActualTimespan = pindexLast->GetMedianTimePast() - pindexFirst->GetMedianTimePast();
    nActualTimespan = params.nAveragingTargetTimespanV4 + (nActualTimespan - params.nAveragingTargetTimespanV4);//Removed / 4 as note doesnt have 4 algos

    if (nActualTimespan < params.nMinActualTimespanV4)
        nActualTimespan = params.nMinActualTimespanV4;
    if (nActualTimespan > params.nMaxActualTimespanV4)
        nActualTimespan = params.nMaxActualTimespanV4;

    //Global retarget
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexPrev->nBits);

    bnNew *= nActualTimespan;
    bnNew /= params.nAveragingTargetTimespanV4;

    //Per-algo retarget
    //Previous code from dgb had pindexPrevAlgo->nHeight - pindexLast->nHeight ,we have it set to a constant of 1 as note has only one pow algo
    //Also removed the forloop since we dont have more than 1 adjustment
    bnNew *= 100;
    bnNew /= (100 + params.nLocalTargetAdjustment);
    //Removed nadjustments < 0 as we dont have that case

    if (bnNew > powLimit)
    {
        bnNew = powLimit;
    }

    return bnNew.GetCompact();
}

unsigned int GetNextWorkRequiredLegacy(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    assert(pindexLast != nullptr);
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    // Only change once per difficulty adjustment interval
    if ((pindexLast->nHeight+1) % params.DifficultyAdjustmentInterval() != 0)
    {
        if (params.fPowAllowMinDifficultyBlocks)
        {
            // Special difficulty rule for testnet:
            // If the new block's timestamp is more than 2* 10 minutes
            // then allow mining of a min-difficulty block.
            if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
                return nProofOfWorkLimit;
            else
            {
                // Return the last non-special-min-difficulty-rules-block
                const CBlockIndex* pindex = pindexLast;
                while (pindex->pprev && pindex->nHeight % params.DifficultyAdjustmentInterval() != 0 && pindex->nBits == nProofOfWorkLimit)
                    pindex = pindex->pprev;
                return pindex->nBits;
            }
        }
        return pindexLast->nBits;
    }

    // Go back by what we want to be 14 days worth of blocks
    // NoteCoin: This fixes an issue where a 51% attack can change difficulty at will.
    // Go back the full period unless it's the first retarget after genesis. Code courtesy of Art Forz
    int blockstogoback = params.DifficultyAdjustmentInterval()-1;
    if ((pindexLast->nHeight+1) != params.DifficultyAdjustmentInterval())
        blockstogoback = params.DifficultyAdjustmentInterval();

    // Go back by what we want to be 14 days worth of blocks
    const CBlockIndex* pindexFirst = pindexLast;
    for (int i = 0; pindexFirst && i < blockstogoback; i++)
        pindexFirst = pindexFirst->pprev;

    assert(pindexFirst);

    return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    if (params.fPowNoRetargeting)
        return pindexLast->nBits;

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    if (nActualTimespan < params.nPowTargetTimespan/4)
        nActualTimespan = params.nPowTargetTimespan/4;
    if (nActualTimespan > params.nPowTargetTimespan*4)
        nActualTimespan = params.nPowTargetTimespan*4;

    // Retarget
    arith_uint256 bnNew;
    arith_uint256 bnOld;
    bnNew.SetCompact(pindexLast->nBits);
    bnOld = bnNew;
    // NoteCoin: intermediate uint256 can overflow by 1 bit
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    bool fShift = bnNew.bits() > bnPowLimit.bits() - 1;
    if (fShift)
        bnNew >>= 1;
    bnNew *= nActualTimespan;
    bnNew /= params.nPowTargetTimespan;
    if (fShift)
        bnNew <<= 1;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}