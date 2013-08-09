#ifndef ARR_CREATE_GUITAR
#include "ARRCreateGuitar.h"
#endif

#ifndef DEBUG_STUFF
#include "debug.h"
#endif

namespace ARR {
	const ARR::Guitar CreateGuitar::Create( const MIDI::Track& t, const bool& bass ) { 
		track = t;
		ARR::Guitar g( t.duration, t.name, bass );
		
		// Global data.
		g.SetTempos( t.GetTempos() );
		g.SetTimeSigs( t.GetTimeSigs() );
		g.SetPhrases( CreatePhrases( t.GetMetaStrings( Base::eMeta::PHRASE ) ) );
		g.SetSections( CreateSections( t.GetMetaStrings( Base::eMeta::MARKER ) ) );
		g.SetAnchors( t.GetAnchors() );
		g.SetEvents( t.GetMetaStrings( Base::eMeta::EVENT ) );
		ConvertSpecialMetas( g );
		
		// Note data.
		std::vector<ARR::Note> notes = ConvertMIDI2ARRNotes( t.GetNotes() );
		std::vector<ARR::Note> notesOff = ConvertMIDI2ARRNotes( t.GetNotes( 0 ) );
		
		// Sorting into a single vector. If there's a mismatch of on and off, 
		// it skips the lot. Better safe than sorry.
		if( notes.size() == notesOff.size() ) {
			auto jt = notesOff.begin();
			for( auto it = notes.begin(); it != notes.end(); ++it, ++jt ) {
				it->duration = jt->GetTime() - it->GetTime();
			}
		} else { 
			for( auto& n : notes ) { n.duration = 0.000f; } 
		}
		
		// Apply techniques.
		SetTechniques( notes );
		
		std::vector<ARR::Chord> chords;
		std::vector<ARR::Difficulty> difficulties;

		// Debug corner.
		// std::cout << "Number of difficulties: " << track.GetMaxDifficulty() ENDLINE

		for( unsigned int i = 0; i <= track.GetMaxDifficulty(); ++i ) {
			Difficulty d = CreateDifficulty( i, notes, chords );
			difficulties.push_back( d );
		};

		g.SetNotes( notes );
		g.SetChords( chords );
		g.SetDifficulties( difficulties );
		
		return g;
	};
	
	// Handles the occurence of special meta events, such as alternate tunings.
	void CreateGuitar::ConvertSpecialMetas( ARR::Guitar& g ) const {
		using Base::eTuning;
		using Base::aTuning;
		auto& mSpecial( track.GetMetaStrings( eMeta::SPECIAL ) );
		eTuning tun = eTuning::STANDARD_E;
		for( auto& m : mSpecial ) {
			if( m.GetString() == "TStandardE" ) { tun = eTuning::STANDARD_E; break; }
			else if( m.GetString() == "TDropD" ) { tun = eTuning::DROP_D; break; }
			else if( m.GetString() == "TStandardEb" ) { tun = eTuning::STANDARD_EB; break; }
			else if( m.GetString() == "TOpenG" ) { tun = eTuning::OPEN_G; break; }
		}
		g.tuning = aTuning[tun];
	}
	
	// Produces ARR::Phrase vector from Base::MetaString vector.
	const std::vector<ARR::Phrase> CreateGuitar::CreatePhrases( const std::vector<Base::MetaString>& source ) const {
		std::vector<ARR::Phrase> dest;
		try {
			if( source.empty() ) { throw Base::VectorEmptyException( "MetaString", "ARR::CreateGuitar::CreatePhrases" ); }
			for( std::vector<Base::MetaString>::const_iterator it = source.begin(); it != source.end(); ++it ) {
				float duration = 0.000f;
				auto jt = it + 1;
				if( jt != source.end() ) { duration = jt->GetTime() - it->GetTime(); }
				ARR::Phrase newPhrase( it->GetTime(), duration, it->GetString() );
				dest.push_back( newPhrase );
			}
		} catch ( Base::VectorEmptyException e ) {
			if( debug ) { std::cerr << e.what() << "\n"; }
			ARR::Phrase badPhrase( 0.000f, 0.000f, "No phrases found." );
			dest.push_back( badPhrase );
		}
		return dest;
	}

