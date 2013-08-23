#ifndef ARR_VOCALS
#define ARR_VOCALS

#ifndef BASE_NOTE
#include "BaseObjects.h"
#endif

#ifndef MIDI_TRACK
#include "MIDITrack.h"
#endif

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace RSXML {
	class VocalsBuilder {
		public:
			VocalsBuilder( const bool& debug = false ) : debug( debug ) { };
			
			virtual void							Build() { };
			
		protected:	
			const bool								debug;

			const std::vector<Base::Lyric>			ConvertLyrics( const std::vector<Base::Note>& notes, const std::vector<Base::MetaString>& words, bool externalLyrics = false );
			const std::vector<Base::MetaString>		GetExternalWords( std::string fileName );
	};

	

	class VocalsBuilderMIDI : public VocalsBuilder {
		public:
			VocalsBuilderMIDI( const bool& debug = false ) : VocalsBuilder( debug ) { };

			const Base::Vocals						Build( const MIDI::Track& track, std::string ext = "" );
	};
	
	
};

#endif