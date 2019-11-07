//____________________________________________________________________________
/*!

\class    genie::DNuDISInteractionListGenerator

\brief    Concrete implementations of the InteractionListGeneratorI interface.
          Generate a list of all the Interaction (= event summary) objects that
          can be generated by the DNuDIS EventGenerator.

\author   Joshua Berger <jberger \at physics.wisc.edu
          University of Wisconsin-Madison
          Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
          University of Liverpool & STFC Rutherford Appleton Lab

\created  September 1, 2017

\cpright  Copyright (c) 2003-2019, The GENIE Collaboration
          For the full text of the license visit http://copyright.genie-mc.org
          or see $GENIE/LICENSE
*/
//____________________________________________________________________________

#ifndef _DNuDIS_INTERACTION_LIST_GENERATOR_H_
#define _DNuDIS_INTERACTION_LIST_GENERATOR_H_

#include <map>

#include "Framework/EventGen/InteractionListGeneratorI.h"

using std::multimap;

namespace genie {

class Interaction;

class DNuDISInteractionListGenerator : public InteractionListGeneratorI {

public :
  DNuDISInteractionListGenerator();
  DNuDISInteractionListGenerator(string config);
 ~DNuDISInteractionListGenerator();

  // implement the InteractionListGeneratorI interface
  InteractionList * CreateInteractionList(const InitialState & init) const;

  // overload the Algorithm::Configure() methods to load private data
  // members from configuration options
  void Configure(const Registry & config);
  void Configure(string config);

private:

  void LoadConfigData(void);

  multimap<int,bool> GetHitQuarks(const Interaction * interaction) const;

  bool fIsDNu;
  bool fSetHitQuark;
  bool fIsCharm;
};

}      // genie namespace

#endif // _DNuDIS_INTERACTION_LIST_GENERATOR_H_
