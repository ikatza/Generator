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

#include "Physics/DarkNeutrino/EventGen/DNuDISOutgoingDarkGenerator.h"
#include "Framework/EventGen/EVGThreadException.h"
#include "Framework/GHEP/GHepRecord.h"
#include "Framework/GHEP/GHepParticle.h"
#include "Framework/GHEP/GHepFlags.h"
#include "Framework/Messenger/Messenger.h"

using namespace genie;

//___________________________________________________________________________
DNuDISOutgoingDarkGenerator::DNuDISOutgoingDarkGenerator() :
OutgoingDarkGenerator("genie::DNuDISOutgoingDarkGenerator")
{

}
//___________________________________________________________________________
DNuDISOutgoingDarkGenerator::DNuDISOutgoingDarkGenerator(string config) :
OutgoingDarkGenerator("genie::DNuDISOutgoingDarkGenerator", config)
{

}
//___________________________________________________________________________
DNuDISOutgoingDarkGenerator::~DNuDISOutgoingDarkGenerator()
{

}
//___________________________________________________________________________
void DNuDISOutgoingDarkGenerator::ProcessEventRecord(GHepRecord * evrec) const
{
// This method generates the final state primary lepton in DIS events

  // no modification is required to the std implementation
  OutgoingDarkGenerator::ProcessEventRecord(evrec);

  if(evrec->FinalStatePrimaryLepton()->IsOffMassShell()) {
    LOG("LeptonicVertex", pERROR)
               << "*** Selected kinematics lead to off mass shell dark matter!";
     evrec->EventFlags()->SetBitNumber(kLeptoGenErr, true);
     genie::exceptions::EVGThreadException exception;
     exception.SetReason("E<m for final state dark matter");
     exception.SwitchOnFastForward();
     throw exception;
  }
}
//___________________________________________________________________________
