#include "GenArr.h"

// Public methods
void CreateArrangement::process()
	{
	// First things first, we need to sort out the tuning.
	// Tuning process.
	
	
	/* We need to apply the various techniques that may be applicable to a 
	note. We may as well get it done first. */
	techniques();
	
	// Creating a beat grid now will pay off down the line.
	createBeatGrid();
	
	createDifficulties();
	
	/* Now we can start processing notes (and, specifically, identifying if
	they should be chords or not). This is a per-difficulty process. */
	for(std::vector<Difficulty>::iterator it = vDifficulties.begin();
		it != vDifficulties.end(); ++it)
		{ difficulty(it - vDifficulties.begin()); }
		
		
		
	
		
	/* Sections and Phrases are static across difficulties.
	As such, they are next at the top of hierarchy below 'Arrangement'. 
	While both sections and phrases may repeat in a song, phrases are 
	complicated by the presence of linkedDiffs, and that phrases may share a
	name or ID yet not the same notes. For now, linked sections and phrases
	may share an ID yet there will be no attempt to optimise the contents 
	there-in. */
	createSections(); // stores sections in vSections.
	createPhrases(); // stores phrases in vPhrases.
	/* Difficulties present an issue insofar as a certain phrase may 'skip' 
	difficulties; similarly, optimising contents above the maximum difficulty 
	of a phrase is necessary. */
	}

// Private methods
void CreateArrangement::techniques()
	{
	metaX.clear();
	vector<Meta> metaX(track.getMetas(tech));
	float nTime = 0, xTime = 0; eTechnique t = none;
	/* As there will always be less techniques than notes, we will iterate
	over the former to reduce redundant searches. We know all meta-events 
	and notes were created in a chronological order (at least with MIDI we do),
	so we don't have to worry about backtracking issues. */
	std::vector<Note>::iterator jt = allNotes.begin();
	for(std::vector<Meta>::iterator it = metaX.begin();
		it != metaX.end(); ++it)
		{
		Meta cM = *it;
		xTime = cM.time;
		/* We shouldn't need to reset this iterator to the beginning ever.
		That said, if any meta events are misaligned with the note, 
		we're buggered. */
		for(jt; jt != allNotes.end(); ++jt)
			{
			Note cN = *jt;
			nTime = cN.getTime();
			if(nTime == xTime)
				{
				if(cM.text == "B1")
					{ t = bendHalf; cN.setBend(1.0); }
				else if(cM.text == "B2")
					{ t = bendFull; cN.setBend(2.0); }
				else if(cM.text == "H")
					{ t = harmonic; }
				else if(cM.text == "HH")
					{ t = hammerOn; }
				else if(cM.text == "HP")
					{ t = pullOff; }
				else if(cM.text == "PM")
					{ t = palmMute; }
				else if(cM.text == "T")
					{ t = tremolo; }
				// Slides are slightly trickier.	
				else if(cM.text == "S")
					{ 
					t = slide;
					std::vector<Note>::iterator nextNote = jt;
					++nextNote;
					Note nN = *nextNote;
					cN.setSlide(nN.getFret());
					// We also want to skip this next note.
					++jt;
					}
				
				cN.setTechnique(t,0); // Needs to implement difficulties.
				
				t = none; break;
				}
			}
		// Just in case...
		if(jt == allNotes.end()) { jt = allNotes.begin(); }		
		}
		
	for(std::vector<Note>::iterator it = allNotes.begin(); 
		it != allNotes.end(); ++it) 
		{ Note cN = *it; cN.setFret(); }
	}
	
void CreateArrangement::createBeatGrid()
	{
	std::vector<Tempo> vT(track.getTempos());
	int beat = 0; // Beat counter.
	int bar = 0; // Bar counter.

	std::vector<Tempo>::iterator it = vT.begin();
	std::vector<Tempo>::iterator jt = it; ++jt;
	for(it; it != vT.end(); ++it, ++jt)
		{
		Tempo cT(*it); Tempo nT(*jt);
		timer = cT.time;
		float increment = convertTempo2TimeFloat(cT.tempo);
		while(timer < nT.time)
			{
			
			Beat b;
			b.time = timer;
			b.tempo = cT.tempo;
			if(beat % DEFAULTTIMESIGNUM == 0)
				{ b.bar = bar; ++bar; }
			else { b.bar = -1; }
			vBeats.push_back(b);
			
			timer += increment;
			++beat;
			}
		}
	}
	
