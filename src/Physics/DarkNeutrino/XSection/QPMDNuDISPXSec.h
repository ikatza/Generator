//____________________________________________________________________________
/*!

\class    genie::QPMDNuDISPXSec

\brief    Computes DNuDIS differential cross sections.
          Is a concrete implementation of the XSecAlgorithmI interface.

\ref      E.A.Paschos and J.Y.Yu, Phys.Rev.D 65.03300

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

#ifndef _DNuDIS_PARTON_MODEL_PARTIAL_XSEC_H_
#define _DNuDIS_PARTON_MODEL_PARTIAL_XSEC_H_

#include "Framework/EventGen/XSecAlgorithmI.h"
#include "Physics/DeepInelastic/XSection/DISStructureFunc.h"

namespace genie {

class DISStructureFuncModelI;
class HadronizationModelI;
class XSecIntegratorI;

class QPMDNuDISPXSec : public XSecAlgorithmI {

public:
  QPMDNuDISPXSec();
  QPMDNuDISPXSec(string config);
  virtual ~QPMDNuDISPXSec();

  // XSecAlgorithmI interface implementation
  double XSec            (const Interaction * i, KinePhaseSpace_t k) const;
  double Integral        (const Interaction * i) const;
  bool   ValidProcess    (const Interaction * i) const;

  // overload the Algorithm::Configure() methods to load private data
  // members from configuration options
  void Configure(const Registry & config);
  void Configure(string config);

private:
  void   LoadConfig                  (void);
  double DNuDISRESJoinSuppressionFactor (const Interaction * in) const;

  mutable DISStructureFunc fDISSF;
  bool                     fInInitPhase;

  const DISStructureFuncModelI * fDISSFModel;         ///< SF model
  const HadronizationModelI *    fHadronizationModel; ///< hadronic multip. model
  const XSecIntegratorI *        fXSecIntegrator;     ///< diff. xsec integrator

  const XSecAlgorithmI * fCharmProdModel;

  bool   fUsingDisResJoin;  ///< use a DNuDIS/RES joining scheme?
  bool   fUseCache;         ///< cache reduction factors used in joining scheme
  double fWcut;             ///< apply DNuDIS/RES joining scheme < Wcut
  double fScale;            ///< cross section scaling factor
  double fSin48w;           ///< sin^4(Weingberg angle)
  int    fVelMode;          ///< velcoity dependence for xsec
  double fMedMass;          ///< Mediator mass
  double fgzp;              ///< Coupling to the mediator Zprime
  double fQchiL;            ///< Left-handed DNu charge
  double fQchiR;            ///< Right-handed DNu charge
  double fQchiS;            ///< Scalar DNu charge
};

}       // genie namespace
#endif  // _DNuDIS_PARTON_MODEL_PARTIAL_XSEC_H_
