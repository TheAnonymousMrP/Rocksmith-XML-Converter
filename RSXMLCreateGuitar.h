#ifndef RSXML_CREATE_GUITAR
#define RSXML_CREATE_GUITAR

#ifndef RSXML_GUITAR
#include "RSXMLGuitar.h"
#endif

#ifndef ARR_GUITAR
#include "ARRGuitar.h"
#endif

namespace RSXML {
	class CreateGuitar {
		public:
			CreateGuitar() { };
			
			const RSXML::Guitar&	Create( const ARR::Guitar& g, const bool& bass = 0 );
			
		private:
			const std::vector<RSXML::Beat>	CreateBeats( const ARR::Guitar& g );
	};
	
	
};

#endif