#ifndef RSXML_STRUCTURE
#define RSXML_STRUCTURE

#ifndef BASE_OBJECTS
#include "BaseObjects.h"
#endif

#include <string>
#include <sstream>

namespace RSXML {
	class Template {
		public:
			Template( const unsigned int& i = 0 ) { id = i; };
			
			unsigned int			id;
	};
	
	class Beat {
		public:
			Beat( const float& tim = 0.000f, const unsigned char& ba = 0 ) 
				: time( tim ), bar( ba ) { };
		
			const float&		GetTime() const { return time; };
			const std::string	ToXML() const;
				
		private:
			float				time;
			unsigned char		bar;
	};
		
	class ChordTemplate : public Template {
		public:
			ChordTemplate( const std::array<unsigned char, NUMSTRINGS>& frets, 
				const std::array<unsigned char, NUMSTRINGS>& fingers, const std::string& cName = "",
				const std::string& dName = "", const unsigned int& i = 0 ) 
				: Template( i ), frets( frets ), fingers( fingers ) { chordName = cName; displayName = dName; };
			
			std::string										chordName;
			std::string										displayName;
						
			const std::array<unsigned char, NUMSTRINGS>&	GetFrets() const { return frets; };
			const std::array<unsigned char, NUMSTRINGS>&	GetFingers() const { return fingers; };
			
			const std::string								ToXML() const;


			// Converts frets from a chord to MIDI pitch.
			std::array<unsigned char, NUMSTRINGS>			ConvertFrets2Pitches( const Base::Tuning& tuning = Base::aTuning[eTuning::STANDARD_E] ) {
				std::array<unsigned char, NUMSTRINGS> pitches; pitches.fill( 0xFF );
				for( unsigned int i = 0; i <= NUMSTRINGS; ++i ) {
					if( frets[i] != 0xFF ) { pitches[i] = frets[i] + tuning.pitch[i]; }
				}
				return pitches;
			};
			// Converts MIDI pitches to frets. Static.
			static std::array<unsigned char, NUMSTRINGS>	ConvertPitches2Frets( const std::array<unsigned char, NUMSTRINGS>& pitches, 
																const Base::Tuning& tuning = Base::aTuning[eTuning::STANDARD_E] ) { 
				std::array<unsigned char, NUMSTRINGS> frets; frets.fill( 0xFF );
				for( unsigned int i = 0; i <= NUMSTRINGS; ++i ) {
					if( frets[i] != 0xFF ) { frets[i] = pitches[i] - tuning.pitch[i]; }
				}
				return frets;
			};
			// Experimental: Converts frets to estimated finger positions. Static.
			static std::array<unsigned char, NUMSTRINGS>	ConvertFrets2Fingers( const std::array<unsigned char, NUMSTRINGS>& frets ) {
				std::array<unsigned char, NUMSTRINGS> sortedFrets = frets;
				std::sort( sortedFrets.begin(), sortedFrets.end() );

				// Removes redundant frets from the sorted array. 4 refers to the number of available fingers.
				std::array<unsigned char, 4> range; range.fill( 0xFF );
				std::array<unsigned char, NUMSTRINGS>::iterator startFrom = sortedFrets.begin();
				for( std::array<unsigned char, 4>::iterator it = range.begin(); it != range.end(); ++it ) {
					for( std::array<unsigned char, NUMSTRINGS>::iterator jt = startFrom; jt != sortedFrets.end(); ++jt ) { 
						if( *jt != *it ) { 
							*it = *jt;
							startFrom = jt;
							jt = sortedFrets.end();
						}
					}
				}

				// Should assign the correct finger to string based on lowest fret -> highest.
				std::array<unsigned char, NUMSTRINGS> fingers; fingers.fill( 0xFF );
				for( std::array<unsigned char, NUMSTRINGS>::iterator it = fingers.begin(); it != fingers.end(); ++it  ) {
					if( frets[ it - fingers.begin() ] != 255 ) {
						for( std::array<unsigned char, 4>::iterator jt = range.begin(); jt != range.end(); ++jt ) {
							if( frets[ it - fingers.begin() ] == *jt ) { 
								*it = jt - range.begin(); 
								jt = range.end();
							}
						}
					}
				}
				return fingers;
			}
						
		private:
			std::array<unsigned char, NUMSTRINGS>			frets;
			std::array<unsigned char, NUMSTRINGS>			fingers;	
	};
		
	class Event : public Base::MetaString {
		public:
			Event( const float& tim = 0.000f, const std::string& tex = "" ) 
				: Base::MetaString( Base::eMeta::EVENT, tim, tex ) { };
		
			const std::string 	ToXML() const;
	};
		
	class PhraseTemplate : public Template {
		public:
			PhraseTemplate( const std::string& nam = "", const unsigned char& dif = 0,
				bool dis = 0, bool ig = 0, bool so = 0, const unsigned int& phraseID = 0 ) 
				: Template( phraseID ) { name = nam; maxDifficulty = dif; disparity = dis; ignore = ig; solo = so; };
			
			const unsigned char&	GetMaxDifficulty() const { return maxDifficulty; };
			const std::string&		GetName() const { return name; };
			const bool&				GetDisparity() const { return disparity; };
			const bool&				GetIgnore() const { return ignore; };
			const bool&				GetSolo() const	{ return solo; };
			
			const std::string		ToXML() const;
				
		private:
			unsigned char			maxDifficulty;
			std::string				name;			
			bool					disparity, ignore, solo;
	
	};
		
	class Phrase : public Base::BaseObject {
		public:
			Phrase( const float& tim = 0.000f, const unsigned int& phraseID = 0, const unsigned char& var = 0x00 ) 
				: BaseObject( tim ), phraseID( phraseID ), variation( var ) { };
			
			const unsigned char&	GetVariation() const { return variation; };
			unsigned char			GetPhraseID() const { return phraseID.id; };
			
			const std::string		ToXML() const;
			
		private:
			unsigned char			variation;
			Template				phraseID;
	};
		
	class LinkedDiff { 
		// <linkedDiff childId="17" parentId="15"/> */
	};
	
	class NewLinkedDiff { 
		// <newLinkedDiff phrases="15,17" levelBreak="-1" ratio="1.000"/>
	};
	
	class PhraseProperty { };
	
	class Section : public Base::BaseObject {
		public:
			Section( const float& tim = 0.000f, const std::string nam = "", const unsigned char& it = 0x00 ) 
				: BaseObject( tim ) { name = nam; iteration = it; };
			
			const std::string&		GetName() const { return name; };
			const unsigned char&	GetIteration() const { return iteration; };
			
			const std::string		ToXML() const;
			
		private:
			std::string				name;
			unsigned char			iteration;
	};
		
};

#endif