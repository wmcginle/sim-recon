// $Id$
//
//    File: DSCDigiHit.h
// Created: Tue Aug  6 12:53:36 EDT 2013
// Creator: davidl (on Darwin harriet.jlab.org 11.4.2 i386)
//

#ifndef _DSCDigiHit_
#define _DSCDigiHit_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>

class DSCDigiHit:public jana::JObject{
	public:
		JOBJECT_PUBLIC(DSCDigiHit);
		
		int sector;		// sector number 1-24
		uint32_t pulse_integral; ///< identified pulse integral as returned by FPGA algorithm
		uint32_t pulse_time;     ///< identified pulse time as returned by FPGA algorithm
		uint32_t pedestal;       ///< pedestal info used by FPGA (if any)
		uint32_t QF;             ///< Quality Factor from FPGA algorithms
		
		// This method is used primarily for pretty printing
		// the second argument to AddString is printf style format
		void toStrings(vector<pair<string,string> > &items)const{
			AddString(items, "sector", "%d", sector);
			AddString(items, "pulse_integral", "%d", pulse_integral);
			AddString(items, "pulse_time", "%d", pulse_time);
			AddString(items, "pedestal", "%d", pedestal);
			AddString(items, "QF", "%d", QF);
		}
		
};

#endif // _DSCDigiHit_
