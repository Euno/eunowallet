// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2016-2020 The PIVX developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "txdb.h"

#include "main.h"
#include "pow.h"
#include "uint256.h"

#include <stdint.h>

#include <boost/thread.hpp>

static const char DB_COINS = 'c';
static const char DB_BLOCK_FILES = 'f';
static const char DB_TXINDEX = 't';
static const char DB_BLOCK_INDEX = 'b';

static const char DB_BEST_BLOCK = 'B';
static const char DB_FLAG = 'F';
static const char DB_REINDEX_FLAG = 'R';
static const char DB_LAST_BLOCK = 'l';
static const char DB_MONEY_SUPPLY = 'M';


CCoinsViewDB::CCoinsViewDB(size_t nCacheSize, bool fMemory, bool fWipe) : db(GetDataDir() / "chainstate", nCacheSize, fMemory, fWipe)
{
}

bool CCoinsViewDB::GetCoins(const uint256& txid, CCoins& coins) const
{
    return db.Read(std::make_pair(DB_COINS, txid), coins);
}

bool CCoinsViewDB::HaveCoins(const uint256& txid) const
{
    return db.Exists(std::make_pair(DB_COINS, txid));
}

uint256 CCoinsViewDB::GetBestBlock() const
{
    uint256 hashBestChain;
    if (!db.Read(DB_BEST_BLOCK, hashBestChain))
        return UINT256_ZERO;
    return hashBestChain;
}

bool CCoinsViewDB::BatchWrite(CCoinsMap& mapCoins, const uint256& hashBlock)
{
    CDBBatch batch;
    size_t count = 0;
    size_t changed = 0;
    for (CCoinsMap::iterator it = mapCoins.begin(); it != mapCoins.end();) {
        if (it->second.flags & CCoinsCacheEntry::DIRTY) {
            if (it->second.coins.IsPruned())
                batch.Erase(std::make_pair(DB_COINS, it->first));
            else
                batch.Write(std::make_pair(DB_COINS, it->first), it->second.coins);
            changed++;
        }
        count++;
        CCoinsMap::iterator itOld = it++;
        mapCoins.erase(itOld);
    }
    if (!hashBlock.IsNull())
        batch.Write(DB_BEST_BLOCK, hashBlock);

    LogPrint(BCLog::COINDB, "Committing %u changed transactions (out of %u) to coin database...\n", (unsigned int)changed, (unsigned int)count);
    return db.WriteBatch(batch);
}

CBlockTreeDB::CBlockTreeDB(size_t nCacheSize, bool fMemory, bool fWipe) : CDBWrapper(GetDataDir() / "blocks" / "index", nCacheSize, fMemory, fWipe)
{
}

bool CBlockTreeDB::WriteBlockIndex(const CDiskBlockIndex& blockindex)
{
    return Write(std::make_pair(DB_BLOCK_INDEX, blockindex.GetBlockHash()), blockindex);
}

bool CBlockTreeDB::ReadBlockFileInfo(int nFile, CBlockFileInfo& info)
{
    return Read(std::make_pair(DB_BLOCK_FILES, nFile), info);
}

bool CBlockTreeDB::WriteReindexing(bool fReindexing)
{
    if (fReindexing)
        return Write(DB_REINDEX_FLAG, '1');
    else
        return Erase(DB_REINDEX_FLAG);
}

bool CBlockTreeDB::ReadReindexing(bool& fReindexing)
{
    fReindexing = Exists(DB_REINDEX_FLAG);
    return true;
}

bool CBlockTreeDB::ReadLastBlockFile(int& nFile)
{
    return Read(DB_LAST_BLOCK, nFile);
}

