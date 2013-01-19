#ifndef _BASE_TRACK_
#define _BASE_TRACK_

#ifndef _BASE_
#include "Base.h"
#endif

class Track
	{
	// Private
	std::string trackName;
	float duration;
	std::vector<Note> notes;
	std::vector<Note> noteOn;
	std::vector<Note> noteOff;
	
	std::vector<Meta> mAnchor; // This holds anchor-related data.
	std::vector<Meta> mBend; // This holds Pitch Bend-derived data.
	std::vector<Meta> mChord; // This holds chord names.
	std::vector<Meta> mMarker; // This hold markers, used for sections.
	std::vector<Meta> mPhrase; // This holds event data for phrases.
	std::vector<Meta> mTech; // This holds all technique-related meta-events.
	std::vector<Meta> mLyrics;
	std::vector<Meta> mSpecial;
	
	std::vector<Tempo> mTempo; // Tempo changes.
	std::vector<TimeSig> mTimeSig; // Time Signature changes.
	
	public:
		Track();
		Track(const Track& t);
		~Track() { };

		void addNote(Note nOn, Note nOff);
		int addNoteI(Note nOn, Note nOff);
		void addNoteOn(Note n);
		void addNoteOff(Note n);
		void addMeta(int i, Meta m);
		void addTempo(float time, float tempo);
		void addTimeSig(TimeSig t);
		
		std::string getName();
		float getDuration();
		Note getNote(int i);
		int getNoteSize();
		std::vector<Note> getNotes();
		// std::vector<Note> getNotesWithinTime(float a, float b);
		
		Meta getMeta(int type, int i);
		int getMetaSize(int type);
		std::vector<Meta> getMetas(int type);	
		
		Tempo getTempo(int i); 
		std::vector<Tempo> getTempos();
		TimeSig getTimeSig(int i);
		std::vector<TimeSig> getTimeSigs();
		
		void setName(std::string s);
		void setDuration(float f);
		
		void sortNotes(); // Reduce the On and Off vectors to a single vector.
	};
	
Track::Track() { trackName = ""; duration = 0; }

Track::Track(const Track& t)
	{
	trackName = t.trackName;
	duration = t.duration;
	
	std::vector<Note> notes(t.notes);
	std::vector<Note> noteOn(t.noteOn);
	std::vector<Note> noteOff(t.noteOn);
	
	std::vector<Meta> mAnchor(t.mAnchor);
	std::vector<Meta> mBend(t.mBend);
	std::vector<Meta> mChord(t.mChord);
	std::vector<Meta> mMarker(t.mMarker);
	std::vector<Meta> mPhrase(t.mPhrase); 
	std::vector<Meta> mTech(t.mTech);
	std::vector<Meta> mLyrics(t.mTech);
	std::vector<Meta> mSpecial(t.mSpecial);
	}
	
// Public ====

// Add methods
void Track::addNote(Note nOn, Note nOff)
	{ Note n(nOn); n.setDuration(nOff.getTime()); notes.push_back(n); }	
int Track::addNoteI(Note nOn, Note nOff) 
	{ addNote(nOn, nOff); return getNoteSize(); }
void Track::addNoteOn(Note n) { noteOn.push_back(n); }
void Track::addNoteOff(Note n) { noteOff.push_back(n); }
void Track::addMeta(int i, Meta m)
	{ 
	switch(i)
		{
		case eAnchor: mAnchor.push_back(m); break;
		case eBend: mBend.push_back(m); break;
		case eChord: mChord.push_back(m); break;
		case eLyrics: mLyrics.push_back(m); break;
		case eMarker: mMarker.push_back(m); break;
		case ePhrase: mPhrase.push_back(m); break;
		case eTech: mTech.push_back(m); break;
		case eSpecial: mSpecial.push_back(m); break;
		}
	}
void Track::addTempo(float time, float tempo) 
	{ 
	Tempo t;
	t.time = time;
	t.tempo = tempo;
	mTempo.push_back(t); 
	};
void Track::addTimeSig(TimeSig t) { mTimeSig.push_back(t); }

// Get methods
std::string Track::getName() { return trackName; }
float Track::getDuration() { return duration; }
Note Track::getNote(int i)
	{
	if(i < notes.size()) { return notes.at(i); }
	else { Note n; return n; }
	}
int Track::getNoteSize() { return notes.size(); }
std::vector<Note> Track::getNotes() { return notes; }
/* std::vector<Note> Track::getNotesWithinTime(float a, float b)
	{
	std::vector<Note> n;
	float f;
	for(std::vector<Note>::iterator it = notes.begin();
		it != notes.end(); ++it)
		{
		Note cN = *it;
		f = cN.getTime();
		if(f > b) { return n; }
		if(f >= a) { n.push_back(cN); }
		}
	} */
	
Meta Track::getMeta(int type, int i)
	{ 
	switch(type)
		{
		case eAnchor: return mAnchor.at(i); break;
		case eBend: return mBend.at(i); break;
		case eChord: return mChord.at(i); break;
		case eLyrics: return mLyrics.at(i); break;
		case eMarker: return mMarker.at(i); break;
		case ePhrase: return mPhrase.at(i); break;
		case eTech: return mTech.at(i); break;
		case eSpecial: return mSpecial.at(i); break;
		}
	}
int Track::getMetaSize(int type)
	{ 
	switch(type)
		{
		case eAnchor: return mAnchor.size(); break;
		case eBend: return mBend.size(); break;
		case eChord: return mChord.size(); break;
		case eLyrics: return mLyrics.size(); break;
		case eMarker: return mMarker.size(); break;
		case ePhrase: return mPhrase.size(); break;
		case eTech: return mTech.size(); break;
		case eSpecial: return mSpecial.size(); break;
		}
	}
std::vector<Meta> Track::getMetas(int type)
	{ 
	switch(type)
		{
		case eAnchor: return mAnchor; break;
		case eBend: return mBend; break;
		case eChord: return mChord; break;
		case eLyrics: return mLyrics; break;
		case eMarker: return mMarker; break;
		case ePhrase: return mPhrase; break;
		case eTech: return mTech; break;
		case eSpecial: return mSpecial; break;
		}
	}
	
Tempo Track::getTempo(int i) 
	{ 
	if(i < mTempo.size()) { return mTempo.at(i); }
	else 
		{ 
		Tempo t;
		t.time = -1;
		t.tempo = -1;
		// cout << "Incorrect tempo count." ENDLINE
		return t;
		}
	}
std::vector<Tempo> Track::getTempos() { return mTempo; }
TimeSig Track::getTimeSig(int i)
	{ }
std::vector<TimeSig> Track::getTimeSigs() { return mTimeSig; }


// Set methods
void Track::setName(std::string s) { trackName = s; }
void Track::setDuration(float f) { duration = f; }

// Miscellaneous methods
void Track::sortNotes() // Convert the On and Off vectors into a single vector.
	{
	if(noteOn.size() == noteOff.size()) // To avoid mis-matches.
		{
		for(std::vector<Note>::iterator it = noteOn.begin();
			it != noteOn.end(); ++it)
			{
			addNote(*it, noteOff.at(it - noteOn.begin()));
			}
		}
	}
	
#endif
