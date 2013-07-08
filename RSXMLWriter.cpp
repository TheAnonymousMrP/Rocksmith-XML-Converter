#include "RSXMLWriter.h"

namespace RSXML {
	// Public methods
	void Writer::WriteGuitar( const RSXML::Guitar& g ) {
		arrangement = g;
		std::ofstream write;
		std::cout << "Name: " << g.GetName() << "\n";
		std::string file = fileName + "-" + g.GetName() + ".xml";
		std::cout << file << "\n";
		write.open( file.c_str() );
		write << "<?xml version='1.0' encoding='UTF-8'?> \n"
		<< "<song>\n" << "\t<title>" << songName << "</title>\n"
		<< "\t<arrangement>" << g.GetName() << "</arrangement>\n"
		<< "\t<part>1</part>\n" // This should be something.
		/* Specifies a negative value (in seconds) for the song to be offset.
		Eg. -10 seconds if the first beat is at 10 seconds. Doesn't seem to work
		right for me, though. It may correlated with startBeat, though, 
		which seems to always be 0 despite where the real start time is. */
		<< "\t<offset>" << 0 - DEFAULTOFFSET << "</offset>\n"
		<< "\t<centOffset>" << DEFAULTOFFSETCENT << "</centOffset>\n" // No idea.
		<< "\t<songLength>" << g.GetDuration() << "</songLength>\n" 
		<< "\t<startBeat>" << DEFAULTSTARTBEAT << "</startBeat>\n"
		<< "\t<averageTempo>" << DEFAULTTEMPO << "</averageTempo>\n"
		// Tuning information. May not be implemented yet.
		<< "\t<tuning ";
		for( auto i = 0; i < NUMSTRINGS; ++i ) {
			write << "string" << i << "=\"" 
			<< g.tuning.pitch[i] - Base::aTuning[Base::eTuning::STANDARD_E].pitch[i]
			<< "\" ";
		}
		write << "/>\n"
		// Song metadata. Not sure how to grab this at present.
		<< "\t<artistName></artistName>\n"
		<< "\t<albumName></albumName>\n"
		<< "\t<albumYear></albumYear>\n"
		<< "\t<albumArt></albumArt>\n"
		<< "\t<crowdSpeed>1</crowdSpeed>\n" // This could probably be changed.
		/* 'arrangementProperties' holds a number of values describing the 
		contents of the arrangement. At a push, I'd guess this may relate to the
		dynamic difficulty of the game. Alternatively, it could provide a 
		reference for the game to recommend the training videos for the techniques.
		Interesting to note is a number of additional techniques which aren't yet 
		implemented in-game, such as pinch-harmonics. Some of these do have 
		videos, though.
		For now, magic numbers ahoy! In the future, I'll make more of an effort 
		to implement this stuff.*/
		<< "\t<arrangementProperties represent=\"0\" standardTuning=\"" << 1 
		<< "\" nonStandardChords=\"" << 0 << "\" barreChords=\"" << 0 << "\" "
		<< "doubleStops=\"" << 0 << "\" dropDPower=\"" << 0 << "\" "
		<< "fifthsAndOctaves=\"" << 0 << "\" powerChords=\"" << 0 << "\" " 
		<< "openChords=\"" << 0 << "\" "
		// Techniques
		<< "bends=\"" << 0 << "\" fretHandMutes=\"" << 0 << "\" harmonics=\"" 
		<< 0 << "\" hopos=\"" << 0 << "\" palmMutes=\"" << 0 << "\" "
		<< "pinchHarmonics=\"" << 0 << "\" slides=\"" << 0 << "\" sustain=\""
		<< 0 << "\" tapping=\"" << 0 << "\" tremolo=\"" << 0 << "\" "
		<< "unpitchedSlides=\"" << 0 << "\" vibrato=\"" << 0 << "\" "
		// Bass stuff. Should probably try to exclude these from guitar charts.
		// Finger picking may not be a bass value.
		<< "bassPick=\"" << 0 << "\" fingerPicking=\"" << 0 << "\" "
		<< "twoFingerPicking=\"" << 0 << "\" slapPop=\"" << 0 << "\" "
		// Misc
		<< "pickDirection=\"" << 0 << "\" syncopation=\"" << 0 << "\" />\n"
		// Irrelevant data
		<< "\t<internalName>" << fileName << "</internalName>\n" // Project name?
		<< "\t<lastConversionDateTime>" << "01-01-1970 00:01" 
		<< "</lastConversionDateTime>\n";
		
		WriteStructure( g );
	
		const std::vector<Difficulty>& difficulties = g.GetDifficulties();
	
		/* Transcription Track -- whatever that is. It seems to be one full 
		difficulty of a song. Maybe max? */	
		write << WriteDifficulty( difficulties.back() );
		
		// Levels -- where shit gets real.
		write << "\t<levels count=\"" << difficulties.size() << "\" >\n";
		for( auto it = difficulties.begin(); it != difficulties.end(); ++it )
			{ write << WriteDifficulty( *it ); } 
		write << "\t</levels>\n";
		write << "</song>";
		write.close();
		}
		
