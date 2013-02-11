#include "GenArr.h"

// Functions		
template <class T>
void assignIDs(std::vector<T>& source) {
	/* This process should sort through a list of X and assign IDs as
	appropriate. */
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

/* void CreateArrangement::assignID(Chord c, std::vector<ChordTemplate> source) 
	{
	bool match = false;
	match = false;
	for(auto it = source.begin(); it != source.end(); ++it) 
		{ if(it->name == jt->name) { match = true; break; } }
	if(!match) { it->setID(); }
	} */

// Public methods
void CreateArrangement::process() {
	// Beat grid is independent of anything else.
	createBeatGrid();
	
	// First things first, we need to sort out the tuning.
	getTuning();

	vNotes = track.getNotes();
	for(Note& n : vNotes) { n.setFret(); }
	/* We need to apply the various techniques that may be applicable to a 
	note before adding it to the Arrangement. */
	setTechniques();
	
	/* If anchors are pre-existing, process them now; like notes, they will 
	be distributed per difficulty later on. If they're not pre-existing,
	there's a basic algorithm to automatically generate them later on. */
	if(anchors) {
	
	}
	createAnchors();
	
	/* Now we can create each difficulty. For this, we process the notes (and 
	identify if they should be chords or not); the anchors (if none exist, we
	auto-generate them); and the handshapes. */
	std::ofstream file;
	file.open("Test/debug.txt"); // This is the file we're writing to.
	for(unsigned int i = 0; i <= track.getMaxDif(); ++i)
		{ vDifficulties.push_back(createDifficulty(i,file)); }
	file.close();
	/* Sections and Phrases are static across difficulties.
	As such, they are next at the top of hierarchy below 'Arrangement'. 
	While both sections and phrases may repeat in a song, phrases are 
	complicated by the presence of linkedDiffs, and that phrases may share a
	name or ID yet not the same notes. For now, linked sections and phrases
	may share an ID yet there will be no attempt to optimise the contents 
	there-in. */
	createPhrases(); // stores phrases in vPhrases.
	createSections(); // stores sections in vSections.
	/* Difficulties present an issue insofar as a certain phrase may 'skip' 
	difficulties; similarly, optimising contents above the maximum difficulty 
	of a phrase is necessary. */

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

// Private methods
void CreateArrangement::createBeatGrid(){
	auto vT(track.getTempos());
	int beat = 0; // Beat counter.
	int bar = 0; // Bar counter.
	auto tCount = vT.begin();
	float timer = tCount->time;
	while( timer < arr.getDuration() ) {
		/* Debug corner!
		std::cout << "tCount: " << (tCount - vT.begin()) 
		<< " tEnd: " << vT.end() - vT.begin() << " Timer: " 
		<< timer << " | Duration: " << arr.getDuration()
		<< " | Bar: " << bar << " | Tempo: " << tCount->tempo ENDLINE */
		if( tCount != (vT.end() - 1) && timer >= (tCount + 1)->time ) 
			{ ++tCount; } 
		
		Beat b;
		b.time = timer;
		b.tempo = tCount->tempo;
		if(beat % DEFAULTTIMESIGNUM == 0)
			{ b.bar = bar; ++bar; beat = 0; }
		else { b.bar = -1; }
		vBeats.push_back(b);
		
		timer += convertTempo2Beat(tCount->tempo);
		++beat;
	}
	arr.setBeats(vBeats);
}
	
void CreateArrangement::getTuning() {
	auto mSpecial(track.getMetas(eMeta::special));
	eTuning t = eTuning::standardE;
	for(Meta m : mSpecial)
		{
		if(m.text == "TDropD")
			{ t = eTuning::dropD; break; }
		else if(m.text == "TStandardE")
			{ t = eTuning::standardE; break;}
		}
	arr.setTuning(t); Note::setTuning(t);
}

void CreateArrangement::setTechniques() {
	auto mTech(track.getMetas(eMeta::tech));
	if(mTech.size() > 0) {
		float nTime = 0, xTime = 0; eTechnique t = none;
		/* As there will always be less techniques than notes, we will iterate
		over the former to reduce redundant searches. We know all meta-events 
		and notes were created in a chronological order (at least with MIDI 
		we do), so we don't have to worry about backtracking issues. */
		auto jt = vNotes.begin();
		for(auto it = mTech.begin(); it != mTech.end(); ++it) {
			xTime = it->time;
			/* We shouldn't need to reset this iterator to the beginning ever.
			That said, if any meta events are misaligned with the note, 
			we're buggered. */
			for(jt = jt; jt != vNotes.end(); ++jt) {
				nTime = jt->getTime();
				if(nTime == xTime) {
					if(it->text == "A") { t = accent; }
					else if(it->text == "B1")
						{ t = bendHalf; jt->setBend(1.0); }
					else if(it->text == "B2")
						{ t = bendFull; jt->setBend(2.0); }
					else if(it->text == "H") { t = harmonic; }
					else if(it->text == "PH") { t = pinchHarmonic; }
					else if(it->text == "HH") { t = hammerOn; }
					else if(it->text == "HP") { t = pullOff; }
					else if(it->text == "PM") { t = palmMute; }
					else if(it->text == "T") { t = tremolo; }
					// Slides are slightly trickier.	
					else if(it->text == "S") { 
						t = slide;
						std::vector<Note>::iterator nextNote = jt;
						++nextNote;
						jt->slide = nextNote->getFret();
						// We also want to skip this next note.
						++jt;
					}
					
					jt->setTechnique(t,0); // Needs to implement difficulties.
					xTime = 0.0; nTime = 0.0; t = none; break;
				}
			}	
		}
	}
}	

void CreateArrangement::createAnchors() {
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
	
Difficulty CreateArrangement::createDifficulty(unsigned int dif, std::ostream& file) {
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

Chord CreateArrangement::createChord2( const std::vector<Note>& notes, 
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

Chord CreateArrangement::createChord(const std::vector<Note>& nSource, 
	std::vector<Note>::iterator it, int chordSize)
	{
	std::vector<Meta> mChords(track.getMetas(eMeta::chord)); // Chord info.
	std::vector<Note> notes;
	std::vector<unsigned int> index;
	auto jt = it + chordSize;
	for(it = it; it != jt && it != nSource.end(); ++it) {
		notes.push_back((*it));
		index.push_back(it - nSource.begin());
	}
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
	Chord c(index, time, id);
	return c;
	}

void CreateArrangement::createPhrases() {
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
		/* The maxDif is needed both for the template's field and for
			optimising the 'printing' of the file. */
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
	
void CreateArrangement::createSections() {
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
		
	

