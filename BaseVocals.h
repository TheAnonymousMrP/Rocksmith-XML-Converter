#ifndef _BASE_VOCALS_
#define _BASE_VOCALS_

#ifndef _BASE-TRACK_
#include "BaseTrack-2.0.1.h"
#endif

struct Lyric
	{
	float time;
	int pitch;
	std::string lyric;
	}

class Vocal
	{
	bool ext;
	std::vector<Lyric> lyrics;
	
	public:
		Vocal() { };
		Vocal(Track t, bool e);
		Vocal(const Vocal v) { };
		~Vocal() { };
		
		Lyric getLyric(int i) { return lyrics.at(i); };
		std::vector<Lyric> getLyrics() { return lyrics; };
	
	}
	
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
	
Vocal::process()
	{
	
	
	}

#endif