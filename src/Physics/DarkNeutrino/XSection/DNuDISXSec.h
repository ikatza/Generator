//____________________________________________________________________________
/*!

\class    genie::DNuDISXSec

\brief    Computes the DNuDIS Cross Section. \n
          Is a concrete implementation of the XSecIntegratorI interface. \n

\author   Joshua Berger <jberger \at physics.wisc.edu>
          University of Wisconsin-Madison

          Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
          University of Liverpool & STFC Rutherford Appleton Lab

\created  September 4, 2017

\cpright  Copyright (c) 2003-2019, The GENIE Collaboration
          For the full text of the license visit http://copyright.genie-mc.org
          or see $GENIE/LICENSE
*/
//____________________________________________________________________________

#ifndef _DNuDIS_XSEC_H_
#define _DNuDIS_XSEC_H_

#include "Physics/XSectionIntegration/XSecIntegratorI.h"

namespace genie {

class DNuDISXSec : public XSecIntegratorI {

public:
  DNuDISXSec();
  DNuDISXSec(string config);
  virtual ~DNuDISXSec();

  //! XSecIntegratorI interface implementation
  double Integrate(const XSecAlgorithmI * model, const Interaction * i) const;

  //! Overload the Algorithm::Configure() methods to load private data
  //! members from configuration options
  void Configure(const Registry & config);
  void Configure(string config);

private:
  void   LoadConfig (void);

  void   CacheFreeNucleonXSec(const XSecAlgorithmI * model, const Interaction * in) const;
  string CacheBranchName     (const XSecAlgorithmI * model, const Interaction * in) const;

  double fVldEmin;
  double fVldEmax;
};

}       // genie namespace
#endif  // _DNuDIS_XSEC_H_
