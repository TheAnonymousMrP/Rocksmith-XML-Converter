#ifndef ARR_GUITAR_BUILDER
#define ARR_GUITAR_BUILDER

#ifndef ARR_GUITAR
#include "ARRGuitar.h"
#endif

#ifndef MIDI_TRACK
#include "MIDITrack.h"
#endif

#include <array>
#include <string>
#include <vector>

/* The intent here is to take all the 'raw' information of a MIDI track and create an 
arrangement general enough to be written to a number of formats, but specifically a Rocksmith XML). */

namespace ARR {
	class GuitarBuilder {
		public:
			GuitarBuilder( const bool& debug = false ) : debug( debug ) { };
			
			const ARR::Guitar 				Create( const MIDI::Track& t );
			
		private:
			const bool						debug;

			MIDI::Track						track;
		
			void 							ConvertSpecialMetas( ARR::Guitar& g ) const;
			std::vector<ARR::Note>			ConvertMIDI2ARRNotes( const std::vector<MIDI::Note>& notes, const Base::Tuning& tuning ) const;
			void 							SetTechniques( std::vector<ARR::Note>& notes, const bool& isBass = false ) const;
			const ARR::Difficulty			CreateDifficulty( const unsigned int& dif, const std::vector<ARR::Note>& notes, std::vector<ARR::Chord>& chords ) const;
			const ARR::Chord				CreateChord( const std::vector<ARR::Note> notes, const std::vector<ARR::Chord>& chords,
												const unsigned int& noteIt, const unsigned char& chordSize ) const;
			const std::vector<ARR::Phrase> 	CreatePhrases( const std::vector<Base::MetaString>& source ) const;
			const std::vector<ARR::Section> CreateSections( const std::vector<Base::MetaString>& source ) const;
			
			// Process Methods
			void 							createAnchors();
			Difficulty 						createDifficulty(unsigned int i, std::ostream& file);
			Chord 							createChord(const std::vector<Note>& nSource, std::vector<Note>::iterator it, int chordSize);
			Chord 							createChord2( const std::vector<Note>& notes, const std::vector<unsigned int>& indexes );
	};

	class GuitarBuilderMIDI {
		public:
			GuitarBuilderMIDI( const bool& debug = false ) : debug( debug ) { };
			
			const ARR::Guitar2 				Build( const MIDI::Track& track );
			
		private:
			const bool						debug;

			MIDI::Track						track;
		
			void 							ConvertSpecialMetas( ARR::Guitar2& arrangement, const MIDI::Track& track ) const;
			const std::vector<ARR::Phrase> 	BuildPhrases( const std::vector<Base::MetaString>& source ) const;
			const std::vector<ARR::Section> BuildSections( const std::vector<Base::MetaString>& source ) const;
			std::vector<ARR::Note>			BuildNotes( const MIDI::Track& track, const Base::Tuning& tuning ) const;
			void 							SetTechniques( std::vector<ARR::Note>& notes, const std::vector<Base::MetaString>& techniques, const bool& isBass = false ) const;
			const ARR::Difficulty2			BuildDifficulty( const unsigned int& dif, const std::vector<ARR::Note>& notes, std::vector<ARR::Chord2>& chords ) const;
			const ARR::Chord2				CreateChord( const std::vector<ARR::Note> notes, const std::vector<ARR::Chord>& chords,
												const unsigned int& noteIt, const unsigned char& chordSize ) const;
	};
};	
	
#endif