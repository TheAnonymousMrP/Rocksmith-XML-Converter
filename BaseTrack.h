#ifndef _BASE_TRACK_
#define _BASE_TRACK_

#ifndef _BASE
#include "Base.h"
#endif

enum eTrackType
	{
	vocal,
	single,
	combo,
	chords,
	bass,
	bassPick,
	};

class Track {
	// Private
	
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
		Track(std::string n = "", float d = 0.0, 
			eTrackType t = eTrackType::combo)
			{ name = n; duration = d; type = t; };
		Track(const Track& t) : notes(t.notes), noteOn(t.noteOn), 
			noteOff(t.noteOff), mAnchor(t.mAnchor), mBend(t.mBend),
			mChord(t.mChord), mMarker(t.mMarker), mPhrase(t.mPhrase),
			mTech(t.mTech), mLyrics(t.mLyrics), mSpecial(t.mSpecial),
			mTempo(t.mTempo), mTimeSig(t.mTimeSig)
			{ name = t.name; duration = t.duration; type = t.type; };
		~Track() { };
		
		std::string name;
		float duration;
		eTrackType type;

		void addNoteOn(Note n);
		void addNoteOff(Note n);
		void addMeta(eMeta i, Meta m);
		void addTempo(float time, float tempo);
		void addTimeSig(TimeSig t);
		
		// Note getNote(int i);
		const int getNumNotes() const { return notes.size(); };
		std::vector<Note>& getNotes(); // Copy
		const std::vector<Note>& getNotes() const;
		
		// Meta getMeta(eMeta m, int i);
		int getMetaSize(eMeta m) const;
		const std::vector<Meta>& getMetas(eMeta m) const; // Copy	
		
		// const Tempo& getTempo(int i) const { return mTempo.at(i); }; 
		Tempo getTempo(int i) const {
			if(i < mTempo.size()) { return mTempo.at(i); }
			else { Tempo t; t.time = -1; return t; }
		};
		int getTempoSize() const { return mTempo.size(); };
		const std::vector<Tempo>& getTempos() const { return mTempo; };
		TimeSig getTimeSig(int i); 
		std::vector<TimeSig> getTimeSigs();
		
		void setMetas(std::vector<Meta> metas, eMeta m);
		void setTempos(std::vector<Tempo> tempos) { mTempo = tempos; };
		void setTimeSigs(std::vector<TimeSig> times) { mTimeSig = times; };
		
		void sortNotes(); // Reduce the On and Off vectors to a single vector.
	};
	
// Public ====
// Add methods
void Track::addNoteOn(Note n) { noteOn.push_back(n); }
void Track::addNoteOff(Note n) { noteOff.push_back(n); }
void Track::addMeta(eMeta i, Meta m)
	{ 
	switch(i)
		{
		case anchor: mAnchor.push_back(m); break;
		case bend: mBend.push_back(m); break;
		case chord: mChord.push_back(m); break;
		case lyrics: mLyrics.push_back(m); break;
		case marker: mMarker.push_back(m); break;
		case phrase: mPhrase.push_back(m); break;
		case tech: mTech.push_back(m); break;
		case special: mSpecial.push_back(m); break;
		}
	}
void Track::addTempo(float time, float tempo) { 
	Tempo t;
	t.time = time;
	t.tempo = tempo;
	mTempo.push_back(t); 
};
void Track::addTimeSig(TimeSig t) { mTimeSig.push_back(t); }

// Get methods
std::vector<Note>& Track::getNotes() { return notes; }
const std::vector<Note>& Track::getNotes() const { return notes; }
	
/* Meta Track::getMeta(eMeta m, int i)
	{ 
	switch(m)
		{
		case anchor: return mAnchor.at(i); break;
		case bend: return mBend.at(i); break;
		case chord: return mChord.at(i); break;
		case lyrics: return mLyrics.at(i); break;
		case marker: return mMarker.at(i); break;
		case phrase: return mPhrase.at(i); break;
		case tech: return mTech.at(i); break;
		case special: return mSpecial.at(i); break;
		}
	} */
int Track::getMetaSize(eMeta m) const { 
	switch(m) {
		case anchor: return mAnchor.size(); break;
		case bend: return mBend.size(); break;
		case chord: return mChord.size(); break;
		case lyrics: return mLyrics.size(); break;
		case marker: return mMarker.size(); break;
		case phrase: return mPhrase.size(); break;
		case tech: return mTech.size(); break;
		case special: return mSpecial.size(); break;
		default: return mSpecial.size(); break;
	}
}
const std::vector<Meta>& Track::getMetas(eMeta m) const { 
	switch(m) {
		case anchor: return mAnchor; break;
		case bend: return mBend; break;
		case chord: return mChord; break;
		case lyrics: return mLyrics; break;
		case marker: return mMarker; break;
		case phrase: return mPhrase; break;
		case tech: return mTech; break;
		case special: return mSpecial; break;
		default: return mSpecial; break;
	}
}

TimeSig Track::getTimeSig(int i) { return mTimeSig.at(i); }
std::vector<TimeSig> Track::getTimeSigs() { return mTimeSig; }

void Track::setMetas(std::vector<Meta> metas, eMeta m) {
	switch(m) {
		case anchor: mAnchor = metas; break;
		case bend: mBend = metas; break;
		case chord: mChord = metas; break;
		case lyrics: mLyrics = metas; break;
		case marker: mMarker = metas; break;
		case phrase: mPhrase = metas; break;
		case tech: mTech = metas; break;
		case special: mSpecial = metas; break;
		default: mSpecial = metas; break;
	}
}

// Miscellaneous methods
void Track::sortNotes() // Convert the On and Off vectors into a single vector.
	{
	if(noteOn.size() == noteOff.size()) {
		
		auto jt = noteOff.begin();
		for(auto it = noteOn.begin(); it != noteOn.end(); ++it, ++jt) {
			/*if(it->getTime() == jt->getTime() 
				&& it->getPitch() == jt->getPitch()) */
				{ 
				Note n((*it)); n.setDuration((*jt).getTime() - n.getTime()); 
				notes.push_back(n);
			}
		}
	}
}
	
#endif