void CreateArrangement::createDifficulties()
	{
	// Scans the vector containing all notes to identify unique difficulties.
	bool match = false;
	for(std::vector<Note>::iterator it = allNotes.begin();
		it != allNotes.end(); ++it)
		{
		match = false;
		Note x = *it;
		for(std::vector<Note>::iterator jt = allNotes.begin();
			jt != it; ++jt)
			{
			Note y = *jt;
			if(x.getDif() == y.getDif())
				{ match = true; break; }
			}
		if(!match) { difList.push_back(x.getDif()); }
		}
	
	// Sorts the vector into order.
	sort(difList.begin(),difList.end());
	
	// Now we can create a base list of all difficulties.
	for(std::vector<int>::iterator it = difList.begin();
		it != difList.end(); ++it)
		{ Difficulty d; d.setDifficulty(); }
	}
	
void CreateArrangement::createSections()
	{
	metaX.clear();
	vector<Meta> metaX(track.getMetas(marker));
	
	vector<Meta>::iterator it, jt;
	it = metaX.begin(); jt = it; ++jt;
	float end = 0;
	for(it; it != metaX.end(); ++it, ++jt)
		{
		Section x(*it);
		if(jt == metaX.end())
			{ end = track.getDuration() - x.getTime(); }
		else { Meta m = *jt; end = m.time; } 
		
		x.setDuration(end);	
		
		vSections.push_back(x);
		}
	assignIDs(vSections);
	}
		
void CreateArrangement::createPhrases()
	{
	metaX.clear();
	std::vector<Meta> metaX(track.getMetas(phrase));

	std::vector<Meta>::iterator it, jt;
	it = metaX.begin(); jt = it; ++jt;
	float end = 0;
	for(it; it != metaX.end(); ++it, ++jt)
		{
		Phrase p(*it);
		if(jt == metaX.end())
			{ end = track.getDuration() - p.getTime(); }
		else { Meta m = *jt; end = m.time; } 
		
		p.setDuration(end);	

		/* By scanning the vector containing all notes within a time frame, 
		we can derive the maximum difficulty of the contents.
		And by cross-referencing this with the difficulty list created earlier,
		we know how many difficulties need to be produced in a given phrase. 
		
		This isn't perfect, as it implies there are no difficulty 'jumps', 
		but for now it'll do. */
		std::vector<Note> tempN(getXsWithinTime(allNotes, p.getTime(), end));
		int max = findMaxDif(tempN);
		p.setMaxDif(max);
		
		vPhrases.push_back(p);
		}
	assignIDs(vPhrases);
	}	
	
void CreateArrangement::difficulty(int i)
	{
	Difficulty& d = vDifficulties.at(i);
	
	
	
	}

void CreateArrangement::chords()
	{
	metaX.clear();
	vector<Meta> metaX(track.getMetas(chord));
	}
	
/* void CreateArrangement::difficulties()
	{
	/*
		vector<Note> tempN(getXCopyWithinTime(vNotes, p.getTime(), end));
		p.addNotes(tempN);
		vector<Chord> tempC(getXCopyWithinTime(vChords, p.getTime(), end));
		p.addChords(tempC);
	} */
	
template <class T>
void CreateArrangement::assignIDs(std::vector<T> source)
	{
	/* This process should sort through a list of X and assign IDs as
	appropriate. */
	bool match = false;
	for(typename std::vector<T>::iterator it = source.begin();
		it != source.end(); ++it)
		{
		match = false;
		T x = *it;
		for(typename std::vector<T>::iterator jt = source.begin();
			jt != it; ++jt)
			{
			T y = *jt;
			if(x.getName() == y.getName())
				{ x.setID(y.getID()); match = true; break; }
			}
		if(!match) { x.setID(); }
		}
	}