#ifndef RSXML_CREATE_GUITAR
#include "RSXMLCreateGuitar.h"
#endif

namespace RSXML {
	const std::vector<RSXML::Beat> CreateGuitar::CreateBeats( const ARR::Guitar& g ) {	
		std::vector<RSXML::Beat> beats;	
	
		auto tempos( g.GetTempos() );
		auto timeSigs( g.GetTimeSigs() );
		auto tempoIt = tempos.begin();
		auto timeSigIt = timeSigs.begin();
		
		int beat = 0; // Beat counter.
		int bar = 0; // Bar counter.
		float timer = tempoIt->GetTime();
		unsigned int numerator = timeSigIt->numerator;
		while( timer < g.GetDuration() ) {
			/* Debug corner!
			std::cout << "tCount: " << (tCount - tempos.begin()) 
			<< " tEnd: " << tempos.end() - tempos.begin() << " Timer: " 
			<< timer << " | Duration: " << g.GetDuration()
			<< " | Bar: " << bar << " | Tempo: " << tCount->tempo ENDLINE */
			if( tempoIt != (tempos.end() - 1) && timer >= (tempoIt + 1)->GetTime() ) 
				{ ++tempoIt; } 
			if( timeSigIt != (timeSigs.end() - 1) && timer >= (timeSigIt + 1)->GetTime() ) 
				{ ++timeSigIt; numerator = timeSigIt->numerator; beat = 0; }
			
			// Needs to implement TimeSigs (properly).
			unsigned int bBar = 0;
			if(beat % numerator == 0)
				{ bBar = bar; ++bar; beat = 0; }
			else { bBar = -1; }
			RSXML::Beat b( timer, bBar );
			beats.push_back(b);
			
			timer += tempoIt->Convert2Beat();
			++beat;
		}
		return beats;
	};
}