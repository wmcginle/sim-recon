// $Id$
//
//    File: Df250PulseRawData.h
// Created: Tue Aug  7 15:24:44 EDT 2012
// Creator: davidl (on Darwin harriet.jlab.org 11.4.0 i386)
//

#ifndef _Df250PulseRawData_
#define _Df250PulseRawData_

#include <JANA/jerror.h>
#include <JANA/JFactory.h>

class Df250PulseRawData:public jana::JObject{
	
	/// Holds pulse raw data for one identified
	/// pulse in one event in one channel of a single
	/// f250 Flash ADC module.
	
	public:
		JOBJECT_PUBLIC(Df250PulseRawData);
		
		Df250PulseRawData():rocid(0),slot(0),channel(0),pulse_number(0),first_sample_number(0),invalid_samples(false),overflow(false){}
		
		uint32_t rocid;                // from EVIO header (crate number)
		uint32_t slot;                 // from Block Header
		uint32_t channel;              // from Pulse Raw Data Data 1st word
		uint32_t pulse_number;         // from Pulse Raw Data Data 1st word
		uint32_t first_sample_number;  // from Pulse Raw Data Data 1st word
		vector<uint16_t> samples;      // from Pulse Raw Data Data words 2-N (each word contains 2 samples)
		bool invalid_samples;          // true if any sample's "not valid" bit set
		bool overflow;                 // true if any sample's "overflow" bit set
		
		// This method is used primarily for pretty printing
		// the second argument to AddString is printf style format
		void toStrings(vector<pair<string,string> > &items)const{
			AddString(items, "rocid", "%d", rocid);
			AddString(items, "slot", "%d", slot);
			AddString(items, "channel", "%d", channel);
			AddString(items, "pulse_number", "%d", pulse_number);
			AddString(items, "first_sample_number", "%d", first_sample_number);
			AddString(items, "Nsamples", "%d", samples.size());
			AddString(items, "invalid_samples", "%d", invalid_samples);
			AddString(items, "overflow", "%d", overflow);
		}
};

#endif // _Df250PulseRawData_
