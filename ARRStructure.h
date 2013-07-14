#ifndef _ARR_STRUCTURE_
#define _ARR_STRUCTURE_

#ifndef _ARR_OBJECTS
#include "ARRObjects.h"
#endif

#include <string>

namespace ARR {
	class Beat : public Base::BaseObject {
		public:
			Beat( const float& tim = 0.000f, const unsigned int& ba = 0,
				const unsigned char& be = 0, const unsigned char& sub = 0 ) 
				: Base::BaseObject( tim ) { bar = ba; beat = be; subBeat = sub; };
			
			const unsigned int&		GetBar() const { return bar; };
			const unsigned char&	GetBeat() const { return beat; };
			const unsigned char&	GetSubBeat() const { return subBeat; };
				
		private:
			unsigned int			bar;
			unsigned char			beat; 
			unsigned char			subBeat;
	};

	class Phrase : public Base::BaseObject {
		public:
			Phrase( const float& tim = 0.000f, const float& dur = 0.000f, 
				const std::string& nam = "" )
				: Base::BaseObject( tim ) { duration = dur; name = nam; };
			
			std::string		name;
			
			const float&	GetDuration() const { return duration; };
				
		private:
			float			duration;
	};
	
	class Section : public Base::BaseObject {
		public:
			Section( const float& tim = 0.000f, const float& dur = 0.000f,
				const std::string& nam = "" )
				: Base::BaseObject( tim ) { duration = dur; name = nam; };
			
			std::string		name;
				
			const float&	GetDuration() const { return duration; };
				
		private:
			float			duration;
	};
	
	class Difficulty {
		public:
			Difficulty( const float& len = 0.000f, const unsigned int& i = 0 ) 
				: difficulty( i ), length( len ) { };
			
			const float& GetLength() const { return length; };
			
			const std::vector<unsigned int>& 
				GetNotesIndex() const { return notesIndex; };
			const std::vector<unsigned int>& 	
				GetChordsIndex() const { return chordsIndex; };
			
			void 			SetNotesIndex( const std::vector<unsigned int>& v ) 
								{ notesIndex = v; };
			void			SetChordsIndex( const std::vector<unsigned int>& v ) 
								{ chordsIndex = v; };
		
			// Too 'complex' to write ToXML(). Use Writer::WriteDifficulty() instead.

		private:
			unsigned int				difficulty;
			float						length;
		
			std::vector<unsigned int>	notesIndex;
			std::vector<unsigned int>	chordsIndex;
	};
};

#endif