#ifndef _ARR_CREATE_VOCALS_
#define _ARR_CREATE_VOCALS_

#ifndef _ARR_VOCALS
#include "ARRVocals.h"
#endif

#ifndef _MIDI_READ_OBJECTS
#include "MIDIReadObjects.h"
#endif

#include <cstring>

class ARRCreateVocals {
	public:
		ARRCreateVocals() { };
		
		const BaseVocals	CreateVocals( const MIDITrack& t, std::string ext = "" );
		
	private:	
		const std::vector<MetaString>	GetExternalLyrics( std::string fileName );
		const std::vector<Lyrics>		CreateLyrics( const std::vector<MIDINote>& notes, 
											const std::vector<MetaString>& words, 
											bool ext = 0 );
};

const BaseVocals ARRCreateVocals::CreateVocals( const MIDITrack& t, std::string ext = "" )
	{
	std::vector<MIDINote> notes = t.GetNotes();
	std::vector<MetaString> words; 
	bool ext = strcmp( argv[i],"" );
	if( ext ) { words = GetExternalLyrics( ext ); } 
	else { words = t.GetMetaStrings( eMeta::LYRICS ); }
	
	std::vector<Lyrics> lyrics = 
		CreateLyrics( notes, words, ext );
		
	BaseVocal v( t.name, lyrics );
	return v;
}

const std::vector<Lyric> ARRCreateVocals::CreateLyrics( 
	const std::vector<MIDINote>& notes, const std::vector<MetaString>& words, 
	bool ext = 0 )
	{
	std::vector<Lyric> lyrics;
	if ( ext ) {
		std::vector<Meta>::iterator jt = words.begin();
		for( auto& it = notes.begin(), auto& jt = words.begin(); 
			it != notes.end(); ++it, ++jt ) {
			if( jt >= words.begin() ) { break; }
			Lyric nL( *it.GetTime(), *it.GetPitch(), *jt.GetString() );
			lyrics.push_back( nL );
		}
			
		
	} else {
		auto& lastSuccess = words.begin();
		for( const MIDINote& n : notes ) {
			for( auto jt = lastSuccess; jt != vText.end(); ++jt ) {
				if( n.GetTime() == jt->GetTime() ) { 
					Lyric nL( n.GetTime(), n.GetPitch(), n.GetString() );
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

const std::vector<MetaString> ARRCreateVocals::GetExternalWords( std::string fileName ) 
	{
	std::vector<MetaString> words;
	
	fileName += ".txt";
	std::cout << "External lyrics from '" << fileName 
		<< "'. \n";
	std::ifstream extlyrics;
	extlyrics.open(fileName.c_str());
	if(extlyrics.is_open())
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


#endif