	// Produces ARR::Section vector from Base::MetaString vector.
	const std::vector<ARR::Section> CreateGuitar::CreateSections( const std::vector<Base::MetaString>& source ) const {
		std::vector<ARR::Section> dest;
		try {
			if( source.empty() ) { throw Base::VectorEmptyException( "MetaString", "ARR::CreateGuitar::CreateSections" ); }
			for( std::vector<Base::MetaString>::const_iterator it = source.begin(); it != source.end(); ++it ) {
				float duration = 0.000f;
				auto jt = it + 1;
				if( jt != source.end() ) { duration = jt->GetTime() - it->GetTime(); }
				ARR::Section newSection( it->GetTime(), duration, it->GetString() );
				dest.push_back( newSection );
			}
		} catch ( Base::VectorEmptyException e ) {
			if( debug ) { std::cerr << e.what() << "\n"; }
			ARR::Section badPhrase( 0.000f, 0.000f, "No sections found." );
			dest.push_back( badPhrase );
		}
		return dest;
	}
	
	std::vector<ARR::Note> CreateGuitar::ConvertMIDI2ARRNotes( std::vector<MIDI::Note> source ) const {
		std::vector<ARR::Note> dest;
		try {
			if( source.empty() ) { throw Base::VectorEmptyException( "MIDI::Note", "ARR::CreateGuitar::ConvertMIDI2ARRNotes" ); }
			/* We intentionally 'slice' the MIDI note to a base note and then add it to 
			the ARR note vector. Provides an index for the note. */
			for( auto& midi : source ) {	
				Base::GuitarNote base( midi ); 
				ARR::Note arr( base, dest.size() );
				arr.SetFret();
				dest.push_back( arr );
			}
		} catch( Base::VectorEmptyException e ) {
			if( debug ) { std::cerr << e.what() << "\n"; }
			ARR::Note badNote;
			dest.push_back( badNote ); 
		}
		return dest;
	}
	
	void CreateGuitar::SetTechniques( std::vector<ARR::Note>& notes ) const {
		auto& techniques( track.GetMetaStrings( eMeta::TECHNIQUE ) );
		try {
			if( techniques.empty() ) { throw Base::VectorEmptyException( "Techniques", "ARR::CreateGuitar::SetTechniques" ); }
			else if ( notes.empty() ) { throw Base::VectorEmptyException( "ARR::Note", "ARR::CreateGuitar::SetTechniques" ); } // Should be impossible.
			/* As there will always be less techniques than notes, we will iterate
			over the former to reduce redundant searches. We know all meta-events 
			and notes were created in a chronological order (at least with MIDI 
			we do), so we don't have to worry about backtracking issues. */
			auto nIt = notes.begin();
			for( auto& tech : techniques ) {
				/* We shouldn't need to reset this iterator to the beginning ever. That 
				said, if any meta events are misaligned with the note, we're buggered. */
				for( nIt = nIt; nIt != notes.end(); ++nIt ) {
					if( nIt->GetTime() == tech.GetTime() ) {
						std::string value = tech.GetString();
						if( value == "A") { 
							nIt->values[eNoteValues::ACCENT] = true; 
						} else if( value == "B1") { 
							nIt->values[eNoteValues::BEND_HALF] = true; 
							nIt->bend = 1.000f;
						} else if( value == "B2") { 
							nIt->values[eNoteValues::BEND_FULL] = true; 
							nIt->bend = 2.000f;
						} else if( value == "HH") { 
							nIt->values[eNoteValues::HOPO] = true;
							nIt->values[eNoteValues::HOPO_ON] = true;
						} else if( value == "HP") {
							nIt->values[eNoteValues::HOPO] = true;
							nIt->values[eNoteValues::HOPO_OFF] = true; 
						} else if( value == "H") { 
							nIt->values[eNoteValues::HARMONIC] = true; 
						} else if( value == "PH") { 
							nIt->values[eNoteValues::PINCHHARMONIC] = true;
						} else if( value == "PM") {
							nIt->values[eNoteValues::PALMMUTE] = true;
						} else if( value == "T") {
							nIt->values[eNoteValues::TREMOLO] = true;
						// Slides are slightly trickier.	
						} else if( value == "S") { 
							nIt->values[eNoteValues::SLIDE] = true;
							if( nIt != ( notes.end() - 1 ) ) {
								nIt->slide = ( nIt + 1 )->GetFret(); ++nIt;
							} else { nIt->slide = 1; }
						}
						/* FRETHANDMUTE,
						IGNORE,
						LINKNEXT,
						PICK_DIRECTION,
						SLIDE_UNPITCH,
						TAP,
						TAP_LEFT,
						TAP_RIGHT,
						VIBRATO,
						BASS_PLUCK,
						BASS_SLAP, */
					} else { break; }
				}	
			}
		} catch( Base::VectorEmptyException e ) {
			if( debug ) { std::cerr << e.what() << "\n"; }
		}
	}
	
