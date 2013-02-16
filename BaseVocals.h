#ifndef _BASE_VOCALS_
#define _BASE_VOCALS_

#ifndef _BASE_NOTE
#include "BaseNote.h"
#endif

#include <string>
#include <vector>

namespace Base {
	struct Lyric : public Base::Note {
		Lyric( const float& tim = 0.000f, const unsigned int& pit = 0, 
			const std::string& w = "" ) : Base::Note( tim, pit )
			{ word = w };
		
		std::string word;
	};
};

namespace ARR {
	class Vocals {
		public:
			Vocals( const std::string& nam, std::vector<Lyric>& lyr ) 
				{ nam = name; lyrics = lyr; };
			Vocals(const Vocals& a) : vLyrics(a.vLyrics), vText(a.vText)
				{ name = a.name; ext = a.ext; };
			
			const Lyric& 				getLyric(int i) const { return lyrics.at(i); };
			const std::vector<Lyric>& 	getLyrics() { return lyrics; };
			
		private:
			std::string 				name;
			std::vector<Lyric> 			lyrics;
	};
};



#endif