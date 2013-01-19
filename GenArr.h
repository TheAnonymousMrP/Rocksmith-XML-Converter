#ifndef _BASE_ARRANGE_
#include "BaseArrange.h"
#endif

/* The intent here is to take all the 'raw' information of a track and 
create an arrangement ready to be written to the appropriate file (ie, a
Rocksmith XML). */
	
class CreateArrangement
	{
	// Private
	Track track;
	Arrangement a;
	
	float timer;
	std::vector<Beat> vBeats;
	std::vector<Difficulty> vDifficulties;
	std::vector<Section> vSections;
	std::vector<Phrase> vPhrases;
	std::vector<Chord> vChords;
	std::vector<ChordTemplate> vChordTemplate;
	std::vector<Note> vNotes;
	std::vector<Note> allNotes;
	
	std::vector<Meta> metaX;
	std::vector<int> difList;
	
	// Process Methods
	void techniques();
	void createBeatGrid();
	void createDifficulties();
	void createPhrases();
	void createSections();
	
	void difficulty(int i);
	void chords();

	template <class T>
	void assignIDs(std::vector<T> source);
	
	public:
		CreateArrangement() { };
		CreateArrangement(Track t);
		~CreateArrangement() { };
		
		void process();
		
		Arrangement getArrangement() { return a; };
	};
	
CreateArrangement::CreateArrangement(Track t) 
	{ 
	track = t; 
	Arrangement a(t);
	
	allNotes = track.getNotes();
	}