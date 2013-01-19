#include "GenArr.cpp"
#include "BaseVocals.h"

class RSXMLWrite
	{
	// Private:
	std::string fileName; std::string arrName;
	Arrangement arr; 
	ArrVocal arrV;
	
	public:
		RSXMLWrite();
		RSXMLWrite(std::string file, Arrangement a);
		RSXMLWrite(std::string file, ArrVocal v);
		~RSXMLWrite() { };
		
		void processArrangement(); // Main process function for RSXMLWrite.
		void processVocals(); // Writing a vocal file.
	};
	
RSXMLWrite::RSXMLWrite() { fileName = ""; arrName = ""; }
RSXMLWrite::RSXMLWrite(std::string file, Arrangement a) 
	{ fileName = file; arr = a; }
RSXMLWrite::RSXMLWrite(std::string file, ArrVocal v) 
	{ fileName = file; arrV = v; } 