bool CCoinsViewDB::GetStats(CCoinsStats& stats) const
{
    /* It seems that there are no "const iterators" for LevelDB.  Since we
       only need read operations on it, use a const-cast to get around
       that restriction.  */
    boost::scoped_ptr<CDBIterator> pcursor(const_cast<CDBWrapper*>(&db)->NewIterator());
    pcursor->Seek(DB_COINS);

    CHashWriter ss(SER_GETHASH, PROTOCOL_VERSION);
    stats.hashBlock = GetBestBlock();
    ss << stats.hashBlock;
    CAmount nTotalAmount = 0;
    while (pcursor->Valid()) {
        boost::this_thread::interruption_point();
        std::pair<char, uint256> key;
        CCoins coins;
        if (pcursor->GetKey(key) && key.first == DB_COINS) {
            if (pcursor->GetValue(coins)) {
                stats.nTransactions++;
                for (unsigned int i = 0; i < coins.vout.size(); i++) {
                    const CTxOut& out = coins.vout[i];
                    if (!out.IsNull()) {
                        stats.nTransactionOutputs++;
                        ss << VARINT(i + 1);
                        ss << out;
                        nTotalAmount += out.nValue;
                    }
                }
                stats.nSerializedSize += 32 + pcursor->GetValueSize();
                ss << VARINT(0);
            } else {
                return error("CCoinsViewDB::GetStats() : unable to read value");
            }
        } else {
            break;
        }
        pcursor->Next();
    }

    stats.nHeight = WITH_LOCK(cs_main, return mapBlockIndex.find(stats.hashBlock)->second->nHeight;);
    stats.hashSerialized = ss.GetHash();
    stats.nTotalAmount = nTotalAmount;
    return true;
}

bool CBlockTreeDB::WriteMoneySupply(const int64_t& nSupply)
{
    return Write(DB_MONEY_SUPPLY, nSupply);
}

bool CBlockTreeDB::ReadMoneySupply(int64_t& nSupply) const
{
    return Read(DB_MONEY_SUPPLY, nSupply);
}

bool CBlockTreeDB::WriteBatchSync(const std::vector<std::pair<int, const CBlockFileInfo*> >& fileInfo, int nLastFile, const std::vector<const CBlockIndex*>& blockinfo) {
    CDBBatch batch;
    for (std::vector<std::pair<int, const CBlockFileInfo*> >::const_iterator it=fileInfo.begin(); it != fileInfo.end(); it++) {
        batch.Write(std::make_pair(DB_BLOCK_FILES, it->first), *it->second);
    }
    batch.Write(DB_LAST_BLOCK, nLastFile);
    for (std::vector<const CBlockIndex*>::const_iterator it=blockinfo.begin(); it != blockinfo.end(); it++) {
        batch.Write(std::make_pair(DB_BLOCK_INDEX, (*it)->GetBlockHash()), CDiskBlockIndex(*it));
    }
    return WriteBatch(batch, true);
}

bool CBlockTreeDB::ReadTxIndex(const uint256& txid, CDiskTxPos& pos)
{
    return Read(std::make_pair(DB_TXINDEX, txid), pos);
}

bool CBlockTreeDB::WriteTxIndex(const std::vector<std::pair<uint256, CDiskTxPos> >& vect)
{
    CDBBatch batch;
    for (std::vector<std::pair<uint256, CDiskTxPos> >::const_iterator it = vect.begin(); it != vect.end(); it++)
        batch.Write(std::make_pair(DB_TXINDEX, it->first), it->second);
    return WriteBatch(batch);
}

bool CBlockTreeDB::WriteFlag(const std::string& name, bool fValue)
{
    return Write(std::make_pair(DB_FLAG, name), fValue ? '1' : '0');
}

bool CBlockTreeDB::ReadFlag(const std::string& name, bool& fValue)
{
    char ch;
    if (!Read(std::make_pair(DB_FLAG, name), ch))
        return false;
    fValue = ch == '1';
    return true;
}

bool CBlockTreeDB::WriteInt(const std::string& name, int nValue)
{
    return Write(std::make_pair('I', name), nValue);
}

bool CBlockTreeDB::ReadInt(const std::string& name, int& nValue)
{
    return Read(std::make_pair('I', name), nValue);
}

