//___________________________________________________________________________________________
/*!

\program gvld_nu_xsec

\brief   Compares GENIE neutrino cross-sections with the world data.

         Syntax:
           gvld_nu_xsec
                [-d data_archive] [-g genie_inputs] [-e] [-c comparison_id]

         Options:

           [] Denotes an optional argument.

           -d Location of the neutrino cross-section data archive.
              By default, the program will look-up the one located in:
              $GENIE/data/validation/vA/xsec/integrated/

           -g An XML file with GENIE inputs. 
              They are files with calculated cross-sections and simulated event samples 
              used for decomposing the inclusive cross-section to exclusive cross-sections. 
              Multiple models can be included in the input file, each identified by a "name" 
              (all model predictions will be overlayed).
              If no GENIE input is specified, only the data collections will be displayed.
              For info on the XML file format see the GSimFiles class documentation.
              A script for preparing inputs for this benchmark test can be found in:
              $GENIE/src/scripts/production/batch/submit_neutrino_xsec_validation_mc_jobs.pl
             
           -e Include error-bands in the GENIE predictions (note that this is slow) 

           -c Use this option to generate only one of the data/MC comparisons defined below.              
              By default, all data/MC comparisons will be generated.
               
\author  Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
         STFC, Rutherford Appleton Laboratory

\created June 06, 2008 

\cpright Copyright (c) 2003-2011, GENIE Neutrino MC Generator Collaboration
         For the full text of the license visit http://copyright.genie-mc.org
         or see $GENIE/LICENSE
*/
//___________________________________________________________________________________________

#include <cstdlib>
#include <cassert>
#include <sstream>
#include <string>

#include <TSystem.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TTree.h>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>
#include <TPostScript.h>
#include <TH1D.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TPavesText.h>
#include <TText.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TChain.h>

#include "Messenger/Messenger.h"
#include "PDG/PDGCodes.h"
#include "Utils/CmdLnArgParser.h"
#include "Utils/GSimFiles.h"
#include "Utils/SystemUtils.h"
#include "Utils/Style.h"
#include "validation/NuXSec/NuXSecData.h"
#include "validation/NuXSec/NuXSecFunc.h"
#include "validation/NuXSec/NuXSecComparison.h"

using std::ostringstream;
using std::string;

using namespace genie;
using namespace genie::mc_vs_data;

//
// Constants
//

// default data archive
const char * kDefDataFile = "data/validation/vA/xsec/integrated/nuXSec.root";  

// number of comparisons
const int kNumOfComparisons = 32;

