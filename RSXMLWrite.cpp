#include "RSXMLWrite.h"

void RSXMLWrite::processArrangement()
	{
	
	}
	
void RSXMLWrite::processVocals()
	{
	std::string file = fileName + "-Vocals.xml";
	ofstream vocals;
	vocals.open(file.c_str()); // This is the file we're writing to.
	
	std::vector<Lyric> vLyrics(arrV.getLyrics());
	
	vocals << "<?xml version='1.0' encoding='UTF-8'?> \n"
	<< "<vocals count=\"" << vLyrics.size() << "\">\n";
	
	for(std::vector<Lyric>::iterator it = vLyrics.begin();
		it != vLyrics.end(); ++it)
		{
		Lyric cL(*it);
		
		vocals << "\t<vocal time=\"" << cL.time << "\" note=\"" << cL.pitch
		<< "\" length=\"" << cL.duration << "\" lyric=\"" 
		<< cL.word << "\"/>\n";
		
		/* Debug
		cout << "WOOP WOOP - Note ID: " << it - vLyrics.begin() << " | " 
		<< cL.pitch << " | " << cL.time << " | " << cL.duration << " | " 
		<< cL.word << "\n"; */
		}
		
	vocals << "</vocals>";
	vocals.close();
	}