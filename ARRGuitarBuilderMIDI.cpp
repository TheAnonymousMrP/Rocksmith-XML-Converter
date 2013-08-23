#ifndef ARR_GUITAR_BUILDER
#include "ARRGuitarBuilder.h"
#endif

#ifndef DEBUG_STUFF
#include "debug.h"
#endif

/* The intent here is to take all the 'raw' information of a MIDI track and create an 
arrangement general enough to be written to a number of formats, but specifically a Rocksmith XML). */

namespace ARR {
	// Overarching process for building an ARR::Guitar object from a MIDI::Track.
	const ARR::Guitar2 ARR::GuitarBuilderMIDI::Build( const MIDI::Track& track ) {
		GuitarBuilderMIDI::track = track;

		Guitar2 arrangement( track.duration, track.name );
		arrangement.SetTempos( track.GetTempos() );
		arrangement.SetTimeSigs( track.GetTimeSigs() );
		ConvertSpecialMetas( arrangement, track );
		arrangement.SetPhrases( BuildPhrases( track.GetMetaStrings( Base::eMeta::PHRASE ) ) );
		arrangement.SetSections( BuildSections( track.GetMetaStrings( Base::eMeta::MARKER ) ) );

		// Simple pass-through to RSXML::Guitar2.
		arrangement.SetAnchors( track.GetAnchors() );
		arrangement.SetEvents( track.GetMetaStrings( Base::eMeta::EVENT ) );

		// Temporary note data.
		std::vector<ARR::Note> notes( BuildNotes( track, arrangement.tuning ) );
		std::vector<ARR::Chord2> chords;

		// Shit gets real. Build difficulties.
		std::vector<ARR::Difficulty2> difficulties;
		for( unsigned int i = 0; i <= track.GetMaxDifficulty(); ++i ) {
			Difficulty2 d = BuildDifficulty( i, notes, chords );
			difficulties.push_back( d );
		};

		arrangement.SetDifficulties( difficulties );

		return arrangement;
	};

	// Handles the occurence of special meta events, such as alternate tunings, bass arrangements, and arrangement-wide flags.
	void ARR::GuitarBuilderMIDI::ConvertSpecialMetas( ARR::Guitar2& arrangement, const MIDI::Track& track ) const {
		using Base::eTuning;
		using Base::aTuning;
		using Base::bassTuning;
		auto& mSpecial( track.GetMetaStrings( eMeta::SPECIAL ) );
		eTuning tuning = eTuning::STANDARD_E;
		for( auto& m : mSpecial ) {
			std::string content = m.GetString();
			if( content == "TStandardE" ) { tuning = eTuning::STANDARD_E; }
			else if( content == "TDropD" ) { tuning = eTuning::DROP_D; }
			else if( content == "TStandardEb" ) { tuning = eTuning::STANDARD_EB; }
			else if( content == "TOpenG" ) { tuning = eTuning::OPEN_G; }
			else if( content.at( 0 ) == 'Q' ) { 
				try {
					std::string quantizeS = content.substr( 1 );
					unsigned int quantizeI = std::stoi( quantizeS );
					arrangement.quantize = quantizeI;
				} catch( const std::invalid_argument& e ) { 
					if( debug ) { std::cerr << e.what() << "\n"; }
				} catch( const std::out_of_range& e ) {
					if( debug ) { std::cerr << e.what() << "\n"; }
				}
			} else if( content == "Bass" ) { arrangement.SetAsBass(); }
		}
		if( arrangement.IsBass() ) { arrangement.tuning = bassTuning[ tuning ]; } 
		else { arrangement.tuning = aTuning[ tuning ]; }
	};

