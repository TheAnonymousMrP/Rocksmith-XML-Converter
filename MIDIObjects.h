#ifndef MIDI_READ_OBJECTS
#define MIDI_READ_OBJECTS

#ifndef BASE_NOTE
#include "BaseNote.h"
#endif

#ifndef BASE_META
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

	float ConvertDelta2Time( const unsigned int& delta, const unsigned int& division, const float& tempo ); 
	
	unsigned int ConvertBytes2VLQ( const std::vector<unsigned char>& vlq ); 

	float ConvertBytes2Float( const std::vector<unsigned char>& b ); 
	
	float ConvertSMPTE2Time( const std::vector<unsigned char>& b, const int& division ); 
		
	std::string ConvertBytes2String( const std::vector<unsigned char>& b ); 

	typedef Base::eMeta 		eMeta;
	typedef Base::MetaFloat 	MetaFloat;
	typedef Base::MetaString 	MetaString;
	typedef Base::MetaUInt		MetaUInt;
	typedef Base::Tempo 		Tempo;
	typedef Base::TimeSig 		TimeSig;
};










#endif