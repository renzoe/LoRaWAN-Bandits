# LoRaWAN-Bandits (2021)

Bandits for LoRaWAN in  NS-3. Work in the context of WP3 T3.1 of ANR Project INTELLIGENTSIA ( https://intelligentsia.roc.cnam.fr/ )


## Index

## Environment Set-Up


###  Installing from a vanilla Ubuntu 21.04 (Minima instalation)

If needed, check ns-3 tutorial https://www.nsnam.org/wiki/Installation

Install Ubuntu dependencies
```
sudo apt update
sudo apt install git
sudo apt install build-essential # To install gcc (C compiler)
```

To enable Phython Bindigs  (Currrently, we do not use Python Bindings)
```
sudo apt install python3 python3-dev pkg-config sqlite3 # 
sudo apt install python3-setuptools bzr # Previous line and this enables python bindings.

sudo apt install python3-pip
pip3 install PyBindGen

#https://www.nsnam.org/wiki/Python_bindings
#https://www.nsnam.org/docs/manual/html/python.html#working-with-python-bindings 
#https://www.nsnam.org/wiki/Installation#Installation
```

Cloning The LoRaWAN-Bandits Repo
```
git clone git@github.com:renzoe/2021-06-LoRaWAN-Bandits.git
```

Configure NS-3 , go to the  `./ns-3` folder, then:
```
./waf configure --enable-examples --enable-tests --disable-werror 
./waf build ? not yet
# if no python bindings use with ./waf --disable-python 
```

Set up  `AIToolboxMDP` library:
```
./config_external-libs.sh # this copies the .so to /usr/local/lib and the includes

#Other dependencies of ATtoolbox (boost, Eigen):
sudo apt-get install libboost-all-dev #500 mb! Maybe make a static lib AIToolboxMDP.a?
sudo apt install libeigen3-dev
```

Testing NS-3 and LoRaWAN module
```
./test.py

```

###  Particular LoRaWAN Bandits Dependencies

* AI-Toolbox: https://github.com/Svalorzen/AI-Toolbox 
In particular, the library `AIToolboxMDP` (See `/ns-3-dev/src/lorawan/wscript` Line #10).

We provide the compiled dynamic library (`AIToolboxMDP.so`) for `Linux x86_64` and the include headers (`.h`) . This provides a build solution on Linux OSs that does not involve building the `AI-Toolbox` dependency from source.

## Running Simulation

###  Main File
The Main Bandits Simulation function is in `./ns-3-dev/src/lorawan/examples/adr-bandit-example.cc`