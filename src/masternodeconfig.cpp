// Copyright (c) 2017-2018 Euno developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "net.h"
#include "masternodeconfig.h"
#include "util.h"

CMasternodeConfig masternodeConfig;

void CMasternodeConfig::add(std::string alias, std::string ip, std::string privKey, std::string txHash,
                            std::string outputIndex, std::string acceptStakeRewards)
{
    CMasternodeEntry cme(alias, ip, privKey, txHash, outputIndex, acceptStakeRewards);
    entries.push_back(cme);
}

bool CMasternodeConfig::read(std::string& strErr)
{
    boost::filesystem::path path = GetMasternodeConfigFile();
    boost::filesystem::ifstream streamConfig(path);

    if (!streamConfig.good())
    {
        if(fDebug)
            LogPrintf("Failed to open masternode configuration file \"%s\".", path);

        return true; // No masternode.conf file is OK
    }

    if(fDebug)
        LogPrintf("Opened masternode configuration file \"%s\".", path);

    for(std::string line; std::getline(streamConfig, line); )
    {
        if(line.empty())
            continue;

        std::istringstream iss(line);
        std::string alias, ip, privKey, txHash, outputIndex, acceptStakeRewards;

        if (!(iss >> alias >> ip >> privKey >> txHash >> outputIndex >> acceptStakeRewards))
        {
            strErr = "Could not parse masternode.conf line: " + line;
            streamConfig.close();
            return false;
        }

        /* if(CService(ip).GetPort() != 19999 && CService(ip).GetPort() != 9999)  {
            strErr = "Invalid port (must be 9999 for mainnet or 19999 for testnet) detected in masternode.conf: " + line;
            streamConfig.close();
            return false;
        } */

        add(alias, ip, privKey, txHash, outputIndex, acceptStakeRewards);
    }

    streamConfig.close();
    return true;
}
