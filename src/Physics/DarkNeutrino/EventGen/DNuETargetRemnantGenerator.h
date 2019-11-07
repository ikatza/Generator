//____________________________________________________________________________
/*!

\class    genie::DNuETargetRemnantGenerator

\brief    Generates all the non-primary lepton final state particles in 
          neutrino-electron events.
          Is a concrete implementation of the EventRecordVisitorI interface.

\author   Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
          University of Liverpool & STFC Rutherford Appleton Lab

\created  July 17, 2005

\cpright  Copyright (c) 2003-2019, The GENIE Collaboration
          For the full text of the license visit http://copyright.genie-mc.org
          or see $GENIE/LICENSE
*/
//____________________________________________________________________________

#ifndef _DNuE_TARGET_REMNANT_GENERATOR_H_
#define _DNuE_TARGET_REMNANT_GENERATOR_H_

#include "Framework/EventGen/EventRecordVisitorI.h"

namespace genie {

class DNuETargetRemnantGenerator : public EventRecordVisitorI {

public :
  DNuETargetRemnantGenerator();
  DNuETargetRemnantGenerator(string config);
 ~DNuETargetRemnantGenerator();

  //-- implement the EventRecordVisitorI interface
  void ProcessEventRecord (GHepRecord * evrec) const;

private:
  void AddElectronNeutrino     (GHepRecord * evrec) const;
  void AddTargetNucleusRemnant (GHepRecord * evrec) const;
};

}      // genie namespace
#endif // _DNuE_TARGET_REMNANT_GENERATOR_H_
