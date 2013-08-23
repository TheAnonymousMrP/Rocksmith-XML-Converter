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
	typedef	Base::eTuning			eTuning;
	typedef RSXML::Note::eValues	eNoteTechniques;
	typedef RSXML::eChordTechniques	eChordTechniques;
	typedef RSXML::Note				Note;
};


#endif