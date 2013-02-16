#ifndef _RSXML_STRUCTURE_
#define _RSXML_STRUCTURE_

#include <string>
#include <sstream>

namespace RSXML {
	class TemplateSync {
		public:
			TemplateSync( const unsigned int& i ) { id = i; };
			
			unsigned int&			id;
	};
	
	class ChordTemplate : public TemplateSync {
		public:
			ChordTemplate( const array<unsigned char, 6>& fre, 
				const array<unsigned char, 6>& fin, const std::string& cN = "",
				const std::string& dN = "", const unsigned int& i = 0 ) 
				: TemplateSync( i ) 
				{ frets = fre; fingers = fin; chordName = cN; displayName = dN; };
			
			std::string			chordName;
			std::string			displayName;
			
			const array<unsigned char, 6>&	GetFrets() const { return frets; };
			const array<unsigned char, 6>&	GetFingers() const { return fingers; };
			
			const std::string				ToXML() const;
			
		private:
			array<unsigned char, 6>			frets;
			array<unsigned char, 6>			fingers;	
	};
	
	const std::string ChordTemplate::ToXML() const {
		std::stringstream ss;
		ss << "\t\t<chordTemplate chordName=\"" << chordName
		<< "\" displayName=\"" << displayName << "\" ";
		// Frets
		for( auto f = frets.begin; f != frets.end; ++f )
			{ ss << "fret" << (f - frets.begin()) << "=\"" << *f << "\" "; }
		// Fingers
		for( auto f = fingers.begin; f != fingers.end; ++f )
			{ ss << "finger" << (f - fingers.begin()) << "=\"" << *f << "\" "; }
		ss << " />\n";
		return ss.str();
	};
	
	// Phrase-related.
	class PhraseTemplate : public TemplateSync {
		public:
			PhraseTemplate( const std::string& nam = "", const unsigned char& dif = 0,
				bool dis = 0, bool ig = 0, bool so = 0, const unsigned int& i = 0 ) 
				: TemplateSync( i ) {
				name = nam; maxDifficulty = dif; disparity = dis; ignore = ig; solo = so;
			};
			
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
	
	const std::string PhraseTemplate::ToXML() const {
		std::stringstream ss;
		ss << "\t\t<phrase name=\"" << name << "\" maxDifficulty=\""
		<< maxDifficulty << "\" disparity=\"" << disparity << "\" ignore=\"" 
		<< ignore << "\" solo=\"" << solo << "\" />\n";
		return ss.str();
	};
	
	class Phrase : public TemplateSync {
		public:
			Phrase( const float& tim = 0.000f, const unsigned int& i = 0, 
				const unsigned char& var = 0x00 ) 
				: TemplateSync( i ) { time = tim; variation = var };
			
			const float&			GetTime() const { return time; };	
			const unsigned char&	GetVariation() const { return variation; };
			
			const std::string		ToXML() const;
			
		private:
			float					time;
			unsigned char			variation;
	};
	
	const std::string Phrase::ToXML() const {
		/* Variation always seems to be incremented. This may be dependent
		on whether the contents are changed, but maybe not. Slightly 
		complicating matters is that the 'official' way seems to be to use
		roman numerals. */
		std::string var = std::to_string( (unsigned int)variation );
		std::stringstream ss;
		ss << "\t\t<phraseIteration time=\"" << time << "\" phraseId=\"" << id 
		<< "\" variation=\"" << var << "\" />\n";
		return ss.str();
	};
	
	class LinkedDiffs;
	
	class NewLinkedDiffs;
	
	class PhraseProperties;
	
	class Section {
		public:
			Section( const float& tim = 0.000f, const std::string nam = "", 
				const unsigned char& it = 0x00 ) 
				{ time = tim; name = nam; iteration = it };
			
			const unsigned char&	GetIteration() const { return iteration; };
			const std::string&		GetName() const { return name; };
			const float&			GetTime() const { return time; };	
			
			const std::string		ToXML() const;
			
		private:
			float					time;
			std::string				name;
			unsigned char			iteration;
	};
	
	const std::string Section::ToXML() const {
		std::stringstream ss;
		ss << "\t\t<section name=\"" << name << "\" number=\"" << iteration 
		<< "\" startTime=\"" << time << "\" />\n";
		return ss.str();
	};
};

#endif