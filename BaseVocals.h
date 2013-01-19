#ifndef _BASE_VOCALS_
#define _BASE_VOCALS_

#ifndef _BASE-TRACK_
#include "BaseTrack.h"
#endif

struct Lyric
	{
	float time;
	float duration;
	int pitch;
	std::string word;
	};

class ArrVocal
	{
	std::string name;
	bool ext;
	std::vector<Lyric> vLyrics;
	std::vector<Meta> vText;
	
	void getNotes(Track t);
	void getExternalLyrics(std::string fileName);
	
	public:
		ArrVocal() { };
		ArrVocal(Track t);
		ArrVocal(Track t, std::string fileName);
		ArrVocal(const ArrVocal& v);
		~ArrVocal() { };
		
		Lyric getLyric(int i) { return vLyrics.at(i); };
		std::vector<Lyric> getLyrics() { return vLyrics; };
	
	};
	
ArrVocal::ArrVocal(Track t)
	{
	eMeta l = lyrics; // We don't want confusion over the meaning of 'lyrics'.
	getNotes(t);
	vText = t.getMetas(l);
	
	std::vector<Meta>::iterator lastSuccess = vText.begin();
	for(std::vector<Lyric>::iterator it = vLyrics.begin();
		it != vLyrics.end(); ++it)
		{
		Lyric& cL(*it);
		for(std::vector<Meta>::iterator jt = lastSuccess; 
			jt != vText.end(); ++jt)
			{
			Meta cM(*jt);
			if(cL.time == cM.time)
				{ cL.word = cM.text; lastSuccess = jt; break; }
			}
		}
	
	/* The offset of notes and words. Negative values mean there's
	not enough words. Positive values mean there's not enough notes.
	I don't believe having blank lyrics is fatal, but it should probably be 
	avoided anyway. */
	int offset = vLyrics.size() - vText.size();
	if(offset != 0)
		{ cout << "Vocal mismatch. Offset of: " << offset << " notes.\n"; }
	}	
	
ArrVocal::ArrVocal(Track t, std::string fileName)
	{ 
	ext = true;
	getNotes(t);
	getExternalLyrics(fileName);

	std::vector<Meta>::iterator jt = vText.begin();
	for(std::vector<Lyric>::iterator it = vLyrics.begin();
		it != vLyrics.end(); ++it)
		{
		Lyric& cL(*it);
		if(jt != vText.end())
			{ Meta cM(*jt); cL.word = cM.text; }
		else { break; }
		++jt;
		}
		
	/* The offset of notes and words. Negative values mean there's
	not enough words. Positive values mean there's not enough notes.
	I don't believe having blank lyrics is fatal, but it should probably be 
	avoided anyway. */
	int offset = vLyrics.size() - vText.size();
	if(offset != 0)
		{ cout << "Vocal mismatch. Offset of: " << offset << " notes.\n"; }
	}

ArrVocal::ArrVocal(const ArrVocal& v)
	{
	name = v.name;
	ext = v.ext;
	std::vector<Lyric> vLyrics(v.vLyrics);
	std::vector<Meta> vText(v.vText);
	}	
	
// Process methods ====
void ArrVocal::getNotes(Track t)
	{
	for(std::vector<Note>::iterator it = t.getNotes().begin();
		it != t.getNotes().end(); ++it)
		{
		Note cN(*it);
		Lyric l;
		l.time = cN.getTime();
		l.duration = cN.getDuration();
		l.pitch = cN.getPitch();
		l.word = "";
		vLyrics.push_back(l);
		}
	}
	
void ArrVocal::getExternalLyrics(std::string fileName)
	{
	fileName += ".txt";
	cout << "External lyrics from '" << fileName << "'. \n";
	ifstream extlyrics;
	extlyrics.open(fileName.c_str());
	if(extlyrics.is_open())
		{
		string line;
		string buf; // Have a buffer string
		stringstream ss;
		
		while(extlyrics.good())
			{
			getline(extlyrics,line);
			ss.str(""); ss.clear();
			ss.str(line); // Insert the string into a stream
			
			while (ss >> buf)
				{
				Meta m;
				m.time = -1.0;
				m.text = buf;
				vText.push_back(m);
				}
			}
		}
	extlyrics.close();
	}

#endif