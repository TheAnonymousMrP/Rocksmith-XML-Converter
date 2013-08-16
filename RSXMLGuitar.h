#ifndef RSXML_GUITAR
#define RSXML_GUITAR

#ifndef RSXML_OBJECTS
#include "RSXMLObjects.h"
#endif

#include <vector>

namespace RSXML {
	enum eGuitarVectors {
		ANCHOR,
		BEAT,
		CHORD,
		CHORD_TEMPLATE,
		DIFFICULTY,
		EVENT,
		HANDSHAPE,
		PHRASE,
		PHRASE_TEMPLATE,
		PHRASE_LINK,
		PHRASE_LINK_NEW,
		PHRASE_PROPERTY,
		SECTION,
	};

	class Guitar : public Base::Guitar {
		enum eValues {
			REPRESENT,
			STANDARDTUNING,
			// Chords
			NONSTANDARDCHORDS,
			BARRECHORDS,
			DOUBLESTOPS,
			DROPDPOWER,
			FIFTHSANDOCTAVES,
			POWERCHORDS,
			OPENCHORDS,
			// Techniques
			BENDS,
			FRETHANDMUTES,
			HARMONICS,
			HOPOS,
			PALMMUTES,
			PINCHHARMONICS,
			SLIDES,
			SUSTAIN,
			TAPPING,
			TREMOLO,
			UNPITCHEDSLIDES,
			VIBRATO,
			// Bass
			BASSPICK,
			FINGERPICKING,
			TWOFINGERPICKING,
			SLAPPOP,
			// Misc
			PICKDIRECTION,
			SYNCOPATION,
			size,
		};

		public:
			Guitar( const float& length = 0.000f, const std::string& name = "", const bool& isBass = false ) 
				: Base::Guitar( length, name, isBass ) { };
			Guitar( const Guitar& g ) : Base::Guitar( g ), beats( g.beats ), 
				chordTemplates( g.chordTemplates), events( g.events ), 
				phrases( g.phrases ), phraseIterations( g.phraseIterations ),
				sections ( g.sections ), difficulties( g.difficulties ), notes( g.notes ),
				chords( g.chords ), anchors( g.anchors ), hands( g.hands ) { };

			std::array<bool,eValues::size>				values;
			
			const std::vector<RSXML::Beat>&				GetBeats() const { return beats; };
			const std::vector<RSXML::ChordTemplate>&	GetChordTemplates() const { return chordTemplates; };
			const std::vector<RSXML::Event>&			GetEvents() const { return events; };
			const std::vector<RSXML::PhraseTemplate>&	GetPhrases() const { return phrases; };
			const std::vector<RSXML::Phrase>&			GetPhraseIterations() const { return phraseIterations; };
			const std::vector<RSXML::Section>&			GetSections() const { return sections; };
						
			const std::vector<RSXML::Difficulty>&		GetDifficulties() const { return difficulties; };
			const std::vector<RSXML::Note>& 			GetNotes() const { return notes; };
			const std::vector<RSXML::Chord>&			GetChords() const { return chords; };
			const std::vector<RSXML::Anchor>&			GetAnchors() const { return anchors; };
			const std::vector<RSXML::HandShape>&		GetHandShapes() const { return hands; };			
			
			void 	SetBeats( const std::vector<RSXML::Beat>& v ) { beats = v; };
			void 	SetChordTemplates( const std::vector<RSXML::ChordTemplate>& v ) { chordTemplates = v; };
			void 	SetEvents( const std::vector<RSXML::Event>& v ) { events = v; };
			void 	SetPhrases( const std::vector<RSXML::PhraseTemplate>& v ) { phrases = v; };
			void 	SetPhraseIterations( const std::vector<RSXML::Phrase>& v ) { phraseIterations = v; };
			void 	SetSections( const std::vector<RSXML::Section>& v ) { sections = v; };
			
			void	SetDifficulties( const std::vector<RSXML::Difficulty>& v ) { difficulties = v; };
			void 	SetNotes( const std::vector<RSXML::Note>& v ) { notes = v; };
			void	SetChords( const std::vector<RSXML::Chord>& v ) { chords = v; };
			void 	SetAnchors( const std::vector<RSXML::Anchor>& v ) { anchors = v; };
			void 	SetHandShapes( const std::vector<RSXML::HandShape>& v ) { hands = v; };
		
		private:
			std::vector<RSXML::Beat>					beats;
			std::vector<RSXML::ChordTemplate>			chordTemplates;
			std::vector<RSXML::Event>					events;
			std::vector<RSXML::LinkedDiff>				linkedDiffs;
			std::vector<RSXML::NewLinkedDiff>			newLinkedDiffs;
			std::vector<RSXML::PhraseTemplate>			phrases;
			std::vector<RSXML::Phrase>					phraseIterations;
			std::vector<RSXML::PhraseProperty>			phraseProperties;
			std::vector<RSXML::Section>					sections;
													
			std::vector<RSXML::Difficulty>				difficulties;
			std::vector<RSXML::Note>					notes;
			std::vector<RSXML::Chord>					chords;
			std::vector<RSXML::Anchor>					anchors;
			std::vector<RSXML::HandShape>				hands;
	};
};

#endif