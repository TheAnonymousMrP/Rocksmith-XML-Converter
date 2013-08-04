#ifndef RSXML_CREATE_GUITAR
#include "RSXMLCreateGuitar.h"
#endif

#ifndef DEBUG_STUFF
#include "debug.h"
#endif

namespace RSXML {
	const RSXML::Guitar CreateGuitar::Create( const ARR::Guitar& arrg, const bool& bass ) {
		Guitar rsg( arrg.GetDuration(), arrg.GetName(), arrg.IsBass() );

		// Global/meta data.
		rsg.SetBeats( CreateBeats( arrg ) );
		rsg.SetEvents( ConvertEvents( arrg.GetEvents() ) );
		// We handle phrases later because of the maxDifficulty value.
		// Sections
		rsg.SetSections( ConvertARR2RSXMLSections( arrg.GetSections() ) );
		
		// Notes
		rsg.SetNotes( ConvertARR2RSXMLNotes( arrg.GetNotes() ) );
		// Chords
		std::vector<RSXML::ChordTemplate> cTemplates; 
		std::vector<RSXML::Chord> chords;
		ConvertARR2RSXMLChords( arrg.GetChords(), cTemplates, chords, rsg.tuning, rsg.GetNotes() );
		rsg.SetChordTemplates( cTemplates );
		rsg.SetChords( chords );

		WOOP
		// Phrases
		std::vector<RSXML::PhraseTemplate> pTemplates;
		std::vector<RSXML::Phrase> pIterations;
		ConvertARR2RSXMLPhrases( arrg.GetPhrases(), pTemplates, pIterations );
		rsg.SetPhrases( pTemplates );
		rsg.SetPhraseIterations( pIterations );
		GOTHERE
		
		return rsg;
	}

	// Creates ebeat vector.
	const std::vector<RSXML::Beat> CreateGuitar::CreateBeats( const Base::Guitar& g ) {	
		std::vector<RSXML::Beat> beats;	
		try {
			const std::vector<Base::Tempo>& tempos( g.GetTempos() );
			const std::vector<Base::TimeSig>& timeSigs( g.GetTimeSigs() );
			if( tempos.empty() ) { throw Base::VectorEmptyException( "Tempo" ); } 
			else if( g.GetTimeSigs().empty() ) { throw Base::VectorEmptyException( "Time Signature" ); }

			std::vector<Base::Tempo>::const_iterator tempoIt = tempos.begin();
			std::vector<Base::TimeSig>::const_iterator timeSigIt = timeSigs.begin();
		
			int beat = 0; // Beat counter.
			int bar = 0; // Bar counter.
			float timer = tempoIt->GetTempo();
			
			unsigned int numerator = timeSigIt->numerator;
			while( timer < g.GetDuration() ) {
				/* Debug corner!
				std::cout << "tCount: " << ( tempoIt - tempos.begin() ) 
				<< " tEnd: " << tempos.end() - tempos.begin() << " Timer: " 
				<< timer << " | Duration: " << g.GetDuration()
				<< " | Bar: " << bar << " | Tempo: " << tempoIt->GetTempo() ENDLINE */
				
				if( tempoIt != ( tempos.end() - 1 ) && timer >= ( tempoIt + 1 )->GetTime() ) 
					{ ++tempoIt; } 
				if( timeSigIt != ( timeSigs.end() - 1 ) && timer >= ( timeSigIt + 1 )->GetTime() ) 
					{ ++timeSigIt; numerator = timeSigIt->numerator; beat = 0; }
			
				// Needs to implement TimeSigs (properly).
				unsigned int bBar = 0;
				if(beat % numerator == 0)
					{ bBar = bar; ++bar; beat = 0; }
				else { bBar = -1; }
				RSXML::Beat b( timer, bBar );
				beats.push_back(b);
			
				timer += tempoIt->Convert2Beat();
				++beat;
			}
		} catch( Base::VectorEmptyException e ) {
			std::cout << e.what() << "\n";
			RSXML::Beat b;
			beats.push_back(b);
		}
		return beats;
	};
	
	// Creates RSXML::Event vector from Base::MetaString vector.
	const std::vector<RSXML::Event>	CreateGuitar::ConvertEvents( const std::vector<Base::MetaString>& source ) { 
		std::vector<RSXML::Event> dest;
		try {
			if( source.empty() ) { throw Base::VectorEmptyException( "Event" ); }
			for( auto& e : source ) { 
				dest.push_back( RSXML::Event( e.GetTime(), e.GetString() ) );
			}
		} catch( Base::VectorEmptyException e ) {
			std::cerr << e.what() << "\n";
		}
		return dest;
	}

	// Converts ARR::Phrase vector into RSXML::Phrase vector.
	void CreateGuitar::ConvertARR2RSXMLPhrases( const std::vector<ARR::Phrase>& source, std::vector<RSXML::PhraseTemplate>& temp, std::vector<RSXML::Phrase>& phrase ) { 
		try {
			if( source.empty() ) { throw Base::VectorEmptyException( "Phrase" ); }
			unsigned int phraseID = 0;
			for( auto& p : source ) {
				if( !temp.empty() ) {
					for( std::vector<RSXML::PhraseTemplate>::iterator it = temp.begin(); it != temp.end(); ++it ) {
						++phraseID;
						if( p.name == it->GetName() ) { 
							phraseID = it - temp.begin(); 
							it = temp.end();
						}
					}
				} else { ++phraseID; }
				if( phraseID == temp.size() ) {	
					unsigned int maxDifficulty = 0;
					RSXML::PhraseTemplate newTemplate( p.name, maxDifficulty, false, false, false, phraseID ); 
					temp.push_back( newTemplate );
				}

				unsigned char variation = 0;
				RSXML::Phrase rsxml( p.GetTime(), phraseID, variation );
				phrase.push_back( rsxml );
			}
		} catch( Base::VectorEmptyException e ) {
			std::cout << e.what() << "\n";
			RSXML::PhraseTemplate badTemplate( "No Phrases Found.", 0, false, false, false, 0 );
			RSXML::Phrase badPhrase;
			temp.push_back( badTemplate );
			phrase.push_back( badPhrase );
		} 	
	}
	
