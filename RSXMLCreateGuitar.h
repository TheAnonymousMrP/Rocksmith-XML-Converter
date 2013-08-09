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
			CreateGuitar( const bool& debug = false ) : debug( debug ) { };
			
			const RSXML::Guitar						Create( const ARR::Guitar& arrg, const bool& bass = 0 );
			
		private:
			const bool								debug;

			// Global/meta data.
			const std::vector<RSXML::Beat>			CreateBeats					( const Base::Guitar& g ); 
			const std::vector<RSXML::Anchor>		ConvertAnchors				( const std::vector<Base::MetaUInt>& source );
			
			const std::vector<RSXML::Event>			ConvertEvents				( const std::vector<Base::MetaString>& source );
			void									ConvertARR2RSXMLPhrases		( const std::vector<ARR::Phrase>& source, 
																					std::vector<RSXML::PhraseTemplate>& temp, 
																					std::vector<RSXML::Phrase>& phrase, 
																					const std::vector<RSXML::Note>& notes );
			const std::vector<RSXML::Section>		ConvertARR2RSXMLSections	( const std::vector<ARR::Section>& source );

			const std::vector<RSXML::Note>			ConvertARR2RSXMLNotes		( const std::vector<ARR::Note>& source );
			void									ConvertARR2RSXMLChords		( const std::vector<ARR::Chord>& source, 
																					std::vector<RSXML::ChordTemplate>& temp, 
																					std::vector<RSXML::Chord>& chord, 
																					const Base::Tuning& tuning, 
																					const std::vector<RSXML::Note>& notes );

			const std::vector<RSXML::Difficulty>	ConvertARR2RSXMLDifficulty	( const std::vector<ARR::Difficulty>& source );

			void									CreateAnchors				( std::vector<RSXML::Anchor>& anchors,
																					std::vector<RSXML::Difficulty>& difficulties,
																					const std::vector<RSXML::Note>& notes,
																					const std::vector<RSXML::Chord>& chords );
			void									CreateHandShapes			( std::vector<RSXML::HandShape>& handShapes,
																					std::vector<RSXML::Difficulty>& difficulties,
																					const std::vector<RSXML::Chord>& chords, 
																					const std::vector<RSXML::ChordTemplate>& templates );
	};
	
};

#endif