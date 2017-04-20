# Pythia Simulator
This is initial setup for simulating events with the Pythia event generator and using fastjet to perform jet clustering.  This is intended as a starting point to develop an analysis.


# First time setup
In addition to this code, you will need to compile and install the following additional packages, whose libraries are used within the generation and analysis framework.

### ROOT: https://root.cern.ch/building-root   

### Pythia8: http://home.thep.lu.se/~torbjorn/Pythia.html 
- This must be compiled/installed in a location that is then accessible when you compile the EventGenerator.  You will need to point the compilation to the Pythia directory.
- If you are not familiar with Pythia then it is worthwhile working through the worksheet outlined (in this tutorial)[http://home.thep.lu.se/~torbjorn/pythia8/worksheet8183.pdf] since the EventGeneration code is built starting from this.

### fastjet : http://fastjet.fr/
- This was developed with fastjet-3.2.1
- - This must be compiled/installed in a location that is then accessible when you compile the next package, fastjet contrib.
- This must be compiled/installed in a location that is then accessible when you compile the EventAnalyzer.  You will need to point the compilation to the fastjet directory so that it can successfully find the configurations


### fastjet contrib : http://fastjet.hepforge.org/contrib/ \\
- It is necessary to point the code to the fastjet install directory as shown in the install directions.  This is done via the `--fastjet-config=/path/to/fastjet-config` argument.  
- If you installed fastjet in the directory above the fjcontrib directory, this means pointing it to the fastjet-config at ../fastjet-3.1.1/fastjet-config.  If you do this correctly, then you should see the /contrib directory housed in `/usr/local/include/fastjet` after running `make install`.



# EventGenerator

This is the first state of analysis in which the Pythia8 event generator is used to produce a set of events for any process you wish and perform the subsequent shower and hadronization to produce a set of final state particles that can then be used for analysis via jet building.

#### Where : 
EventGeneration

#### How : 
```
source compile.sh
./Generate 1 pythia_ww.root 1000
```

#### What :
This will generate 1000 events and save to the pythia_ww.root output file a flat ntuple that contains truth vectors of the relevant particles (W,Z) for truth tagging later along with vector<double> branches for all of the truth particles with final state particles.


# EventAnalyzer

This is the second stage of analysis that takes as input the set of final state particles from Pythia and runs jet building and calculation of jet moments (substructure) on these jets using fastjet and fastjet contrib.  In particular, the Telescoping Jets code is run at this level of analysis.  This should always be made to produce a flat ntuple that has no complicated structure to it but is composed simply of branches of doubles.

### Where : 
EventAnalyzer

### How : 
```
source compile.sh
./NTupler 1 path/to/pythia\_ww.root output\_wz.root
```

#### What :
This will process the 1000 events generated with the `MakeNTupleFromPythia` code and do the following:
1) Run jet finding on the truth constituents 
2) Calculate basic observables
3) Make an output file containing some basic histograms and a tree of jets

The output will be saved in the file output_ww.root




