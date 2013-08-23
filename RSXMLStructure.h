#ifndef RSXML_STRUCTURE
#define RSXML_STRUCTURE

#ifndef BASE_OBJECTS
#include "BaseObjects.h"
#endif

#ifndef DEBUG_STUFF
#include "debug.h"
#endif

#include <string>
#include <sstream>

namespace RSXML {
	class Template {
		public:
			Template( const unsigned int& id = 0 ) : id( id ) { };
			
			unsigned int			id;
	};
	
	class Beat {
		public:
			Beat( const float& time = 0.000f, const int& bar = 0 ) : time( time ), bar( bar ) { };
		
			const float&		GetTime() const { return time; };
			const std::string	ToXML() const;
				
		private:
			float				time;
			int					bar;
	};
		
	class ChordTemplate : public Template {
		public:
			ChordTemplate( const std::array<unsigned char, GUITARSTRINGS>& frets, 
				const std::array<unsigned char, GUITARSTRINGS>& fingers, const std::string& cName = "",
				const std::string& dName = "", const unsigned int& i = 0 ) 
				: Template( i ), frets( frets ), fingers( fingers ) { chordName = cName; displayName = dName; };
			
			std::string										chordName;
			std::string										displayName;
						
			const std::array<unsigned char, GUITARSTRINGS>&	GetFrets() const { return frets; };
			const std::array<unsigned char, GUITARSTRINGS>&	GetFingers() const { return fingers; };
			
			const std::string								ToXML() const;


			// Converts frets from a chord to MIDI pitch.
			std::array<unsigned char, GUITARSTRINGS>			ConvertFrets2Pitches( const Base::Tuning& tuning = Base::aTuning[eTuning::STANDARD_E] ) const {
				std::array<unsigned char, GUITARSTRINGS> pitches; pitches.fill( 0xFF );
				// for( auto it = frets.begin(); it != frets.end(); ++it ) { std::cout << it - frets.begin() << ": " << (unsigned int)*it << "\t"; } std::cout ENDLINE
				for( unsigned int i = 0; i < GUITARSTRINGS; ++i ) {
					if( frets[i] != 0xFF ) { pitches[i] = frets[i] + tuning.pitch[i]; }
				}
				return pitches;
			};
			// Converts MIDI pitches to frets. Static.
			static std::array<unsigned char, GUITARSTRINGS>	ConvertPitches2Frets( const std::array<unsigned char, GUITARSTRINGS>& pitches, 
																const Base::Tuning& tuning = Base::aTuning[eTuning::STANDARD_E] ) { 
				std::array<unsigned char, GUITARSTRINGS> frets; frets.fill( 0xFF );
				for( unsigned int i = 0; i <= GUITARSTRINGS; ++i ) {
					if( frets[i] != 0xFF ) { frets[i] = pitches[i] - tuning.pitch[i]; }
				}
				return frets;
			};
			// Experimental: Converts frets to estimated finger positions. Static.
			static std::array<unsigned char, GUITARSTRINGS>	ConvertFrets2Fingers( const std::array<unsigned char, GUITARSTRINGS>& frets ) {
				std::array<unsigned char, GUITARSTRINGS> sortedFrets = frets;
				std::sort( sortedFrets.begin(), sortedFrets.end() );

				/* Removes redundant frets from the sorted array. 4 refers to the number of available fingers.
				GOTHERE
				std::array<unsigned char, 4> range; range.fill( 0xFF );
				std::array<unsigned char, GUITARSTRINGS>::iterator startFrom = sortedFrets.begin();
				GOTHERE
				for( std::array<unsigned char, 4>::iterator it = range.begin(); it != range.end(); ++it ) {
					for( std::array<unsigned char, GUITARSTRINGS>::iterator jt = startFrom; jt != sortedFrets.end(); ++jt ) { 
						if( *jt != *it ) { 
							*it = *jt;
							startFrom = jt;
							jt = sortedFrets.end();
						}
					}
				} 
				GOTHERE
				// Should assign the correct finger to string based on lowest fret -> highest.
				std::array<unsigned char, GUITARSTRINGS> fingers; fingers.fill( 0xFF );
				for( std::array<unsigned char, GUITARSTRINGS>::iterator it = fingers.begin(); it != fingers.end(); ++it  ) {
					if( frets[ it - fingers.begin() ] != 255 ) {
						for( std::array<unsigned char, 4>::iterator jt = range.begin(); jt != range.end(); ++jt ) {
							if( frets[ it - fingers.begin() ] == *jt ) { 
								*it = jt - range.begin(); 
								jt = range.end();
							}
						}
					}
				}
				GOTHERE */

				// Should assign the correct finger to string based on lowest fret -> highest.
				std::array<unsigned char, GUITARSTRINGS> fingers; fingers.fill( 0xFF );
				unsigned char last = 0xFF;
				for( auto it = frets.begin(); it != frets.end(); ++it ) {
					if( *it != 0xFF ) { 
						for( auto jt = sortedFrets.begin(); jt != sortedFrets.end(); ++jt ) {
							if( *it == *jt ) { fingers[ it - frets.begin() ] = jt - sortedFrets.begin(); sortedFrets.end(); }
						}
					}
				}
				return fingers;
			}
						
