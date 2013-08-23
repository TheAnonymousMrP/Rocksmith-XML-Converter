#ifndef RSXML_CREATE_GUITAR
#include "RSXMLGuitarBuilder.h"
#endif

#ifndef DEBUG_STUFF
#include "debug.h"
#endif

namespace RSXML {
	const RSXML::Guitar GuitarBuilder::Create( const ARR::Guitar& arrg ) {
		Guitar rsg( arrg.GetDuration(), arrg.GetName(), arrg.IsBass() );

		// Global/meta data.
		rsg.SetTempos( arrg.GetTempos() );
		rsg.SetBeats( CreateBeats( arrg ) );
		// Converts pre-existing anchors. See line 34 onwards if none exist.
		if( !arrg.GetAnchors().empty() ) { rsg.SetAnchors( ConvertAnchors( arrg.GetAnchors() ) ); }
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

		// Difficulty
		std::vector<RSXML::Difficulty> difficulties = ConvertARR2RSXMLDifficulty( arrg.GetDifficulties() );
			// Anchors
			if( arrg.GetAnchors().empty() ) {
				std::vector<RSXML::Anchor> anchors;
				CreateAnchors( anchors, difficulties, rsg.GetNotes(), rsg.GetChords() ); 
				rsg.SetAnchors( anchors );
			}
			// HandShapes
			std::vector<RSXML::HandShape> handShapes;
			CreateHandShapes( handShapes, difficulties, rsg.GetChords(), rsg.GetChordTemplates() );
			rsg.SetHandShapes( handShapes );
		rsg.SetDifficulties( difficulties );

		// Phrases
		std::vector<RSXML::PhraseTemplate> pTemplates;
		std::vector<RSXML::Phrase> pIterations;
		ConvertARR2RSXMLPhrases( arrg.GetPhrases(), pTemplates, pIterations, rsg.GetNotes() );
		rsg.SetPhrases( pTemplates );
		rsg.SetPhraseIterations( pIterations );
			
		return rsg;
	}

