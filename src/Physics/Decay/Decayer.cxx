//____________________________________________________________________________
/*
 Copyright (c) 2003-2018, The GENIE Collaboration
 For the full text of the license visit http://copyright.genie-mc.org

 Author: Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
         University of Liverpool & STFC Rutherford Appleton Lab
*/
//____________________________________________________________________________

#include <algorithm>
#include <sstream>

#include <TParticlePDG.h>
#include <TDecayChannel.h>

#include "Framework/Algorithm/AlgConfigPool.h"
#include "Framework/Conventions/Constants.h"
#include "Framework/Conventions/Units.h"
#include "Framework/ParticleData/BaryonResUtils.h"
#include "Framework/ParticleData/PDGLibrary.h"
#include "Framework/ParticleData/PDGCodes.h"
#include "Framework/ParticleData/PDGUtils.h"
#include "Framework/GHEP/GHepStatus.h"
#include "Framework/GHEP/GHepParticle.h"
#include "Framework/GHEP/GHepRecord.h"
#include "Framework/Messenger/Messenger.h"
#include "Framework/Registry/Registry.h"
#include "Framework/Utils/StringUtils.h"
#include "Physics/Decay/Decayer.h"

using std::count;
using std::ostringstream;

using namespace genie;
using namespace genie::constants;

