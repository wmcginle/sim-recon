// $Id$
//
//    File: DReaction_factory_trackeff_missing.cc
// Created: Wed Feb 25 08:58:19 EST 2015
// Creator: pmatt (on Linux pmattdesktop.jlab.org 2.6.32-504.8.1.el6.x86_64 x86_64)
//

#include "DReaction_factory_trackeff_missing.h"
#include "DCustomAction_TrackingEfficiency.h"
#include "DCustomAction_CutExtraPi0.h"

//------------------
// init
//------------------
jerror_t DReaction_factory_trackeff_missing::init(void)
{
	// Make as many DReaction objects as desired
	DReactionStep* locReactionStep = NULL;
	DReaction* locReaction = new DReaction("TrackEff_MissingProton"); //needs to be a unique name for each DReaction object, CANNOT (!) be "Thrown"

	// DOCUMENTATION:
	// ANALYSIS library: https://halldweb1.jlab.org/wiki/index.php/GlueX_Analysis_Software
	// DReaction factory: https://halldweb1.jlab.org/wiki/index.php/Analysis_DReaction

	/**************************************************** TrackEff_MissingProton Reaction Steps ****************************************************/

	//Required: DReactionSteps to specify the channel and decay chain you want to study
		//Particles are of type Particle_t, an enum defined in sim-recon/src/libraries/include/particleType.h

	//Example: g, p -> pi+, pi-, (p)
	locReactionStep = new DReactionStep();
	locReactionStep->Set_InitialParticleID(Gamma);
	locReactionStep->Set_TargetParticleID(Proton);
	locReactionStep->Add_FinalParticleID(PiPlus);
	locReactionStep->Add_FinalParticleID(PiMinus);
	locReactionStep->Add_FinalParticleID(Proton, true); //true: proton missing
	locReaction->Add_ReactionStep(locReactionStep);
	dReactionStepPool.push_back(locReactionStep); //register so will be deleted later: prevent memory leak

	/**************************************************** TrackEff_MissingProton Control Settings ****************************************************/

	// Recommended: Type of kinematic fit to perform (default is d_NoFit)
		//fit types are of type DKinFitType, an enum defined in sim-recon/src/libraries/ANALYSIS/DKinFitResults.h
	locReaction->Set_KinFitType(d_P4AndVertexFit); //simultaneously constrain apply four-momentum conservation, invariant masses, and common-vertex constraints

	// Highly Recommended: When generating particle combinations, reject all photon candidates with a PID confidence level < 5.73303E-7 (+/- 5-sigma)
	// locReaction->Set_MinPhotonPIDFOM(5.73303E-7);

	// Highly Recommended: When generating particle combinations, reject all charged track candidates with a PID confidence level < 5.73303E-7 (+/- 5-sigma)
	// locReaction->Set_MinChargedPIDFOM(5.73303E-7);

	// Highly Recommended: When generating particle combinations, reject all beam photons that match to a different RF bunch (delta_t > 1.002 ns)
	locReaction->Set_MaxPhotonRFDeltaT(2.004); //beam bunches are every 4.008 ns, (2.004 should be minimum cut value)

	// Highly Recommended: Cut on number of extra "good" tracks. "Good" tracks are ones that survive the "PreSelect" (or user custom) factory.
		// Current (09/26/2014): "Good" tracks have a detector-hit match, and tracking FOM > 0.0027 (+/- 3 sigma). 
		// Important: Keep cut large: Can have many ghost and accidental tracks that look "good"
	locReaction->Set_MaxExtraGoodTracks(1);

	locReaction->Set_MaxNumBeamPhotonsInBunch(1); //not ideal: throws away a lot of signal

	/************************************************** TrackEff_MissingProton Pre-Combo Custom Cuts *************************************************/

	// Highly Recommended: Very loose DAnalysisAction cuts, applied just after creating the combination (before saving it)
	// Example: Missing mass squared of proton
	locReaction->Add_ComboPreSelectionAction(new DCutAction_MissingMassSquared(locReaction, false, -0.1, 2.56));

	/**************************************************** TrackEff_MissingProton Analysis Actions ****************************************************/

	// Recommended: Analysis actions automatically performed by the DAnalysisResults factories to histogram useful quantities.
		//These actions are executed sequentially, and are executed on each surviving (non-cut) particle combination 
		//Pre-defined actions can be found in ANALYSIS/DHistogramActions.h and ANALYSIS/DCutActions.h

	// Hist PID
	locReaction->Add_AnalysisAction(new DHistogramAction_PID(locReaction));

	// PID Cuts
	locReaction->Add_AnalysisAction(new DCutAction_TrackFCALShowerEOverP(locReaction, false, 0.5)); //false: measured data //value: cut e+/e- below this, tracks above this
	locReaction->Add_AnalysisAction(new DCutAction_PIDDeltaT(locReaction, false, 1.0, Unknown, SYS_TOF)); //false: measured data //Unknown: All PIDs //SYS_NULL: All systems
	locReaction->Add_AnalysisAction(new DCutAction_ProtonPiPlusdEdx(locReaction, 2.2, true)); //select p/pi+ above/below 2.2, //true/false: cut all/no proton candidates above p = 1 GeV/c

	// Kinematic Fit Results
	locReaction->Add_AnalysisAction(new DHistogramAction_KinFitResults(locReaction, 0.05)); //5% confidence level cut on pull histograms only

	// Kinematic Fit: Vertex-Only Fit
	locReaction->Add_AnalysisAction(new DCutAction_OneVertexKinFit(locReaction, 5.73303E-7)); //cut +/- 5 sigma

	// Track DOCA, etc.
	locReaction->Add_AnalysisAction(new DHistogramAction_TrackVertexComparison(locReaction));
//	locReaction->Add_AnalysisAction(new DCutAction_MaxTrackDOCA(locReaction, Gamma, 3.0));

	// Missing Mass Squared
	locReaction->Add_AnalysisAction(new DHistogramAction_MissingMassSquared(locReaction, false, 1064, -0.1, 2.56));
	locReaction->Add_AnalysisAction(new DCustomAction_CutExtraPi0(locReaction, 0.1, 0.16));
	locReaction->Add_AnalysisAction(new DHistogramAction_MissingMassSquared(locReaction, false, 1064, -0.1, 2.56, "PostPi0"));
	locReaction->Add_AnalysisAction(new DCutAction_MissingMassSquared(locReaction, false, 0.75, 0.95));

	// Kinematics
	locReaction->Add_AnalysisAction(new DHistogramAction_ParticleComboKinematics(locReaction, true)); //true: fill histograms with kinematic-fit particle data

	// Tracking Efficiency
	locReaction->Add_AnalysisAction(new DCustomAction_TrackingEfficiency(locReaction, true, 1)); //1: 1 vertex-z bin

	_data.push_back(locReaction); //Register the DReaction with the factory

	/**************************************************** TrackEff_MissingPiMinus Reaction Steps ****************************************************/

	locReaction = new DReaction("TrackEff_MissingPiMinus"); //needs to be a unique name for each DReaction object, CANNOT (!) be "Thrown"

	//Required: DReactionSteps to specify the channel and decay chain you want to study
		//Particles are of type Particle_t, an enum defined in sim-recon/src/libraries/include/particleType.h

	//Example: g, p -> pi+, p, (pi-)
	locReactionStep = new DReactionStep();
	locReactionStep->Set_InitialParticleID(Gamma);
	locReactionStep->Set_TargetParticleID(Proton);
	locReactionStep->Add_FinalParticleID(PiPlus);
	locReactionStep->Add_FinalParticleID(Proton);
	locReactionStep->Add_FinalParticleID(PiMinus, true); //true: pi- missing
	locReaction->Add_ReactionStep(locReactionStep);
	dReactionStepPool.push_back(locReactionStep); //register so will be deleted later: prevent memory leak

	/**************************************************** TrackEff_MissingPiMinus Control Settings ****************************************************/

	// Recommended: Type of kinematic fit to perform (default is d_NoFit)
		//fit types are of type DKinFitType, an enum defined in sim-recon/src/libraries/ANALYSIS/DKinFitResults.h
	locReaction->Set_KinFitType(d_P4AndVertexFit); //simultaneously constrain apply four-momentum conservation, invariant masses, and common-vertex constraints

	// Highly Recommended: When generating particle combinations, reject all photon candidates with a PID confidence level < 5.73303E-7 (+/- 5-sigma)
	// locReaction->Set_MinPhotonPIDFOM(5.73303E-7);

	// Highly Recommended: When generating particle combinations, reject all charged track candidates with a PID confidence level < 5.73303E-7 (+/- 5-sigma)
	// locReaction->Set_MinChargedPIDFOM(5.73303E-7);

	// Highly Recommended: When generating particle combinations, reject all beam photons that match to a different RF bunch (delta_t > 1.002 ns)
	locReaction->Set_MaxPhotonRFDeltaT(2.004); //beam bunches are every 4.008 ns, (2.004 should be minimum cut value)

	// Highly Recommended: Cut on number of extra "good" tracks. "Good" tracks are ones that survive the "PreSelect" (or user custom) factory.
		// Current (09/26/2014): "Good" tracks have a detector-hit match, and tracking FOM > 0.0027 (+/- 3 sigma).
		// Important: Keep cut large: Can have many ghost and accidental tracks that look "good"
	locReaction->Set_MaxExtraGoodTracks(1);

	locReaction->Set_MaxNumBeamPhotonsInBunch(1); //not ideal: throws away a lot of signal

	/************************************************** TrackEff_MissingPiMinus Pre-Combo Custom Cuts *************************************************/

	// Highly Recommended: Very loose DAnalysisAction cuts, applied just after creating the combination (before saving it)
	// Example: Missing mass squared of proton
	locReaction->Add_ComboPreSelectionAction(new DCutAction_MissingMassSquared(locReaction, false, -0.4, 0.4));

	/**************************************************** TrackEff_MissingPiMinus Analysis Actions ****************************************************/

	// Recommended: Analysis actions automatically performed by the DAnalysisResults factories to histogram useful quantities.
		//These actions are executed sequentially, and are executed on each surviving (non-cut) particle combination
		//Pre-defined actions can be found in ANALYSIS/DHistogramActions.h and ANALYSIS/DCutActions.h

	// Hist PID
	locReaction->Add_AnalysisAction(new DHistogramAction_PID(locReaction));

	// PID Cuts
	locReaction->Add_AnalysisAction(new DCutAction_TrackFCALShowerEOverP(locReaction, false, 0.5)); //false: measured data //value: cut e+/e- below this, tracks above this
	locReaction->Add_AnalysisAction(new DCutAction_PIDDeltaT(locReaction, false, 1.0, Unknown, SYS_NULL)); //false: measured data //Unknown: All PIDs //SYS_NULL: All systems
	locReaction->Add_AnalysisAction(new DCutAction_ProtonPiPlusdEdx(locReaction, 2.2, true)); //select p/pi+ above/below 2.2, //true/false: cut all/no proton candidates above p = 1 GeV/c

	// Kinematic Fit Results
	locReaction->Add_AnalysisAction(new DHistogramAction_KinFitResults(locReaction, 0.05)); //5% confidence level cut on pull histograms only

	// Kinematic Fit: Vertex-Only Fit
	locReaction->Add_AnalysisAction(new DCutAction_OneVertexKinFit(locReaction, 5.73303E-7)); //cut +/- 5 sigma

	// Track DOCA, etc.
	locReaction->Add_AnalysisAction(new DHistogramAction_TrackVertexComparison(locReaction));
//	locReaction->Add_AnalysisAction(new DCutAction_MaxTrackDOCA(locReaction, Gamma, 3.0));

	// Missing Mass Squared
	locReaction->Add_AnalysisAction(new DHistogramAction_MissingMassSquared(locReaction, false, 800, -0.4, 0.4));
	locReaction->Add_AnalysisAction(new DCustomAction_CutExtraPi0(locReaction, 0.1, 0.16));
	locReaction->Add_AnalysisAction(new DHistogramAction_MissingMassSquared(locReaction, false, 800, -0.4, 0.4, "PostPi0"));
	locReaction->Add_AnalysisAction(new DCutAction_MissingMassSquared(locReaction, false, -0.03, 0.06));

	// Kinematics
	locReaction->Add_AnalysisAction(new DHistogramAction_ParticleComboKinematics(locReaction, true)); //true: fill histograms with kinematic-fit particle data

	// Tracking Efficiency
	locReaction->Add_AnalysisAction(new DCustomAction_TrackingEfficiency(locReaction, true, 1)); //1: 1 vertex-z bin

	_data.push_back(locReaction); //Register the DReaction with the factory


	/**************************************************** TrackEff_MissingPiPlus Reaction Steps ****************************************************/

	locReaction = new DReaction("TrackEff_MissingPiPlus"); //needs to be a unique name for each DReaction object, CANNOT (!) be "Thrown"

	//Required: DReactionSteps to specify the channel and decay chain you want to study
		//Particles are of type Particle_t, an enum defined in sim-recon/src/libraries/include/particleType.h

	//Example: g, p -> pi-, p, (pi+)
	locReactionStep = new DReactionStep();
	locReactionStep->Set_InitialParticleID(Gamma);
	locReactionStep->Set_TargetParticleID(Proton);
	locReactionStep->Add_FinalParticleID(PiMinus);
	locReactionStep->Add_FinalParticleID(Proton);
	locReactionStep->Add_FinalParticleID(PiPlus, true); //true: pi+ missing
	locReaction->Add_ReactionStep(locReactionStep);
	dReactionStepPool.push_back(locReactionStep); //register so will be deleted later: prevent memory leak

	/**************************************************** TrackEff_MissingPiPlus Control Settings ****************************************************/

	// Recommended: Type of kinematic fit to perform (default is d_NoFit)
		//fit types are of type DKinFitType, an enum defined in sim-recon/src/libraries/ANALYSIS/DKinFitResults.h
	locReaction->Set_KinFitType(d_P4AndVertexFit); //simultaneously constrain apply four-momentum conservation, invariant masses, and common-vertex constraints

	// Highly Recommended: When generating particle combinations, reject all photon candidates with a PID confidence level < 5.73303E-7 (+/- 5-sigma)
	// locReaction->Set_MinPhotonPIDFOM(5.73303E-7);

	// Highly Recommended: When generating particle combinations, reject all charged track candidates with a PID confidence level < 5.73303E-7 (+/- 5-sigma)
	// locReaction->Set_MinChargedPIDFOM(5.73303E-7);

	// Highly Recommended: When generating particle combinations, reject all beam photons that match to a different RF bunch (delta_t > 1.002 ns)
	locReaction->Set_MaxPhotonRFDeltaT(2.004); //beam bunches are every 4.008 ns, (2.004 should be minimum cut value)

	// Highly Recommended: Cut on number of extra "good" tracks. "Good" tracks are ones that survive the "PreSelect" (or user custom) factory.
		// Current (09/26/2014): "Good" tracks have a detector-hit match, and tracking FOM > 0.0027 (+/- 3 sigma).
		// Important: Keep cut large: Can have many ghost and accidental tracks that look "good"
	locReaction->Set_MaxExtraGoodTracks(1);

	locReaction->Set_MaxNumBeamPhotonsInBunch(1); //not ideal: throws away a lot of signal

	/************************************************** TrackEff_MissingPiPlus Pre-Combo Custom Cuts *************************************************/

	// Highly Recommended: Very loose DAnalysisAction cuts, applied just after creating the combination (before saving it)
	// Example: Missing mass squared of proton
	locReaction->Add_ComboPreSelectionAction(new DCutAction_MissingMassSquared(locReaction, false, -0.4, 0.4));

	/**************************************************** TrackEff_MissingPiPlus Analysis Actions ****************************************************/

	// Recommended: Analysis actions automatically performed by the DAnalysisResults factories to histogram useful quantities.
		//These actions are executed sequentially, and are executed on each surviving (non-cut) particle combination
		//Pre-defined actions can be found in ANALYSIS/DHistogramActions.h and ANALYSIS/DCutActions.h

	// Hist PID
	locReaction->Add_AnalysisAction(new DHistogramAction_PID(locReaction));

	// PID Cuts
	locReaction->Add_AnalysisAction(new DCutAction_TrackFCALShowerEOverP(locReaction, false, 0.5)); //false: measured data //value: cut e+/e- below this, tracks above this
	locReaction->Add_AnalysisAction(new DCutAction_PIDDeltaT(locReaction, false, 1.0, Unknown, SYS_NULL)); //false: measured data //Unknown: All PIDs //SYS_NULL: All systems
	locReaction->Add_AnalysisAction(new DCutAction_ProtonPiPlusdEdx(locReaction, 2.2, true)); //select p/pi+ above/below 2.2, //true/false: cut all/no proton candidates above p = 1 GeV/c

	// Kinematic Fit Results
	locReaction->Add_AnalysisAction(new DHistogramAction_KinFitResults(locReaction, 0.05)); //5% confidence level cut on pull histograms only

	// Kinematic Fit: Vertex-Only Fit
	locReaction->Add_AnalysisAction(new DCutAction_OneVertexKinFit(locReaction, 5.73303E-7)); //cut +/- 5 sigma

	// Track DOCA, etc.
	locReaction->Add_AnalysisAction(new DHistogramAction_TrackVertexComparison(locReaction));
//	locReaction->Add_AnalysisAction(new DCutAction_MaxTrackDOCA(locReaction, Gamma, 3.0));

	// Missing Mass Squared
	locReaction->Add_AnalysisAction(new DHistogramAction_MissingMassSquared(locReaction, false, 800, -0.4, 0.4));
	locReaction->Add_AnalysisAction(new DCustomAction_CutExtraPi0(locReaction, 0.1, 0.16));
	locReaction->Add_AnalysisAction(new DHistogramAction_MissingMassSquared(locReaction, false, 800, -0.4, 0.4, "PostPi0"));
	locReaction->Add_AnalysisAction(new DCutAction_MissingMassSquared(locReaction, false, -0.03, 0.06));

	// Kinematics
	locReaction->Add_AnalysisAction(new DHistogramAction_ParticleComboKinematics(locReaction, true)); //true: fill histograms with kinematic-fit particle data

	// Tracking Efficiency
	locReaction->Add_AnalysisAction(new DCustomAction_TrackingEfficiency(locReaction, true, 1)); //1: 1 vertex-z bin

	_data.push_back(locReaction); //Register the DReaction with the factory

	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t DReaction_factory_trackeff_missing::fini(void)
{
	for(size_t loc_i = 0; loc_i < dReactionStepPool.size(); ++loc_i)
		delete dReactionStepPool[loc_i]; //cleanup memory
	return NOERROR;
}

