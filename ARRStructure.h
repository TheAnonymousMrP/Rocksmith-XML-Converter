#ifndef _ARR_STRUCTURE_
#define _ARR_STRUCTURE_

#ifndef _BASE_NOTE
#include "BaseNote.h"
#endif

#include <string>

namespace ARR {
	class Phrase : public virtual BaseObject {
		public:
			Phrase( const float& tim = 0.000f, const float& dur = 0.000f, 
				const std::string& nam = "" )
				: BaseObject( tim ) { duration = dur; name = nam; };
			
			std::string		name;
			
			const float&	GetDuration() const { return duration; };
				
		private:
			float			duration;
	};
	
	class Section : public virtual BaseObject {
		public:
			Section( const float& tim = 0.000f, const float& dur = 0.000f,
				const std::string& nam = "" )
				: BaseObject( tim ) { duration = dur; name = nam; };
			
			std::string		name;
				
			const float&	GetDuration() const { return duration; };
				
		private:
			float			duration;
	};
};

#endif