	// Builds phrase from meta strings.
	const std::vector<ARR::Phrase> ARR::GuitarBuilderMIDI::BuildPhrases( const std::vector<Base::MetaString>& source ) const {
		std::vector<ARR::Phrase> phrases;
		try {
			if( source.empty() ) { throw Base::VectorEmptyException( "MetaString", "ARR::GuitarBuilderMIDI::CreatePhrases" ); }
			for( std::vector<Base::MetaString>::const_iterator it = source.begin(); it != source.end(); ++it ) {
				float duration = 0.000f;
				auto next = it + 1;
				if( next != source.end() ) { duration = next->GetTime() - it->GetTime(); }
				ARR::Phrase newPhrase( it->GetTime(), duration, it->GetString() );
				phrases.push_back( newPhrase );
			}
		} catch ( Base::VectorEmptyException e ) {
			if( debug ) { std::cerr << e.what() << "\n"; }
			ARR::Phrase badPhrase( 0.000f, 0.000f, "No phrases found." );
			phrases.push_back( badPhrase );
		}
		return phrases;
	};
	// Builds sections from MIDI markers.
	const std::vector<ARR::Section> ARR::GuitarBuilderMIDI::BuildSections( const std::vector<Base::MetaString>& source ) const {
		std::vector<ARR::Section> sections;
		try {
			if( source.empty() ) { throw Base::VectorEmptyException( "MetaString", "ARR::GuitarBuilderMIDI::CreateSections" ); }
			for( std::vector<Base::MetaString>::const_iterator it = source.begin(); it != source.end(); ++it ) {
				float duration = 0.000f;
				auto next = it + 1;
				if( next != source.end() ) { duration = next->GetTime() - it->GetTime(); }
				ARR::Section newSection( it->GetTime(), duration, it->GetString() );
				sections.push_back( newSection );
			}
		} catch ( Base::VectorEmptyException e ) {
			if( debug ) { std::cerr << e.what() << "\n"; }
			ARR::Section badPhrase( 0.000f, 0.000f, "No sections found." );
			sections.push_back( badPhrase );
		}
		return sections;
	};

	// Builds ARR::Notes from MIDI::Notes.
	std::vector<ARR::Note> ARR::GuitarBuilderMIDI::BuildNotes( const MIDI::Track& track, const Base::Tuning& tuning ) const {
		const std::vector<MIDI::Note>& nOn = track.GetNotes();
		const std::vector<MIDI::Note>& nOff = track.GetNotes( false );
		std::vector<ARR::Note> notes;
		
		try {
			if( nOn.empty() || nOff.empty() ) { throw Base::VectorEmptyException( "MIDI::Note", "ARR::GuitarBuilder::ConvertMIDI2ARRNotes" ); }
			auto itOn = nOn.begin(); 
			auto itOff = nOff.begin();
			for( itOn, itOff; itOn != nOn.end(); ++itOn ) {
				float duration = 0.000f;
				if( itOff != nOff.end() ) {
					// This bit could go terribly wrong.
					for( auto jtOff = itOff; jtOff != nOff.end(); ++jtOff ) {
						if( itOn->GetPitch() == jtOff->GetPitch() ) {
							duration = jtOff->GetTime() - itOn->GetTime();
							// If this is the first for-loop iteration, then we can safely move the Off iterator forwards.
							if( jtOff == itOff ) { ++itOff; } 
							break;
						}
					}
				ARR::Note newNote( itOn->GetTime(), itOn->GetString(), itOn->GetPitch(), itOn->normalisedDifficulty, notes.size() );
				newNote.SetFret( tuning );
				notes.push_back( newNote );
				}

			SetTechniques( notes, track.GetMetaStrings( Base::eMeta::TECHNIQUE ), ( tuning.instrument == Base::eInstrument::BASS ) ); 
			}
		} catch( Base::VectorEmptyException e ) {
			if( debug ) { std::cerr << e.what() << " | Notes On: " << nOn.size() << " NotesOff: " << nOff.size() << "\n"; }
			ARR::Note badNote;
			notes.push_back( badNote ); 
		}

		return notes;
	};

