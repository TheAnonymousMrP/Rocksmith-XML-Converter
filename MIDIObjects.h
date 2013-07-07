#ifndef _MIDI_READ_OBJECTS_
#define _MIDI_READ_OBJECTS_

#ifndef _BASE_NOTE
#include "BaseNote.h"
#endif

#ifndef _BASE_META
#include "BaseMeta.h"
#endif

#include <cmath>
#include <algorithm>
#include <string>
#include <vector>

namespace MIDI {
	class Note : public Base::GuitarNote { 
		public:
			Note( const float& tim = -1.0, const unsigned char& pit = 0x00, 
				const unsigned char& str = 0x00, const unsigned int& dif = 0 ) 
				: Base::GuitarNote( tim, pit, str ) { arbitraryDifficulty = dif; };
				
			const unsigned int&		GetArbitraryDifficulty() const 
										{ return arbitraryDifficulty; };
	
		private:
			unsigned int 			arbitraryDifficulty;
	};
	
	float ConvertDelta2Time( const int& delta, const int& division, const float& tempo ) 
		{
		float length = (float)delta / (float)division;
		return Base::ConvertTempo2Beat( tempo ) * length;
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
	
	float ConvertSMPTE2Time( const std::vector<unsigned char>& b, const int& division ) 
		{
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
	
	typedef Base::eMeta 		eMeta;
	typedef Base::MetaFloat 	MetaFloat;
	typedef Base::MetaString 	MetaString;
	typedef Base::MetaUInt		MetaUInt;
	typedef Base::Tempo 		Tempo;
	typedef Base::TimeSig 		TimeSig;
};










#endif