// specify how exactly to construct all comparisons
NuXSecComparison * kComparison[kNumOfComparisons] = 
{
  // nu_mu CC inclusive,
  new NuXSecComparison(
    "numuCC_all", 
    "#nu_{#mu} CC inclusive",
    "ANL_12FT,2;ANL_12FT,4;BEBC,0;BEBC,2;BEBC,5;BEBC,8;BNL_7FT,0;BNL_7FT,4;CCFR,2;CCFRR,0;CHARM,0;CHARM,4;FNAL_15FT,1;FNAL_15FT,2;Gargamelle,0;Gargamelle,10;Gargamelle,12;IHEP_ITEP,0;IHEP_ITEP,2;IHEP_JINR,0;SKAT,0;MINOS,0;SciBooNE,0",
     new CCIsoInclXSec(kPdgNuMu),
     0.1, 120.0, true, false, true
  ),
  // nu_mu_bar CC inclusive
  new NuXSecComparison(
    "numubarCC_all", 
    "#bar{#nu_{#mu}} CC inclusive",
    "BEBC,1;BEBC,3;BEBC,6;BEBC,7;BNL_7FT,1;CCFR,3;CHARM,1;CHARM,5;FNAL_15FT,4;FNAL_15FT,5;Gargamelle,1;Gargamelle,11;Gargamelle,13;IHEP_ITEP,1;IHEP_ITEP,3;IHEP_JINR,1;MINOS,1",
     new CCIsoInclXSec(kPdgAntiNuMu),
     0.1, 120.0, true, false, true
  ),
  // nu_mu CC inclusive, low-energy data only
  new NuXSecComparison(
    "numuCC_lowE", 
    "#nu_{#mu} CC inclusive, low-energy data only",
    "ANL_12FT,2;ANL_12FT,4;BEBC,0;BEBC,2;BEBC,5;BEBC,8;BNL_7FT,0;BNL_7FT,4;CCFR,2;CCFRR,0;CHARM,0;CHARM,4;FNAL_15FT,1;FNAL_15FT,2;Gargamelle,0;Gargamelle,10;Gargamelle,12;IHEP_ITEP,0;IHEP_ITEP,2;IHEP_JINR,0;SKAT,0;MINOS,0;SciBooNE,0",
     new CCIsoInclXSec(kPdgNuMu),
     0.1, 10.0, false, false, true
  ),
  // nu_mu_bar CC inclusive, low-energy data only
  new NuXSecComparison(
    "numubarCC_lowE", 
    "#bar{#nu_{#mu}} CC inclusive, low-energy data only",
    "BEBC,1;BEBC,3;BEBC,6;BEBC,7;BNL_7FT,1;CCFR,3;CHARM,1;CHARM,5;FNAL_15FT,4;FNAL_15FT,5;Gargamelle,1;Gargamelle,11;Gargamelle,13;IHEP_ITEP,1;IHEP_ITEP,3;IHEP_JINR,1;MINOS,1",
     new CCIsoInclXSec(kPdgAntiNuMu),
     0.1, 10.0,  false, false, true
  ),
  // nu_mu CC inclusive, high-energy data only
  new NuXSecComparison(
    "numuCC_highE", 
    "#nu_{#mu} CC inclusive, medium/high-energy data only",
    "ANL_12FT,2;ANL_12FT,4;BEBC,0;BEBC,2;BEBC,5;BEBC,8;BNL_7FT,0;BNL_7FT,4;CCFR,2;CCFRR,0;CHARM,0;CHARM,4;FNAL_15FT,1;FNAL_15FT,2;Gargamelle,0;Gargamelle,10;Gargamelle,12;IHEP_ITEP,0;IHEP_ITEP,2;IHEP_JINR,0;SKAT,0;MINOS,0",
     new CCIsoInclXSec(kPdgNuMu),
     10, 120.0, false, false, true
  ),
  // nu_mu_bar CC inclusive, high-energy data only
  new NuXSecComparison(
    "numubarCC_lowE", 
    "#bar{#nu_{#mu}} CC inclusive, medium/high-energy data only",
    "BEBC,1;BEBC,3;BEBC,6;BEBC,7;BNL_7FT,1;CCFR,3;CHARM,1;CHARM,5;FNAL_15FT,4;FNAL_15FT,5;Gargamelle,1;Gargamelle,11;Gargamelle,13;IHEP_ITEP,1;IHEP_ITEP,3;IHEP_JINR,1;MINOS,1",
     new CCIsoInclXSec(kPdgAntiNuMu),
     10, 120.0, false, false, true
  ),
  // nu_mu CC inclusive, MINOS data only
  new NuXSecComparison(
    "numuCC_minos", 
    "#nu_{#mu} CC inclusive, MINOS data only",
    "MINOS,0",
     new CCIsoInclXSec(kPdgNuMu),
     1.0, 60.0, false, false, true
  ),
  // nu_mu_bar CC inclusive, MINOS data only
  new NuXSecComparison(
    "numubarCC_minos", 
    "#bar{#nu_{#mu}} CC inclusive, MINOS data only",
    "MINOS,1",
     new CCIsoInclXSec(kPdgAntiNuMu),
     1.0, 60.0, false, false, true
  ),
  // nu_mu CC inclusive, SciBooNE data only
  new NuXSecComparison(
    "numuCC_sciboone", 
    "#nu_{#mu} CC inclusive, SciBooNE data only",
    "SciBooNE,0",
     new CCIsoInclXSec(kPdgNuMu),
     0.1, 6.0, false, false, false
  ),
  // nu_mu CC QE, all data
  new NuXSecComparison(
    "numuCCQE_all", 
    "#nu_{#mu} CCQE, all data",
    "ANL_12FT,1;ANL_12FT,3;BEBC,12;BNL_7FT,3;FNAL_15FT,3;Gargamelle,2;SERP_A1,0;SERP_A1,1;SKAT,8;NOMAD,2",
     new CCQEXSec(kPdgNuMu,kPdgTgtFreeN,kPdgNeutron),
     0.1, 30.0, true, false, false
  ),
  // nu_mu CC QE, deuterium data only
  new NuXSecComparison(
    "numuCCQE_deuterium", 
    "#nu_{#mu} CCQE, deuterium data",
    "ANL_12FT,1;ANL_12FT,3;BEBC,12;BNL_7FT,3;FNAL_15FT,3",
     new CCQEXSec(kPdgNuMu,kPdgTgtFreeN,kPdgNeutron),
     0.1, 30.0, true, false, false
  ),
  // nu_mu CC QE, data on heavier targets
  new NuXSecComparison(
    "numuCCQE_heavy_target", 
    "#nu_{#mu} CCQE, heavy target data",
    "Gargamelle,2;SERP_A1,0;SERP_A1,1;SKAT,8;NOMAD,2",
     new CCQEXSec(kPdgNuMu,kPdgTgtFreeN,kPdgNeutron),
     0.1, 30.0, true, false, false
  ),
  // nu_mu CC QE, NOMAD, nuclear cross-section
  new NuXSecComparison(
    "numuCCQE_nomad_nuclear", 
    "#nu_{#mu} CCQE, NOMAD, ^{12}C cross-section per neutron (no nuclear correction)",
    "NOMAD,0",
     new CCQEXSec(kPdgNuMu,kPdgTgtC12,kPdgNeutron,1./6.),
     2.0, 100.0, true, false, false
  ),
  // nu_mu CC QE, NOMAD, free-nucleon cross-section
  new NuXSecComparison(
    "numuCCQE_nomad_nucleon", 
    "#nu_{#mu} CCQE, NOMAD, free-nucleon cross-section (incl. Smith-Moniz correction)",
    "NOMAD,2",
     new CCQEXSec(kPdgNuMu,kPdgTgtFreeN,kPdgNeutron),
     2.0, 100.0, true, false, false
  ),
  // nu_mu_bar CC QE, all data
  new NuXSecComparison(
    "numubarCCQE_all", 
    "#bar{#nu_{#mu}} CCQE, all data",
    "BNL_7FT,2;Gargamelle,3;Gargamelle,5;SERP_A1,2;SKAT,9;NOMAD,3",
     new CCQEXSec(kPdgAntiNuMu,kPdgTgtFreeP,kPdgProton),
     0.1, 30.0, true, false, false
  ),
  // nu_mu_bar CC QE, deuterium data only
  new NuXSecComparison(
    "numubarCCQE_deuterium", 
    "#bar{#nu_{#mu}} CCQE, deuterium data",
    "BNL_7FT,2",
     new CCQEXSec(kPdgAntiNuMu,kPdgTgtFreeP,kPdgProton),
     0.1, 30.0, true, false, false
  ),
  // nu_mu_bar CC QE, data on heavier targets
  new NuXSecComparison(
    "numubarCCQE_heavy_target", 
    "#bar{#nu_{#mu}} CCQE, heavy target data",
    "Gargamelle,3;Gargamelle,5;SERP_A1,2;SKAT,9;NOMAD,3",
     new CCQEXSec(kPdgAntiNuMu,kPdgTgtFreeP,kPdgProton),
     0.1, 30.0, true, false, false
  ),
  // nu_mu_bar CC QE, NOMAD, nuclear cross-section
  new NuXSecComparison(
    "numubarCCQE_nomad_nuclear", 
    "#bar{#nu_{#mu}} CCQE, NOMAD, ^{12}C cross-section per proton (no nuclear correction)",
    "NOMAD,1",
    new CCQEXSec(kPdgAntiNuMu,kPdgTgtC12,kPdgProton,1./6.),
     2.0, 100.0, true, false, false
  ),
  // nu_mu_bar CC QE, NOMAD, free-nucleon cross-section
  new NuXSecComparison(
    "numubarCCQE_nomad_nucleon", 
    "#bar{#nu_{#mu}} CCQE, NOMAD, free-nucleon cross-section (incl. Smith-Moniz correction)",
    "NOMAD,3",
     new CCQEXSec(kPdgAntiNuMu,kPdgTgtFreeP,kPdgProton),
     2.0, 100.0, true, false, false
  ),
  // nu_mu + p -> mu- + p + pi+ 
  new NuXSecComparison(
    "numuCCppip", 
    "#nu_{#mu} CC1#pi^{+} (#nu_{#mu} p #rightarrow #mu^{-} p #pi^{+})",
    "ANL_12FT,0;ANL_12FT,5;ANL_12FT,8;BEBC,4;BEBC,9;BEBC,13;BNL_7FT,5;FNAL_15FT,0;Gargamelle,4;SKAT,4;SKAT,5",
     new CCPionXSec(kPdgNuMu,kPdgTgtFreeP,kPdgProton,1,0,0,1,0),
     0.1, 30.0, true, false, false
  ),
  // nu_mu + n -> mu- + n + pi+
  new NuXSecComparison(
    "numuCCnpip", 
    "#nu_{#mu} CC1#pi^{+} (#nu_{#mu} n #rightarrow #mu^{-} n #pi^{+})",
    "ANL_12FT,7;ANL_12FT,10;BNL_7FT,7;SKAT,7",
     new CCPionXSec(kPdgNuMu,kPdgTgtFreeN,kPdgNeutron,1,0,0,0,1),
     0.1, 30.0, true, false, false
  ),
  // nu_mu + n -> mu- + p + pi0
  new NuXSecComparison(
    "numuCCppi0", 
    "#nu_{#mu} CC1#pi^{0} (#nu_{#mu} n #rightarrow #mu^{-} p #pi^{0})",
    "ANL_12FT,6;ANL_12FT,9;BNL_7FT,6;SKAT,6",
     new CCPionXSec(kPdgNuMu,kPdgTgtFreeN,kPdgNeutron,0,1,0,1,0),
     0.1, 30.0, true, false, false
  ),
  // nu_mu + p -> mu- + n + pi+ + pi+
  new NuXSecComparison(
    "numuCCn2pip", 
    "#nu_{#mu} CC#pi^{+}#pi^{+} (#nu_{#mu} p #rightarrow #mu^{-} n #pi^{+} #pi^{+})",
    "ANL_12FT,13",
     new CCPionXSec(kPdgNuMu,kPdgTgtFreeP,kPdgProton,2,0,0,0,1),
     1.0, 120.0, true, false, false
  ),
  // nu_mu + p -> mu- + p + pi+ + pi0
  new NuXSecComparison(
    "numuCCppippi0", 
    "#nu_{#mu} CC#pi^{+}#pi^{0} (#nu_{#mu} p #rightarrow #mu^{-} p #pi^{+} #pi^{0})",
    "ANL_12FT,12",
     new CCPionXSec(kPdgNuMu,kPdgTgtFreeP,kPdgProton,1,1,0,1,0),
     1.0, 120.0, true, false, false
  ),
  // nu_mu + n -> mu- + p + pi+ + pi-
  new NuXSecComparison(
    "numuCCppippim", 
    "#nu_{#mu} CC#pi^{+}#pi^{-} (#nu_{#mu} n #rightarrow #mu^{-} p #pi^{+} #pi^{-})",
    "ANL_12FT,11;BNL_7FT,8",
     new CCPionXSec(kPdgNuMu,kPdgTgtFreeN,kPdgNeutron,1,0,1,1,0),
     1.0, 120.0, true, false, false
  ),
  // numu NC coherent pi, A = 20
  new NuXSecComparison(
    "numuNCcohpi0_Ne20",
    "#nu_{#mu} NC coherent #pi^{0} (^{20}Ne)", 
    "CHARM,2",
     new CohPionXSec(kPdgNuMu, 1000100200, kPdgPi0),
     1.0, 150.0, false, false, false
  ),
  // numu CC coherent pi, A = 20
  new NuXSecComparison(
    "numuCCcohpip_Ne20",
    "#nu_{#mu} CC coherent #pi^{+} (^{20}Ne)",
    "BEBC,11;CHARM,6;FNAL_15FT,8",
     new CohPionXSec(kPdgNuMu, 1000100200, kPdgPiP),
     1.0, 150.0, false, false, false
  ),
  // nu_mu_bar CC coherent pi, A = 20
  new NuXSecComparison(
    "numubarCCcohpim_Ne20",
    "#bar{#nu_{#mu}} CC coherent #pi^{-} (^{20}Ne)",
    "BEBC,10;CHARM,7;FNAL_15FT,7",
     new CohPionXSec(kPdgAntiNuMu, 1000100200, kPdgPiM),
     1.0, 150.0, false, false, false
  ),
  // numu NC coherent pi, A = 27
  new NuXSecComparison(
    "numuNCcohpi0_Al27",
    "#nu_{#mu} NC coherent #pi^{0} (^{27}Al)",
    "AachenPadova,0",
     new CohPionXSec(kPdgNuMu, 1000130270, kPdgPi0),
     1.0, 150.0, false, false, false
  ),
  // numu NC coherent pi, A = 30
  new NuXSecComparison(
    "numuNCcohpi0_Si30",
    "#nu_{#mu} NC coherent #pi^{0} (^{30}Si)",
    "Gargamelle,14;SKAT,3",
     new CohPionXSec(kPdgNuMu, 1000140300, kPdgPi0),
     1.0, 150.0, false, false, false
  ),
  // numu CC coherent pi, A = 30
  new NuXSecComparison(
    "numuCCcohpip_Si30",
    "#nu_{#mu} CC coherent #pi^{+} (^{30}Si)",
    "SKAT,1",
     new CohPionXSec(kPdgNuMu, 1000140300, kPdgPiP),
     1.0, 150.0, false, false, false
  ),
  // nu_mu_bar CC coherent pi, A = 30
  new NuXSecComparison(
    "numubarCCcohpim_Si30",
    "#bar{#nu_{#mu}} CC coherent #pi^{-} (^{30}Si)",
    "SKAT,2",
     new CohPionXSec(kPdgAntiNuMu, 1000140300, kPdgPiM),
     1.0, 150.0, false, false, false
  ) 
};