	void ARR::GuitarBuilderMIDI::SetTechniques( std::vector<ARR::Note>& notes, const std::vector<Base::MetaString>& techniques, const bool& isBass ) const {
		try {
			if( techniques.empty() ) { throw Base::VectorEmptyException( "Techniques", "ARR::GuitarBuilder::SetTechniques" ); }
			else if ( notes.empty() ) { throw Base::VectorEmptyException( "ARR::Note", "ARR::GuitarBuilder::SetTechniques" ); } // Should be impossible.
			/* As there will always be less techniques than notes, we will iterate over the former to reduce redundant searches -- NOT ANY MORE.
			Because techniques can apply to multiple notes with the introduction of chord techniques in RS2014, we will pass each note and iterate
			the techniques when we are sure we have passed its purview by. We know all meta-events and notes were created in a chronological order 
			(at least with MIDI we do), so we shouldn't have to worry about backtracking issues. 
			The meta-event approach may need to be reconsidered with the increasing complexity of techniques in Rocksmith 2014. */
			auto lastSuccess = techniques.begin();
			for( auto itNote = notes.begin(); itNote != notes.end(); ++itNote ) {
				for( auto itTech = lastSuccess; itTech != techniques.end(); ++itTech ) {
					if( itNote->GetTime() < itTech->GetTime() ) { break; }
					else if( itNote->GetTime() == itTech->GetTime() ) {
						std::string value = itTech->GetString();
						if( value == "A") { 
							itNote->values[eNoteTechniques::ACCENT] = true; 
						} else if( value == "B1" ) { 
							itNote->values[eNoteTechniques::BEND_HALF] = true; 
							itNote->bend = 1.000f;
						} else if( value == "B2" ) { 
							itNote->values[eNoteTechniques::BEND_FULL] = true; 
							itNote->bend = 2.000f;
						} else if( value == "FM" ) {
							itNote->values[eNoteTechniques::FRETHANDMUTE] = true;
						} else if( value == "HH" ) { 
							if( debug ) { std::cerr << itNote->GetTime() << ": Hammer on" ENDLINE }
							itNote->values[eNoteTechniques::HOPO] = true;
							itNote->values[eNoteTechniques::HOPO_ON] = true;
						} else if( value == "HP" ) {
							if( debug ) { std::cerr << itNote->GetTime() << ": Pull off"  ENDLINE }
							itNote->values[eNoteTechniques::HOPO] = true;
							itNote->values[eNoteTechniques::HOPO_OFF] = true; 
						} else if( value == "H" ) { 
							itNote->values[eNoteTechniques::HARMONIC] = true; 
						} else if( value == "PH" ) { 
							itNote->values[eNoteTechniques::PINCHHARMONIC] = true;
						} else if( value == "PM" ) {
							itNote->values[eNoteTechniques::PALMMUTE] = true;
						} else if( value == "T" ) {
							itNote->values[eNoteTechniques::TREMOLO] = true;
						// Slides are slightly trickier.	
						} else if( value.front() == 'S' ) { 
							itNote->values[eNoteTechniques::SLIDE] = true;
							if( value.size() > 1 ) {
								unsigned int buffer = std::stoi( value.substr( 1 ) );
								std::cerr << buffer ENDLINE
								if( buffer > 0 && buffer <= NUMFRETS ) { itNote->slide = buffer; }
								else { itNote->slide = 1; }
							} else {
								if( itNote + 1 != notes.end() ) {
									itNote->slide = ( itNote + 1 )->GetFret();
								} else { itNote->slide = 1; }
							}
							if( debug ) { std::cerr << itNote->GetTime() << ": Slide to " << (unsigned int)itNote->slide ENDLINE }
						} else if( value == "V" ) {
							itNote->values[ eNoteTechniques::VIBRATO ] = true;
						} else if( value == "BP" && isBass ) {
							itNote->values[ eNoteTechniques::BASS_PLUCK ] = true;
						} else if( value == "BS" && isBass ) {
							itNote->values[ eNoteTechniques::BASS_SLAP ] = true;
						} 
						/* IGNORE,
						LINKNEXT,
						PICK_DIRECTION,
						SLIDE_UNPITCH,
						TAP,
						TAP_LEFT,
						TAP_RIGHT */

						lastSuccess = itTech;
						break;
					} 
				}	
			}
		} catch( Base::VectorEmptyException e ) {
			if( debug ) { std::cerr << e.what() << "\n"; }
		}
	}; 

	const ARR::Difficulty2 ARR::GuitarBuilderMIDI::BuildDifficulty( const unsigned int& dif, const std::vector<ARR::Note>& notes, std::vector<ARR::Chord2>& chords ) const {
		std::vector< std::shared_ptr<ARR::Note> > notePointers;
		std::vector< std::shared_ptr<ARR::Chord2> > chordPointers;
		for( auto it = notes.begin(); it != notes.end(); ++it ) { 
			if( it->normalisedDifficulty <= dif ) { 
				std::shared_ptr<ARR::Note> note( new ARR::Note( *it ) );
				// Irrespective of difficulty, if the next note is not at the same time, we know it isn't a chord.
				if( ( it + 1 ) != notes.end() ) {
					auto jt = it + 1;
					ARR::Chord2 chordNotes;
					for( jt; jt != notes.end(); ++jt ) {
						if( it->GetTime() != jt->GetTime() ) { break; }
						else if( jt->normalisedDifficulty <= dif ) { 
							chordNotes.Add( *jt ); 
						}
					}
					if( chordNotes.GetPointers().size() ) { notePointers.push_back( note ); }
					else {
						ARR::Chord2 newChord( chordNotes );
					}
				} else { 
					notePointers.push_back( note );
				}
			} 
		}

		ARR::Difficulty2 newDifficulty( notePointers, chordPointers, dif );
		return newDifficulty;
	};

}