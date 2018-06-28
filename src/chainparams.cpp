// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "assert.h"

#include "chainparams.h"
#include "main.h"
#include "util.h"

#include <boost/assign/list_of.hpp>

using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

//
// Main network
//

// Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress> &vSeedsOut, const SeedSpec6 *data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7*24*60*60;
    for (unsigned int i = 0; i < count; i++)
    {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

class CMainParams : public CChainParams {
public:
    CMainParams() {
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0xdb;
        pchMessageStart[1] = 0xd7;
        pchMessageStart[2] = 0xb1;
        pchMessageStart[3] = 0xef;
        vAlertPubKey = ParseHex("04ce63dbd957aeb2de7824f0c06a6d9d425ed05c90864c28ff5187eaa564b1448d110337aa636f37daf961f77b811b31fb129d46a994a5b3b714fe6e545ebb81ca");
        nDefaultPort = 31333;
        nRPCPort = 21333;
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 20);

        // Build the genesis block. Note that the output of the genesis coinbase cannot
        // be spent as it did not originally exist in the database.
        //
        //CBlock(hash=72030bc953fda05808070281e2c97aae61a9e31f3347aaee46bf825a1abefed0, ver=1, hashPrevBlock=0000000000000000000000000000000000000000000000000000000000000000, hashMerkleRoot=ad6f8414d38b88fc6daddd79a1ca2064993c86effa004f7772aa51a048f5939e, nTime=1528438062, nBits=1e0fffff, nNonce=4428133, vtx=1, vchBlockSig=);
        //  Coinbase(hash=12630d16a9, nTime=1528438062, ver=1, vin.size=1, vout.size=1, nLockTime=0);
        //    CTxIn(COutPoint(0000000000, 4294967295), coinbase 00012a24323020466562203230313420426974636f696e2041544d7320636f6d6520746f20555341);
        //    CTxOut(empty);
        //  vMerkleTree: 12630d16a9;
        const char* pszTimestamp = "Trump Shakes Up World Stage in Break With U.S. Allies 8/06/2018";
        std::vector<CTxIn> vin;
        vin.resize(1);
        vin[0].scriptSig = CScript() << 0 << CBigNum(42) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        std::vector<CTxOut> vout;
        vout.resize(1);
        vout[0].SetEmpty();
        CTransaction txNew(1, 1496692022, vin, vout, 0);
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime    = 1528438062;
        genesis.nBits    = 0x1e0fffff;
        genesis.nNonce   = 2216093;



        hashGenesisBlock = genesis.GetHash();
      // printf("genesis.GetHash = %s\n", genesis.GetHash().ToString().c_str());
      // printf("merkle = %s\n", genesis.hashMerkleRoot.ToString().c_str());
        assert(hashGenesisBlock == uint256("0x786ccd6a7811a72f46bf8e30475fe59c6f9bb02977d7f569a6e548a3fe0a2206"));
        assert(genesis.hashMerkleRoot == uint256("0xad6f8414d38b88fc6daddd79a1ca2064993c86effa004f7772aa51a048f5939e"));
      
      //  List of nodes:
        vSeeds.push_back(CDNSSeedData("sd1", "node01.euno.biz"));
        vSeeds.push_back(CDNSSeedData("sd2", "node02.euno.biz"));
        vSeeds.push_back(CDNSSeedData("sd3", "seed01.euno.biz"));
      	vSeeds.push_back(CDNSSeedData("sd4", "seed02.euno.biz"));
      //	vSeeds.push_back(CDNSSeedData("sd5", "80.211.206.236"));
      //	vSeeds.push_back(CDNSSeedData("sd6", "45.77.155.37"));
      //	vSeeds.push_back(CDNSSeedData("sd7", "54.218.118.59:9340"));
      //	vSeeds.push_back(CDNSSeedData("sd8", "54.218.118.59:9341"));


        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,33);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,5);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,155);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

        nLastPOWBlock = 6000000;
    }

    virtual const CBlock& GenesisBlock() const { return genesis; }
    virtual Network NetworkID() const { return CChainParams::MAIN; }

    virtual const vector<CAddress>& FixedSeeds() const {
        return vFixedSeeds;
    }
protected:
    CBlock genesis;
    vector<CAddress> vFixedSeeds;
};
static CMainParams mainParams;


//
// Testnet
//

class CTestNetParams : public CMainParams {
public:
    CTestNetParams() {
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0x21;
        pchMessageStart[1] = 0x5f;
        pchMessageStart[2] = 0xde;
        pchMessageStart[3] = 0x19;
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 16);
        vAlertPubKey = ParseHex("");
        nDefaultPort = 34333;
        nRPCPort = 24333;
        strDataDir = "testnet";

        // Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nBits  = 0x1e0fffff; //504365055 Decimal Number
        genesis.nTime    = 1528438062;
        genesis.nNonce = 2216093;

        assert(hashGenesisBlock == uint256("0x786ccd6a7811a72f46bf8e30475fe59c6f9bb02977d7f569a6e548a3fe0a2206"));

        vFixedSeeds.clear();
        vSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,92);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,192);
        base58Prefixes[SECRET_KEY]     = std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));

        nLastPOWBlock = 0x7fffffff;
    }
    virtual Network NetworkID() const { return CChainParams::TESTNET; }
};
static CTestNetParams testNetParams;


static CChainParams *pCurrentParams = &mainParams;

const CChainParams &Params() {
    return *pCurrentParams;
}

void SelectParams(CChainParams::Network network) {
    switch (network) {
        case CChainParams::MAIN:
            pCurrentParams = &mainParams;
            break;
        case CChainParams::TESTNET:
            pCurrentParams = &testNetParams;
            break;
        default:
            assert(false && "Unimplemented network");
            return;
    }
}

bool SelectParamsFromCommandLine() {

    bool fTestNet = GetBoolArg("-testnet", false);

    if (fTestNet) {
        SelectParams(CChainParams::TESTNET);
    } else {
        SelectParams(CChainParams::MAIN);
    }
    return true;
}
