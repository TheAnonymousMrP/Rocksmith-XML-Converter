#ifndef BASE_TRACK
#define BASE_TRACK

#ifndef BASE_META
#include "BaseMeta.h"
#endif

#include <vector>

// Deprecated. 

namespace Base {
	enum eTrackType {
		vocal,
		single,
		combo,
		chords,
		bass,
		bassPick,
	};

	class Track {
		// Private
		unsigned int			maxDif;
	
		std::vector<Note>				notes;
		std::vector<Note>				noteOn;
		std::vector<Note>				noteOff;
	
		std::vector<Meta>				mAnchor; // This holds anchor-related data.
		std::vector<Meta>				mBend; // This holds Pitch Bend-derived data.
		std::vector<Meta>				mChord; // This holds chord names.
		std::vector<Meta>				mEvent;
		std::vector<Meta>				mMarker; // This hold markers, used for sections.
		std::vector<Meta>				mPhrase; // This holds event data for phrases.
		std::vector<Meta>				mTech; // This holds all technique-related meta-events.
		std::vector<Meta>				mLyrics;
		std::vector<Meta>				mSpecial;
	
		std::vector<Tempo>				mTempo; // Tempo changes.
		std::vector<TimeSig>			mTimeSig; // Time Signature changes.
	
		public:
			Track(std::string n = "", float d = 0.0, eTrackType t = eTrackType::combo)
				{ name = n; duration = d; type = t; };
			Track(const Track& t) : notes(t.notes), noteOn(t.noteOn), noteOff(t.noteOff), 
				mAnchor(t.mAnchor), mBend(t.mBend), mChord(t.mChord), mEvent(t.mEvent), 
				mMarker(t.mMarker), mPhrase(t.mPhrase), mTech(t.mTech), mLyrics(t.mLyrics), 
				mSpecial(t.mSpecial), mTempo(t.mTempo), mTimeSig(t.mTimeSig) { 
				name = t.name; duration = t.duration; type = t.type; 
				maxDif = t.maxDif; 
			};
			~Track() { };
		
			std::string					name;
			float						duration; 
			eTrackType					type;

			void						addNoteOn(Note n);
			void						addNoteOff(Note n);
			void						addMeta(eMeta i, Meta m);
			void						addTempo(float time, float tempo);
			void						addTimeSig(TimeSig t);
		
			// Debug
			const int					getNumNotesOn() const { return noteOn.size(); };
			const int					getNumNotesOff() const { return noteOff.size(); };
			const						std::vector<Note>& getNotesOn() const { return noteOn; };
			const						std::vector<Note>& getNotesOff() const { return noteOn; };
		
			// Note						getNote(int i);
			const unsigned int&			getMaxDif() const { return maxDif; };
			const int					getNumNotes() const { return notes.size(); };
			std::vector<Note>&			getNotes(); // Copy
			const std::vector<Note>&	getNotes() const;
		
			// Meta						getMeta(eMeta m, int i);
			int							getMetaSize(eMeta m) const;
			const std::vector<Meta>&	getMetas(eMeta m) const; // Copy	
		
			// const Tempo&				getTempo(int i) const { return mTempo.at(i); }; 
			Tempo						getTempo(unsigned int i) const {
											if(i < mTempo.size()) { return mTempo.at(i); }
											else { Tempo t( -1 ); return t; }
										};
			int							getTempoSize() const { return mTempo.size(); };
			const std::vector<Tempo>&	getTempos() const { return mTempo; };
			TimeSig						getTimeSig(int i); 
			std::vector<TimeSig>		getTimeSigs();
		
			void						setMetas(std::vector<Meta> metas, eMeta m);
			void						setTempos(std::vector<Tempo> tempos) { mTempo = tempos; };
			void						setTimeSigs(std::vector<TimeSig> times) { mTimeSig = times; };
		
			void						sortNotes(); // Convert the On and Off vectors to a single vector.
			void						normaliseDifs(); // Sort note difficulties into a normalised form.
	};

