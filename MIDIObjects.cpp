#ifndef MIDI_READ_OBJECTS
#include "MIDIObjects.h"
#endif

#ifndef DEBUG_STUFF
#include "debug.h"
#endif

namespace MIDI {
	float ConvertDelta2Time( const unsigned int& delta, const unsigned int& division, const float& tempo ) {
		float length = (float)delta / (float)division;
		return ( ONEMINUTE / tempo ) * length;
	}
	
	unsigned int ConvertBytes2VLQ( const std::vector<unsigned char>& vlq ) {
		unsigned int buffer = vlq.back();
		for(auto it = vlq.begin(); it != vlq.end() - 1; ++it) 
			{ buffer += (unsigned int)(*it - 0x80) * pow(128,(vlq.end() - 1 - it)); }
		return buffer;
	}
	
	float ConvertBytes2Float( const std::vector<unsigned char>& b ) {
		float buffer = 0;
		for(auto it = b.begin(); it != b.end(); ++it) 
			{ buffer += (int)*it * pow(256.0,(b.end() - 1 - it)); }
		return buffer;
	}
	
	float ConvertSMPTE2Time( const std::vector<unsigned char>& b, const int& division ) {
		float time = 0;
		// time += (b.at(0) * 60);
		time += b.at(1);
		time += ( b.at(2) / division );
		time += ( ( b.at(3) / 100 ) / division );
		return time;
	}
	
	std::string ConvertBytes2String( const std::vector<unsigned char>& b ) { 
		std::string s = "";
		for(auto& c : b) { s += c; }
		return s;
	}
};