//___________________________________________________________________________
Decayer::Decayer() :
EventRecordVisitorI("genie::Decayer")
{

}
//___________________________________________________________________________
Decayer::Decayer(string config) :
EventRecordVisitorI("genie::Decayer", config)
{

}
//___________________________________________________________________________
Decayer::~Decayer()
{

}
//___________________________________________________________________________
bool Decayer::ToBeDecayed(GHepParticle * particle) const
{
   if(particle->Pdg() != 0) {
     bool check = false;
     GHepStatus_t ist = particle->Status();

     if(fRunBefHadroTransp) {
       check = (ist == kIStHadronInTheNucleus ||
                ist == kIStStableFinalState);
     } else {
       check = (ist == kIStStableFinalState);
     }
     if(check) { return this->IsUnstable(particle); }
   }
   return false;
}
//___________________________________________________________________________
bool Decayer::IsUnstable(GHepParticle * particle) const
{
  int pdg_code = particle->Pdg();

  // ROOT's TParticlepdg::Lifetime() does not work properly
  // do something else instead (temporarily)
  //
  // TParticlePDG * ppdg = PDGLibrary::Instance()->Find(pdg_code);
  //if( ppdg->Lifetime() < fMaxLifetime ) { /* ... */ };
  //

  // <temp/>
  if( fRunBefHadroTransp ) {
    //
    // Run *before* the hadron transport MC
    // At this point we decay only baryon resonances
    //
    bool decay = utils::res::IsBaryonResonance(pdg_code);
    return decay;
  }
  else {
    //
    // Run *after* the hadron transport MC
    // At this point we decay only particles in the fParticlesToDecay
    // PDGCodeList (filled in from config inputs)
    //
    bool decay = fParticlesToDecay.ExistsInPDGCodeList(pdg_code);
    return decay;
  }
  // </temp>

  return false;
}
//___________________________________________________________________________
void Decayer::Configure(const Registry & config)
{
  Algorithm::Configure(config);
  this->LoadConfig();

  fAllowReconfig = false;
}
//___________________________________________________________________________
void Decayer::Configure(string config)
{
  Algorithm::Configure(config);
  this->LoadConfig();

  fAllowReconfig = false;
}
//___________________________________________________________________________
void Decayer::LoadConfig(void)
{
  // Get the specified maximum lifetime tmax (decay with lifetime < tmax)
  //
  //fMaxLifetime = fConfig->GetDoubleDef("MaxLifetime", 1e-9);

  // Check whether the module is being run before or after the hadron
  // transport (intranuclear rescattering) module.
  //
  // If it is run before the hadron transport (and after the hadronization)
  // step it should decay only "unstable" particles (marked as hadrons in
  // the nucleus) which would typically decay within the time required to
  // exit the nucleus - so, the algorithm wouldn't decay particles that
  // have to be rescattered first. In case that the generated event is off
  // a free nucleon target, thi instance of the algorithm should do nothing.
  //
  // If it is run after the hadon transport, then it should decay all the
  // 'unstable' particles marked as 'present in the final state' and which
  // should be decay before the event is passed to the detector particle
  // transport MC.
  //
  this->GetParam("RunBeforeHadronTransport", fRunBefHadroTransp) ;

  // Allow user to specify a list of particles to be decayed
  //
  RgKeyList klist = GetConfig().FindKeys("DecayParticleWithCode=");
  RgKeyList::const_iterator kiter = klist.begin();
  for( ; kiter != klist.end(); ++kiter) {
    RgKey key = *kiter;
    bool decay = GetConfig().GetBool(key);
    vector<string> kv = utils::str::Split(key,"=");
    assert(kv.size()==2);
    int pdgc = atoi(kv[1].c_str());
    TParticlePDG * p = PDGLibrary::Instance()->Find(pdgc);
    if(decay) {
       LOG("ParticleDecayer", pDEBUG)
            << "Configured to decay " <<  p->GetName();
       fParticlesToDecay.push_back(pdgc);
       this->UnInhibitDecay(pdgc);
       // vector <const DecayModelI *>::iterator diter = fDecayers->begin();
       // for ( ; diter != fDecayers->end(); ++diter) {
       //     const DecayModelI * decayer = *diter;
       //     decayer->UnInhibitDecay(pdgc);
       // }// decayer
    }
    else {
       LOG("ParticleDecayer", pDEBUG)
            << "Configured to inhibit decays for  " <<  p->GetName();
       fParticlesNotToDecay.push_back(pdgc);
       this->InhibitDecay(pdgc);
       // vector <const DecayModelI *>::iterator diter = fDecayers->begin();
       // for ( ; diter != fDecayers->end(); ++diter) {
       //     const DecayModelI * decayer = *diter;
       //     decayer->InhibitDecay(pdgc);
       // }// decayer
    }// decay?
  }// key iterator

  // Allow user to inhibit certain decay channels
  //
  klist = GetConfig().FindKeys("InhibitDecay/");
  kiter = klist.begin();
  for( ; kiter != klist.end(); ++kiter) {
    RgKey key = *kiter;
    if(GetConfig().GetBool(key)) {
      string filtkey = utils::str::FilterString("InhibitDecay/", key);
      vector<string> kv = utils::str::Split(filtkey,",");
      assert(kv.size()==2);
      int pdgc = atoi(utils::str::FilterString("Particle=",kv[0]).c_str());
      int dc   = atoi(utils::str::FilterString("Channel=", kv[1]).c_str());
      TParticlePDG * p = PDGLibrary::Instance()->Find(pdgc);
      if(!p) continue;
      LOG("ParticleDecayer", pINFO)
         << "Configured to inhibit " <<  p->GetName()
         << "'s decay channel " << dc;
      this->InhibitDecay(pdgc, p->DecayChannel(dc));
      // vector <const DecayModelI *>::iterator diter = fDecayers->begin();
      // for ( ; diter != fDecayers->end(); ++diter) {
      //    const DecayModelI * decayer = *diter;
      //    decayer->InhibitDecay(pdgc, p->DecayChannel(dc));
      // }//decayer iterator
    }//val[key]=true?
  }//key iterator


  sort(fParticlesToDecay.begin(),    fParticlesToDecay.end());
  sort(fParticlesNotToDecay.begin(), fParticlesNotToDecay.end());

  // Print-out for only one of the two instances of this module
  if(!fRunBefHadroTransp) {
    LOG("ParticleDecayer", pNOTICE)
       << "\nConfigured to decay: " << fParticlesToDecay
       << "\nConfigured to inhibit decays of: " << fParticlesNotToDecay
       << "\n";
  }
}
//___________________________________________________________________________
