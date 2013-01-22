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
	std::string fileName, songName, arrName;
	Arrangement arr; 
	ArrVocal arrV;
	
	float avgTempo; // May have no in-game effect. For now, '120'.
	
	void writeNote(ofstream arrangement, Note n, int indent = 4);
	
	public:
		RSXMLWrite();
		RSXMLWrite(std::string file, std::string title, Arrangement a);
		RSXMLWrite(std::string file, ArrVocal v);
		~RSXMLWrite() { };
		
		void processArrangement(); // Main process function for RSXMLWrite.
		void processVocals(); // Writing a vocal file.
	};
	
RSXMLWrite::RSXMLWrite() 
	{ fileName = ""; songName = ""; arrName = ""; avgTempo = DEFAULTTEMPO; }
RSXMLWrite::RSXMLWrite(std::string file, std::string title, Arrangement a) 
	{ fileName = file; songName = title; arr = a; avgTempo = DEFAULTTEMPO; }
RSXMLWrite::RSXMLWrite(std::string file, ArrVocal v) 
	{ fileName = file; arrV = v; avgTempo = DEFAULTTEMPO; } 