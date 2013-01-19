#ifndef _BASE_VOCALS_
#define _BASE_VOCALS_

#ifndef _BASE-TRACK_
#include "BaseTrack.h"
#endif

struct Lyric
	{
	float time;
	int pitch;
	std::string lyric;
	};

class Vocal
	{
	bool ext;
	std::vector<Lyric> lyrics;
	
	void process();
	
	public:
		Vocal() { };
		Vocal(Track t, bool e);
		Vocal(const Vocal& v) { };
		~Vocal() { };
		
		Lyric getLyric(int i) { return lyrics.at(i); };
		std::vector<Lyric> getLyrics() { return lyrics; };
	
	};
	
Vocal::Vocal(Track t, bool e)
	{ 
	ext = e; 
	for(std::vector<Note>::iterator it = t.getNotes().begin();
		it != t.getNotes().end(); ++it)
		{
		Note cN(*it);
		Lyric l;
		l.time = cN.getTime();
		l.pitch = cN.getPitch();
		lyrics.push_back(l);
		}
	if(ext)
		{
		
		}
	else
		{
		
		}
	}
	
void Vocal::process()
	{
	
	
	}

#endif