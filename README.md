# LoRaWAN-Bandits (2021)

Bandits for LoRaWAN in  NS-3. Work in the context of WP3 T3.1 of ANR Project INTELLIGENTSIA ( https://intelligentsia.roc.cnam.fr/ )


## Index

 1. Environment Set-Up and Test
 2. Running Bandits Simulation
 2. Capturing Simulation Data


## 1) Environment Set-Up and Test


###  Installing from a vanilla Ubuntu 21.04 ("Minima instalation")

**Note**: Tested on a VM on Virtual Box. Rreserve 16GB HHDD at least (12GB is not enough).

This source code uses:
 * Ns-3 (https://www.nsnam.org/wiki/Installation )
      *  [(v3.34+dev) ns-3-dev master 01/AGO/21 commit [75f15af2d6aba513886f273124249adaf9236778](https://gitlab.com/nsnam/ns-3-dev/-/commit/75f15af2d6aba513886f273124249adaf9236778)    ] (base v3.34 should work)
 * LoRaWAN ns-3 module (https://github.com/signetlabdei/lorawan)
    * [lorawan develop 30/APR/21 commit  [d412b3081b5741a2587082ec1f67c8ae202ef8e9](https://github.com/signetlabdei/lorawan/commit/d412b3081b5741a2587082ec1f67c8ae202ef8e9) ]
 * AI-Toolbox (https://github.com/Svalorzen/AI-Toolbox ) 
   *  [AI-Toolbox master  23/APR/21 commit [7046d767a8f048f6985fb7166eb8dc7ea353199f](https://github.com/Svalorzen/AI-Toolbox/commit/7046d767a8f048f6985fb7166eb8dc7ea353199f) ]
 *

####  Installing Dependencies
Install basic dependencies 
```
sudo apt update
sudo apt install git
sudo apt install build-essential # To install gcc (C compiler)
```

Cloning The LoRaWAN Bandits Repo (i.e., this repo!)
```
git clone git@gitlab.inria.fr:intelligentsia/LoRaWAN-Bandits.git
```

Go to the  `./ns-3` folder. 

Set up  `AIToolboxMDP` library: (1) Copy the library:
```
./config_external-libs.sh # this copies the .so to /usr/lib and the includes to /usr/local/include
```

Set up  `AIToolboxMDP` library: (2) Install its depedencies:
```
#Other dependencies of lib ATToolbox (boost, Eigen3.3, liblpsolve55):
#a) Boost:
sudo apt install libboost-all-dev #500 mb! (TODO: Maybe use a static lib AIToolboxMDP.a?)

#b) Eigen3.3: No need, is an all-headers lib, we already copied it with ./config_external-libs.sh 
#sudo apt install libeigen3-dev # No need to apt install but just in case.

#c) lp solve 5.5
sudo apt install lp-solve # Removed "apt install liblpsolve55-dev" (TODO: check if was needed on clean install)
sudo cp /usr/lib/lp_solve/liblpsolve55.so  /usr/lib/liblpsolve55.so # Because in ubuntu the library is not on a path that the linker ld looks (TODO: check if a linker cache update solves this)
```


####  Building and Testing 

Configure NS-3. 
```
./waf configure --enable-examples --enable-tests --disable-werror --disable-python 
```

Building NS-3 and Testing LoRaWAN module
```
./waf build  # This will take a while depending on VM (+40 mins)
./test.py -s lorawan -v
```

**Testing One LoRaWAN Bandit! (Finally!)**:
```
./waf --run "src/lorawan/examples/adr-bandit-example  --nDevices=1 --HistoryRange=10000 --PeriodsToSimulate=100"
```



## 2) Running Bandits Simulation

###  Main File
The Main Bandits Simulation function is in `./ns-3-dev/src/lorawan/examples/adr-bandit-example.cc`


## 3) Capturing Simulation Data
TODO proper

After a simulation three files are created:
```
globalPerformance.txt
nodeData.txt
phyPerformance.txt
```

For quick gnuplot graphs refer to `/data/gnubars.txt` and `/data/gnuscattered.txt`

## MISC

###  Comment on Particular LoRaWAN Bandits Dependencies (AI-Toolbox)

* AI-Toolbox: https://github.com/Svalorzen/AI-Toolbox  
   + In particular, the library `AIToolboxMDP` (See `/ns-3-dev/src/lorawan/wscript` Line #10).
   + AI-Toolbox's Dependencies :  boost,  Eigen 3.3, lp-solve 5.5.
   + The version we use requires C++17 (See `/ns-3-dev/src/lorawan/wscript` Line #31). However, as of 27/JUL/21 the library requires C++20.

We provide the compiled dynamic library (`AIToolboxMDP.so`) for `Linux x86_64` and the include headers (`.h`) . This provides a build solution on Linux OSs that does not involve building the `AI-Toolbox` dependency from source. However, we need to install the pre-compiled lib dependencies by hand. **TODO:** See if use of a static library is better of if we can streamline the installation of this lib.


###  Python Bindings (TODO, Not Working)
**NOT WORKING**
To enable Phython Bindigs  (Currrently, we do not use Python Bindings and **it is not workin**)
```
sudo apt install python3 python3-dev pkg-config sqlite3 # 
sudo apt install python3-setuptools bzr # 

sudo apt install python3-pip
sudo pip3 install PyBindGen # Better to use --user, but waf does not recognize it later

#https://www.nsnam.org/wiki/Python_bindings
#https://www.nsnam.org/docs/manual/html/python.html#working-with-python-bindings 
#https://www.nsnam.org/wiki/Installation#Installation
```

Re-Configure NS-3 , go to the  `./ns-3` folder, then:
```
./waf configure --enable-examples --enable-tests --disable-werror 
# Currently there is a binding error with fd-net-device module, bindings to API need to be re-done (more dependencies installed)
```
