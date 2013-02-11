#ifndef _BASE_ARRANGE_
#include "BaseArrange.h"
#endif

/* The intent here is to take all the 'raw' information of a track and 
create an arrangement ready to be written to the appropriate file (ie, a
Rocksmith XML). */
	
class ARRCreate {
	// Private
	Arrangement arr;
	Track track;
	eTrackType trackType = combo;
	eTuning tuning = standardE;
	
	bool anchors;
	
	float timer;
	std::vector<Beat> vBeats;
	std::vector<ChordTemplate> vChordTemplate;
	std::vector<PhraseTemplate> vPhraseTemplate;
	std::vector<Phrase> vPhrases;
	std::vector<Section> vSections;
	std::vector<Difficulty> vDifficulties;
	std::vector<Note> vNotes;
	std::vector<Chord> vChords;
	std::vector<Anchor> vAnchors;
	std::vector<HandShape> vHands;
	
	// Process Methods
	void createBeatGrid();
	void getTuning();
	void setTechniques();
	void createAnchors();
	Difficulty createDifficulty(unsigned int i, std::ostream& file);
		Chord createChord(const std::vector<Note>& nSource, 
			std::vector<Note>::iterator it, int chordSize);
		Chord createChord2( const std::vector<Note>& notes, 
			const std::vector<unsigned int>& indexes );

	void createPhrases();
	void createSections();
	
	public:
		ARRCreate(const Track& t) : arr(t), track(t) { 
			if(t.getMetas(eMeta::anchor).size() > 0) { anchors = true; }
			else { anchors = false; }
		};
		
		void process();
		
		const Arrangement& getArrangement() const { return arr; };
};