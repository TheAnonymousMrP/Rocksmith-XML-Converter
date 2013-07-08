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
			Guitar( const float& len = 0.000f, const std::string& nam = "", const bool& bas = false ) : Base::Guitar( len, nam, bas ) { };
			Guitar( const Guitar& g ) : Base::Guitar( g ), phrases( g.phrases ), sections( g.sections ), notes( g.notes ), chords(g.chords) { }; 
			
			const 	std::vector<ARR::Phrase>&		GetPhrases() const { return phrases; };
			const	std::vector<ARR::Section>&		GetSections() const { return sections; };
			const 	std::vector<ARR::Difficulty>&	GetDifficulties() const { return difficulties; };
			const 	std::vector<ARR::Note>& 		GetNotes() const { return notes; };
			const 	std::vector<ARR::Chord>&		GetChords() const { return chords; };
			
			void	SetPhrases( const std::vector<ARR::Phrase>& v ) { phrases = v; };
			void	SetSections( const std::vector<ARR::Section>& v ) { sections = v; };
			void	SetDifficulties( const std::vector<ARR::Difficulty>& v ) { difficulties = v; };
			void	SetNotes( const std::vector<ARR::Note>& v ) { notes = v; };
			void	SetChords( const std::vector<ARR::Chord>& v ) { chords = v; };
			
		private:
			std::vector<ARR::Phrase>				phrases;
			std::vector<ARR::Section>				sections;
			std::vector<ARR::Difficulty>			difficulties;
			std::vector<ARR::Note> 					notes;
			std::vector<ARR::Chord>					chords;
	};
};


#endif