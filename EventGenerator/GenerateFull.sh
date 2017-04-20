#compile
source compile.sh

#generate dijet
./Generate.exe 0 pythia_dijet.root     1000

#generate W'-->WZ
./Generate.exe 1 pythia_ww.root        1000
