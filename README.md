# LoRaWAN Bandits

Bandits for LoRaWAN in the `ns-3` simulator.


>(...) From this point of view there can be no objection to the use of data, however meagre, as a guide to action required before more can be collected; although serious objection can otherwise be raised to argument based upon a small number of observations. Indeed, the fact that such objection can never be eliminated entirely—no matter how great the number of observations—suggested the possible value of seeking other modes of operation than that of taking  a large number of observations before analysis or any attempt to direct our course.
>
>—William Robin Thompson.
>  "On the likelihood that one unknown probability exceeds another in view of the evidence of two samples." Biometrika 25, no. 3-4 (1933).
>



Work in the context of WP3 T3.1 of ANR Project INTELLIGENTSIA ( https://intelligentsia.roc.cnam.fr) (grant number: [ANR-20-CE25-0011](https://anr.fr/Project-ANR-20-CE25-0011)).


### Companion Paper
This source code and data goes with the peer-reviewed paper :
```
Renzo E. Navas, Ghina Dandachi, Yassine Hadjadj-Aoul, and Patrick Maillé.
"Energy-Aware Spreading Factor Selection in LoRaWAN Using Delayed-Feedback Bandits"
2023 IFIP Networking Conference (IFIP Networking), Barcelona, Spain, 2023, pp. 1-9. 
IEEE, 2023.
```

**Please, if you use or are inspired by this source code, cite our aforementioned article.**

This is the source code used to generate the experimental results of the paper. 
Most of the source code development was made on the first half of the year 2021.

**[DESIGN]** We share the design information of our solution that justifies the metrics and some important decision we took along the way. They are located in the folder [`/design/`](design/).

**[DATA]** We share the data of the experiments we ran and some useful scripts we used to calculate metrics and do some plots. They are located in the folder [`/data/`](data/).

In the folder [`/data/2023-04-27-AgressiveBandits/`](data/2023-04-27-AgressiveBandits), we also include many other experiments that where not integrated to the  publication (mostly due to a mix of lack of time and article space) in which we experiment with parameters to more agressively learn (but this yield to varying results depeding the scenario. The parameters on the article are good all-around for the scenarios we simulate).







---------------------------
# Index

 1. [Environment Set-Up and Test](#setup)
 2. [Running Bandits Simulation](#running)
 2. [Capturing Simulation Data](#reading)
 4. [Appendix (`AI-Toolbox`)](#appendix)


---------------------------

# 1) Environment Set-Up and Test <a name="setup"></a>



##  Installing in a vanilla `Ubuntu 21.04 x86_64` and `Ubuntu 22.10 ARM64`
**Note**:  Please instantiate the VM with at least `20GB` of HHDD (12GB is not enough. 16GB is ok for Ubuntu 21.04 x86_64).

Tested on:
* `Ubuntu 21.04 x86_64` VM on `Virtual Box` using as host a `x86_64` running `Fedora 33`.
* `Ubuntu 21.04 x86_64` VM on `VMWare Player` using as host a `x86_64` running `Ubuntu 20.04`.
* `Ubuntu 22.10 ARM64`  VM on `VMWare Fusion (13.0.1)` using as host a `arm64` running `macOS 13.3.1 (Ventura)`.







This source code uses these other open source projects (no need to download nor compile, already included), acknowledgment again to the worderful persons who worked on them:
 * `Ns-3` (https://www.nsnam.org/wiki/Installation )
      *  [(v3.34+dev) ns-3-dev master 01/AGO/21 commit [75f15af2d6aba513886f273124249adaf9236778](https://gitlab.com/nsnam/ns-3-dev/-/commit/75f15af2d6aba513886f273124249adaf9236778)    ] (base v3.34 should work)
 * `LoRaWAN ns-3 module` (https://github.com/signetlabdei/lorawan)
    * [lorawan develop 30/APR/21 commit  [d412b3081b5741a2587082ec1f67c8ae202ef8e9](https://github.com/signetlabdei/lorawan/commit/d412b3081b5741a2587082ec1f67c8ae202ef8e9) ]
 * `AI-Toolbox` (https://github.com/Svalorzen/AI-Toolbox ) 
   *  [AI-Toolbox master  23/APR/21 commit [7046d767a8f048f6985fb7166eb8dc7ea353199f](https://github.com/Svalorzen/AI-Toolbox/commit/7046d767a8f048f6985fb7166eb8dc7ea353199f) ]
 

###  A) Installing Dependencies
Install basic dependencies 
```
sudo apt update
sudo apt install git
sudo apt install build-essential # To install gcc (C compiler)
```

Cloning The LoRaWAN Bandits Repo (i.e., this repo!)
```
git clone https://github.com/renzoe/LoRaWAN-Bandits.git
```

Go to the  `./ns-3` folder. 

Set up  `AIToolboxMDP` library: (1) Copy the library:
```
sudo ./config_external-libs.sh # this copies the .so to /usr/lib and the includes to /usr/local/include 
```

Set up  `AIToolboxMDP` library: (2) Install its depedencies:
```
#Other dependencies of lib ATToolbox (boost, Eigen3.3, liblpsolve55):
#a) Boost:
sudo apt install libboost-all-dev #500 mb!

#b) Eigen3.3: There is no need, is an all-headers lib, we already copied it with ./config_external-libs.sh 
#sudo apt install libeigen3-dev # No need to apt install, we already copied the headers.

#c) lp solve 5.5
sudo apt install lp-solve # No need for "apt install liblpsolve55-dev" (liblpsolve55-dev is needed to complie AI-Toolbox from source, but not if we already have its pre-compiled libraries like in our case)

sudo cp /usr/lib/lp_solve/liblpsolve55.so  /usr/lib/liblpsolve55.so # This is needed because in Ubuntu the library is not on a path that the linker ld looks (TODO: check if a linker cache update solves this)
```

###  B) Building and Testing 

Configure `ns-3`:
```
./waf configure --enable-examples --enable-tests --disable-werror --disable-python 
```

Building `ns-3` and Testing the vanilla `LoRaWAN module`
```
./waf build  # This will take a while depending on your hardware (+40 mins)
./test.py -s lorawan -v
```

**Testing one LoRaWAN Bandit! (Finally!)**:
```
./waf --run "src/lorawan/examples/adr-bandit-example  --nDevices=1 --HistoryRange=10000 --PeriodsToSimulate=100"
```

You should have an output like this:
```
Waf: Entering directory `/home/yourusername/LoRaWAN-Bandits/ns-3/build'
Waf: Leaving directory `/home/yourusername/LoRaWAN-Bandits/ns-3/build'
Build commands will be stored in build/compile_commands.json
'build' finished successfully (0.398s)
1.000000 0.000000
```



# 2) Running a `Bandit`/`LoRAWAN ADR` Simulation  <a name="running"></a>

##  A) Main File
The Main Bandits Simulations files (as defined on the companion article) are the following:
* Single-GW :  [`./ns-3/src/lorawan/examples/adr-bandit-example.cc`](ns-3/src/lorawan/examples/adr-bandit-example.cc)
* Multi-GWs  : [`./ns-3/src/lorawan/examples/adr-bandit-example-multi-gw.cc`](ns-3/src/lorawan/examples/adr-bandit-example-multi-gw.cc)

The nature of the End Devices (ED), `Bandits` or `LoRaWAN ADR`, is specified on those same files, in a line  ( [Line#269](ns-3/src/lorawan/examples/adr-bandit-example.cc#L269) and [Line#346](ns-3/src/lorawan/examples/adr-bandit-example-multi-gw.cc#L346)) that looks like this:
```
//macHelper.SetDeviceType (LorawanMacHelper::ED_A); // We create normal ADR nodes
 macHelper.SetDeviceType (LorawanMacHelper::ED_A_ADR_BANDIT); // We create ADR Bandits nodes :)
```
You should uncomment the type of ED that you want to use for your experiments.

## B)  Bandit's Rewards and other parameters
The  reward definition is set on [`./ns-3/src/lorawan/model/bandits/bandit-constants.h`](ns-3/src/lorawan/model/bandits/bandit-constants.h):
```
/*                                       Rewards = {SF12, SF11, SF10, SF9 , SF8  , SF7 }   */
  
 inline constexpr double rewardsEnergySimple[]   = { 1  , 2   , 4   , 8   , 16   , 32  } ; // Naif energy pondered reward
 inline constexpr double rewardsPurePDR[]        = { 1  , 1   , 1   , 1   , 1    , 1   } ; // Naif pure PDR
 
 inline constexpr const double * rewardsDefinition     =  rewardsEnergySimple ; // Here you set the active Reward Definition
```

You can easily define more rewards definitions/strategies by declaring a new array of six elements and assigning it to `rewardsDefinition`.

There, you can also change other important parameters of our solution like the initial phase number of messages in which we will not ask for feedback ($b$ in the paper, and $b=15$)

```
inline constexpr int framesForBoostraping  = 15   ; // The number of frames before the bandit starts asking for feedback
```

In this initial phase, each arm will be chosen equiprobably by the Thomson Sampling Bandit ^_^ --independently of the reward definition--.

Finally, another important parameter is $p$ used in the long-term strategy (in the paper $p=1/20$). This value determines the the probability of a given packet to request for a feedback message ~$Bernoulli(p)$. 


```
  inline constexpr double pAskingForFeedback = 0.05 ; // p of asking for feedback (Bernoulli)
```
### End Device Bandit Class Diagram

Here a high level class diagram of the Bandit for LoRAWAN MAC Class A devices:

![Alt text](design/NodeA-AI-ClassDiagram.png?raw=true "Title")

(A given node may have more than one bandit implementation and use the same learned Experience, we could even extend the class to use more advanced RL agents like Markov Decision Process)

##  C) Example Running Simulation: `Single-GW`
A typical run:

```
./waf --run "src/lorawan/examples/adr-bandit-example  --nDevices=1000 --HistoryRange=10000 --PeriodsToSimulate=100"
```

**Notes**:

* If you are using the LoRaWAN's Network Server ADR you should set the `HistoryRange=10` (To simulate The Things Network ADR):

      ` ./waf --run "src/lorawan/examples/adr-bandit-example  --nDevices=1000 --HistoryRange=10 --PeriodsToSimulate=100"`

* Also, we are using an `AVERAGE` value of the of `HistoryRange`, while TTN uses the `MAXIMUM` value. You can change this with
modifying the attributes `MultiplePacketsCombiningMethod` (and `MultipleGWCombiningMethod)` from the file  `./ns-3-dev/src/lorawan/model/adr-component.c` 



##  D)  Example Running Simulation: `Multi-GWs`
A typical run:

```
./waf --run "src/lorawan/examples/adr-bandit-example-multi-gw  --nDevices=2000 --HistoryRange=10000 --PeriodsToSimulate=100" 
```


**Note**: 
* We are using an `AVERAGE` value to combine GWs values for the same received packet. You can change this with
modifying the attribute `MultipleGWCombiningMethod` from the file  `./ns-3-dev/src/lorawan/model/adr-component.c`   (Suggestion: try with `MAXIMUM`? We chose `AVERAGE` as is more conservative. )


# 3) Capturing and Processing Simulation Data <a name="reading"></a>

## A) Output Files
After any given `lorawan` module simulation three files are created:
```
globalPerformance.txt
nodeData.txt
phyPerformance.txt
```

For Multi-GW simulations, additional files are created with the buildings and gateways positioning on the grid:
```
buildings.txt
gwData.txt
```


## B) Processing Scripts

We include relevants scripts to plot (`.gp`) and calculate (`.py`) the paper defined metrics on the folders 
 *  [/data/2022-06-29/01-SIngleGW/](data/2022-06-29/01-SIngleGW/)
 *  [/data/2022-06-29/02-MultiGW/](data/2022-06-29/02-MultiGW/)

The scripts are the following:
```
01-single-gw-bars.gp          # (Only relevant for Single-GW)
02-gnuscattered.gp            # Shows the spatial distribution of nodes, with color codes for the Speading Factors
03-calculate_pdr_last10.py.   # Metric: PDR          last 10 simulation periods (parametrizable)
04-calculate_joules_last10.py # Metric: Total Joules last 10 simulation periods (parametrizable)
05-calculate_unec_last10.py.  # Metric: uNEC         last 10 simulation periods (parametrizable)
```

(`.gp` is a `gnuplot` script and `.py` a `python3`)

For python, `numpy` and `matplotlib` are the dependencies:
```
sudo apt install python3-pip
pip install numpy matplotlib
```


The scripts will automattically ingest data from the three ADR strategies/subfolders
```
01-LegacyADR
02-BanditPDR
03-BanditEnergyPDR 
```

But they can be easily extended to more folders, or modified to ingest data from a particular  folder.



# Appendix) About `AI-Toolbox` <a name="appendix"></a>

##  A) `AI-Toolbox`: commentary on this library's dependency

Our code depends on library AI-Toolbox: https://github.com/Svalorzen/AI-Toolbox  
   + In particular, the library `AIToolboxMDP` (See `/ns-3-dev/src/lorawan/wscript` Line #10).
   + `AI-Toolbox`'s Dependencies are :  `boost`,  `Eigen 3.3`, `lp-solve 5.5`.
   + The `AI-Toolbox` version we use* requires `C++17` (See `/ns-3-dev/src/lorawan/wscript` Line #31). However, as of 27/JUL/21 the library requires `C++20`.
     + *Version we use: [AI-Toolbox master  23/APR/21 commit [7046d767a8f048f6985fb7166eb8dc7ea353199f](https://github.com/Svalorzen/AI-Toolbox/commit/7046d767a8f048f6985fb7166eb8dc7ea353199f) ]


We provide the compiled dynamic library (`AIToolboxMDP.so`) for `Linux x86_64` and `Linux arm64` and the include headers (`.h`) . This provides a build solution for `x86_64/arm64 Linux` OSs that does not involve building the `AI-Toolbox` dependency from source.

However, we need to install the pre-compiled lib dependencies by hand, this is provided on the script `config_external-libs.sh`. (The script detects an `arm64` architecture automatically and copies the corresponding pre-compiled files.)

```
./ns-3/config_external-libs.sh # this copies the .so to /usr/lib and the includes to /usr/local/include
```
**TODO:** See if the use of a static library (`.a`) is better. Note: We also include the compiled `.a` libraries for  `arm64`. In the end, we streamlined the installation of this library thanks to  the pre-compilation of the `.so` library and our `config_external-libs.sh` script. 


##  B) `AI-Toolbox`:  compiling the library in a `Ubuntu 22.10 ARM64`

**Note**: This compilation is not needed because we provide the pre-compiled `.so`. However, we provide these steps for the sake of documentation; it can be useful, for ex., if in the future the user wants to use an updated `AI-Toolbox` library, or modify it, and needs to compile from source.

Tested on a Virtualized Ubuntu 22.10 on a MacBook Pro 2023 (Chip: Apple M2 Pro):
* `Ubuntu 22.10 ARM64` VM on `VMWare Fusion (13.0.1)` using as host a `arm64` running `macOS 13.3.1 (Ventura)`.
  *  Please instantiate the VM with at least `20GB` of HHDD 

[//]: # (### Pre-requisite: `AI-Toolbox` compiled for `arm64` ###)

Compiling the required version of AI-Toolbox (commit sha `7046d767a8f048f6985fb7166eb8dc7ea353199f`)
```
git clone https://github.com/Svalorzen/AI-Toolbox.git
cd AI-Toolbox/
git fetch origin 7046d767a8f048f6985fb7166eb8dc7ea353199f
git reset --hard FETCH_HEAD
```

To build `AI-Toolbox` first we install his dependencies
```
sudo apt install cmake install g++-10 libboost-all-dev liblpsolve55-dev lp-solve libeigen3-dev 
```
[//]: # ( libboost1.71-all-dev liblpsolve55-dev lp-solve libeigen3-dev )


In order to compile, I had to manually add the line : 
 ```
 #include <optional>
 ```
to the file `./AI-Toolbox/include/AIToolbox/Factored/MDP/Types.hpp`

(Explanaition: This is because Ubuntu 22.10 uses C++20 (g++-10), and this `AI-Toolbox` version used C++17 to compile, and apprently some library names changed. This was not needed on an Ubuntu 21.04)


Once you have all required dependencies, you can  execute the following commands from the project's main folder:

```
mkdir build
cd build/
#cmake ..
#cmake -DMAKE_LIB=1  .. # (this generates .a static library, but we want an .so,  static):
cmake -D BUILD_SHARED_LIBS=1  ..
make
```
