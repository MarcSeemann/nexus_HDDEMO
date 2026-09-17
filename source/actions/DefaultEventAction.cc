// ----------------------------------------------------------------------------
// nexus | DefaultEventAction.cc
//
// This is the default event action of the NEXT simulations. Only events with
// deposited energy larger than 0 are saved in the nexus output file.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "DefaultEventAction.h"
#include "Trajectory.h"
#include "PersistencyManager.h"
#include "IonizationSD.h"
#include "IonizationHit.h"
#include "SensorSD.h"
#include "SensorHit.h"
#include "FactoryBase.h"

#include <G4Event.hh>
#include <G4VVisManager.hh>
#include <G4Trajectory.hh>
#include <G4GenericMessenger.hh>
#include <G4HCofThisEvent.hh>
#include <G4SDManager.hh>
#include <G4HCtable.hh>
#include <globals.hh>


namespace nexus {

REGISTER_CLASS(DefaultEventAction, G4UserEventAction)

  DefaultEventAction::DefaultEventAction():
    G4UserEventAction(), nevt_(0), nupdate_(10), energy_min_(0.), energy_max_(DBL_MAX)
  {
    msg_ = new G4GenericMessenger(this, "/Actions/DefaultEventAction/");

    G4GenericMessenger::Command& thresh_cmd =
       msg_->DeclareProperty("min_energy", energy_min_,
                             "Minimum deposited energy to save the event to file.");
    thresh_cmd.SetParameterName("min_energy", true);
    thresh_cmd.SetUnitCategory("Energy");
    thresh_cmd.SetRange("min_energy>0.");

    G4GenericMessenger::Command& max_energy_cmd =
      msg_->DeclareProperty("max_energy", energy_max_,
                            "Maximum deposited energy to save the event to file.");
    max_energy_cmd.SetParameterName("max_energy", true);
    max_energy_cmd.SetUnitCategory("Energy");
    max_energy_cmd.SetRange("max_energy>0.");

    PersistencyManager* pm = dynamic_cast<PersistencyManager*>
      (G4VPersistencyManager::GetPersistencyManager());

    pm->SaveNumbOfInteractingEvents(true);
  }



  DefaultEventAction::~DefaultEventAction()
  {
  }



  void DefaultEventAction::BeginOfEventAction(const G4Event* /*event*/)
  {
    // Print out event number info
    if ((nevt_ % nupdate_) == 0) {
      G4cout << " >> Event no. " << nevt_  << G4endl;
      if (nevt_  == (10 * nupdate_)) nupdate_ *= 10;
    }
  }



  void DefaultEventAction::EndOfEventAction(const G4Event* event)
  {
    nevt_++;

    // Determine whether total energy deposit in ionization sensitive
    // detectors is above threshold
    if (energy_min_ >= 0.) {

      // Get the trajectories stored for this event and loop through them
      // to calculate the total energy deposit

      G4double edep = 0.;

      G4TrajectoryContainer* tc = event->GetTrajectoryContainer();
      if (tc) {
        for (unsigned int i = 0; i < tc->size(); ++i) {
          Trajectory* tr = dynamic_cast<Trajectory*>((*tc)[i]);
          if (tr == nullptr) {
            G4Exception("[DefaultEventAction]", "EndOfEventAction()", FatalException,
                        "DefaultTrackingAction is required when using DefaultEventAction");
          }
          edep += tr->GetEnergyDeposit();
        }
      }

      PersistencyManager* pm = dynamic_cast<PersistencyManager*>
        (G4VPersistencyManager::GetPersistencyManager());

      // Check whether this event has ionization hits in the gas or sensor hits.
      bool has_ionization_hits = false;
      bool has_sensor_hits = false;
      G4HCofThisEvent* hce = event->GetHCofThisEvent();
      if (hce) {
        G4SDManager* sdmgr = G4SDManager::GetSDMpointer();
        G4HCtable* hct = sdmgr->GetHCtable();
        for (auto i = 0; i < hct->entries(); ++i) {
          G4String hcname = hct->GetHCname(i);
          G4String sdname = hct->GetSDname(i);
          if (hcname == IonizationSD::GetCollectionUniqueName()) {
            int hcid = sdmgr->GetCollectionID(sdname + "/" + hcname);
            G4VHitsCollection* hits = hce->GetHC(hcid);
            IonizationHitsCollection* ihc = dynamic_cast<IonizationHitsCollection*>(hits);
            if (ihc && ihc->entries() > 0) {
              has_ionization_hits = true;
            }
          }
          if (hcname == SensorSD::GetCollectionUniqueName()) {
            int hcid = sdmgr->GetCollectionID(sdname + "/" + hcname);
            G4VHitsCollection* hits = hce->GetHC(hcid);
            // G4VHitsCollection doesn't expose entries() — cast to
            // SensorHitsCollection to access entries().
            SensorHitsCollection* shc = dynamic_cast<SensorHitsCollection*>(hits);
            if (shc && shc->entries() > 0) {
              has_sensor_hits = true;
              break;
            }
          }
        }
      }

      // Mark interacting if ionization edep, ionization hits, or sensor hits were detected.
      if (!event->IsAborted() && (edep > 0. || has_ionization_hits || has_sensor_hits)) {
        pm->InteractingEvent(true);
      } else {
        pm->InteractingEvent(false);
      }

      // Store the event if it passes the energy thresholds or if it has
      // ionization hits in the gas (or sensor hits, if present).
      if (!event->IsAborted() && ((edep > energy_min_ && edep < energy_max_) ||
                                  has_ionization_hits || has_sensor_hits)) {
        pm->StoreCurrentEvent(true);
      } else {
        pm->StoreCurrentEvent(false);
      }

    }
  }


} // end namespace nexus