	void Writer::WriteVocals( const ARR::Vocals& v ) {
		std::ofstream write;
		std::string file = fileName + "-Vocals.xml";
		write.open(file.c_str()); // This is the file we're writing to.
		
		auto& lyrics = ConvertARR2RSXMLLyrics( v.GetLyrics() );
		
		write << "<?xml version='1.0' encoding='UTF-8'?> \n"
		<< "<vocals count=\"" << lyrics.size() << "\">\n";
		
		for( auto& l : lyrics ) { write << l.ToXML(); }
			
		write << "</vocals>";
		write.close();
	}
		
	// Private methods 
	const std::string Writer::WriteStructure( const RSXML::Guitar& g ) {
		std::stringstream ss("");
		// Phrases (aka Phrase Templates)
		auto& phrases = g.GetPhrases();
		ss << "\t<phrases count=\"" << phrases.size() << "\">\n";
		for( auto& p : phrases ) { ss << p.ToXML(); }
		ss << "\t</phrases>\n";
		
		// Phrase Iterations
		auto& phraseIt = g.GetPhraseIterations();
		ss << "\t<phraseIterations count=\"" << phraseIt.size() << "\">\n";
		for( auto& pI : phraseIt ) { ss << pI.ToXML(); }
		ss << "\t</phraseIterations>\n";
		
		// Linked Diffs
		ss << "\t<newLinkedDiffs count=\"" << 0 << "\" />\n";
		ss << "\t<linkedDiffs count=\"" << 0 << "\" />\n";
		ss << "\t<phraseProperties count=\"" << 0 << "\" />\n";
		
		// Chord Templates
		auto& chords = g.GetChordTemplates();
		ss << "\t<chordTemplates count=\"" << chords.size() << "\">\n";
		for( auto& t : chords ) { t.ToXML(); }
		ss << "\t</chordTemplates>\n";
		
		// Fret Hand Mutes
		ss << "\t<fretHandMuteTemplates count=\"0\" />\n";
		
		// Events
		auto& events = g.GetEvents();
		ss << "\t<events count=\"" << events.size() << "\">\n";
		for( auto& e : events ) { ss << e.ToXML(); }
		ss << "\t</events>\n";
		
		// Beat Grid
		auto& beats = g.GetBeats();
		ss << "\t<ebeats count=\"" << beats.size() << "\">\n";
		for(auto& b : beats) { ss << b.ToXML(); }
		ss << "\t</ebeats>\n";
		
		// Sections
		auto& sections = g.GetSections();
		ss << "\t<sections count=\"" << sections.size() << "\">\n";
		for(auto& s : sections) { ss << s.ToXML(); }
		ss << "\t</sections>\n";
		
		return ss.str();
	}
	