bool CBlockTreeDB::LoadBlockIndexGuts()
{
    boost::scoped_ptr<CDBIterator> pcursor(NewIterator());

    pcursor->Seek(std::make_pair(DB_BLOCK_INDEX, UINT256_ZERO));

    // Load mapBlockIndex
    while (pcursor->Valid()) {
        boost::this_thread::interruption_point();
        std::pair<char, uint256> key;
        if (pcursor->GetKey(key) && key.first == DB_BLOCK_INDEX) {
            CDiskBlockIndex diskindex;
            if (pcursor->GetValue(diskindex)) {
                // Construct block index object
                CBlockIndex* pindexNew = InsertBlockIndex(diskindex.GetBlockHash());
                pindexNew->pprev = InsertBlockIndex(diskindex.hashPrev);
                pindexNew->nHeight = diskindex.nHeight;
                pindexNew->nFile = diskindex.nFile;
                pindexNew->nDataPos = diskindex.nDataPos;
                pindexNew->nUndoPos = diskindex.nUndoPos;
                pindexNew->nVersion = diskindex.nVersion;
                pindexNew->hashMerkleRoot = diskindex.hashMerkleRoot;
                pindexNew->nTime = diskindex.nTime;
                pindexNew->nBits = diskindex.nBits;
                pindexNew->nNonce = diskindex.nNonce;
                pindexNew->nStatus = diskindex.nStatus;
                pindexNew->nTx = diskindex.nTx;

                //zerocoin
                pindexNew->nAccumulatorCheckpoint = diskindex.nAccumulatorCheckpoint;

                //Proof Of Stake
                pindexNew->nFlags = diskindex.nFlags;
                pindexNew->vStakeModifier = diskindex.vStakeModifier;

                if (!Params().GetConsensus().NetworkUpgradeActive(pindexNew->nHeight, Consensus::UPGRADE_POS)) {
                    if (!CheckProofOfWork(pindexNew->GetBlockHash(), pindexNew->nBits))
                        return error("LoadBlockIndex() : CheckProofOfWork failed: %s", pindexNew->ToString());
                }

                pcursor->Next();
            } else {
                return error("%s : failed to read value", __func__);
            }
        } else {
            break;
        }
    }

    return true;
}

bool CBlockTreeDB::ReadLegacyBlockIndex(const uint256& blockHash, CLegacyBlockIndex& biRet)
{
    return Read(std::make_pair(DB_BLOCK_INDEX, blockHash), biRet);
}

CZerocoinDB::CZerocoinDB(size_t nCacheSize, bool fMemory, bool fWipe) : CDBWrapper(GetDataDir() / "zerocoin", nCacheSize, fMemory, fWipe)
{
}

bool CZerocoinDB::WriteCoinMintBatch(const std::vector<std::pair<libzerocoin::PublicCoin, uint256> >& mintInfo)
{
    CDBBatch batch;
    size_t count = 0;
    for (std::vector<std::pair<libzerocoin::PublicCoin, uint256> >::const_iterator it=mintInfo.begin(); it != mintInfo.end(); it++) {
        libzerocoin::PublicCoin pubCoin = it->first;
        uint256 hash = GetPubCoinHash(pubCoin.getValue());
        batch.Write(std::make_pair('m', hash), it->second);
        ++count;
    }

    LogPrint(BCLog::COINDB, "Writing %u coin mints to db.\n", (unsigned int)count);
    return WriteBatch(batch, true);
}

bool CZerocoinDB::ReadCoinMint(const CBigNum& bnPubcoin, uint256& hashTx)
{
    return ReadCoinMint(GetPubCoinHash(bnPubcoin), hashTx);
}

bool CZerocoinDB::ReadCoinMint(const uint256& hashPubcoin, uint256& hashTx)
{
    return Read(std::make_pair('m', hashPubcoin), hashTx);
}

bool CZerocoinDB::EraseCoinMint(const CBigNum& bnPubcoin)
{
    uint256 hash = GetPubCoinHash(bnPubcoin);
    return Erase(std::make_pair('m', hash));
}

bool CZerocoinDB::WriteCoinSpendBatch(const std::vector<std::pair<libzerocoin::CoinSpend, uint256> >& spendInfo)
{
    CDBBatch batch;
    size_t count = 0;
    for (std::vector<std::pair<libzerocoin::CoinSpend, uint256> >::const_iterator it=spendInfo.begin(); it != spendInfo.end(); it++) {
        CBigNum bnSerial = it->first.getCoinSerialNumber();
        CDataStream ss(SER_GETHASH, 0);
        ss << bnSerial;
        uint256 hash = Hash(ss.begin(), ss.end());
        batch.Write(std::make_pair('s', hash), it->second);
        ++count;
    }

    LogPrint(BCLog::COINDB, "Writing %u coin spends to db.\n", (unsigned int)count);
    return WriteBatch(batch, true);
}

