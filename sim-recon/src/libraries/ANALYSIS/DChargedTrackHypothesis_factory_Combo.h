// $Id$
//
//    File: DChargedTrackHypothesis_factory_Combo.h
// Created: Tue Aug  9 14:29:24 EST 2011
// Creator: pmatt (on Linux ifarml6 2.6.18-128.el5 x86_64)
//

#ifndef _DChargedTrackHypothesis_factory_Combo_
#define _DChargedTrackHypothesis_factory_Combo_

#include "JANA/JFactory.h"
#include "TRACKING/DTrackTimeBased.h"
#include "PID/DChargedTrack.h"
#include "PID/DDetectorMatches.h"
#include "PID/DChargedTrackHypothesis.h"
#include "PID/DChargedTrackHypothesis_factory.h"
#include "ANALYSIS/DParticleComboBlueprint.h"

using namespace jana;
using namespace std;

class DChargedTrackHypothesis_factory_Combo : public jana::JFactory<DChargedTrackHypothesis>
{
	public:
		DChargedTrackHypothesis_factory_Combo(){};
		~DChargedTrackHypothesis_factory_Combo(){};
		const char* Tag(void){return "Combo";}

	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *locEventLoop, int runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *locEventLoop, int eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.

		DChargedTrackHypothesis_factory* dChargedTrackHypothesisFactory;
};

#endif // _DChargedTrackHypothesis_factory_Combo_
