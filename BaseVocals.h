#ifndef _BASE_VOCALS_
#define _BASE_VOCALS_

#ifndef _BASE_TRACK
#include "BaseTrack.h"
#endif

struct Lyric : public BaseNote {
	Lyric(float t = -1.0, float d = -1.0, int p = -1, std::string w = "")
		{ time = t; duration = d; pitch = p; word = w; };
	
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
		ArrVocal(const Track& t);
		ArrVocal(const Track& t, std::string fileName);
		ArrVocal(const ArrVocal& v);
		~ArrVocal() { };
		
		Lyric getLyric(int i) { return vLyrics.at(i); };
		std::vector<Lyric> getLyrics() { return vLyrics; };
	
	};
	
ArrVocal::ArrVocal(const Track& t) {
	getNotes(t);
	vText = t.getMetas(eMeta::lyrics);
	
	std::vector<Meta>::iterator lastSuccess = vText.begin();
	for(Lyric& l : vLyrics)
		{
		for(auto jt = lastSuccess; jt != vText.end(); ++jt)
			{
			if(l.getTime() == jt->time)
				{ l.word = jt->text; lastSuccess = jt; break; }
			}
		}
	
	/* The offset of notes and words. Negative values mean there's
	not enough words. Positive values mean there's not enough notes.
	I don't believe having blank lyrics is fatal, but it should probably be 
	avoided anyway. */
	int offset = vLyrics.size() - vText.size();
	if(offset != 0) { 
		std::cout << "Vocal mismatch. Offset of: " << offset << " notes.\n"; 
	}
}	
	
ArrVocal::ArrVocal(const Track& t, std::string fileName) { 
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
	if(offset != 0) { 
		std::cout << "Vocal mismatch. Offset of: " << offset 
		<< " notes.\n"; 
	}
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
	for(Note& n : t.getNotes())
		{
		Lyric l(n.getTime(), n.getDuration(), n.getPitch());
		vLyrics.push_back(l);
		}
	}
	
void ArrVocal::getExternalLyrics(std::string fileName)
	{
	fileName += ".txt";
	std::cout << "External lyrics from '" << fileName << "'. \n";
	std::ifstream extlyrics;
	extlyrics.open(fileName.c_str());
	if(extlyrics.is_open())
		{
		std::string line;
		std::string buf; // Have a buffer string
		std::stringstream ss;
		
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