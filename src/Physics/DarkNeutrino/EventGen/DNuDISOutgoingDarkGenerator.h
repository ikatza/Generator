//____________________________________________________________________________
/*!

\class    genie::DNuDISOutgoingDarkGenerator

\brief    Generates the final state dark matter in DNu DIS interactions.
          Is a concrete implementation of the EventRecordVisitorI interface.

\author   Joshua Berger <jberger \at physics.wisc.edu
          University of Wisconsin-Madison
          Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
          University of Liverpool & STFC Rutherford Appleton Lab

\created  September 4, 2017

\cpright  Copyright (c) 2003-2019, The GENIE Collaboration
          For the full text of the license visit http://copyright.genie-mc.org
          or see $GENIE/LICENSE
*/
//____________________________________________________________________________

#ifndef _DNuDIS_OUTGOING_DARK_GENERATOR_H_
#define _DNuDIS_OUTGOING_DARK_GENERATOR_H_

#include "Physics/Common/OutgoingDarkGenerator.h"

namespace genie {

class DNuDISOutgoingDarkGenerator : public OutgoingDarkGenerator {

public :
  DNuDISOutgoingDarkGenerator();
  DNuDISOutgoingDarkGenerator(string config);
  ~DNuDISOutgoingDarkGenerator();

  // implement the EventRecordVisitorI interface
  void ProcessEventRecord(GHepRecord * event_rec) const;
};

}      // genie namespace
#endif // _DNuDIS_OUTGOING_DARK_GENERATOR_H_
