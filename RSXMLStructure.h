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
			Beat( const float& tim = 0.000f, const int& ba = -1 ) 
				: time( tim ), bar( ba ) { };
		
			const float&		GetTime() const { return time; };
			const std::string	ToXML() const;
				
		private:
			float				time;
			int					bar;
	};
		
	class ChordTemplate : public Template {
		public:
			ChordTemplate( const std::array<unsigned char, 6>& fre, 
				const std::array<unsigned char, 6>& fin, const std::string& cN = "",
				const std::string& dN = "", const unsigned int& i = 0 ) 
				: Template( i ), frets( fre ), fingers( fin ) 
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
		
	class Event : public Base::MetaString {
		public:
			Event( const float& tim = 0.000f, const std::string& tex = "" ) 
				: Base::MetaString( Base::eMeta::EVENT, tim, tex ) { };
		
			const std::string 	ToXML() const;
	};
		
	class PhraseTemplate : public Template {
		public:
			PhraseTemplate( const std::string& nam = "", const unsigned char& dif = 0,
				bool dis = 0, bool ig = 0, bool so = 0, const unsigned int& i = 0 ) 
				: Template( i ) {
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
		
	class Phrase : public Base::BaseObject, public Template {
		public:
			Phrase( const float& tim = 0.000f, const unsigned int& i = 0, 
				const unsigned char& var = 0x00 ) : BaseObject( tim ), Template( i ) 
				{ variation = var; };
			
			const unsigned char&	GetVariation() const { return variation; };
			
			const std::string		ToXML() const;
			
		private:
			unsigned char			variation;
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
		
};

#endif