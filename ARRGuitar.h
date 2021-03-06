#ifndef ARR_GUITAR
#define ARR_GUITAR

#ifndef ARR_STRUCTURE
#include "ARRStructure.h"
#endif

#include <string>
#include <vector>

namespace ARR {
	class Guitar : public Base::Guitar {
		public:
			Guitar( const float& length = 0.000f, const std::string& name = "", const bool& isBass = false ) : Base::Guitar( length, name, isBass ) { };
			Guitar( const Guitar& g ) : Base::Guitar( g ), phrases( g.phrases ), sections( g.sections ), difficulties( g.difficulties), 
											notes( g.notes ), chords( g.chords ), anchors( g.anchors ), events( g.events) { }; 
			
			const 	std::vector<ARR::Phrase>&		GetPhrases() const { return phrases; };
			const	std::vector<ARR::Section>&		GetSections() const { return sections; };
			const 	std::vector<ARR::Difficulty>&	GetDifficulties() const { return difficulties; };
			const 	std::vector<ARR::Note>& 		GetNotes() const { return notes; };
			const 	std::vector<ARR::Chord>&		GetChords() const { return chords; };
			const	std::vector<Base::MetaUInt>&	GetAnchors() const { return anchors; };
			const	std::vector<Base::MetaString>&	GetEvents() const { return events; };
			
			void	SetPhrases( const std::vector<ARR::Phrase>& v ) { phrases = v; };
			void	SetSections( const std::vector<ARR::Section>& v ) { sections = v; };
			void	SetDifficulties( const std::vector<ARR::Difficulty>& v ) { difficulties = v; };
			void	SetNotes( const std::vector<ARR::Note>& v ) { notes = v; };
			void	SetChords( const std::vector<ARR::Chord>& v ) { chords = v; };
			void	SetAnchors( const std::vector<Base::MetaUInt>& v ) { anchors = v; };
			void	SetEvents( const std::vector<Base::MetaString>& v ) { events = v; };
			
		private:
			std::vector<ARR::Phrase>				phrases;
			std::vector<ARR::Section>				sections;
			std::vector<ARR::Difficulty>			difficulties;
			std::vector<ARR::Note> 					notes;
			std::vector<ARR::Chord>					chords;
			// Unused in arrangements, but necessary to pass through to RSXML.
			std::vector<Base::MetaUInt>				anchors;
			std::vector<Base::MetaString>			events; 
	};
};


#endif