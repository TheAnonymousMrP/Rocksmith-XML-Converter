#ifndef _ARR_CREATE_VOCALS_
#define _ARR_CREATE_VOCALS_

#ifndef _ARR_VOCALS
#include "ARRVocals.h"
#endif

#ifndef _MIDI_TRACK
#include "MIDITrack.h"
#endif

#include <cstring>
#include <string>
#include <vector>

namespace ARR { 
	class CreateVocals {
		public:
			CreateVocals() { };
			
			const Vocals Create( const MIDI::Track& t, std::string ext = "" );
			
		private:	
			const std::vector<Base::MetaString> GetExternalWords( std::string fileName );
			const std::vector<Base::Lyric> 
				CreateLyrics( const std::vector<MIDI::Note>& notes, 
					const std::vector<Base::MetaString>& words, bool ext = 0 );
	};
	
	const Vocals CreateVocals::Create( const MIDI::Track& t, std::string ext ) {
		std::vector<MIDI::Note> notes = t.GetNotes();
		std::vector<Base::MetaString> words; 
		if( ext == "" ) { words = GetExternalWords( ext ); } 
		else { words = t.GetMetaStrings( eMeta::LYRICS ); }
		
		std::vector<Base::Lyric> lyrics = CreateLyrics( notes, words, ( ext == "" ) );
			
		Vocals v( t.name, lyrics );
		return v;
	}
	
	const std::vector<Base::Lyric> CreateVocals::CreateLyrics( 
		const std::vector<MIDI::Note>& notes, const std::vector<Base::MetaString>& words, 
		bool ext )
		{
		std::vector<Base::Lyric> lyrics;
		if ( ext ) {
			auto jt = words.begin();
			for( auto it = notes.begin(); it != notes.end(); ++it, ++jt ) {
				if( jt == words.end() ) { break; }
				Base::Lyric nL( it->GetTime(), it->GetPitch(), jt->GetString() );
				lyrics.push_back( nL );
			}	
		} else {
			auto lastSuccess = words.begin();
			for( const MIDI::Note& n : notes ) {
				for( auto jt = lastSuccess; jt != words.end(); ++jt ) {
					if( n.GetTime() == jt->GetTime() ) { 
						Base::Lyric nL( n.GetTime(), n.GetPitch(), jt->GetString() );
						lyrics.push_back( nL );
						lastSuccess = jt; 
						break; 
					}
				}
			}
		}
				
		/* The offset of notes and words. Negative values mean there's
		not enough words. Positive values mean there's not enough notes.
		I don't believe having blank lyrics is fatal, but it should probably be 
		avoided anyway. */
		int offset = notes.size() - words.size();
		if( offset != 0 ) { 
			std::cout << "Vocal mismatch. Offset of: " << offset << " notes.\n"; 
		}
		return lyrics;
	}
	
	const std::vector<Base::MetaString> CreateVocals::GetExternalWords( 
		std::string fileName ) 
		{
		std::vector<Base::MetaString> words;
		
		fileName += ".txt";
		std::cout << "External lyrics from '" << fileName 
			<< "'. \n";
		std::ifstream extlyrics;
		extlyrics.open(fileName.c_str());
		if( extlyrics.is_open() )
			{
			std::string line;
			std::string buf; // Have a buffer string
			std::stringstream ss;
			
			while(extlyrics.good())
				{
				getline(extlyrics,line);
				ss.str(""); ss.clear();
				ss.str(line); 
				
				while (ss >> buf)
					{
					MetaString word( eMeta::LYRICS, 0.000f, buf );
					words.push_back(word);
					}
				}
			}
		extlyrics.close();
		
		return words;
	}
};




#endif