	// Creates ebeat vector.
	const std::vector<RSXML::Beat> GuitarBuilder::CreateBeats( const Base::Guitar& g ) {	
		std::vector<RSXML::Beat> beats;	
		try {
			const std::vector<Base::Tempo>& tempos( g.GetTempos() );
			const std::vector<Base::TimeSig>& timeSigs( g.GetTimeSigs() );
			if( tempos.empty() ) { throw Base::VectorEmptyException( "Tempo", "RSXML::GuitarBuilder::CreateBeats" ); } 
			else if( g.GetTimeSigs().empty() ) { throw Base::VectorEmptyException( "Time Signature", "RSXML::GuitarBuilder::CreateBeats" ); }

			std::vector<Base::Tempo>::const_iterator tempoIt = tempos.begin();
			std::vector<Base::TimeSig>::const_iterator timeSigIt = timeSigs.begin();
		
			int beat = 0; // Beat counter.
			int bar = 0; // Bar counter.
			float timer = tempoIt->GetTime();
			
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
				int bBar = 0;
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

	// Converts pre-existing anchors (Base::MetaUInt) to RSXML::Anchor vector.
	const std::vector<RSXML::Anchor> GuitarBuilder::ConvertAnchors( const std::vector<Base::MetaUInt>& source ) {
		std::vector<RSXML::Anchor> dest;
		try {
			if( source.empty() ) { throw Base::VectorEmptyException( "Base::Anchor", "RSXML::GuitarBuilder::ConvertAnchors" ); }
			for( auto& e : source ) { 
				dest.push_back( RSXML::Anchor( e.GetTime(), e.GetUInt(), DEFAULTANCHORWIDTH, dest.size() ) );
			}
		} catch( Base::VectorEmptyException e ) { 
			if( debug ) { std::cerr << e.what() << "\n"; } 
		}
		return dest;
	}

	// Creates RSXML::Event vector from Base::MetaString vector.
	const std::vector<RSXML::Event>	GuitarBuilder::ConvertEvents( const std::vector<Base::MetaString>& source ) { 
		std::vector<RSXML::Event> dest;
		try {
			if( source.empty() ) { throw Base::VectorEmptyException( "Base::Event", "RSXML::GuitarBuilder::ConvertEvents" ); }
			for( auto& e : source ) { 
				dest.push_back( RSXML::Event( e.GetTime(), e.GetString() ) );
			}
		} catch( Base::VectorEmptyException e ) {
			if( debug ) { std::cerr << e.what() << "\n"; }
		}
		return dest;
	}

	// Converts ARR::Phrase vector into RSXML::Phrase vector.
	void GuitarBuilder::ConvertARR2RSXMLPhrases( const std::vector<ARR::Phrase>& source, 
		std::vector<RSXML::PhraseTemplate>& temp, std::vector<RSXML::Phrase>& phrase, const std::vector<RSXML::Note>& notes ) { 
		try {
			if( source.empty() ) { throw Base::VectorEmptyException( "ARR::Phrase", "RSXML::GuitarBuilder::ConvertARR2RSXMLPhrases" ); }
			for( auto& p : source ) {
				unsigned int phraseID = 0;
				if( !temp.empty() ) {
					for( std::vector<RSXML::PhraseTemplate>::iterator it = temp.begin(); it != temp.end(); ) {
						++phraseID;
						if( p.GetName() == it->GetName() ) { 
							phraseID = it - temp.begin(); 
							it = temp.end();
						} else { ++it; }
					}
				}
				if( phraseID == temp.size() ) {	
					unsigned int maxDifficulty = 0;
					std::vector<RSXML::Note> phraseNotes;
					try {
						phraseNotes = GetLevelObjectsWithinTime( notes, p.GetTime(), p.GetTime() + p.GetDuration() );
					} catch( Base::VectorEmptyException e ) { if( debug ) { std::cerr << e.what() << "\n"; } }
					for( auto& n : phraseNotes ) { if( n.normalisedDifficulty > maxDifficulty ) { maxDifficulty = n.normalisedDifficulty; } }
					RSXML::PhraseTemplate newTemplate( p.GetName(), maxDifficulty, false, false, false, phraseID ); 
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
		// std::cerr << "Phrases: " << phrase.size() << " \tTemplates: " << temp.size() << "\n";
	}
	
	// Converts ARR::Section vector into RSXML::Section vector.
	const std::vector<RSXML::Section> GuitarBuilder::ConvertARR2RSXMLSections( const std::vector<ARR::Section>& source ) { 
		std::vector<RSXML::Section> dest;
		try {
			if( source.empty() ) { throw Base::VectorEmptyException( "ARR::Section", "RSXML::GuitarBuilder::ConvertARR2RSXMLSections" ); } 
			for( auto& arr : source ) {
				unsigned int iteration = 1;
				// Compare by name to previous sections.
				for( auto it = dest.begin(); it != dest.end(); ++it ) { if( arr.GetName() == it->GetName() ) { ++iteration; } }
				RSXML::Section newSection( arr.GetTime(), arr.GetName(), iteration );
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
	const std::vector<RSXML::Note> GuitarBuilder::ConvertARR2RSXMLNotes( const std::vector<ARR::Note>& source ) {
		// At present, ARR and RSXML notes are the same. This may not be the case in the future.
		
		std::vector<RSXML::Note> dest;
		try {
			if( source.empty() ) { throw Base::VectorEmptyException( "ARR::Note", "RSXML::GuitarBuilder::ConvertARR2RSXMLNotes" ); }
			for( auto& arr : source ) {	
				// We strip out any notes with the 'Fret Hand Mute' attribute set. For now, anyway. A program-wide flag may be introduced for this.
				if( arr.values[ RSXML::Note::eValues::FRETHANDMUTE ] == false ) {
					RSXML::Note rsxml( arr );
					dest.push_back( rsxml );
				}
			}
		} catch( Base::VectorEmptyException e ) {
			std::cout << e.what() << "\n";
			RSXML::Note badNote;
			dest.push_back( badNote ); 
		}
		return dest;
	}

	// Converts ARR::Chord vector into RSXML::Chord & RSXML::ChordTemplate vectors.
	void GuitarBuilder::ConvertARR2RSXMLChords( const std::vector<ARR::Chord>& source, std::vector<RSXML::ChordTemplate>& temp, 
		std::vector<RSXML::Chord>& chord, const Base::Tuning& tuning, const std::vector<RSXML::Note>& notes ) {
		try {
			if( source.empty() ) { throw Base::VectorEmptyException( "ARR::Chord", "RSXML::GuitarBuilder::ConvertARR2RSXMLChords" ); }
			if( notes.empty() ) { throw Base::VectorEmptyException( "ARR::Note", "RSXML::GuitarBuilder::ConvertARR2RSXMLChords" ); }
			for( auto& arr : source ) {
				// We need to compare the generate templates here.
				unsigned int chordTemplate = 0;
				// Acquire notes and pitches via indices.
				std::array<unsigned int, GUITARSTRINGS> cIndexes = arr.GetNotesIndex();
				std::array<RSXML::Note, GUITARSTRINGS> cNotes;
				std::array<unsigned char, GUITARSTRINGS> cPitches;
				std::array<unsigned char, GUITARSTRINGS> tFrets;
				// for( std::array<unsigned int, GUITARSTRINGS>::iterator it = cIndexes.begin(); it != cIndexes.end(); ++it ) {
				for( unsigned char i = 0; i < GUITARSTRINGS; ++i ) {
					if( cIndexes[i] != CHORDERROR ) { cNotes[i] = notes.at( cIndexes[i] ); }
					/* Debug corner!
						std::cerr << "Note " << (unsigned int)i << ":  Index: " << (unsigned int)cIndexes[i] 
						<< " \tTime: " << cNotes[i].GetTime()
						<< " \tPitch: " << (unsigned int)cNotes[i].GetPitch()
						<< " \tString: " << (unsigned int)cNotes[i].GetString()
						<< "  Fret: " << (unsigned int)cNotes[i].GetFret() ENDLINE */
					cPitches[i] = cNotes[i].GetPitch();
					tFrets[i] = cNotes[i].GetFret();
				}
				if( !temp.empty() ) {
					for( std::vector<RSXML::ChordTemplate>::iterator it = temp.begin(); it != temp.end(); ) {
						++chordTemplate;
						if( tFrets == it->GetFrets() ) { 
							chordTemplate = it - temp.begin();
							it = temp.end();
						} else { ++it; }
					}
				}
				if( chordTemplate == temp.size() ) {	
					// Frets acquired above.
					std::array<unsigned char, GUITARSTRINGS> tFingers = RSXML::ChordTemplate::ConvertFrets2Fingers( tFrets );
					std::string displayName = arr.chordName;
					RSXML::ChordTemplate newTemplate( tFrets, tFingers, arr.chordName, displayName, chordTemplate ); 
					temp.push_back( newTemplate );
				}

				Base::Chord base( arr );
				RSXML::Chord rsxml( base, arr.values, chordTemplate, arr.index );
				chord.push_back( rsxml );
			}
		} catch( Base::VectorEmptyException e ) {
			if( debug ) { std::cerr << e.what() << "\n"; }
		} catch( std::exception e ) { if( debug ) { std::cerr << e.what() << "\n"; } }
		// std::cerr << "Chords: " << chord.size() << " \tTemplates: " << temp.size() << "\n";
	}

	// Converts ARR::Difficulty vector into RSXML::Difficulty vector. Additionally creates anchors if none already exist.
	const std::vector<RSXML::Difficulty> GuitarBuilder::ConvertARR2RSXMLDifficulty( const std::vector<ARR::Difficulty>& source ) {
		std::vector<RSXML::Difficulty> dest;
		try {
			if( source.empty() ) { throw Base::VectorEmptyException( "ARR::Difficulty", "RSXML::GuitarBuilder::ConvertARR2RSXMLDifficulty" ); }
			for( auto& arr : source ) {	
				RSXML::Difficulty rsxml( arr.GetLength(), arr.GetDifficulty() );
				rsxml.SetNotesIndex( arr.GetNotesIndex() );
				rsxml.SetChordsIndex( arr.GetChordsIndex() );
				// rsxml.SetAnchorsIndex( );
				// rsxml.SetHandShapesIndex( );
				dest.push_back( rsxml );
			}
		} catch( Base::VectorEmptyException e ) {
			std::cout << e.what() << "\n";
			RSXML::Difficulty badDifficulty;
			// Should add a note just to be safe. There will always be at least one note because of error prevention in Convert...Notes.
			std::vector<unsigned int> badNoteIndex;
			badNoteIndex.push_back( 0 );
			badDifficulty.SetNotesIndex( badNoteIndex );
			dest.push_back( badDifficulty ); 
		}
		return dest;
	}

	// Creates RSXML::Anchor vector algorithmicly. Processed after difficulties are created.
	void GuitarBuilder::CreateAnchors( std::vector<RSXML::Anchor>& anchors, 
		std::vector<RSXML::Difficulty>& difficulties, const std::vector<RSXML::Note>& notes, const std::vector<RSXML::Chord>& chords ) {
		try {
			if( difficulties.empty() ) { throw Base::VectorEmptyException( "RSXML::Difficulty", "RSXML::GuitarBuilder::CreateAnchors" ); }
			if( notes.empty() ) { throw Base::VectorEmptyException( "RSXML::Note", "RSXML::GuitarBuilder::CreateAnchors" ); }
			if( chords.empty() ) { throw Base::VectorEmptyException( "RSXML::Chord", "RSXML::GuitarBuilder::CreateAnchors" ); }

			/* The aim is to:
				- Scroll through the full notes vector to find suitable points to place an anchor.
				- Identify which anchors are relevant per difficulty.

				Possible method:
				- Produce an anchor per note (excluding repeated frets/pitches) in a buffer.
				- Identify anchors per difficulty, only adding those to the final anchor vector.
					- Chords should get their own anchor, or at least should if the width extends 
					beyond the width of the pre-existing anchor.
					- Will need to process chords separately and sort them into the main index after.
				- Working from most-to-least difficulty could provide a more logical indexing 
				order, which == profit???
				Advantage: By starting with more than needed, a situation where two difficulties
				have massively conflicting anchors are avoided. 
				Optional Development: On stage 2, identify the appropriate anchor width. */
			std::vector<RSXML::Anchor> buffer;
			buffer.push_back( RSXML::Anchor() );
			RSXML::Note lastNote( -1.000f );
			for( std::vector<RSXML::Note>::const_iterator it = notes.begin(); it != notes.end(); ++it ) {
				if( it->GetTime() != lastNote.GetTime() || it->GetFret() != lastNote.GetFret() ) {
					buffer.push_back( RSXML::Anchor( it->GetTime(), it->GetFret(), DEFAULTANCHORWIDTH, buffer.size() ) );
				}
			}
			if( buffer.empty() ) { throw Base::VectorEmptyException( "RSXML::Anchor", "RSXML::GuitarBuilder::CreateAnchors" ); }

			for( std::vector<RSXML::Difficulty>::reverse_iterator it = difficulties.rbegin(); it != difficulties.rend(); ++it ) {
				std::vector<unsigned int> bufferIndexNotes;
				std::vector<unsigned int> bufferIndexChords;
		
				// Scan notes index.
				try {
					// if( it->GetNotesIndex().empty() ) { throw Base::VectorEmptyException( "Difficulty::NoteIndexes", "RSXML::GuitarBuilder::CreateAnchors" ); }
					const std::vector<unsigned int>& indexes = it->GetNotesIndex();
					
					// No need to redundantly search the anchor buffer vector.
					std::vector<RSXML::Anchor>::iterator anchorIterator = buffer.begin();
					RSXML::Anchor lastAnchor( -1.000f, 0xFF, DEFAULTANCHORWIDTH, 0 );
					unsigned char lastFret( lastAnchor.GetFret() ); 
					
					for( std::vector<unsigned int>::const_iterator jt = indexes.begin(); jt != indexes.end(); ++jt ) {
						if( *jt >= notes.size() ) { throw RSXML::LevelObject::IndexRangeError( "RSXML::Note", "GuitarBuilder::CreateAnchor" ); }
						const RSXML::Note currentNote( notes.at( *jt ) );
						const unsigned char currentFret( currentNote.GetFret() );

						if( currentFret != lastFret && currentFret != 0 ) {
							if( ( currentFret < lastFret ) || ( currentFret >= ( lastFret + lastAnchor.GetWidth() ) ) ) {
								// Scan buffer for relevant anchor.
								for( auto kt = anchorIterator; kt != buffer.end(); ++kt ) {
									if( currentNote.GetTime() == kt->GetTime() ) { 
										bufferIndexNotes.push_back( kt->GetIndex() ); 
										anchorIterator = kt;
										kt = buffer.end() - 1;
									}
								}
								lastFret = currentFret;
								lastAnchor = *anchorIterator;
							}	
						} 
					}
				} catch( RSXML::LevelObject::IndexRangeError e ) { if( debug ) { std::cerr << e.what() << "\n"; } }

				// Scan Chords index.
				try {
					const std::vector<unsigned int>& indexes = it->GetChordsIndex();
					// if( indexes.empty() ) { throw Base::VectorEmptyException( "Difficulty::ChordIndexes", "RSXML::GuitarBuilder::CreateAnchors" ); }

					// No need to redundantly search the anchor buffer vector.
					std::vector<RSXML::Anchor>::iterator anchorIterator = buffer.begin();
					RSXML::Anchor lastAnchor( -1.000f, 0xFF, DEFAULTANCHORWIDTH, 0 );
					unsigned char lastLowFret( lastAnchor.GetFret() ); 
					unsigned char lastHighFret( 0 );

					for( std::vector<unsigned int>::const_iterator jt = indexes.begin(); jt != indexes.end(); ++jt ) {
						if( *jt >= chords.size() ) { throw RSXML::LevelObject::IndexRangeError( "RSXML::Chord", "GuitarBuilder::CreateAnchor" ); }
						const RSXML::Chord currentChord( chords.at( *jt ) );
						unsigned char lowFret = 0xFF;
						unsigned char highFret = 0;
						for( auto& i : currentChord.GetNotesIndex() ) { 
							if( i != CHORDERROR ) {
								if( notes.at( i ).GetFret() < lowFret && notes.at( i ).GetFret() != 0 ) { lowFret = notes.at( i ).GetFret(); }
								if( notes.at( i ).GetFret() > highFret && notes.at( i ).GetFret() <= NUMFRETS ) { highFret = notes.at( i ).GetFret(); }
							}
						}
						// Lowest fret cannot be 0; highest fret cannot exceed the maximum support fret.
						if( lowFret == 0xFF && highFret == 0 ) {
							// Accounting for a very unlikely scenario. Should only ever be the first index.
							bufferIndexChords.push_back( anchorIterator->GetIndex() );
							lastAnchor = *anchorIterator;
						} else if( 
							lowFret < lastLowFret ||
							lowFret >= lastLowFret + lastAnchor.GetWidth() ||
							( lowFret > lastLowFret && highFret >= lastLowFret + lastAnchor.GetWidth() ) 
							) { 
								// Scan buffer for relevant anchor.
								for( auto kt = anchorIterator; kt != buffer.end(); ++kt ) {
									if( currentChord.GetTime() == kt->GetTime() ) { 
										bufferIndexChords.push_back( kt->GetIndex() ); 
										anchorIterator = kt;
										kt = buffer.end() - 1;
									}
								}
								lastLowFret = lowFret;
								lastHighFret = highFret;
								lastAnchor = *anchorIterator;

						} else if( lowFret == lastLowFret && highFret >= lowFret + lastAnchor.GetWidth() ) {
							// When the anchor fret doesn't change yet the highest chord fret increases.
							/* This scenario will require a new, larger-width anchor.
							As variable-width anchors aren't yet implemented in Rocksmith, it will be ignored for now.
							
							Suggested implementation: 
							- Second chord buffer.
							- After sorting note buffer and primary chord buffer into combined buffer. 
							- Insert (not sort) second chord buffer into combined buffer.

							Alternatively:
							- Second chord buffer ( could use vector<array<index,width>> ).
							- After sorting note & chord & second chord buffer into final combined index vector.
							- Create second RSXML::Anchor buffer, copying 1-to-1 from the first, changing the 
							anchor width by cross-referencing indexes with second chord buffer. 
								- This could cause cross-difficulty conflicts, however. That said, because 
								anchor width is mostly an aesthetic feature, it's not the end of the world,
								though admittedly potentially confusing and not in the spirit of things. */
						}
					}
				} catch( RSXML::LevelObject::IndexRangeError e ) { if( debug ) { std::cerr << e.what() << "\n"; } }

				// Sort note & chord buffers.
				std::vector<unsigned int> bufferIndex = bufferIndexNotes;
				for( auto& i : bufferIndexChords ) { bufferIndex.push_back( i ); }
				sort( bufferIndex.begin(), bufferIndex.end() );

				it->SetAnchorsIndex( bufferIndex );
			}

			anchors = buffer;
		} catch( Base::VectorEmptyException e ) { if( debug ) { std::cerr << e.what() << "\n"; } }

		if( anchors.empty() ) {
			RSXML::Anchor badAnchor;
			anchors.push_back( badAnchor );
			std::vector<unsigned int> index;
			index.push_back( 0 );
			for( auto& d : difficulties ) { d.SetAnchorsIndex( index ); }
		}
	}
	
	// Creates RSXML::HandShape vector algorithmicly. Processed after difficulties are created.
	void GuitarBuilder::CreateHandShapes( std::vector<RSXML::HandShape>& handShapes, std::vector<RSXML::Difficulty>& difficulties,
		const std::vector<RSXML::Chord>& chords, const std::vector<RSXML::ChordTemplate>& templates ) {
		try {
			if( chords.empty() ) { throw Base::VectorEmptyException( "RSXML::Chord", "GuitarBuilder::CreateHandShapes" ); }
			if( difficulties.empty() ) { throw Base::VectorEmptyException( "RSXML::Difficulty", "GuitarBuilder::CreateHandShapes" ); }

		} catch( Base::VectorEmptyException e ) { if( debug ) { std::cerr << e.what() << "\n"; } }

		if( handShapes.empty() ) {
			RSXML::HandShape badHand( 0.000f, 0, 0.000f, 0 );
			handShapes.push_back( badHand );
			std::vector<unsigned int> index;
			index.push_back( 0 );
			for( auto& d : difficulties ) { d.SetHandShapesIndex( index ); }
		}
	}
}