	// Converts ARR::Section vector into RSXML::Section vector.
	const std::vector<RSXML::Section> CreateGuitar::ConvertARR2RSXMLSections( const std::vector<ARR::Section>& source ) { 
		std::vector<RSXML::Section> dest;
		try {
			if( source.empty() ) { throw Base::VectorEmptyException( "Section" ); } 
			for( auto& arr : source ) {
				unsigned int iteration = 1;
				// Compare by name to previous sections.
				for( auto it = dest.begin(); it != dest.end(); ++it ) { if( arr.name == it->GetName() ) { ++iteration; } }
				RSXML::Section newSection( arr.GetTime(), arr.name, iteration );
				dest.push_back( newSection);
			}
		} catch( Base::VectorEmptyException e ) {
			std::cout << e.what() << "\n";
			RSXML::Section badSection( 0.000f, "No Sections found.", 1 );
			dest.push_back( badSection );
		}
		return dest;
	}

	// Converts ARR::Note vector into RSXML::Note vector.
	const std::vector<RSXML::Note> CreateGuitar::ConvertARR2RSXMLNotes( const std::vector<ARR::Note>& source ) {
		// At present, ARR and RSXML notes are the same. This may not be the case in the future.
		std::vector<RSXML::Note> dest;
		try {
			if( source.empty() ) { throw Base::VectorEmptyException( "Note" ); }
			for( auto& arr : source ) {	
				RSXML::Note rsxml( arr );
				dest.push_back( rsxml );
			}
		} catch( Base::VectorEmptyException e ) {
			std::cout << e.what() << "\n";
			RSXML::Note badNote;
			dest.push_back( badNote ); 
		}
		return dest;
	}

	// Converts ARR::Chord vector into RSXML::Chord & RSXML::ChordTemplate vectors.
	void CreateGuitar::ConvertARR2RSXMLChords( const std::vector<ARR::Chord>& source, std::vector<RSXML::ChordTemplate>& temp, 
		std::vector<RSXML::Chord>& chord, const Base::Tuning& tuning, const std::vector<RSXML::Note>& notes ) {
		try {
			
			if( source.empty() ) { throw Base::VectorEmptyException( "Chord" ); }
			if( notes.empty() ) { throw Base::VectorEmptyException( "Note" ); }
			/* Debug corner!
				for( auto it = notes.begin(); it != notes.end(); ++it ) { 
					std::cerr << "Note " << it - notes.begin() << ": \tPitch: " << (unsigned int)it->GetPitch() ENDLINE
				} */
			for( auto& arr : source ) {
				// We need to compare the generate templates here.
				unsigned int chordTemplate = 0;
				// Acquire notes and pitches via indices.
				std::array<unsigned int, NUMSTRINGS> cIndexes = arr.GetNotesIndex();
				std::array<RSXML::Note, NUMSTRINGS> cNotes;
				std::array<unsigned char, NUMSTRINGS> cPitches;
				std::array<unsigned char, NUMSTRINGS> tFrets;
				// for( std::array<unsigned int, NUMSTRINGS>::iterator it = cIndexes.begin(); it != cIndexes.end(); ++it ) {
				for( unsigned char i = 0; i < NUMSTRINGS; ++i ) {
					if( cIndexes[i] != CHORDERROR ) { cNotes[i] = notes.at( cIndexes[i] ); }
					/* Debug corner!
						std::cerr << "Note " << (unsigned int)i << ": \tIndex: " << (unsigned int)cIndexes[i] 
						<< " \tPitch: " << (unsigned int)cNotes[i].GetPitch()
						<< " \tString: " << (unsigned int)cNotes[i].GetString()
						<< " \tFret: " << (unsigned int)cNotes[i].GetFret() ENDLINE */
					cPitches[i] = cNotes[i].GetPitch();
					tFrets[i] = cNotes[i].GetFret();
				}
				if( !temp.empty() ) {
					for( std::vector<RSXML::ChordTemplate>::iterator it = temp.begin(); it != temp.end(); ++it ) {
						++chordTemplate;
						if( cPitches == it->ConvertFrets2Pitches( tuning ) ) { 
							chordTemplate = it - temp.begin(); 
							it = temp.end();
						}
					}
				} else { ++chordTemplate; }
				if( chordTemplate == temp.size() ) {	
					// Frets acquired above.
					std::array<unsigned char, NUMSTRINGS> tFingers = RSXML::ChordTemplate::ConvertFrets2Fingers( tFrets );
					std::string displayName = arr.chordName;
					RSXML::ChordTemplate newTemplate( tFrets, tFingers, arr.chordName, displayName, chordTemplate ); 
					temp.push_back( newTemplate );
				}

				Base::Chord base( arr );
				RSXML::Chord rsxml( base, arr.values, arr.index, chordTemplate );
				chord.push_back( rsxml );
			}
		} catch( Base::VectorEmptyException e ) {
			std::cerr << e.what() << "\n";
		} catch( std::exception e ) { std::cerr << e.what() << "\n"; }
	}

}