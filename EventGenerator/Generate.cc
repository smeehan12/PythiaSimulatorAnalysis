/*----------------------------------------------------------------------

TITLE : MakeNTupleFromPythia.cc

DESCRIPTION : This runs the Pythia8 event generator, parsing the particle list
to determine which particles are to be used as final state particles [status = 81-99]
and writes these out as a vector of doubles for the components of the 4-vectors.

COMPILE :
$ source compile.sh

RUN :
$ ./MakeNTupleFromPythia <type> <output> <nevents>

type    : 0 = dijet , 1 = G*->WW (this determines which config file is used)
output  : Anything you want - but being logical.
nevents : The number of events you want to generate

//----------------------------------------------------------------------*/

#include <TROOT.h>
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"

#include <vector>
#include <string>

#include "Pythia8/Pythia.h"
using namespace Pythia8;

int main(int argc, char* argv[]){

  //debug flag
  bool debug=false;

  //exit if you dont pass a run card
  if(argc<4){
    cout<<"You need to specify more arguments"<<endl;
    return 1;
  }

  //get processtype
  int ProcessType = atoi(argv[1]);

  //get config file
  string ConfigFile   = "config_DEFAULT.cmnd";
  if(ProcessType==0)
    ConfigFile="config_dijet.cmnd";
  else if(ProcessType==1)
    ConfigFile="config_ww.cmnd";
  else if(ProcessType==2)
    ConfigFile="config_ttbar.cmnd";
  else if(ProcessType==3)
    ConfigFile="config_higgs.cmnd";
  else{
    cout<<"Bad process type!"<<endl;
    return 0;
  }

  //get output file name
  string OutfileName = argv[2];

  //number of events
  int nEvents = atoi(argv[3]);


  cout<<"InputArguments:  ProcessType="<<ProcessType<<"   ConfigFile="<<ConfigFile<<"  OutfileName="<<OutfileName<<endl;


  ///////////////////////////////////
  //initialize the output ttree and branches
  ///////////////////////////////////
  double truth_q1_pt;
  double truth_q1_eta;
  double truth_q1_phi;
  double truth_q1_m;

  double truth_q2_pt;
  double truth_q2_eta;
  double truth_q2_phi;
  double truth_q2_m;

  double truth_t1_pt;
  double truth_t1_eta;
  double truth_t1_phi;
  double truth_t1_m;

  double truth_t2_pt;
  double truth_t2_eta;
  double truth_t2_phi;
  double truth_t2_m;

  double truth_W_pt;
  double truth_W_eta;
  double truth_W_phi;
  double truth_W_m;

  double truth_Z_pt;
  double truth_Z_eta;
  double truth_Z_phi;
  double truth_Z_m;

  double truth_H_pt;
  double truth_H_eta;
  double truth_H_phi;
  double truth_H_m;

  vector<int>    fspart_id;
  vector<double> fspart_pt;
  vector<double> fspart_eta;
  vector<double> fspart_phi;
  vector<double> fspart_m;

  TTree *tree = new TTree("tree","tree");
  tree->Branch("fspart_id", &fspart_id);
  tree->Branch("fspart_pt", &fspart_pt);
  tree->Branch("fspart_eta",&fspart_eta);
  tree->Branch("fspart_phi",&fspart_phi);
  tree->Branch("fspart_m",  &fspart_m);

  tree->Branch("truth_q1_pt",  &truth_q1_pt);
  tree->Branch("truth_q1_eta", &truth_q1_eta);
  tree->Branch("truth_q1_phi", &truth_q1_phi);
  tree->Branch("truth_q1_m",   &truth_q1_m);

  tree->Branch("truth_q2_pt",  &truth_q2_pt);
  tree->Branch("truth_q2_eta", &truth_q2_eta);
  tree->Branch("truth_q2_phi", &truth_q2_phi);
  tree->Branch("truth_q2_m",   &truth_q2_m);

  tree->Branch("truth_t1_pt",  &truth_t1_pt);
  tree->Branch("truth_t1_eta", &truth_t1_eta);
  tree->Branch("truth_t1_phi", &truth_t1_phi);
  tree->Branch("truth_t1_m",   &truth_t1_m);

  tree->Branch("truth_t2_pt",  &truth_t2_pt);
  tree->Branch("truth_t2_eta", &truth_t2_eta);
  tree->Branch("truth_t2_phi", &truth_t2_phi);
  tree->Branch("truth_t2_m",   &truth_t2_m);

  tree->Branch("truth_W_pt",  &truth_W_pt);
  tree->Branch("truth_W_eta", &truth_W_eta);
  tree->Branch("truth_W_phi", &truth_W_phi);
  tree->Branch("truth_W_m",   &truth_W_m);

  tree->Branch("truth_Z_pt",  &truth_Z_pt);
  tree->Branch("truth_Z_eta", &truth_Z_eta);
  tree->Branch("truth_Z_phi", &truth_Z_phi);
  tree->Branch("truth_Z_m",   &truth_Z_m);

  tree->Branch("truth_H_pt",  &truth_H_pt);
  tree->Branch("truth_H_eta", &truth_H_eta);
  tree->Branch("truth_H_phi", &truth_H_phi);
  tree->Branch("truth_H_m",   &truth_H_m);

  //initialize pythia
  Pythia pythia;

  //read in config card
  pythia.readFile(ConfigFile);

  //get the number of events from the config file
  //int nEvents = pythia.mode("Main:numberOfEvents");

  //collide protons with setup defined above
  pythia.init();

  int nAcceptedEvents=0;

  ///////////////////////////////////
  //main event loop
  ///////////////////////////////////
  for (int iEvent = 0; iEvent < nEvents; ++iEvent) {

    //print out to show progress
    if(iEvent%100==0)
      cout<<"Pythia: ProcessType="<<ProcessType<<"  entry="<<iEvent<<endl;

    //generates a new event
    pythia.next();

    //accept event flag
    bool acceptevent=false;

    //clear all branches in preparation for next event
    fspart_id.clear();
    fspart_pt.clear();
    fspart_eta.clear();
    fspart_phi.clear();
    fspart_m.clear();

    truth_q1_pt  = 100.0;
    truth_q1_eta = 10.0;
    truth_q1_phi = 0.0;
    truth_q1_m   = 1.0;

    truth_q2_pt  = 100.0;
    truth_q2_eta = 10.0;
    truth_q2_phi = 0.0;
    truth_q2_m   = 1.0;

    truth_t1_pt  = 100.0;
    truth_t1_eta = 10.0;
    truth_t1_phi = 0.0;
    truth_t1_m   = 1.0;

    truth_t2_pt  = 100.0;
    truth_t2_eta = 10.0;
    truth_t2_phi = 0.0;
    truth_t2_m   = 1.0;

    truth_W_pt  = 100.0;
    truth_W_eta = 10.0;
    truth_W_phi = 0.0;
    truth_W_m   = 1.0;

    truth_Z_pt  = 100.0;
    truth_Z_eta = 10.0;
    truth_Z_phi = 0.0;
    truth_Z_m   = 1.0;

    truth_H_pt  = 100.0;
    truth_H_eta = 10.0;
    truth_H_phi = 0.0;
    truth_H_m   = 1.0;

    //loops through the particles in the event just generated
    for (int iPart = 0; iPart < pythia.event.size(); ++iPart) {

      //accept all events
      acceptevent=true;

      //only save particles to output ttree if they are final state particles
      if(pythia.event[iPart].isFinal()){
        if(debug) {
          if(iEvent%10==0){
            cout<<pythia.event[iPart].status()<<"  "
                <<pythia.event[iPart].id()<<"  "
                <<pythia.event[iPart].pT()<<"  "
                <<pythia.event[iPart].eta()<<"  "
                <<pythia.event[iPart].phi()<<"  "
                <<pythia.event[iPart].m()<<endl;
          }
        }

        fspart_id .push_back(pythia.event[iPart].id());
        fspart_pt .push_back(pythia.event[iPart].pT());
        fspart_eta.push_back(pythia.event[iPart].eta());
        fspart_phi.push_back(pythia.event[iPart].phi());
        fspart_m  .push_back(pythia.event[iPart].m());
      }
    }

    //fill event into tree if it has passed the event filter
    if(acceptevent){
      nAcceptedEvents+=1;
      tree->Fill();
    }


  }

  ///////////////////////////////////
  //Summary of truth filters
  ///////////////////////////////////
  cout<<"ProcessedEvents:"<<endl
      <<"total      = "<<nEvents<<endl
      <<"accepted   = "<<nAcceptedEvents<<endl
      <<"efficiency = "<<float(nAcceptedEvents)/float(nEvents)<<endl;

  TFile *fout = new TFile(OutfileName.c_str(),"RECREATE");
  tree->Write("tree");
  fout->Close();

  return 0;
}
