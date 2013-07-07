#ifndef _RSXML_STRUCTURE_
#define _RSXML_STRUCTURE_

#ifndef _BASE_OBJECTS
#include "BaseObjects.h"
#endif

#include <string>
#include <sstream>

namespace RSXML {
	class TemplateSync {
		public:
			TemplateSync( const unsigned int& i = 0 ) { id = i; };
			
			unsigned int			id;
	};
	
	class Beat : public virtual Base::BaseObject {
		public:
			Beat( const float& tim = 0.000f, const int& ba = -1 ) 
				: BaseObject( tim ) { bar = ba; };
		
			const std::string	ToXML() const;
				
		private:
			int					bar;
	};
	
	const std::string Beat::ToXML() const {
		std::stringstream ss;
		ss << "\t\t<ebeat time=\"" << time << "\" measure=\"" << bar << "\" />\n";
		return ss.str();
	};
	
	class ChordTemplate : public TemplateSync {
		public:
			ChordTemplate( const std::array<unsigned char, 6>& fre, 
				const std::array<unsigned char, 6>& fin, const std::string& cN = "",
				const std::string& dN = "", const unsigned int& i = 0 ) 
				: TemplateSync( i ), frets( fre ), fingers( fin ) 
				{ chordName = cN; displayName = dN; };
			
			std::string							chordName;
			std::string							displayName;
			
			const std::array<unsigned char, 6>&	GetFrets() const { return frets; };
			const std::array<unsigned char, 6>&	GetFingers() const { return fingers; };
			
			const std::string					ToXML() const;
			
		private:
			std::array<unsigned char, 6>		frets;
			std::array<unsigned char, 6>		fingers;	
	};
	
	const std::string ChordTemplate::ToXML() const {
		std::stringstream ss;
		ss << "\t\t<chordTemplate chordName=\"" << chordName
		<< "\" displayName=\"" << displayName << "\" ";
		// Frets
		for( auto f = frets.begin(); f != frets.end(); ++f )
			{ ss << "fret" << (f - frets.begin()) << "=\"" << *f << "\" "; }
		// Fingers
		for( auto f = fingers.begin(); f != fingers.end(); ++f )
			{ ss << "finger" << (f - fingers.begin()) << "=\"" << *f << "\" "; }
		ss << " />\n";
		return ss.str();
	};
	
	class Event : public Base::MetaString {
		public:
			Event( const float& tim = 0.000f, const std::string& tex = "" ) 
				: Base::MetaString( Base::eMeta::EVENT, tim, tex ) { };
		
			const std::string 	ToXML() const;
	};
	
	const std::string Event::ToXML() const { 
		std::stringstream ss;
		ss << "\t\t<event time=\"" << time << "\" code=\"" << text << "\" />\n";
		return ss.str();
	};
	
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
	
	class Phrase : public virtual Base::BaseObject, public TemplateSync {
		public:
			Phrase( const float& tim = 0.000f, const unsigned int& i = 0, 
				const unsigned char& var = 0x00 ) : BaseObject( tim ), TemplateSync( i ) 
				{ variation = var; };
			
			const unsigned char&	GetVariation() const { return variation; };
			
			const std::string		ToXML() const;
			
		private:
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
	
	class LinkedDiff { 
		// <linkedDiff childId="17" parentId="15"/> */
	};
	
	class NewLinkedDiff { 
		// <newLinkedDiff phrases="15,17" levelBreak="-1" ratio="1.000"/>
	};
	
	class PhraseProperty { };
	
	class Section : public virtual Base::BaseObject {
		public:
			Section( const float& tim = 0.000f, const std::string nam = "", 
				const unsigned char& it = 0x00 ) : BaseObject( tim )
				{ name = nam; iteration = it; };
			
			const std::string&		GetName() const { return name; };
			const unsigned char&	GetIteration() const { return iteration; };
			
			const std::string		ToXML() const;
			
		private:
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