export FJINSTALL=/Users/meehan/work/TelescopingJets/TelescopingJets/fastjet-3.2.1

echo
echo "Compiling with : "
echo "$ROOTSYS    : "${ROOTSYS}
echo "$FJINSTALL : "${FJINSTALL}
echo

g++ -o Analyze.exe Analyze.cc -I$ROOTSYS/include -I$FJINSTALL/include  -rpath $ROOTSYS/lib `$ROOTSYS/bin/root-config --glibs` `$FJINSTALL/fastjet-config --cxxflags --libs --plugins` -lNsubjettiness -lEnergyCorrelator   -std=c++11