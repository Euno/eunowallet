<p align="center">
	<img src="https://www.euno.co/wp-content/uploads/eunologoblack.png" />
</p>
<p align="center">
	<h1>EUNO - A privacy based cryptocurrency</h1>
</p>

<br><br>

 **Website:** https://www.euno.co<br>
 **WhitePaper** [https://www.euno.co/eunowhitepaper/](https://www.euno.co/wp-content/uploads/2019/01/EUNO-Coin-White-Paper.pdf)<br>
 **LightPaper** https://www.euno.co/euno-lite-paper/<br>
 **Governance** https://www.euno.co/governance-decentralization-and-structure/<br>
 **Telegram:** https://t.me/EUNOofficial<br>
 **Discord:** https://discord.gg/VNC6V2Z<br>
 **Twitter:** https://twitter.com/EunoCoin<br>
 **Reddit:** https://www.reddit.com/r/EunoCoinOfficial<br>
 **BitCoinTalk:** https://bitcointalk.org/index.php?topic=4563505.0<br>
 **CoinMarketCap:** https://coinmarketcap.com/currencies/euno/<br>
 **MasterNodesPro:** https://masternodes.pro/stats/euno/statistics<br>

### Technical details

 **System Protocol:** Hybrid - Proof-of-Work and Proof-of-Stake<br>
 **Hash Algorithm:** X11<br>
 **Currency symbols:** EUNO<br>
 **Market Cap:** 50 millions<br>
 **Block Time:** 120 seconds<br>
 **Masternode Collateral:** 50.000 EUNO<br>
 **Masternode Rewards:** 60% from PoS<br>
 **RPC Port:** 21333<br>
 **P2P Port:** 31333<br>

<br><br>
### Windows, MacOS and Linux QT wallets are available at:
https://github.com/Euno/eunowallet/releases

### To compile Raspberry PI wallet use:
https://github.com/Euno/install-wallet

<br><br>
### Build Linux CLI wallet

Build preparation:

    sudo apt-get -y update
    sudo apt-get install git
    sudo apt-get install dnsutils unzip
    sudo apt-get install build-essential libssl-dev libboost-all-dev git libdb5.3++-dev libminiupnpc-dev screen autoconf

Compile the EUNO cli wallet:

    cd ~
    git clone https://github.com/euno/eunowallet.git

    -->> Level DB
    cd ~/eunowallet/src/leveldb
    chmod +x build_detect_platform
    make libleveldb.a libmemenv.a

    -->> SECP 256
    cd ~/eunowallet/src/secp256k1
    chmod +x autogen.sh
    ./autogen.sh
    ./configure
    make

    -->> WALLET
    cd ~/eunowallet/src
    make -f makefile.unix
<br>
    After successfully compilation binary can be found ~/eunowallet/src/eunod

<br>

### Build Linux GUI wallet

Build preparation:
    
    sudo apt-get install qt5-default qttools5-dev-tools 

Compile the EUNO-QT GUI wallet:


    cd ~
    git clone https://github.com/euno/eunowallet.git

    -->> Level DB
    cd ~/eunowallet/src/leveldb
    chmod +x build_detect_platform
    make libleveldb.a libmemenv.a

    -->> SECP 256
    cd ~/eunowallet/src/secp256k1
    chmod +x autogen.sh
    ./autogen.sh
    ./configure --prefix=/usr
    make
    sudo make install

    -->> WALLET
    cd ~/eunowallet/
    qmake -o Makefile euno.pro
    make
<br>
    After successfully compilation binary can be found ~/eunowallet/euno-qt

<br><br>