	const ARR::Difficulty CreateGuitar::CreateDifficulty( const unsigned int& dif, const std::vector<ARR::Note>& notes, std::vector<ARR::Chord>& chords ) const {
		std::vector<unsigned int> index;
		std::vector<unsigned int> notesIndex;
		std::vector<unsigned int> chordsIndex;
		for( const Note& n : notes ) { 
			if( n.normalisedDifficulty <= dif ) { index.push_back( n.GetIndex() ); } 
		}
		std::vector<unsigned int>::iterator noteIt = index.begin();
		while( noteIt != index.end() ) {
			unsigned int chordSize = 0;
			auto chordIt = noteIt + 1;

			while( chordIt != index.end() && notes.at( *chordIt ).GetTime() == notes.at( *noteIt ).GetTime() ) { 
				// std::cout << "Chord Size: " << chordSize << " ChordIt: " << chordIt - index.begin() << " Note Time: " << notes.at( *chordIt ).GetTime() ENDLINE
				++chordSize;
				++chordIt;
			}	

			if( chordSize == 0 ) { notesIndex.push_back( *noteIt ); }
			else {
				chordsIndex.push_back( chords.size() );
				chords.push_back( CreateChord( notes, chords, *noteIt, chordSize ) );
			}
			noteIt += chordSize + 1;
		} 

		Difficulty d( notes.at( index.back() ).GetTime(), dif );
		d.SetNotesIndex( notesIndex );
		d.SetChordsIndex( chordsIndex );
		return d;
	}
	
	const ARR::Chord CreateGuitar::CreateChord( const std::vector<ARR::Note> notes, const std::vector<ARR::Chord>& chords,
			const unsigned int& noteIt, const unsigned char& chordSize ) const {
		float time = notes.at( noteIt ).GetTime();
		std::array<unsigned int, NUMSTRINGS> indexes = Base::DEFAULTINDEX;

		for( unsigned int i = noteIt; i <= noteIt + chordSize; ++i ) { indexes[ i - noteIt ] = i; }

		// Finding a name for the chord.
		std::string name;
		bool match = false;
		for( auto& c : chords ) {
			if( c.GetNotesIndex() == indexes ) { match = true; name = c.chordName; break; }
		}	
		if( !match ) {	
			auto& chordNames = track.GetMetaStrings( Base::eMeta::CHORD );
			for( auto& n : chordNames ) { 
				if( n.GetTime() == time ) { name = n.GetString(); break; } 
			}
		}
				
		ARR::Chord newChord( time, indexes, name, chords.size() );
		return newChord;
	}
};

