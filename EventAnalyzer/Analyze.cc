/*----------------------------------------------------------------------

TITLE : Analyze.cc

DESCRIPTION : Takes input final state particle ntuple from EventGenerator
and allows for one to perform analysis after performing jet clustering

COMPILE :
$ source compile.sh

RUN :
$ ./Analyze <input> <output>

input  : The full path to the input file
output : The name of the output file

//----------------------------------------------------------------------*/


#include "Analyze.h"

int main(int argc, char* argv[]){


  //exit if you dont pass a run card
  if(argc<3){
    cout<<"You need to specify more arguments"<<endl;
    return 1;
  }

  //inputfile
  string InputFile = argv[1];

  //outputfile
  string OutputFile = argv[2];

  //debug flag
  bool debug=false;

  //dR truth matching
  double dR_match = 1.0;

  //////////////////////////////////////////////
  //INPUT
  //////////////////////////////////////////////
  //get input file and tree
  filein = new TFile( InputFile.c_str() );
  treein = (TTree*)filein->Get( "tree" );
  if(debug) treein->Print();

  //set up branch linking to addresses
  treein->SetBranchAddress("fspart_id", &fspart_id);
  treein->SetBranchAddress("fspart_pt", &fspart_pt);
  treein->SetBranchAddress("fspart_eta",&fspart_eta);
  treein->SetBranchAddress("fspart_phi",&fspart_phi);
  treein->SetBranchAddress("fspart_m",  &fspart_m);

  //////////////////////////////////////////////
  //OUTPUT
  //////////////////////////////////////////////
  fileout = new TFile( OutputFile.c_str() ,"RECREATE");

  treeout = new TTree("JetTree","JetTree");
  treeout->Branch("NumberOfVertices",    &NumberOfVertices);
  treeout->Branch("TruthRaw_pt",            &TruthRaw_pt);
  treeout->Branch("TruthRaw_eta",           &TruthRaw_eta);
  treeout->Branch("TruthRaw_phi",           &TruthRaw_phi);
  treeout->Branch("TruthRaw_m",             &TruthRaw_m);

  j1_pt   = new TH1F("j1_pt","j1_pt",1000,0,1000);
  j1_eta  = new TH1F("j1_eta","j1_eta",80,-4,4);
  j1_phi  = new TH1F("j1_phi","j1_phi",80,-4,4);
  j1_m    = new TH1F("j1_m","j1_m",1000,0,1000);


  ////////////////////////////////
  //random number generator for pileup
  ////////////////////////////////
  TRandom3 *rand_pileup = new TRandom3();


  //////////////////////////////////////////////
  //main event loop
  //////////////////////////////////////////////
  nEvents = treein->GetEntries();
  cout<<"Number of events: "<<nEvents<<endl;

  for (Long64_t jentry=0; jentry<nEvents; jentry++) {

    if(jentry%100==0)
      cout<<"Analyzed NEvent : "<<jentry<<endl;

    //Get next event from input ntuple
    filein->cd();
    treein->GetEntry(jentry);

    /////////////////////////////
    //Reset branches for next event
    /////////////////////////////
    ResetBranches();

    ///////////////////////////////////////////////////
    //read in all final state particles for jet building from pythia input
    ///////////////////////////////////////////////////
    vector<PseudoJet> input_particles;
    input_particles.clear();

    int n_fspart = fspart_id->size();
    for(int i_fspart=0; i_fspart<n_fspart; i_fspart++){

      if(debug){
        cout<<fspart_id->at(i_fspart)<<"  "
            <<fspart_pt->at(i_fspart)<<"  "
            <<fspart_eta->at(i_fspart)<<"  "
            <<fspart_phi->at(i_fspart)<<"  "
            <<fspart_m->at(i_fspart)<<"  "<<endl;
      }

      TLorentzVector temp_p4;
      temp_p4.SetPtEtaPhiM(fspart_pt ->at(i_fspart),
                           fspart_eta->at(i_fspart),
                           fspart_phi->at(i_fspart),
                           fspart_m  ->at(i_fspart));


      input_particles.push_back(PseudoJet(temp_p4.Px(),temp_p4.Py(),temp_p4.Pz(),temp_p4.E()));
    }

    //////////////////////////////////////////////
    //make new input particles collection with pileup
    //////////////////////////////////////////////
    vector<PseudoJet> input_particles_Pileup;
    input_particles_Pileup.clear();
    for(int ipart=0; ipart<n_fspart; ipart++){
      input_particles_Pileup.push_back(input_particles.at(ipart));
    }

    int n_pileup_vertices      = (int)rand_pileup->Poisson(10);
    int n_particles_per_vertex = 5;
    int n_pileup_particles = n_pileup_vertices*n_particles_per_vertex;

    NumberOfVertices = n_pileup_vertices;

    if(debug) cout<<"Pileup: "<<NumberOfVertices<<"  "<<n_particles_per_vertex<<"  "<<n_pileup_particles<<endl;

    for(int ipart=0; ipart<n_pileup_particles; ipart++){

      double m  = 0.0;
      double px = rand_pileup->Gaus(0,5.0);
      double py = rand_pileup->Gaus(0,5.0);
      double pz = rand_pileup->Gaus(0,5.0);
      double E  = pow( m*m + px*px + py*py + pz*pz , 0.5);

      if(debug) cout<<"Pileup: "<<ipart<<"  "<<px<<"  "<<py<<"  "<<pz<<"  "<<E<<endl;

      input_particles_Pileup.push_back(PseudoJet(px,py,pz,E));

    }


    //////////////////////////////////////////////
    //make pseudocalorimeter cells
    //////////////////////////////////////////////
    vector<PseudoJet> calo_cells        = ToyCalorimeter(input_particles);
    vector<PseudoJet> calo_cells_Pileup = ToyCalorimeter(input_particles_Pileup);


    //////////////////////////////////////////////
    // get the resulting jets ordered in pt
    //////////////////////////////////////////////
    fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, 1.0);

    fastjet::ClusterSequence clust_seq_TruthRaw(input_particles, jet_def);
    vector<fastjet::PseudoJet> inclusive_jets_TruthRaw = sorted_by_pt(clust_seq_TruthRaw.inclusive_jets(5.0));

    if(debug){
      // label the columns
      cout<<"jet#  pt  eta  phi  mass"<<endl;
      cout<<"Inclusive"<<endl;
      // print out the details for each jet
      for (unsigned int i = 0; i < inclusive_jets_TruthRaw.size(); i++) {
        cout<<i<<"  "<<inclusive_jets_TruthRaw[i].pt()
               <<"  "<<inclusive_jets_TruthRaw[i].eta()
               <<"  "<<inclusive_jets_TruthRaw[i].phi()
               <<"  "<<inclusive_jets_TruthRaw[i].m()<<endl;
      }
    }

    //check to make sure there are jets in the event
    if(inclusive_jets_TruthRaw.size()==0)
      continue;

    if(inclusive_jets_TruthRaw.size()>=1){
      j1_pt ->Fill(inclusive_jets_TruthRaw.at(0).pt());
      j1_eta->Fill(inclusive_jets_TruthRaw.at(0).eta());
      j1_phi->Fill(inclusive_jets_TruthRaw.at(0).phi());
      j1_m  ->Fill(inclusive_jets_TruthRaw.at(0).m());
    }

    //////////////////////////////////////////////
    //Setup tools for substructure calculation
    //////////////////////////////////////////////

    //N-subjettiness
    fastjet::contrib::UnnormalizedMeasure nsubMeasure(1.);
    fastjet::contrib::Nsubjettiness nsub1(1, fastjet::contrib::WTA_KT_Axes(), nsubMeasure);
    fastjet::contrib::Nsubjettiness nsub2(2, fastjet::contrib::WTA_KT_Axes(), nsubMeasure);

    // Filtering with a pt cut as for trimming (arXiv:0912.1342)
    Transformer *trimmer = new Filter(JetDefinition(kt_algorithm, 0.2), SelectorPtFractionMin(0.05) );
    const Transformer &f = *trimmer;


    /////////////////////////////
    //TruthRaw
    /////////////////////////////
    if(debug) cout<<"TruthRaw jet"<<endl;
    for(int ijet=0; ijet<inclusive_jets_TruthRaw.size(); ijet++){
      TLorentzVector jettemp;
      jettemp.SetPtEtaPhiM(inclusive_jets_TruthRaw.at(ijet).pt(),
                           inclusive_jets_TruthRaw.at(ijet).eta(),
                           inclusive_jets_TruthRaw.at(ijet).phi(),
                           inclusive_jets_TruthRaw.at(ijet).m());

      /////////////////////////////////
      //Fill variables that will go into ntuple
      /////////////////////////////////
      tempJet_pt             = jettemp.Pt();
      tempJet_eta            = jettemp.Eta();
      tempJet_phi            = jettemp.Phi();
      tempJet_m              = jettemp.M();

      TruthRaw_pt        .push_back(tempJet_pt);
      TruthRaw_eta       .push_back(tempJet_eta);
      TruthRaw_phi       .push_back(tempJet_phi);
      TruthRaw_m         .push_back(tempJet_m);
    }

    //////////////////////////////////////
    //Fill event into tree
    //////////////////////////////////////
    if(debug) cout<<"Filling Tree"<<endl;
    treeout->Fill();
  }


  /////////////////////////////////
  //Write the output TTree to the OutputFile
  /////////////////////////////////
  fileout->cd();

  treeout->Write();

  j1_pt ->Write();
  j1_eta->Write();
  j1_phi->Write();
  j1_m  ->Write();

  fileout->Close();

  return 0;

}



