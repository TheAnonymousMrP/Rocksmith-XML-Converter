#ifndef BASE_ARRANGE
#define BASE_ARRANGE

#ifndef BASE_DIFFICULTY
#include "BaseDifficulty.h"
#endif

#ifndef BASE_STRUCTURE
#include "BaseStructure.h"
#endif

namespace Base {
	// Class declarations
	struct Beat {
		// int bar, beat;
		int bar; // The current bar. If not the first beat, '-1'.
		float time;
		float tempo;
		// TimeSig timeSig;
	};

	class ChordTemplate {
		int				id; 
		static int		count; 
		int				firstString;
		int				fret[NUMSTRINGS]; 
		int				finger[NUMSTRINGS]; 
	
		public:
			ChordTemplate( const std::string& n = "", std::string d = "" );
			ChordTemplate( const std::vector<Note>& notes, const std::string& n = "", const std::string& d = "");
			ChordTemplate( const ChordTemplate& ); // Copy constructor
			~ChordTemplate() { };
		
			std::string		name; 
			std::string		display; // Display name.
		
			const int&		getID() const { return id; };
			const int&		getFirstString() const { return firstString; };
			const int&		getFret(int i) const { return fret[i]; };
			const int&		getFinger(int i) const { return finger[i]; };
		
			void			setID() { id = count; count++; };
			void			setFingers() { };
		
			// For quick-and-easy compares. Returns string of fingers and frets.
			std::string	toString() const; 
			void reset() { count = 0; };
		
	};

	class PhraseTemplate {
		unsigned int id; static unsigned int count;
	
		public:
			PhraseTemplate(std::string n = "") { 
				name = n; inc = 0; 
				disparity = false; ignore = false; solo = false; 
			};
		
			std::string name;
			unsigned int maxDif; unsigned int inc;
			bool disparity, ignore, solo;
		
			const unsigned int& getID() const { return id; };
		
			void setID() { id = count; count++; }
	};
	
	class Arrangement {
		// Private
		std::string name;
		float duration;
		eTuning tuning;
		std::vector<Beat> beats;
		std::vector<ChordTemplate> chords;
		std::vector<PhraseTemplate> phrases;
		std::vector<Section> sections;
		std::vector<Phrase> vPhrases;
		std::vector<Difficulty> difficulties;
		std::vector<Note> vNotes;
		std::vector<Chord> vChords;
		std::vector<Anchor> vAnchors;
		std::vector<HandShape> vHands;
	
		public:
			Arrangement() : name(""), duration( 0.0 ), tuning( eTuning::STANDARD_E )
				{ };
			Arrangement(const Base::Track& t) { name = t.name; duration = t.duration; };
			Arrangement(const Arrangement& a) : 
				beats(a.beats), chords(a.chords), phrases(a.phrases),
				sections(a.sections), vPhrases(a.vPhrases), 
				difficulties(a.difficulties), vNotes(a.vNotes), 
				vChords(a.vChords), vAnchors(a.vAnchors), vHands(a.vHands) 
				{ name = a.name; duration = a.duration; };
			// Arrangement operator=(const Arrangement& a); // Assignment operator.
			~Arrangement() { };
		
			void addSections(std::vector<Section> s) 
				{ addXs(s, sections); };
		
			void addDifficulties(std::vector<Difficulty> d) 
				{ addXs(d, difficulties); };
		
			void addNotes(std::vector<Note> n) { addXs(n, vNotes); };
			void addChords(std::vector<Chord> c) { addXs(c, vChords); };
			void addAnchors(std::vector<Anchor> a) { addXs(a, vAnchors); };
			void addHands(std::vector<HandShape> h) { addXs(h, vHands); };
		
			const std::string& getName() const { return name; };
			const float& getDuration() const { return duration; };
			const eTuning& getTuning() const { return tuning; };
		
			const std::vector<ChordTemplate>& getChordTemplates() const
				{ return chords; };
			const std::vector<PhraseTemplate>& getPhraseTemplates() const
				{ return phrases; };
			const std::vector<Section>& getSections() const { return sections; };
			/* const std::vector<Phrase> getPhrases() const {
				std::vector<Phrase> pList;
				for(const Section& s : sections) {
					for(const Phrase& p : s.getPhrases()) { pList.push_back(p); }
				}
				return pList;
			}; */
			const std::vector<Phrase>& getPhrases() const { return vPhrases; };
			const std::vector<Difficulty>& getDifficulties() const 
				{ return difficulties; };
			const int getNumDifficulties() const 
				{ return difficulties.size(); }; 
			const std::vector<Note>& getNotes() const 
				{ return vNotes; };
			const int getNumNotes() const { return vNotes.size(); };
			const std::vector<Chord>& getChords() const 
				{ return vChords; };
			const std::vector<Anchor>& getAnchors() const 
				{ return vAnchors; };
			const std::vector<HandShape>& getHands() 
				const { return vHands; };
			const std::vector<Beat>& getBeats() const 
				{ return beats; };
		
			void setBeats(std::vector<Beat> b) { beats = b; };
			void setChordTemplates(std::vector<ChordTemplate> t) { chords = t; };
			void setPhraseTemplates(std::vector<PhraseTemplate> t) 
				{ phrases = t; };
			void setSections(std::vector<Section> s) { sections = s; };
			void setPhrases(std::vector<Phrase> p) { vPhrases = p; };
			void setDifficulties(std::vector<Difficulty> d) 
				{ difficulties = d; };
		
			void setNotes(std::vector<Note> n) { vNotes = n; };
			void setChords(std::vector<Chord> c) { vChords = c; };
			void setAnchors(std::vector<Anchor> a) { vAnchors = a; };
			void setHands(std::vector<HandShape> h) { vHands = h; };
		
			void setTuning(eTuning t) { tuning = t; };
	};
};

#endif