bool CZerocoinDB::ReadCoinSpend(const CBigNum& bnSerial, uint256& txHash)
{
    CDataStream ss(SER_GETHASH, 0);
    ss << bnSerial;
    uint256 hash = Hash(ss.begin(), ss.end());

    return Read(std::make_pair('s', hash), txHash);
}

bool CZerocoinDB::ReadCoinSpend(const uint256& hashSerial, uint256 &txHash)
{
    return Read(std::make_pair('s', hashSerial), txHash);
}

bool CZerocoinDB::EraseCoinSpend(const CBigNum& bnSerial)
{
    CDataStream ss(SER_GETHASH, 0);
    ss << bnSerial;
    uint256 hash = Hash(ss.begin(), ss.end());

    return Erase(std::make_pair('s', hash));
}

bool CZerocoinDB::WipeCoins(std::string strType)
{
    if (strType != "spends" && strType != "mints")
        return error("%s: did not recognize type %s", __func__, strType);

    boost::scoped_ptr<CDBIterator> pcursor(NewIterator());

    char type = (strType == "spends" ? 's' : 'm');
    pcursor->Seek(std::make_pair(type, UINT256_ZERO));
    // Load mapBlockIndex
    std::set<uint256> setDelete;
    while (pcursor->Valid()) {
        boost::this_thread::interruption_point();
        std::pair<char, uint256> key;
        if (pcursor->GetKey(key) && key.first == type) {
            uint256 hash;
            if (pcursor->GetValue(hash)) {
                setDelete.insert(hash);
                pcursor->Next();
            } else {
                return error("%s : failed to read value", __func__);
            }
        } else {
            break;
        }
    }

    for (auto& hash : setDelete) {
        if (!Erase(std::make_pair(type, hash)))
            LogPrintf("%s: error failed to delete %s\n", __func__, hash.GetHex());
    }

    return true;
}


// Legacy Zerocoin Database
static const char LZC_ACCUMCS = 'A';
static const char LZC_MAPSUPPLY = 'M';

bool CZerocoinDB::WriteZCSupply(const std::map<libzerocoin::CoinDenomination, int64_t>& mapZCS)
{
    return Write(LZC_MAPSUPPLY, mapZCS);
}

bool CZerocoinDB::ReadZCSupply(std::map<libzerocoin::CoinDenomination, int64_t>& mapZCS) const
{
    return Read(LZC_MAPSUPPLY, mapZCS);
}

bool CZerocoinDB::WriteAccChecksum(const uint32_t& nChecksum, const libzerocoin::CoinDenomination denom, const int nHeight)
{
    return Write(std::make_pair(LZC_ACCUMCS, std::make_pair(nChecksum, denom)), nHeight);
}

bool CZerocoinDB::ReadAccChecksum(const uint32_t& nChecksum, const libzerocoin::CoinDenomination denom, int& nHeightRet)
{
    return Read(std::make_pair(LZC_ACCUMCS, std::make_pair(nChecksum, denom)), nHeightRet);
}

bool CZerocoinDB::EraseAccChecksum(const uint32_t& nChecksum, const libzerocoin::CoinDenomination denom)
{
    return Erase(std::make_pair(LZC_ACCUMCS, std::make_pair(nChecksum, denom)));
}

bool CZerocoinDB::WipeAccChecksums()
{
    boost::scoped_ptr<CDBIterator> pcursor(NewIterator());
    pcursor->Seek(std::make_pair(LZC_ACCUMCS, (uint32_t) 0));
    std::set<uint32_t> setDelete;
    while (pcursor->Valid()) {
        boost::this_thread::interruption_point();
        std::pair<char, uint32_t> key;
        if (pcursor->GetKey(key) && key.first == LZC_ACCUMCS) {
            uint32_t acs;
            if (pcursor->GetValue(acs)) {
                setDelete.insert(acs);
                pcursor->Next();
            } else {
                return error("%s : failed to read value", __func__);
            }
        } else {
            break;
        }
    }

    for (auto& acs : setDelete) {
        if (!Erase(std::make_pair(LZC_ACCUMCS, acs)))
            LogPrintf("%s: error failed to acc checksum %s\n", __func__, acs);
    }

    LogPrintf("%s: AccChecksum database removed.\n", __func__);
    return true;
}
