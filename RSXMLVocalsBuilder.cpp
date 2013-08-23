#include "RSXMLVocalsBuilder.h"

namespace RSXML {
	// Public Builders.
	// Vocals builder from MIDI track.
	const Base::Vocals VocalsBuilderMIDI::Build( const MIDI::Track& t, std::string ext ) {
		std::vector<MIDI::Note> notesMIDI = t.GetNotes();
		std::vector<Base::Note> notes;
		for( auto& n : notesMIDI ) {
			Base::Note buffer( n.GetTime(), n.GetPitch(), n.GetDuration() );
			notes.push_back( buffer );
		}

		std::vector<Base::MetaString> words; 
		if( ext != "" ) { words = GetExternalWords( ext ); } 
		else { words = t.GetMetaStrings( Base::eMeta::LYRICS ); }
		
		std::vector<Base::Lyric> lyrics = ConvertLyrics( notes, words, ( ext != "" ) );
			
		Base::Vocals v( lyrics, t.name );
		return v;
	}

	// Protected methods for core Builder.
	const std::vector<Base::Lyric> VocalsBuilder::ConvertLyrics( const std::vector<Base::Note>& notes, const std::vector<Base::MetaString>& words, bool ext ) {
		std::vector<Base::Lyric> lyrics;
		/* The offset of notes and words. Negative values mean there's
		not enough notes. Positive values mean there's not enough words.
		I don't believe having blank lyrics is fatal, but it should probably be 
		avoided anyway. */
		int offset = notes.size() - words.size();
		if( debug && offset != 0 ) { std::cerr << "Vocal mismatch. Offset of " << offset << " note(s).\n"; }

		if ( ext ) {
			auto jt = words.begin();
			for( auto it = notes.begin(); it != notes.end(); ++it, ++jt ) {
				if( jt == words.end() ) { break; }
				Base::Lyric nL( it->GetTime(), it->GetPitch(), jt->GetString() );
				lyrics.push_back( nL );
			}	
		} else {
			auto lastSuccess = words.begin();
			for( const Base::Note& n : notes ) {
				for( auto jt = lastSuccess; jt != words.end(); ++jt ) {
					if( n.GetTime() == jt->GetTime() ) { 
						Base::Lyric nL( n.GetTime(), n.GetPitch(), jt->GetString() );
						lyrics.push_back( nL );
						lastSuccess = jt; 
						break; 
					}
					if( debug && ( jt == words.end() - 1 ) ) { std::cerr << "No word found at: " << n.GetTime() << "\n"; }
				}
			}
		}
		return lyrics;
	}
		
	const std::vector<Base::MetaString> VocalsBuilder::GetExternalWords( std::string fileName ) {
		std::vector<Base::MetaString> words;
		
		fileName += ".txt";
		std::cout << "External lyrics from '" << fileName << "'. \n";
		std::ifstream extlyrics;
		extlyrics.open( fileName.c_str() );
		if( extlyrics.is_open() ) {
			std::string line;
			std::string buf; // Have a buffer string
			std::stringstream ss;
			
			while( extlyrics.good() ) {
				getline( extlyrics,line );
				ss.str(""); ss.clear();
				ss.str(line); 
				
				while (ss >> buf) {
					Base::MetaString word( Base::eMeta::LYRICS, 0.000f, buf );
					words.push_back( word );
				}
			}
		}
		extlyrics.close();
		
		return words;
	}
};

