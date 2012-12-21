// $Id$
//
//    File: DChargedTrackHypothesis_factory.cc
// Created: Tue Aug  9 14:29:24 EST 2011
// Creator: pmatt (on Linux ifarml6 2.6.18-128.el5 x86_64)
//


#include <iostream>
#include <iomanip>
using namespace std;

#include <TMath.h>

#include "DChargedTrackHypothesis_factory.h"
using namespace jana;

//------------------
// init
//------------------
jerror_t DChargedTrackHypothesis_factory::init(void)
{
	//this parameter controls what BCAL reconstruction algorithm to use
	//the same parameter is used in DNeutralShowerCandidate_factory
	USE_KLOE = 1;
	gPARMS->SetDefaultParameter("BCALRECON:USE_KLOE", USE_KLOE);

	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t DChargedTrackHypothesis_factory::brun(jana::JEventLoop *locEventLoop, int runnumber)
{
  // Get the particle ID algorithms
	vector<const DParticleID *> locPIDAlgorithms;
	locEventLoop->Get(locPIDAlgorithms);
	if(locPIDAlgorithms.size() < 1){
		_DBG_<<"Unable to get a DParticleID object! NO PID will be done!"<<endl;
		return RESOURCE_UNAVAILABLE;
	}
	// Drop the const qualifier from the DParticleID pointer (I'm surely going to hell for this!)
	dPIDAlgorithm = locPIDAlgorithms[0];
  
	// Warn user if something happened that caused us NOT to get a dPIDAlgorithm object pointer
	if(!dPIDAlgorithm){
		_DBG_<<"Unable to get a DParticleID object! NO PID will be done!"<<endl;
		return RESOURCE_UNAVAILABLE;
	}

	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t DChargedTrackHypothesis_factory::evnt(jana::JEventLoop* locEventLoop, int eventnumber)
{

 	vector<const DTrackTimeBased*> locTrackTimeBasedVector;
	locEventLoop->Get(locTrackTimeBasedVector);

	vector<DChargedTrackHypothesis*> locChargedTrackHypotheses;
	jerror_t locJError = Get_ChargedTrackHypotheses(locEventLoop, locTrackTimeBasedVector, locChargedTrackHypotheses);
	if(locJError != NOERROR)
		return locJError;

	for(size_t loc_i = 0; loc_i < locChargedTrackHypotheses.size(); ++loc_i)
		_data.push_back(locChargedTrackHypotheses[loc_i]);

	return NOERROR;
}

jerror_t DChargedTrackHypothesis_factory::Get_ChargedTrackHypotheses(JEventLoop* locEventLoop, vector<const DTrackTimeBased*>& locTrackTimeBasedVector, vector<DChargedTrackHypothesis*>& locChargedTrackHypotheses)
{
	const DTrackTimeBased *locTrackTimeBased;
	DChargedTrackHypothesis *locChargedTrackHypothesis;
	DMatrixDSym locCovarianceMatrix(7);

	unsigned int locTOFIndex, locSCIndex;
	double locTempProjectedTime = 0.0, locPathLength = 0.0, locFlightTime = 0.0;

	double locInitialStartTime;

	vector<const DEventRFBunch*> locEventRFBunches;
	vector<const DTOFPoint*> locTOFPoints;
	vector<const DBCALShower*> locBCALShowers;
	vector<const DFCALShower*> locFCALShowers;
	deque<const DBCALShower*> locMatchedBCALShowers;
	deque<const DFCALShower*> locMatchedFCALShowers;
	vector<const DSCHit*> locSCHits;
	locEventLoop->Get(locTOFPoints);
	locEventLoop->Get(locSCHits);
	if (USE_KLOE) {
		locEventLoop->Get(locBCALShowers, "KLOE");
	} else { 
		locEventLoop->Get(locBCALShowers);
	}
	locEventLoop->Get(locFCALShowers);
	locEventLoop->Get(locEventRFBunches);
	const DEventRFBunch* locEventRFBunch = (!locEventRFBunches.empty()) ? locEventRFBunches[0] : NULL;

	for(size_t loc_i = 0; loc_i < locTrackTimeBasedVector.size(); loc_i++)
	{
		locChargedTrackHypothesis = new DChargedTrackHypothesis();
		locTrackTimeBased = locTrackTimeBasedVector[loc_i];
		locChargedTrackHypothesis->AddAssociatedObject(locTrackTimeBased);
		locChargedTrackHypothesis->candidateid = locTrackTimeBased->candidateid;
		locChargedTrackHypothesis->dRT = locTrackTimeBased->rt;

		// Chi square and degree-of-freedom data from the track fit
		locChargedTrackHypothesis->dChiSq_Track=locTrackTimeBased->chisq;
		locChargedTrackHypothesis->dNDF_Track=locTrackTimeBased->Ndof;
		
		//Set DKinematicData Members
		DKinematicData *locKinematicData = locChargedTrackHypothesis;
		*locKinematicData = *(static_cast<const DKinematicData*>(locTrackTimeBased));
		locCovarianceMatrix = locTrackTimeBased->errorMatrix();

		//useful kinematic variable
		double betagamma=locTrackTimeBased->momentum().Mag()/locTrackTimeBased->mass();

		// Use time-based tracking time as initial guess
		locInitialStartTime = locChargedTrackHypothesis->t0(); // used to reject hits that are not in time with the track
		double locInitialStartTimeUncertainty = locChargedTrackHypothesis->t0_err();
		locChargedTrackHypothesis->setTime(locChargedTrackHypothesis->t0());
		locCovarianceMatrix(6, 6) = locInitialStartTimeUncertainty*locInitialStartTimeUncertainty;
		locChargedTrackHypothesis->setT0(locInitialStartTime, locInitialStartTimeUncertainty, SYS_CDC);
		locChargedTrackHypothesis->setT1(locInitialStartTime, locInitialStartTimeUncertainty, SYS_CDC);
		locChargedTrackHypothesis->setPathLength(NaN, 0.0); //zero uncertainty (for now)

		// Match to the start counter using the result of the time-based fit
		locTempProjectedTime = locInitialStartTime; // to reject hits that are not in time with the track
		pair<double,double>locTempSCdEdx;
		locChargedTrackHypothesis->dStartCounterdEdx=0.; 
		if (dPIDAlgorithm->MatchToSC(locTrackTimeBased->rt, DTrackFitter::kTimeBased, locSCHits, locTempProjectedTime, locSCIndex, locPathLength, locFlightTime,&locTempSCdEdx) == NOERROR)
		{
			locChargedTrackHypothesis->setT0(locTempProjectedTime, 0.3, SYS_START); //uncertainty guess for now
			locCovarianceMatrix(6, 6) = 0.3*0.3; // guess for now //will be overriden by other detector systems if hit match
			locChargedTrackHypothesis->setPathLength(locPathLength, 0.0); //zero uncertainty (for now) //will be overriden by other detector systems if hit match
			locChargedTrackHypothesis->dStartCounterdEdx=locTempSCdEdx.first;
			locChargedTrackHypothesis->dStartCounterdEdx_norm_residual=(locTempSCdEdx.second-2.204+1.893/betagamma)/1.858;
	
			locChargedTrackHypothesis->AddAssociatedObject(locSCHits[locSCIndex]);
		}

		// Try matching the track with hits in the outer detectors
		locTempProjectedTime = locInitialStartTime; // to reject hits that are not in time with the track
		if (dPIDAlgorithm->MatchToBCAL(locTrackTimeBased->rt, locBCALShowers, locMatchedBCALShowers, locTempProjectedTime, locPathLength, locFlightTime) == NOERROR)
		{
			for(unsigned int loc_j = 0; loc_j < locMatchedBCALShowers.size(); ++loc_j)
				locChargedTrackHypothesis->AddAssociatedObject(locMatchedBCALShowers[loc_j]);
			locChargedTrackHypothesis->setTime(locTempProjectedTime);
			locCovarianceMatrix(6, 6) = 0.00255*pow(locChargedTrackHypothesis->momentum().Mag(), -2.52) + 0.220;
			locCovarianceMatrix(6, 6) = locCovarianceMatrix(6, 6)*locCovarianceMatrix(6, 6);
			locChargedTrackHypothesis->setT1(locMatchedBCALShowers[0]->t, locMatchedBCALShowers[0]->tErr, SYS_BCAL);
			locChargedTrackHypothesis->setPathLength(locPathLength, 0.0); //zero uncertainty (for now)
		}
		locTempProjectedTime = locInitialStartTime; // to reject hits that are not in time with the track
		pair<double,double>locTOFdEdx;
		if (dPIDAlgorithm->MatchToTOF(locTrackTimeBased->rt, DTrackFitter::kTimeBased, locTOFPoints, locTempProjectedTime, locTOFIndex, locPathLength, locFlightTime,&locTOFdEdx) == NOERROR)
		{
		  locChargedTrackHypothesis->AddAssociatedObject(locTOFPoints[locTOFIndex]);
		  locChargedTrackHypothesis->dTOFdEdx=locTOFdEdx.first;
		  locChargedTrackHypothesis->dTOFdEdx_norm_residual
		    =(locTOFdEdx.second-2.462+1.488/betagamma)/1.273;
			if(locChargedTrackHypothesis->t1_detector() == SYS_CDC)
			{
				locChargedTrackHypothesis->setTime(locTempProjectedTime);
				locCovarianceMatrix(6, 6) = 0.08*0.08;
				locChargedTrackHypothesis->setT1(locTOFPoints[locTOFIndex]->t, 0.08, SYS_TOF);
				locChargedTrackHypothesis->setPathLength(locPathLength, 0.0); //zero uncertainty (for now)
			}
		}
		locTempProjectedTime = locInitialStartTime; // to reject hits that are not in time with the track
		double locFCALdEdx=0.;
		if (dPIDAlgorithm->MatchToFCAL(locTrackTimeBased->rt, locFCALShowers, locMatchedFCALShowers, locTempProjectedTime, locPathLength, locFlightTime,&locFCALdEdx) == NOERROR)
		{
			for(unsigned int loc_j = 0; loc_j < locMatchedFCALShowers.size(); ++loc_j)
				locChargedTrackHypothesis->AddAssociatedObject(locMatchedFCALShowers[loc_j]);
			locChargedTrackHypothesis->dFCALdEdx=locFCALdEdx;
			if(locChargedTrackHypothesis->t1_detector() == SYS_CDC)
			{
				locChargedTrackHypothesis->setTime(locTempProjectedTime);
				locCovarianceMatrix(6, 6) = 0.6*0.6; // straight-line fit to high momentum data
				locChargedTrackHypothesis->setT1(locMatchedFCALShowers[0]->getTime(), 0.6, SYS_FCAL);
				locChargedTrackHypothesis->setPathLength(locPathLength, 0.0); //zero uncertainty (for now)
			}
		}
		locChargedTrackHypothesis->setErrorMatrix(locCovarianceMatrix);

		//Calculate PID ChiSq, NDF, FOM
		dPIDAlgorithm->Calc_ChargedPIDFOM(locChargedTrackHypothesis, locEventRFBunch);
		locChargedTrackHypotheses.push_back(locChargedTrackHypothesis);
	}

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t DChargedTrackHypothesis_factory::erun(void)
{
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t DChargedTrackHypothesis_factory::fini(void)
{
	return NOERROR;
}


