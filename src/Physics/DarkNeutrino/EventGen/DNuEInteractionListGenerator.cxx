//____________________________________________________________________________
/*
 Copyright (c) 2003-2019, The GENIE Collaboration
 For the full text of the license visit http://copyright.genie-mc.org
 or see $GENIE/LICENSE

 Author: Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
         University of Liverpool & STFC Rutherford Appleton Lab 

 For the class documentation see the corresponding header file.

 Important revisions after version 2.0.0 :
 @ Feb 09, 2009 - CA
   Moved into the DNuE package from its previous location (EVGModules package)
 @ Feb 12, 2013 - CA (code from Rosen Matev)
   Handle the IMD annihilation channel.

*/
//____________________________________________________________________________

#include "Physics/DarkNeutrino/EventGen/DNuEInteractionListGenerator.h"
#include "Framework/EventGen/InteractionList.h"
#include "Framework/Interaction/Interaction.h"
#include "Framework/Messenger/Messenger.h"
#include "Framework/ParticleData/PDGCodes.h"
#include "Framework/ParticleData/PDGUtils.h"

using namespace genie;

//___________________________________________________________________________
DNuEInteractionListGenerator::DNuEInteractionListGenerator() :
InteractionListGeneratorI("genie::DNuEInteractionListGenerator")
{

}
//___________________________________________________________________________
DNuEInteractionListGenerator::DNuEInteractionListGenerator(string config) :
InteractionListGeneratorI("genie::DNuEInteractionListGenerator", config)
{

}
//___________________________________________________________________________
DNuEInteractionListGenerator::~DNuEInteractionListGenerator()
{

}
//___________________________________________________________________________
InteractionList * DNuEInteractionListGenerator::CreateInteractionList(
                                       const InitialState & init_state) const
{
  LOG("IntLst", pINFO) << "InitialState = " << init_state.AsString();

  return this -> DNuEELInteractionList (init_state);
}
//___________________________________________________________________________
InteractionList * DNuEInteractionListGenerator::DNuEELInteractionList(
                                       const InitialState & init_state) const
{
// channels:
// DNu       + e- -> DNu       + e-   
// DNubar    + e- -> DNubar   + e-   [CC + NC + interference]

  int nupdg = init_state.ProbePdg();
  InteractionList * intlist = new InteractionList;

  // clone init state and de-activate the struck nucleon info
  InitialState init(init_state);
  init_state.TgtPtr()->SetHitNucPdg(0);

  if(nupdg == kPdgDarkMatter  || nupdg == kPdgAntiDarkMatter) {
     ProcessInfo   proc_info(kScDarkMatterElectron,  kIntDarkMatter);
     Interaction * interaction = new Interaction(init, proc_info);
     intlist->push_back(interaction);
  }

  return intlist;
}
//___________________________________________________________________________
void DNuEInteractionListGenerator::Configure(const Registry & config)
{
  Algorithm::Configure(config);
  this->LoadConfig();
}
//____________________________________________________________________________
void DNuEInteractionListGenerator::Configure(string config)
{
  Algorithm::Configure(config);
  this->LoadConfig();
}
//____________________________________________________________________________
void DNuEInteractionListGenerator::LoadConfig(void)
{
  
}
//____________________________________________________________________________