///=========================================
/// Reset Branches
///=========================================
void ResetBranches(){

  NumberOfVertices = 0;

  TruthRaw_pt.clear();
  TruthRaw_eta.clear();
  TruthRaw_phi.clear();
  TruthRaw_m.clear();

}


///=========================================
/// Calorimeter Simulation
///=========================================
vector<PseudoJet> ToyCalorimeter(vector<PseudoJet> truth_particles) {
  const double pi = 3.14159265359;
  const double etaLim = 5.0;
  const int nEta = 100;
  const int nPhi = 63;
  double dEta = 2*etaLim/nEta;
  double dPhi = 2*pi/nPhi;

  double tower[nEta][nPhi];
  for (int i = 0; i < nEta; i++)  for (int j = 0; j < nPhi; j++)  tower[i][j] = -0.001;

  vector<fastjet::PseudoJet> cell_particles;
  for (int p=0; p < (int)truth_particles.size(); p++) {
    fastjet::PseudoJet part = truth_particles.at(p);

    int etaCell = int((part.eta()+etaLim)/dEta);
    int phiCell = int(part.phi()/dPhi);
    if (etaCell >= 0 && etaCell < nEta && phiCell >=0 && phiCell < nPhi){
      tower[etaCell][phiCell] += part.e();
    }
  }

  for (int i = 0; i < nEta; i++)  for (int j = 0; j < nPhi; j++) {
    if (tower[i][j] > 0) {
      double etaLocal = -etaLim + dEta*(i+0.5);
      double phiLocal = dPhi*(j+0.5);
      double thetaLocal = 2*atan(exp(-etaLocal));
      cell_particles.push_back(fastjet::PseudoJet(sin(thetaLocal)*cos(phiLocal),sin(thetaLocal)*sin(phiLocal),cos(thetaLocal),1)*tower[i][j]);
    }
  }
  return cell_particles;
}

