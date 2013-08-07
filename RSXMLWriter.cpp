#include "RSXMLWriter.h"

namespace RSXML {
	// Public methods
	void Writer::WriteGuitar( const RSXML::Guitar& g ) {
		arrangement = g;
		std::ofstream write;
		std::string file = fileName + "-" + g.GetName() + ".xml";
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
		try {
			auto& tuning = g.tuning.pitch;
			if( tuning.empty() ) { throw Writer::VectorEmptyException( "Tuning" ); }
			for( auto it = tuning.begin(); it != tuning.end(); ++it ) {
				write << "string" << it - tuning.begin() << "=\"" 
				<< *it - Base::aTuning[ Base::eTuning::STANDARD_E ].pitch[ it - tuning.begin() ]
				<< "\" ";
			}
		} catch( Writer::VectorEmptyException e ) {
			std::cerr << e.what() << "\n";
			write << "string0=\"52\" string1=\"57\" string2=\"62\" string3=\"67\" string4=\"71\" string5=\"76\" ";
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
		
		write << WriteStructure( g );
		write << WriteDifficulties( g );
		
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

	// Returns std::string containing XML output for structure-related data.
	const std::string Writer::WriteStructure( const RSXML::Guitar& g ) const {
		std::stringstream write("");
		// Phrases (aka Phrase Templates)
		try {
			auto& phrases = g.GetPhrases();
			if( phrases.empty() ) { throw Writer::VectorEmptyException( "RSXML::PhraseTemplate" ); }
			write << "\t<phrases count=\"" << phrases.size() << "\">\n";
			for( auto& p : phrases ) { write << p.ToXML(); }
			write << "\t</phrases>\n";
		} catch ( Writer::VectorEmptyException e ) {
			std::cerr << e.what() << "\n";
			write << "\t<phrases count=\"" << 0 << "\" />\n";
		}

		// Phrase Iterations
		try {
			auto& phraseIt = g.GetPhraseIterations();
			if( phraseIt.empty() ) { throw Writer::VectorEmptyException( "RSXML::Phrase" ); }
			write << "\t<phraseIterations count=\"" << phraseIt.size() << "\">\n";
			for( auto& pI : phraseIt ) { write << pI.ToXML(); }
			write << "\t</phraseIterations>\n";
		} catch( Writer::VectorEmptyException e ) {
			std::cerr << e.what() << "\n";
			write << "\t<phraseIterations count=\"" << 0 << "\" />\n";
		}
		
		
		// Linked Diffs
		write << "\t<newLinkedDiffs count=\"" << 0 << "\" />\n";
		write << "\t<linkedDiffs count=\"" << 0 << "\" />\n";
		write << "\t<phraseProperties count=\"" << 0 << "\" />\n";
		
		// Chord Templates
		try {
			auto& chords = g.GetChordTemplates();
			if( chords.empty() ) { throw Writer::VectorEmptyException( "RSXML::ChordTemplate" ); }
			write << "\t<chordTemplates count=\"" << chords.size() << "\">\n";
			for( auto& t : chords ) { write << t.ToXML(); }
			write << "\t</chordTemplates>\n";
		} catch( Writer::VectorEmptyException e ) {
			std::cerr << e.what() << "\n";
			write << "\t<chordTemplates count=\"" << 0 << "\" />\n";
		}
		
		
		// Fret Hand Mutes
		write << "\t<fretHandMuteTemplates count=\"0\" />\n";
		
		// Events
		try {
			auto& events = g.GetEvents();
			if( events.empty() ) { throw Writer::VectorEmptyException( "RSXML::Event" ); }
			write << "\t<events count=\"" << events.size() << "\">\n";
			for( auto& e : events ) { write << e.ToXML(); }
			write << "\t</events>\n";
		} catch( Writer::VectorEmptyException e ) {
			std::cerr << e.what() << "\n";
			write << "\t<events count=\"" << 0 << "\" />\n";
		}
		
		// Beat Grid
		try {
			auto& beats = g.GetBeats();
			if( beats.empty() ) { throw Writer::VectorEmptyException( "RSXML::Beat" ); }
			write << "\t<ebeats count=\"" << beats.size() << "\">\n";
			for(auto& b : beats) { write << b.ToXML(); }
			write << "\t</ebeats>\n";
		} catch( Writer::VectorEmptyException e ) {
			std::cerr << e.what() << "\n";
			write << "\t<something count=\"" << 0 << "\" />\n";
		}
		
		// Sections
		try {
			auto& sections = g.GetSections();
			if( sections.empty() ) { throw Writer::VectorEmptyException( "RSXML::Section" ); }
			write << "\t<sections count=\"" << sections.size() << "\">\n";
			for(auto& s : sections) { write << s.ToXML(); }
			write << "\t</sections>\n";
		} catch( Writer::VectorEmptyException e ) {
			std::cerr << e.what() << "\n";
			write << "\t<something count=\"" << 0 << "\" />\n";
		}
		
		return write.str();
	}

	// Returns std::string containing XML output for all difficulties.
	const std::string Writer::WriteDifficulties( const RSXML::Guitar& g ) const {
		std::stringstream write("");
		const std::vector<Difficulty>& difficulties = g.GetDifficulties();
		try {
			if( difficulties.empty() ) { throw Writer::VectorEmptyException( "Difficulty" ); }
			/* Transcription Track -- whatever that is. It seems to be one full difficulty of a song. Maybe max? */	
			Difficulty transcription( difficulties.back(), true );
			write << WriteDifficulty( transcription );
			// Levels -- where shit gets real.
			write << "\t<levels count=\"" << difficulties.size() << "\" >\n";
			for( auto it = difficulties.begin(); it != difficulties.end(); ++it )
				{ write << WriteDifficulty( *it ); } 
			write << "\t</levels>\n";
		} catch( Writer::VectorEmptyException e ) {
			std::cerr << e.what() << "\n"; 
			write << "\t<levels count=\"" << 0 << "\" />\n";
		}
		return write.str();
	}
	
	// Returns std::string containing XML output for a single difficulty.
	const std::string Writer::WriteDifficulty( const RSXML::Difficulty& d ) const {
		std::stringstream ss("");
		std::string t = "";
		if( d.IsTranscription() ) { ss << "\t<transcriptionTrack "; t = "\t\t"; }
		else { ss << "\t\t<level "; t = "\t\t\t"; }
		
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
			
			auto& notes( GetLevelObjectsFromIndexesWithinTime( nSource, nIndex, start, end ) );
			auto& chords( GetLevelObjectsFromIndexesWithinTime( cSource, cIndex, start, end ) );
			auto& ans( GetLevelObjectsFromIndexesWithinTime( aSource, aIndex, start, end ) );
			auto& hands( GetLevelObjectsFromIndexesWithinTime( hSource, hIndex, start, end ) );

			RSXML::PhraseTemplate p = arrangement.GetPhrases().at( it->GetPhraseID() );
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

			/* std::cerr << "ID: " << (unsigned int)it->GetPhraseID() << " Difficulty: " << d.GetIndex() << " Max Dif: " << p.GetMaxDifficulty() 
				<< " Start: " << start << " End: " << end ENDLINE
			std::cerr << "\tCurrent Notes: " << notes.size() << " Current Chords: " << chords.size() ENDLINE */
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
	
	// Converts Base::Lyric vector to RSXML::Lyric vector.
	const std::vector<RSXML::Lyric> Writer::ConvertARR2RSXMLLyrics( const std::vector<Base::Lyric> source ) const {
		std::vector<RSXML::Lyric> dest;
		for( auto& oldLyric : source ) { 
			RSXML::Lyric newLyric( oldLyric ); 
			dest.push_back( newLyric );
		}
		return dest;
	}
};
