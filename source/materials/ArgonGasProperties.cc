// ----------------------------------------------------------------------------
// nexus | ArgonGasProperties.cc
//
// Relevant physical properties of gaseous argon.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "ArgonGasProperties.h"

#include <G4AnalyticalPolSolver.hh>
#include <G4MaterialPropertiesTable.hh>

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include <stdexcept>


namespace nexus {

  using namespace CLHEP;


  G4double ArgonDensity(G4double pressure)
  {
    //These values are for a temperature of 300 K
    // taken from http://www.nist.gov/srd/upload/jpcrd363.pdf
    G4double density = 1.60279*kg/m3;
    // some of the folowing 2 values are interpolated from the other ones, and may not be accurate
  if (pressure/bar > 0.9 && pressure/bar < 1.1)
      density = 1.60279*kg/m3;
  else if (pressure/bar > 1.9 && pressure/bar < 2.1)
      density = 3.20719*kg/m3;
  else if (pressure/bar > 4.9 && pressure/bar < 5.1)
      density = 8.032*kg/m3;
  else if (pressure/bar > 8.55 && pressure/bar < 8.65)
      density = 13.8495*kg/m3;
  else if (pressure/bar > 8.65 && pressure/bar < 8.75)
      density = 14.0111*kg/m3;
  else if (pressure/bar > 9.55 && pressure/bar < 9.65)
      density = 15.4654*kg/m3;
  else if (pressure/bar > 9.9 && pressure/bar < 10.1)
      density = 16.1118*kg/m3;
  else if (pressure/bar > 10.55 && pressure/bar < 10.65)
      density = 17.0868*kg/m3;
  else if (pressure/bar > 11.55 && pressure/bar < 11.65)
      density = 18.7118*kg/m3;
  else if (pressure/bar > 12.55 && pressure/bar < 12.65)
      density = 20.3369*kg/m3;
  else if (pressure/bar > 13.55 && pressure/bar < 13.65)
      density = 21.9619*kg/m3;
  else if (pressure/bar > 14.55 && pressure/bar < 14.65)
      density = 23.5870*kg/m3;
  else if (pressure/bar > 14.9 && pressure/bar < 15.1)
      density = 24.2369 *kg/m3;
  else if (pressure/bar > 15.55 && pressure/bar < 15.65)
      density = 25.2173*kg/m3;
  else if (pressure/bar > 15.9 && pressure/bar < 16.1)
      density = 25.8672*kg/m3;
  else if (pressure/bar > 19.9 && pressure/bar < 20.1)
      density = 32.4066*kg/m3;
  else if (pressure/bar > 29.9 && pressure/bar < 30.1)
      density = 48.8708*kg/m3;
  else if (pressure/bar > 39.9 && pressure/bar < 40.1)
      density = 65.494*kg/m3;
  else
      G4Exception("[ArgonProperties]", "ArgonDensity()", FatalException,
                  "Unknown argon density for this pressure!");

    return density;
  }


  G4double ArgonELLightYield(G4double field_strength, G4double pressure)
  {
    // Empirical formula taken from
    // C.M.B. Monteiro thesis https://estudogeral.sib.uc.pt/bitstream/10316/14635/3/Tese_Doutoramento_Cristina%20Monteiro.pdf.

    // Y/x = (a E/p + b) p,
    // where Y/x is the number of photons per unit lenght (cm),
    // E is the electric field strength, p is the pressure, and a and b
    // are empirically determined constants:
    const G4double a = 81. / kilovolt;
    const G4double b = 47. / (bar*cm);

    G4double yield = (a * field_strength/pressure - b) * pressure;
    if (yield < 0.) yield = 0.;

    return yield;
  }


} // end namespace nexus