	// Public ====
	// Add methods
	void Track::addNoteOn(Note n) { noteOn.push_back(n); }
	void Track::addNoteOff(Note n) { noteOff.push_back(n); }
	void Track::addMeta(eMeta i, Meta m) { 
		switch(i) {
			case eMeta::ANCHOR: mAnchor.push_back(m); break;
			case eMeta::BEND: mBend.push_back(m); break;
			case eMeta::CHORD: mChord.push_back(m); break;
			case eMeta::EVENT: mEvent.push_back(m); break;
			case eMeta::LYRICS: mLyrics.push_back(m); break;
			case eMeta::MARKER: mMarker.push_back(m); break;
			case eMeta::PHRASE: mPhrase.push_back(m); break;
			case eMeta::TECHNIQUE: mTech.push_back(m); break;
			case eMeta::SPECIAL: mSpecial.push_back(m); break;
		}
	}
	void Track::addTempo(float time, float tempo) { 
		Tempo t( time, tempo );
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
			case eMeta::anchor: return mAnchor.at(i); break;
			case eMeta::bend: return mBend.at(i); break;
			case eMeta::chord: return mChord.at(i); break;
			case eMeta::lyrics: return mLyrics.at(i); break;
			case eMeta::marker: return mMarker.at(i); break;
			case eMeta::phrase: return mPhrase.at(i); break;
			case eMeta::tech: return mTech.at(i); break;
			case eMeta::special: return mSpecial.at(i); break;
			}
		} */
	int Track::getMetaSize(eMeta m) const { 
		switch(m) {
			case eMeta::ANCHOR: return mAnchor.size(); break;
			case eMeta::BEND: return mBend.size(); break;
			case eMeta::CHORD: return mChord.size(); break;
			case eMeta::EVENT: return mEvent.size(); break;
			case eMeta::LYRICS: return mLyrics.size(); break;
			case eMeta::MARKER: return mMarker.size(); break;
			case eMeta::PHRASE: return mPhrase.size(); break;
			case eMeta::TECHNIQUE: return mTech.size(); break;
			case eMeta::SPECIAL: return mSpecial.size(); break;
			default: return mSpecial.size(); break;
		}
	}
	const std::vector<Meta>& Track::getMetas(eMeta m) const { 
		switch(m) {
			case eMeta::ANCHOR: return mAnchor; break;
			case eMeta::BEND: return mBend; break;
			case eMeta::CHORD: return mChord; break;
			case eMeta::EVENT: return mEvent; break;
			case eMeta::LYRICS: return mLyrics; break;
			case eMeta::MARKER: return mMarker; break;
			case eMeta::PHRASE: return mPhrase; break;
			case eMeta::TECHNIQUE: return mTech; break;
			case eMeta::SPECIAL: return mSpecial; break;
			default: return mSpecial; break;
		}
	}

	TimeSig Track::getTimeSig(int i) { return mTimeSig.at(i); }
	std::vector<TimeSig> Track::getTimeSigs() { return mTimeSig; }

	void Track::setMetas(std::vector<Meta> metas, eMeta m) {
		switch(m) {
			case eMeta::ANCHOR: mAnchor = metas; break;
			case eMeta::BEND: mBend = metas; break;
			case eMeta::CHORD: mChord = metas; break;
			case eMeta::EVENT: mEvent = metas; break;
			case eMeta::LYRICS: mLyrics = metas; break;
			case eMeta::MARKER: mMarker = metas; break;
			case eMeta::PHRASE: mPhrase = metas; break;
			case eMeta::TECHNIQUE: mTech = metas; break;
			case eMeta::SPECIAL: mSpecial = metas; break;
			default: mSpecial = metas; break;
		}
	}

	// Miscellaneous methods
	void Track::normaliseDifs() {
		std::vector<unsigned int> difs;
		bool match = false;
		for(auto it = notes.begin(); it != notes.end(); ++it) {
			match = false;
			for(auto jt = notes.begin(); jt != it; ++jt) {
				if(it->minDif == jt->minDif) { match = true; break; }
			}
			if(!match) { difs.push_back(it->minDif); }
		}
		maxDif = difs.size()-1;

		// Sorts the vector into order.
		sort(difs.begin(),difs.end());
	
		for(Note& n : notes) { 
			// Set a normalised difficulty.
			for(auto it = difs.begin(); it != difs.end(); ++it) {
				if(n.minDif == (*it)) 
					{ n.minDif = (it - difs.begin()); break; }
			}
		}
	}

	void Track::sortNotes() {
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
		normaliseDifs();
	}
};
	
#endif
