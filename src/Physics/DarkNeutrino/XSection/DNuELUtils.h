#ifndef _DNuEL_UTILS_H_
#define _DNuEL_UTILS_H_

#include "Framework/Interaction/Interaction.h"
#include "Physics/NuclearState/NuclearModelI.h"
#include "Framework/EventGen/XSecAlgorithmI.h"

#include "TLorentzVector.h"
#include "TVector3.h"

#include <string>

namespace genie {

  // Enumerated type used to specify the method for determining the off-shell energy
  // of the hit nucleon for quasielastic events
  typedef enum EDNuELEvGenBindingMode {

    // Use removal energy from the nuclear model
    kUseNuclearModel,

    // Calculate binding energy assuming that the remnant nucleus is left in its
    // ground state
    kUseGroundStateRemnant,

    // Leave the struck nucleon on shell, effectively ignoring its binding energy
    kOnShell
  } DNuELEvGen_BindingMode_t;

  namespace utils {

    double EnergyDeltaFunctionSolutionDNuEL(const Interaction& inter);

    DNuELEvGen_BindingMode_t StringToDNuELBindingMode( const std::string& mode_str );

    double ComputeFullDNuELPXSec(Interaction* interaction,
      const NuclearModelI* nucl_model, const XSecAlgorithmI* xsec_model,
      double cos_theta_0, double phi_0, double& Eb,
      DNuELEvGen_BindingMode_t hitNucleonBindingMode, double min_angle_EM = 0.,
      bool bind_nucleon = true);

    double CosTheta0Max(const genie::Interaction& interaction);

    void BindHitNucleon(Interaction& interaction, const NuclearModelI& nucl_model,
      double& Eb, DNuELEvGen_BindingMode_t hitNucleonBindingMode);
  }
}

#endif