		private:
			std::array<unsigned char, GUITARSTRINGS>			frets;
			std::array<unsigned char, GUITARSTRINGS>			fingers;	
	};
		
	class Event : public Base::MetaString {
		public:
			Event( const float& tim = 0.000f, const std::string& tex = "" ) 
				: Base::MetaString( Base::eMeta::EVENT, tim, tex ) { };
		
			const std::string 	ToXML() const;
	};
		
	class PhraseTemplate : public Template {
		public:
			PhraseTemplate( const std::string& name = "", const unsigned int& maxDifficulty = 0, bool disparity = 0, bool ignore = 0, 
				bool solo = 0, const unsigned int& phraseID = 0 ) : Template( phraseID ), maxDifficulty( maxDifficulty ), name( name ), 
				disparity( disparity ), ignore( ignore ), solo( solo ) { };
			
			const unsigned int&		GetMaxDifficulty() const { return maxDifficulty; };
			const std::string&		GetName() const { return name; };
			const bool&				GetDisparity() const { return disparity; };
			const bool&				GetIgnore() const { return ignore; };
			const bool&				GetSolo() const	{ return solo; };
			
			const std::string		ToXML() const;
				
		private:
			unsigned int			maxDifficulty;
			std::string				name;			
			bool					disparity, ignore, solo;
	
	};
		
	class Phrase : public Base::TimeObject {
		public:
			Phrase( const float& time = 0.000f, const unsigned int& phraseID = 0, const unsigned char& variation = 0x00 ) 
				: TimeObject( time ), phraseID( phraseID ), variation( variation ) { };
			
			unsigned char			GetPhraseID() const { return phraseID.id; };
			const unsigned char&	GetVariation() const { return variation; };
			
			const std::string		ToXML() const;
			
		private:
			Template				phraseID;
			unsigned char			variation;
	};
		
	class LinkedDiff { 
		// <linkedDiff childId="17" parentId="15"/> */
	};
	
	class NewLinkedDiff { 
		// <newLinkedDiff phrases="15,17" levelBreak="-1" ratio="1.000"/>
	};
	
	class PhraseProperty { };
	
	class Section : public Base::TimeObject {
		public:
			Section( const float& time = 0.000f, const std::string name = "", const unsigned char& iteration = 0x00 ) 
				: TimeObject( time ), name( name ), iteration( iteration ) { };
			
			const std::string&		GetName() const { return name; };
			const unsigned char&	GetIteration() const { return iteration; };
			
			const std::string		ToXML() const;
			
		private:
			std::string				name;
			unsigned char			iteration;
	};
};

#endif