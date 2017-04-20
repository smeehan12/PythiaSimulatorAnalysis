#ifndef NTUPLER_H
#define NTUPLER_H

#define CERRD cout<<"Problem on "<<__FILE__<<"  "<<__LINE__<<endl;

#include <TROOT.h>
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TString.h"
#include "TLorentzVector.h"
#include "TRandom3.h"

#include <iostream>
#include <vector>
#include <string>
#include <cstdio>

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
#include <fastjet/Selector.hh>

#include "fastjet/tools/Filter.hh"
#include "fastjet/tools/Pruner.hh"

#include "fastjet/contrib/Nsubjettiness.hh"

using namespace fastjet;
using namespace std;


///////////////////////////
//input file and tree
///////////////////////////
TTree *treein;
TFile *filein;

int nEvents;

///////////////////////////
//input tree branches
///////////////////////////
vector<int>*    fspart_id;
vector<double>* fspart_pt;
vector<double>* fspart_eta;
vector<double>* fspart_phi;
vector<double>* fspart_m;


///////////////////////////
//output file and tree
///////////////////////////
TTree *treeout;
TFile *fileout;

///////////////////////////
//Temporary variables for jet observables
///////////////////////////
double tempJet_pt;
double tempJet_eta;
double tempJet_phi;
double tempJet_m;

///////////////////////////
//Output Content
///////////////////////////
int    NumberOfVertices;

vector<double> TruthRaw_pt;
vector<double> TruthRaw_eta;
vector<double> TruthRaw_phi;
vector<double> TruthRaw_m;

TH1F *j1_pt;
TH1F *j1_eta;
TH1F *j1_phi;
TH1F *j1_m;

///////////////////////////
//extra functions
///////////////////////////
void ResetBranches();

vector<PseudoJet> ToyCalorimeter(vector<PseudoJet> truth_particles);

#endif