	const std::string Writer::WriteDifficulty( const RSXML::Difficulty& d ) {
		std::stringstream ss("");
		std::string t = "";
		if( d.IsTranscription() ) { ss << "\t<transcriptionTrack "; t = "\t"; }
		else { ss << "\t\t<level "; t = "\t\t"; }
		
		ss << "difficulty=\"" << d.GetIndex() << "\">\n"; 
		
		auto& phraseIt = arrangement.GetPhraseIterations();	
		// Notes
		auto& nSource( arrangement.GetNotes() );
		auto& nIndex( d.GetNotesIndex() );
		unsigned int nSize = 0;
		// Chords
		auto& cSource( arrangement.GetChords() );
		auto& cIndex( d.GetChordsIndex() );
		unsigned int cSize = 0;
		// Anchors
		auto& aSource( arrangement.GetAnchors() );
		auto& aIndex( d.GetAnchorsIndex() );
		unsigned int aSize = 0;
		// Handshapes
		auto& hSource( arrangement.GetHandShapes() );
		auto& hIndex( d.GetHandShapesIndex() );
		unsigned int hSize = 0;
		
		/* For the sake of not repeating code, we're going to use 
		stringstreams, and pop the results in after. */
		std::ostringstream nS(""), cS(""), aS(""), hS("");
		for( auto it = phraseIt.begin(); it != phraseIt.end(); ++it ) {
			float start = it->GetTime();
			float end = 0.000f; 
			if( it != (phraseIt.end() - 1) ) { end = (it + 1)->GetTime(); }
			else { end = d.GetLength(); }
			
			auto& notes( getXsFromIsWithinTime( nSource, nIndex, start, end ) );
			auto& chords( getXsFromIsWithinTime( cSource, cIndex, start, end ) );
			auto& ans( getXsFromIsWithinTime( aSource, aIndex, start, end ) );
			auto& hands( getXsFromIsWithinTime( hSource, hIndex, start, end ) );
			
			/* std::cout << "ID: " << p.getID() << " Difficulty: " << dif
			<< " Start: " << start << " End: " << end ENDLINE
			std::cout << "\tCurrent Notes: " << notes.size() << " Current Chords: "
			<< chords.size() ENDLINE */
			
			RSXML::PhraseTemplate p = arrangement.GetPhrases().at( it->id );
			if( d.IsTranscription() || d.GetIndex() <= p.GetMaxDifficulty() ) { 
				for( const Note& n : notes ) { nS << t << n.ToXML(); }
				nSize += notes.size();
				for( const Chord& c : chords ) { cS << t << c.ToXML(); }
				cSize += chords.size();
				for( const Anchor& a : ans ) { aS << t << a.ToXML(); }
				aSize += ans.size();
				for( const HandShape& h : hands ) { hS << t << h.ToXML(); }
				hSize += hands.size();
			}
		}
		// Notes
		ss << t << "<notes count=\"" << nSize << "\">\n";
		ss << nS.str();
		ss << t << "</notes>\n";
		
		// Chords
		ss << t << "<chords count=\"" << cSize << "\">\n";
		ss << cS.str();
		ss << t << "</chords>\n";
		
		// Anchors
		ss << t << "<anchors count=\"" << aSize << "\">\n";
		ss << aS.str();
		ss << t << "</anchors>\n";
	
		// Handshapes	
		ss << t << "<handShapes count=\"" << hSize << "\">\n";
		ss << hS.str();
		ss << t << "</handShapes>\n";
		if( d.IsTranscription() ) { ss << "\t</transcriptionTrack>\n"; }
		else { ss << "\t\t</level>\n"; }
		
		return ss.str();
	}
	
	const std::vector<RSXML::Lyric> Writer::ConvertARR2RSXMLLyrics( 
		const std::vector<Base::Lyric> source )
		{
		std::vector<RSXML::Lyric> dest;
		for( auto& oldLyric : source ) { 
			RSXML::Lyric newLyric( oldLyric ); 
			dest.push_back( newLyric );
		}
		return dest;
	}
};
