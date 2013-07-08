#ifndef ARR_OBJECTS
#define ARR_OBJECTS

#ifndef BASE_OBJECTS
#include "BaseObjects.h"
#endif

#ifndef RSXML_OBJECTS
#include "RSXMLObjects.h"
#endif

namespace ARR {
	typedef Base::eMeta				eMeta;
	typedef Base::MetaFloat			MetaFloat;
	typedef Base::MetaString		MetaString;
	typedef Base::MetaUInt			MetaUInt;
	typedef Base::Tempo				Tempo;
	typedef Base::TimeSig			TimeSig;
	typedef	Base::eTuning			eTuning;
	typedef RSXML::Note::eValues	eNoteValues;
	typedef RSXML::Chord::eValues	eChordValues;
	typedef RSXML::Note				Note;
	
	class Chord : public Base::Chord {
		public:
			Chord(  const float& tim = 0.000f, 
				const std::array<unsigned int, 6>& nIn = Base::DEFAULTINDEX,
				const std::string& cN = "", const unsigned int& i = 0 ) 
				: Base::Chord( tim, nIn ), chordName( cN ), index( i ) 
				{ values.fill( false ); };
				
			std::string							chordName;
			unsigned int						index;
			std::array<bool,eChordValues::size>	values;
	};
};


#endif