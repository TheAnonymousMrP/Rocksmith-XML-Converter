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
	
	// Scans the vector containing all notes to identify unique difficulties.
	bool match = false;
	for(auto it = vNotes.begin(); it != vNotes.end(); ++it) {
		match = false;
		for(auto jt = vNotes.begin(); jt != it; ++jt) {
			if(it->getDif() == jt->getDif())
				{ match = true; break; }
		}
		if(!match) { difList.push_back(it->getDif()); }
	}
	// Sorts the vector into order.
	sort(difList.begin(),difList.end());
	
	/* We need to apply the various techniques that may be applicable to a 
	note before adding it to the Arrangement. */
	compileNotes();
	
	/* If anchors are pre-existing, process them now; like notes, they will 
	be distributed per difficulty later on. If they're not pre-existing,
	there's a basic algorithm to automatically generate them later on. */
	if(anchors) {
	
	}
	
	/* Now we can create each difficulty. For this, we process the notes (and 
	identify if they should be chords or not); the anchors (if none exist, we
	auto-generate them); and the handshapes. */
	for(int& i : difList)
		{ vDifficulties.push_back(createDifficulty(i)); }
	arr.setDifficulties(vDifficulties);	
		
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
}

// Private methods
void CreateArrangement::createBeatGrid(){
	std::vector<Tempo> vT(track.getTempos());
	int beat = 0; // Beat counter.
	int bar = 0; // Bar counter.

	auto jt = vT.begin(); ++jt;
	for(auto it = vT.begin(); it != vT.end(); ++it, ++jt) {
		timer = it->time;
		while(timer < jt->time)
			{
			Beat b;
			b.time = timer;
			b.tempo = it->tempo;
			if(beat % DEFAULTTIMESIGNUM == 0)
				{ b.bar = bar; ++bar; }
			else { b.bar = -1; }
			vBeats.push_back(b);
			
			timer += convertTempo2Beat(it->tempo);
			++beat;
			}
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

void CreateArrangement::compileNotes() {
	vNotes = track.getNotes(); 
	for(Note& n : vNotes) { 
		n.setFret(); 
		// Create a normalised difficulty.
		for(auto it = difList.begin(); it != difList.end(); ++it) {
			if(n.getDif() == (*it)) 
				{ n.normalisedDif = (it - difList.begin()); break; }
		}
	}
	vector<Meta> mTech(track.getMetas(eMeta::tech));
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
	arr.setNotes(vNotes);
}	
	
Difficulty CreateArrangement::createDifficulty(int dif) {
	Difficulty d; d.setDifficulty();
	// Identifying chords.
	auto it = vNotes.begin();
	while(it != vNotes.end()) {
		if(it->getDif() <= dif) {
			int chordSize = isChord(vNotes, it, dif);
			if(chordSize == 1) { d.addNoteI(it - vNotes.begin()); }
			else { 
				vChords.push_back(createChord(vNotes, it, chordSize));
				d.addChordI(vChords.size() - 1); 
			}
			it += chordSize;
		}
		else { ++it; }
	}
	// Generating anchors. 
	if(anchors) {
		/* for(const Meta& m : track.getMetas(eMeta::anchor)) {
			// Somehow reduce number based on difficulty. 
		} */
	}
	// Very basic auto-generation of anchors.
	else { createAnchors(d, dif); }
	
	return d;
}

Chord CreateArrangement::createChord(const std::vector<Note>& nSource, 
	std::vector<Note>::iterator& it, int chordSize)
	{
	std::vector<Meta> mChords(track.getMetas(eMeta::chord)); // Chord info.
	std::vector<Note> notes;
	std::vector<unsigned int> index;
	for(it = it; it != it + chordSize && it != vNotes.end(); ++it) {
		notes.push_back((*it));
		index.push_back(it - vNotes.begin());
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
	
void CreateArrangement::createAnchors(Difficulty& d, int dif) {
	int low = 0; bool newAnchor = true;
	for(auto it = vNotes.begin(); it != vNotes.end(); ++it) {
		int chordSize = isChord(vNotes, it, dif);
		int fret = it->getFret();
		if(chordSize > 1) {
			auto n(getXsWithinRange(vNotes, it, chordSize));	
			low = Note::findLowestFret(n);
		}
		else if(fret < low) { low = fret; }
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
			d.addAnchorI(vAnchors.size()-1);
			if(chordSize > 1) { it += chordSize - 1; }
		}
		else { newAnchor = true; }
	}
}

void CreateArrangement::createPhrases() {
	std::vector<Meta> mPhrase(track.getMetas(eMeta::phrase));

	std::vector<Meta>::iterator it = mPhrase.begin(); ++it;
	for(Meta& m : mPhrase)
		{
		Phrase p(m);
		if(it == mPhrase.end())
			{ p.duration = track.duration - m.time; }
		else { p.duration = it->time - m.time; ++it; }	

		/* By scanning the vector containing all notes within a time frame, 
		we can derive the maximum difficulty of the contents.
		And by cross-referencing this with the difficulty list created earlier,
		we know how many difficulties need to be produced in a given phrase. 
		
		This isn't perfect, as it implies there are no difficulty 'jumps', 
		but for now it'll do. */
		auto tempI(getIsWithinTime(vNotes, m.time, p.duration));
		auto tempN(getXsWithinTime(vNotes, m.time, p.duration));
		int max = findMaxDif(tempN);
		p.setMaxDif(max);
		// DANGER ZONE
		// p.startNoteI = tempI.front(); p.endNoteI = tempI.back();
		vPhrases.push_back(p);
		}
	assignIDs(vPhrases);
}
	
void CreateArrangement::createSections() {
	vector<Meta> mSection(track.getMetas(eMeta::marker));
	
	vector<Meta>::iterator it = mSection.begin(); ++it;
	for(Meta& m : mSection) {
		Section s(m);
		if(it == mSection.end())
			{ s.duration = track.duration - m.time; }
		else { s.duration = it->time - m.time; ++it; } 
		
		s.addPhrases(getXsWithinTime(vPhrases, m.time, s.duration));
		
		vSections.push_back(s);
	}
	assignIDs(vSections);
	arr.setSections(vSections);
}
		
	