/* Functions		
template <class T>
void assignIDs(std::vector<T>& source) {
	// This process should sort through a list of X and assign IDs as appropriate.
	bool match = false;
	for(auto it = source.begin(); it != source.end(); ++it) {
		match = false;
		for(auto jt = it - 1; jt >= source.begin() ; --jt) {
		// for(auto jt = source.begin(); jt != it ; ++jt) {
			if(it->getName() == jt->getName())
				{ it->setID(jt->getID()); match = true; break; }
		}
		if(!match) { it->setID(); }
	}
}

// void ARRCreate::assignID(Chord c, std::vector<ChordTemplate> source) 
	{
	bool match = false;
	match = false;
	for(auto it = source.begin(); it != source.end(); ++it) 
		{ if(it->name == jt->name) { match = true; break; } }
	if(!match) { it->setID(); }
	} 
	
void ARRCreate::process() {
	// Beat grid is independent of anything else.
	createBeatGrid();
	
	// First things first, we need to sort out the tuning.
	getTuning();

	vNotes = track.getNotes();
	for(Note& n : vNotes) { n.setFret(); }
	// We need to apply the various techniques that may be applicable to a 
	// note before adding it to the Arrangement. 
	setTechniques();
	
	// If anchors are pre-existing, process them now; like notes, they will 
	// be distributed per difficulty later on. If they're not pre-existing,
	// there's a basic algorithm to automatically generate them later on. 
	if(anchors) {
	
	}
	createAnchors();
	
	// Now we can create each difficulty. For this, we process the notes (and 
	// identify if they should be chords or not); the anchors (if none exist, we
	// auto-generate them); and the handshapes. 
	std::ofstream file;
	file.open("Test/debug.txt"); // This is the file we're writing to.
	for(unsigned int i = 0; i <= track.getMaxDif(); ++i)
		{ vDifficulties.push_back(createDifficulty(i,file)); }
	file.close();
	// Sections and Phrases are static across difficulties.
	// As such, they are next at the top of hierarchy below 'Arrangement'. 
	// While both sections and phrases may repeat in a song, phrases are 
	// complicated by the presence of linkedDiffs, and that phrases may share a
	// name or ID yet not the same notes. For now, linked sections and phrases
	// may share an ID yet there will be no attempt to optimise the contents 
	// there-in. 
	createPhrases(); // stores phrases in vPhrases.
	createSections(); // stores sections in vSections.
	// Difficulties present an issue insofar as a certain phrase may 'skip' 
	// difficulties; similarly, optimising contents above the maximum difficulty 
	// of a phrase is necessary. 

	arr.setDifficulties(vDifficulties);	
	arr.setNotes(vNotes);
	arr.setChords(vChords);
	arr.setAnchors(vAnchors);
	arr.setHands(vHands);
	arr.setChordTemplates(vChordTemplate);
	arr.setPhraseTemplates(vPhraseTemplate);
	arr.setPhrases(vPhrases);
	arr.setSections(vSections);
}

void ARRCreate::createAnchors() {
	int low = 1; bool newAnchor = true;
	for(auto it = vNotes.begin(); it != vNotes.end(); ++it) {
		int chordSize = isChord(vNotes, it, track.getMaxDif());
		int fret = it->getFret();
		if(chordSize > 1) {
			auto n(getXsWithinRange(vNotes, it, chordSize));
			fret = Note::findLowestFret(n, 1);
			if(fret == low) { newAnchor = false; }
			else { low = fret; }
		}
		else if(fret < low && low > 1) { low = fret; }
		else if(fret >= low + DEFAULTANCHORWIDTH) { low = fret; }
		// Bends tend to be on the third finger. Sometimes. I think.
		// else if(it->isBend() && fret > 2) { low = fret-2; }
		else { newAnchor = false; }
		
		if(newAnchor) {
			Anchor a; a.time = it->getTime();
			if(low == 0) { low = 1; }
			a.fret = low;
			a.width = DEFAULTANCHORWIDTH;
			
			vAnchors.push_back(a);
			if(chordSize > 1) { it += chordSize - 1; }
		}
		else { newAnchor = true; }
	}
}
	
Difficulty ARRCreate::createDifficulty(unsigned int dif, std::ostream& file) {
	Difficulty d;
	// Identifying chords.
	std::vector<Note> notes;
	
	file << "Difficulty: " << dif ENDLINE
	for(Note& n : vNotes) { if(n.minDif <= dif) { notes.push_back(n); } }
	auto anchorIt = vAnchors.begin();
	auto it = vNotes.begin();
	while(it < vNotes.end()) {
		if( it->minDif <= dif ) {
			file << "\tNote Time: " << it->getTime() << " Dif: " 
			<< it->minDif ENDLINE
			while( anchorIt->time < it->getTime() ) { ++anchorIt; }
			if( anchorIt->time == it->getTime() ) { 
				d.addAnchorI( (anchorIt - vAnchors.begin()) );
				++anchorIt;
			}
			auto jt = it;
			std::vector<Note> chordNotes;
			std::vector<unsigned int> chordRef;
			while( jt->getTime() == it->getTime() ) { 
				if( jt->minDif <= dif) { 
					chordNotes.push_back( *jt ); 
					chordRef.push_back( (jt - vNotes.begin()) );
				}
				++jt; 
			}
			if( chordNotes.size() == 1 ) { d.addNoteI( it - vNotes.begin() ); }
			else {
				vChords.push_back(createChord2( chordNotes, chordRef ));
				d.addChordI(vChords.size() - 1); 
			}
			it += chordNotes.size();
		} else { ++it; }
	} 
	return d;
}

Chord ARRCreate::createChord2( const std::vector<Note>& notes, 
	const std::vector<unsigned int>& indexes ) 
	{
	std::vector<Meta> mChords(track.getMetas(eMeta::chord)); // Chord info.
	
	float time = notes.at(0).getTime();
	// The Template.
	ChordTemplate newT(notes);
	bool match = false; int id = -1;
	for(ChordTemplate& oldT : vChordTemplate) { 
		if(newT.toString() == oldT.toString()) 
			{ id = oldT.getID(); match = true; break; } 
	}
	if(!match) { 
		for(Meta& m : mChords) 
			{ if(m.time == time) { newT.name = m.text; break; } }	
		newT.setID(); id = newT.getID(); vChordTemplate.push_back(newT); 
	}
	// The Chord.
	Chord c(indexes, time, id);
	return c;
}

void ARRCreate::createPhrases() {
	std::vector<Meta> mPhrase(track.getMetas(eMeta::phrase));

	std::vector<Meta>::iterator it = mPhrase.begin() + 1;
	for(Meta& m : mPhrase) {
		// std::cout << "Phrase time: " << m.time ENDLINE
		PhraseTemplate newT(m.text);
		bool match = false; 
		unsigned int id = 0; unsigned int count = 0;
		for(PhraseTemplate& oldT : vPhraseTemplate) { 
			if(newT.name == oldT.name) 
				{ id = oldT.getID(); count = ++oldT.inc; match = true; break; } 
		}
		// The maxDif is needed both for the template's field and for 
		// optimising the 'printing' of the file. 
		float duration = 0.0;
		if(it == mPhrase.end()) { duration = track.duration - m.time; }
		else { duration = it->time - m.time; ++it; }
				
		auto tempN(getXsWithinTime(vNotes, m.time, m.time + duration));
		unsigned int tempDif = findMaxDif(tempN);
		
		// Add 'new' phrases to the template list.
		if(!match) { 
			newT.maxDif = tempDif;
			newT.name = m.text;	
			newT.setID(); id = newT.getID(); vPhraseTemplate.push_back(newT); 
		}
		
		Phrase p(m, id, count);
		p.duration = duration;
		p.maxDif = tempDif;
				
		vPhrases.push_back(p);
	}
}

void ARRCreate::createSections() {
	std::vector<Meta> mSection(track.getMetas(eMeta::marker));
	
	std::vector<Meta>::iterator it = mSection.begin(); ++it;
	for(Meta& m : mSection) {
		// std::cout << "Section time: " << m.time ENDLINE
		Section s(m);
		if(it == mSection.end())
			{ s.duration = track.duration - m.time; }
		else { s.duration = it->time - m.time; ++it; } 
		
		s.addPhrases(getXsWithinTime(vPhrases, m.time, s.duration));
		
		vSections.push_back(s);
	}
	assignIDs(vSections);
}
*/
		
	

