#ifndef _BASE_OBJECTS_
#define _BASE_OBJECTS_

#include <string>
#include <vector>

#ifndef _BASE_NOTE
#include "BaseNote.h"
#endif

namespace Base {
	class Chord : public virtual BaseObject {
		public:
			Chord( const float& tim = 0.000f, const array<unsigned int, 6>& nIn ) 
				: BaseObject( tim ) { notesIndex = nIn; };
			
			void	AddNoteIndex( const unsigned int& i ) 
						{ notesIndex.push_back( i ); };
			void	AddNotesIndex( const std::vector<unsigned int>& i )
						{ notesIndex = i; };
			
			const array<unsigned int, 6>&	GetNotesIndex() const { return notesIndex; };
			
		protected:
			array<unsigned int, 6>&			notesIndex;
	};
};

#ifndef _BASE_STRUCTURE
#include "ARRStructure.h"
#endif


#endif