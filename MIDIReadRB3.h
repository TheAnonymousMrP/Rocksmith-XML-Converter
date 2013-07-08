#ifndef READ_MIDI
#include "MIDIReader.h"
#endif

namespace MIDI {
	class ReaderRB3 : public MIDI::Reader {
		public:
			/* void debug(); Will generate a file listing the contents of the 
			pulled tracks. For debug purposes. */
		
		private:
			unsigned int 		ProcessNote( unsigned int it, const float& timer, 
										MIDI::Track& track, unsigned char c = 0 );
			unsigned int 		ProcessMeta( unsigned int it, float& timer, 
										MIDI::Track& track );
			unsigned int 		ProcessSysEx( unsigned int it, const float& timer, 
										MIDI::Track& track ) { return 0; };	
	};
};