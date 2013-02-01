#ifndef _BASE_ARRANGE_
#include "BaseArrange.h"
#endif

/* The intent here is to take all the 'raw' information of a track and 
create an arrangement ready to be written to the appropriate file (ie, a
Rocksmith XML). */
	
class CreateArrangement {
	// Private
	Arrangement arr;
	Track track;
	eTrackType trackType = combo;
	eTuning tuning = standardE;
	
	bool anchors;
	
	float timer;
	std::vector<Beat> vBeats;
	std::vector<Difficulty> vDifficulties;
	std::vector<Section> vSections;
	std::vector<Phrase> vPhrases;
	std::vector<ChordTemplate> vChordTemplate;
	std::vector<Chord> vChords;
	std::vector<Anchor> vAnchors;
	std::vector<HandShape> vHands;
	std::vector<Note> vNotes;
	
	std::vector<int> difList;
	
	// Process Methods
	void createBeatGrid();
	void getTuning();
	void compileNotes();
	Difficulty createDifficulty(int i);
		Chord createChord(const std::vector<Note>& nSource, 
			std::vector<Note>::iterator& it, int chordSize);
		void createAnchors(Difficulty& d, int dif);

	void createPhrases();
	void createSections();
	
	public:
		CreateArrangement(const Track& t) : arr(t), track(t) { 
			if(t.getMetas(eMeta::anchor).size() > 0) { anchors = true; }
			else { anchors = false; }
		};
		~CreateArrangement() { };
		
		void process();
		
		const Arrangement& getArrangement() const { return arr; };
};