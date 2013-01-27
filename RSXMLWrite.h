#include "GenArr.cpp"
#include "BaseVocals.h"

#define ENDLINE << "\n";

#define DEFAULTOFFSET 5.25 /* Default offset (in seconds) for songs.
An arbitrary value; still need to fully comprehend how it works. */
#define DEFAULTOFFSETCENT 0.0 // No idea.
#define DEFAULTSTARTBEAT 0.000 // No idea.
#define DEFAULTTEMPO 120

class RSXMLWrite
	{
	// Private:
	std::string fileName, songName;
	Arrangement arr; 
	ArrVocal arrV;
	
	const std::vector<Section> vSection;
	std::vector<int> vPhraseRef;
	const std::vector<Phrase> vPhrase;
	
	float avgTempo; // May have no in-game effect. For now, '120'.
	
	void writeDifficulty(std::ofstream& arrangement, int dif);
	
	void writeAnchor(std::ofstream& arrangment, Anchor a, int indent = 4);
	void writeChord(std::ofstream& arrangement, Chord c, int indent = 4);
	void writeHand(std::ofstream& arrangement, HandShape h, int indent = 4);
	void writeNote(std::ofstream& arrangement, Note n, int indent = 4);
	
	public:
		RSXMLWrite();
		RSXMLWrite(std::string file, std::string title, Arrangement a);
		RSXMLWrite(std::string file, ArrVocal v);
		~RSXMLWrite() { };
		
		void processArrangement(); // Main process function for RSXMLWrite.
		void processVocals(); // Writing a vocal file.
	};

RSXMLWrite::RSXMLWrite(std::string file, std::string title, Arrangement a) 
	{ 
	fileName = file; songName = title; 
	avgTempo = DEFAULTTEMPO;
	arr = a;  
	std::vector<Section> vSection(arr.getSections()); 
	std::vector<Phrase> allPhrases;
	for(std::vector<Section>::iterator it = vSection.begin();
		it != vSection.end(); ++it)
		{
		Section cS(*it);
		std::vector<Phrase> tempPhrases(cS.getPhrases());
		int i = tempPhrases.size();
		vPhraseRef.push_back(i);
		for(std::vector<Phrase>::iterator jt = tempPhrases.begin();
			jt != tempPhrases.end(); ++jt)
			{
			Phrase cP(*jt);
			allPhrases.push_back(cP);
			}
		}
	std::vector<Phrase> vPhrase(allPhrases);

	/*
	std::vector<Difficulty> allDif;
	for(int i = 0; i < arr.getDifficulties(); ++i)
		{
		for(std::vector<Phrase>::iterator itP = vPhrase.begin();
			itP != vPhrase.end(); ++it)
			{
			Phrase tempPhrase(*it);
			std::vector<Difficulty> tempDif(tempPhrase.getDifficulties());
			int size = tempDif.size();
			vDifPerPhrase.push_back(size);
			if(i < size)
				{ allDif.push_back(tempDif.at(i)); }
			}
		}	
	std::vector<Difficulty> vDifficulty(allDif);
	*/
	}
RSXMLWrite::RSXMLWrite(std::string file, ArrVocal v) 
	{ fileName = file; songName = ""; arrV = v; avgTempo = DEFAULTTEMPO; } 