//____________________________________________________________________________
/*
 Copyright (c) 2003-2019, The GENIE Collaboration
 For the full text of the license visit http://copyright.genie-mc.org
 or see $GENIE/LICENSE

 Author: Joshua Berger <jberger \at physics.wisc.edu>
         University of Wisconsin-Madison

         Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
         University of Liverpool & STFC Rutherford Appleton Lab 
*/
//____________________________________________________________________________

#include <TMath.h>
#include <Math/IFunction.h>
#include <Math/Integrator.h>

#include "Framework/Algorithm/AlgConfigPool.h"
#include "Framework/Algorithm/AlgFactory.h"
#include "Framework/Conventions/GBuild.h"
#include "Framework/Conventions/Constants.h"
#include "Framework/Conventions/Controls.h"
#include "Framework/Conventions/Units.h"
#include "Framework/Conventions/KineVar.h"
#include "Framework/Conventions/RefFrame.h"
#include "Physics/DarkNeutrino/XSection/DNuELXSec.h"

#include "Physics/NuclearState/NuclearModelI.h"
#include "Physics/XSectionIntegration/GSLXSecFunc.h"
#include "Framework/Messenger/Messenger.h"
#include "Framework/Numerical/RandomGen.h"
#include "Framework/ParticleData/PDGUtils.h"
#include "Framework/Utils/KineUtils.h"
#include "Physics/NuclearState/NuclearUtils.h"
#include "Framework/Utils/Range1.h"
#include "Framework/Numerical/GSLUtils.h"

using namespace genie;
using namespace genie::constants;
using namespace genie::controls;

//____________________________________________________________________________
DNuELXSec::DNuELXSec() :
XSecIntegratorI("genie::DNuELXSec")
{

}
//____________________________________________________________________________
DNuELXSec::DNuELXSec(string config) :
XSecIntegratorI("genie::DNuELXSec", config)
{

}
//____________________________________________________________________________
DNuELXSec::~DNuELXSec()
{

}
//____________________________________________________________________________
double DNuELXSec::Integrate(
                  const XSecAlgorithmI * model, const Interaction * in) const
{
  LOG("DNuELXSec",pDEBUG) << "Beginning integrate";
  if(! model->ValidProcess(in)) return 0.;

  const KPhaseSpace & kps = in->PhaseSpace();
  if(!kps.IsAboveThreshold()) {
     LOG("DNuELXSec", pDEBUG)  << "*** Below energy threshold";
     return 0;
  }
  Range1D_t rQ2 = kps.Limits(kKVQ2);
  
  if(rQ2.min<0 || rQ2.max<0) return 0;
  LOG("DNuELXSec", pDEBUG) 
          << "Q2 integration range = (" << rQ2.min << ", " << rQ2.max << ")";

  Interaction * interaction = new Interaction(*in);
  interaction->SetBit(kISkipProcessChk);
  interaction->SetBit(kISkipKinematicChk);

  ROOT::Math::IBaseFunctionOneDim * func = new 
      utils::gsl::dXSec_dQ2_E(model, interaction);
  ROOT::Math::IntegrationOneDim::Type ig_type = 
      utils::gsl::Integration1DimTypeFromString(fGSLIntgType);
  
  double abstol = 1; //We mostly care about relative tolerance
  ROOT::Math::Integrator ig(*func,ig_type,abstol,fGSLRelTol,fGSLMaxEval);
  double xsec = ig.Integral(rQ2.min, rQ2.max) * (1E-38 * units::cm2);
     
  //LOG("DNuELXSec", pDEBUG) << "XSec[DNuEL] (E = " << E << ") = " << xsec;

  delete func;
  delete interaction;

  return xsec;
}
//____________________________________________________________________________
void DNuELXSec::Configure(const Registry & config)
{
  Algorithm::Configure(config);
  this->LoadConfig();
}
//____________________________________________________________________________
void DNuELXSec::Configure(string config)
{
  Algorithm::Configure(config);
  this->LoadConfig();
}
//____________________________________________________________________________
void DNuELXSec::LoadConfig(void)
{
  // Get GSL integration type & relative tolerance
	GetParamDef( "gsl-integration-type", fGSLIntgType, string("adaptive"));
	GetParamDef( "gsl-relative-tolerance", fGSLRelTol, 0.01 ) ;
	int max;
	GetParamDef( "gsl-max-eval", max, 100000) ;
	fGSLMaxEval  = (unsigned int) max ;
}
//____________________________________________________________________________
