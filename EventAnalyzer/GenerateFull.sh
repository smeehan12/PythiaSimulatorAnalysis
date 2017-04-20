#compile
source compile.sh

#generate dijet
./Analyze.exe ../EventGenerator/pythia_dijet.root output_dijet.root

#generate W'-->WW
./Analyze.exe ../EventGenerator/pythia_ww.root    output_ww.root