// styles
const int kNMaxModels = 3;
const int kModelLineStyle [kNMaxModels] = 
{ 
  kSolid, kDashed, kDotted
};

//
// globals
//

// command-line arguments
GSimFiles gOptGenieInputs;
string    gOptDataFilename  = "";     // -d
string    gOptGenieFileList = "";     // -g
bool      gOptShowErrBands  = false;  // -e
string    gOptComparison    = "";     // -c, if unset will generate all data/MC comparisons

NuXSecData    gNuXSecData;
TPostScript * gOutPS           = 0;
TFile *       gOutRF           = 0;
TCanvas *     gC               = 0;
TLegend *     gLS              = 0;
bool          gShowModel       = false;

//
// function prototypes
//

void     Init               (void);
void     End                (void);
void     Draw               (int icomparison);
TH1F *   DrawFrame          (TGraph * gr0, TGraph * gr1);
TH1F *   DrawFrame          (double xmin, double xmax, double ymin, double yman);
void     GetCommandLineArgs (int argc, char ** argv);
void     PrintSyntax        (void);

//_________________________________________________________________________________
int main(int argc, char ** argv)
{
  GetCommandLineArgs (argc,argv);

  Init();

  bool show_all_comparisons = (gOptComparison.size()==0);

  // loop over specified comparisons and generate plots
  for(int i = 0; i < kNumOfComparisons; i++) 
  {
    if(!show_all_comparisons) {
      if(gOptComparison != kComparison[i]->ID()) continue;
    }
    LOG("gvldtest", pNOTICE) 
      << "** Performing data/MC comparison: " << kComparison[i]->Label();
    Draw(i);
  }

  End();

  LOG("gvldtest", pNOTICE)  << "Done!";
  return 0;
}
//_________________________________________________________________________________
void Init(void)
{
  LOG("gvldtest", pNOTICE) << "Initializing...";

  // Set GENIE style
  utils::style::SetDefaultStyle();

  // Open data archive
  bool ok = gNuXSecData.OpenArchive(gOptDataFilename);
  if(!ok) {
      LOG("gvldtest", pFATAL) 
         << "Could not open the neutrino cross-section data archive: "
         << gOptDataFilename;
      gAbortingInErr = true;
      exit(1);
  }

  // Read GENIE inputs
  if(gShowModel) {
     ok = gOptGenieInputs.LoadFromFile(gOptGenieFileList);
     if(!ok) {
        LOG("gvldtest", pFATAL) 
         << "Could not read GENIE inputs specified in XML file: " 
         << gOptGenieFileList;
        gAbortingInErr = true;
        exit(1);
     }

     // init functors
     for(int icomparison = 0; icomparison < kNumOfComparisons; icomparison++) {
        NuXSecFunc * xsec_func = kComparison[icomparison]->XSecFunc();
        xsec_func->Init(&gOptGenieInputs);
     }
  }

  // Create plot canvas
  gC = new TCanvas("c","",20,20,500,650);
  gC->SetBorderMode(0);
  gC->SetFillColor(0);
  gC->SetGridx();
  gC->SetGridy();

  gLS = new TLegend(0.15,0.92,0.85,0.98);
  gLS -> SetFillColor(0);
  gLS -> SetBorderSize(0);

  // Get local time to tag outputs
  string lt_for_filename   = utils::system::LocalTimeAsString("%02d.%02d.%02d_%02d.%02d.%02d"); 
  string lt_for_cover_page = utils::system::LocalTimeAsString("%02d/%02d/%02d %02d:%02d:%02d"); 

  // Create output ROOT file
  string root_filename  = Form("genie-world_nu_xsec_data_comp-%s.root",lt_for_filename.c_str());
  gOutRF = new TFile(root_filename.c_str(), "recreate");

  // Create output postscript file
  string ps_filename  = Form("genie-world_nu_xsec_data_comp-%s.ps",lt_for_filename.c_str());
  gOutPS = new TPostScript(ps_filename.c_str(), 111);

  // Add cover page
  gOutPS->NewPage();
  gC->Range(0,0,100,100);
  TPavesText hdr(10,40,90,70,3,"tr");
  hdr.AddText(" ");
  hdr.AddText("GENIE comparison with world neutrino cross-section data");
  hdr.AddText(" ");
  hdr.AddText(" ");
  hdr.AddText(" ");
  hdr.AddText("Models included in the comparison: ");
  for(int imodel=0; imodel< gOptGenieInputs.NModels(); imodel++) {
     hdr.AddText(Form("%d : %s", imodel, gOptGenieInputs.ModelTag(imodel).c_str()));
  }
  hdr.AddText(" ");
  hdr.AddText(" ");
  hdr.AddText(" ");
  hdr.AddText(lt_for_cover_page.c_str());
  hdr.AddText(" ");
  hdr.Draw();
  gC->Update();
}
//_________________________________________________________________________________
void End(void)
{
  LOG("gvldtest", pNOTICE) << "Cleaning up...";

  gOutPS->Close();
  gOutRF->Close();

  delete gC;
  delete gLS;
  delete gOutPS;
  delete gOutRF;
}
//_________________________________________________________________________________
void Draw(int icomparison)
{
  // info on current comparison
  string dataset_keys = kComparison[icomparison]->DataSetKeys();
  double emin         = kComparison[icomparison]->Emin();
  double emax         = kComparison[icomparison]->Emax();
  bool   scale        = kComparison[icomparison]->ScaleWithE();
  bool   inlogx       = kComparison[icomparison]->InLogX();
  bool   inlogy       = kComparison[icomparison]->InLogY();

  LOG("gvldtest", pNOTICE) 
      << "\n Dataset keys: " << dataset_keys 
      << "\n Energy range: [" << emin << ", " << emax << "] GeV";

  // get all measurements for the current channel
  vector<TGraphAsymmErrors *> data = gNuXSecData.Retrieve(dataset_keys,emin,emax,scale);

  // get the corresponding GENIE model prediction
  vector<TGraphAsymmErrors *> models;
  if(gShowModel) {
    for(int imodel=0; imodel< gOptGenieInputs.NModels(); imodel++) {
      // to avoid generating exceptionally busy plots, if I was asked to generate
      // error envelopes for the GENIE model and I was also asked to superimpose multiple
      // models, then I will show the error bands only for the first model
      bool show_err_band = (imodel==0) ? gOptShowErrBands : false;
      NuXSecFunc & xsec_func = *kComparison[icomparison]->XSecFunc();
      TGraphAsymmErrors * model = xsec_func.ExtractFromEventSample(
                 imodel, emin, emax, 30, inlogx, scale, show_err_band);
      model->SetTitle(gOptGenieInputs.ModelTag(imodel).c_str());
      int lsty = kModelLineStyle[imodel];     
      utils::style::Format(model,kBlack,lsty,2,1,1,1);
      if(show_err_band) {
         model->SetFillColor(6);
         model->SetFillStyle(3005);
      }
      models.push_back(model);
    }
  }

  // add a new page in the output ps file
  gOutPS->NewPage();
  gC->Clear();
  gC->Divide(2,1);
  gC->GetPad(1)->SetPad("mplots_pad","",0.01,0.25,0.99,0.99);
  gC->GetPad(2)->SetPad("legend_pad","",0.01,0.01,0.99,0.24);
  gC->GetPad(1)->SetFillColor(0);
  gC->GetPad(1)->SetBorderMode(0);
  gC->GetPad(2)->SetFillColor(0);
  gC->GetPad(2)->SetBorderMode(0);
  gC->GetPad(1)->cd();
  gC->GetPad(1)->SetBorderMode(0);
  gC->GetPad(1)->SetLogx(inlogx);
  gC->GetPad(1)->SetLogy(inlogy);

  // set header
  gLS->SetHeader( kComparison[icomparison]->Label().c_str() );

  // create frame from the data point range
  TH1F * hframe = 0;
  double xmin =  9999999;
  double xmax = -9999999;
  double ymin =  9999999;
  double ymax = -9999999;
  for(unsigned int i = 0; i< data.size(); i++) {
    if(!data[i]) continue;
    xmin  = TMath::Min(xmin, (data[i]->GetX())[TMath::LocMin(data[i]->GetN(),data[i]->GetX())]);
    xmax  = TMath::Max(xmax, (data[i]->GetX())[TMath::LocMax(data[i]->GetN(),data[i]->GetX())]);
    ymin  = TMath::Min(ymin, (data[i]->GetY())[TMath::LocMin(data[i]->GetN(),data[i]->GetY())]);
    ymax  = TMath::Max(ymax, (data[i]->GetY())[TMath::LocMax(data[i]->GetN(),data[i]->GetY())] );
  }
  for(unsigned int imodel = 0; imodel < models.size(); imodel++) {
    TGraph * mm = models[imodel];
    if(!mm) continue;
    for(int k=0; k<mm->GetN(); k++) {
         double x = (mm->GetX())[k];
         if(x < xmin || x > xmax) continue;
         ymin = TMath::Min(ymin, (mm->GetY())[k]);
         ymax = TMath::Max(ymax, (mm->GetY())[k]);
     }
  }
  double ymax_scale = (inlogy) ? 2. : 1.4;
  hframe = (TH1F*) gC->GetPad(1)->DrawFrame(0.5*xmin, 0.4*ymin, 1.2*xmax, ymax_scale*ymax);
  hframe->GetXaxis()->SetTitle("E_{#nu} (GeV)");
  if(scale) {
    hframe->GetYaxis()->SetTitle("#sigma_{#nu}/E_{#nu} (1E-38 cm^{2}/GeV^{2})");
  } else {
    hframe->GetYaxis()->SetTitle("#sigma_{#nu} (1E-38 cm^{2}/GeV)");
  }
  hframe->Draw();
  
  // add legend
  TLegend * legend = new TLegend(0.01, 0.01, 0.99, 0.99);
  legend->SetLineStyle(0);
  legend->SetFillColor(0);
  legend->SetTextSize(0.06);

  // draw current data set
  for(unsigned int i = 0; i< data.size(); i++) {
    if(!data[i]) continue;
    data[i]->Draw("P");
    legend->AddEntry(data[i], data[i]->GetTitle(), "LP");
  }  

  // have model prediction to plot?
  for(unsigned int imodel = 0; imodel < models.size(); imodel++) {
    if(!models[imodel]) continue;
    bool show_err_band = (imodel==0) ? gOptShowErrBands : false;
    if(show_err_band) {
       models[imodel]->Draw("c4");
       legend->AddEntry(models[imodel], models[imodel]->GetTitle(), "LF");
    } else {
       models[imodel]->Draw("c");
       legend->AddEntry(models[imodel], models[imodel]->GetTitle(), "L");
    }
  }

  gLS->Draw();

  gC->GetPad(2)->cd();
  legend->Draw();

  gC->GetPad(2)->Update();
  gC->Update();

 //gC->SaveAs(Form("gxs%d.eps",iset));

  // Save to output ROOT file
  gOutRF->cd();
  gC->Write(Form("canvas_%d",icomparison));
  for(unsigned int i = 0; i< data.size(); i++) {
    if(!data[i]) continue;
    data[i]->Write(Form("data_%d_%d",icomparison,i));
  }
  for(unsigned int imodel = 0; imodel < models.size(); imodel++) {
    if(!models[imodel]) continue;
    models[imodel]->Write(Form("model_%d_%d",icomparison,imodel));
  }
}
//_________________________________________________________________________________
// Formatting
//.................................................................................
TH1F* DrawFrame(TGraph * gr0, TGraph * gr1)
{
  double xmin = 1E-5;
  double xmax = 1;
  double ymin = 1E-5;
  double ymax = 1;

  if(gr0) {  
    TAxis * x0 = gr0 -> GetXaxis();
    TAxis * y0 = gr0 -> GetYaxis();
    xmin = x0 -> GetXmin();
    xmax = x0 -> GetXmax();
    ymin = y0 -> GetXmin();
    ymax = y0 -> GetXmax();
  }
  if(gr1) {
     TAxis * x1 = gr1 -> GetXaxis();
     TAxis * y1 = gr1 -> GetYaxis();
     xmin = TMath::Min(xmin, x1 -> GetXmin());
     xmax = TMath::Max(xmax, x1 -> GetXmax());
     ymin = TMath::Min(ymin, y1 -> GetXmin());
     ymax = TMath::Max(ymax, y1 -> GetXmax());
  }
  xmin *= 0.5;
  xmax *= 1.5;
  ymin *= 0.5;
  ymax *= 1.5;
  xmin = TMath::Max(0.1, xmin);
  
  return DrawFrame(xmin, xmax, ymin, ymax);
}
//_________________________________________________________________________________
TH1F* DrawFrame(double xmin, double xmax, double ymin, double ymax)
{
  TH1F * hf = (TH1F*) gC->DrawFrame(xmin, ymin, xmax, ymax);
  hf->GetXaxis()->SetTitle("E (GeV)");
  hf->GetYaxis()->SetTitle("#sigma (10^{-38} cm^{2})");
  hf->GetYaxis()->SetTitleSize(0.03);
  hf->GetYaxis()->SetTitleOffset(1.3);
  hf->GetXaxis()->SetLabelSize(0.03);
  hf->GetYaxis()->SetLabelSize(0.03);
  return hf;
}
//_________________________________________________________________________________
void GetCommandLineArgs(int argc, char ** argv)
{
  LOG("gvldtest", pNOTICE) << "*** Parsing command line arguments";

  CmdLnArgParser parser(argc,argv);

  // get data archive
  if(parser.OptionExists('d')){
     string filename = parser.ArgAsString('d');
     gOptDataFilename = filename;
  } else {
     if(gSystem->Getenv("GENIE")) {
        string base_dir = string( gSystem->Getenv("GENIE") );
        string filename = base_dir + "/" + kDefDataFile;
        gOptDataFilename = filename;
     } else { 
        LOG("gvldtest", pFATAL) 
          << "\n Please make sure that $GENIE is defined, or use the -d option"
          << "\n You didn't specify a data file and I can not pick the default one either";
        gAbortingInErr = true;
        exit(1);
     }
  }

  // get GENIE inputs
  gShowModel = false;
  if( parser.OptionExists('g') ) {
     gOptGenieFileList = parser.ArgAsString('g');
     gShowModel = true;
  }

  // show err envelopes?
  gOptShowErrBands = parser.OptionExists('e');

  // want some data/MC comparison in particular?
  if(parser.OptionExists('c')){
     gOptComparison = parser.ArgAsString('c');
  }

}
//_________________________________________________________________________________
void PrintSyntax(void)
{
  LOG("gvldtest", pNOTICE)
    << "\n\n" << "Syntax:" << "\n"
    << "   gvld_nu_xsec [-g genie_inputs] [-d data_archive]\n";
}
//_________________________________________________________________________________

