#include "MIDIRead.h"

// Public ====

void MIDIRead::process(unsigned int numArrs) {
	if(fileName == "")
		{ std::cout << "No MIDI file was selected to be read." ENDLINE }
	else {
		int fileSize = getMIDI(fileName);
		division = (memblock[12] * 256) + memblock[13]; /* The time division 
		is stored as a short in bytes 12 and 13. This is an inelegant yet 
		accurate solution, code-reusability be damned. */
		
		// Single-track case.
		if(memblock[FILETYPEBYTE] == 0) { processDelta(HEADERLENGTH); }
		// Multiple tracks.
		else {
			// Global track. Holds information such as tempo and time signature.
			int filePos = processDelta(HEADERLENGTH);
			bool moarTrack = true;
			while(filePos < fileSize && moarTrack) {
				filePos = processDelta(filePos);
				if(numArrs != 0 && tracks.size() - 1 >= numArrs)
					{ moarTrack = false; } 
			}
		}	
	}
}
	
void MIDIRead::debug() {
	std::string name = fileName + "-test.txt";
	std::ofstream file;
	file.open(name.c_str()); // This is the file we're writing to.
	
	file << "Test File for " << fileName << ".\n" ENDLINE
	
	std::cout << "Tracks: " << tracks.size() ENDLINE
	
	for(auto it = tracks.begin(); it != tracks.end(); ++it) {
		file << "Track " << (it - tracks.begin()) << ": Name: "
		<< it->name << " | Duration: " << it->duration ENDLINE
		std::vector<Note> notes = it->getNotesOn();
		// Should print all notes in a track.
		unsigned int failCount = 0;
		for(auto jt = notes.begin(); jt != notes.end(); ++jt)
			{
			file << "\tNote " << jt - notes.begin() << "  Time: " 
			<< jt->getTime() << " | String: " << jt->getString() ENDLINE
			if( jt->getString() > 79 ) { ++failCount; }
			}
		file << "\nFail Count: " << failCount ENDLINE
		file << "\nMetadata:" ENDLINE
		file << "\nTempo Changes:" ENDLINE
		auto tempo = it->getTempos();
		for(Tempo& t : tempo) {
			file << "\tTime: " << t.time << " | Tempo: " << t.tempo ENDLINE
		}
		file << "Techniques: " ENDLINE
		auto mTech = it->getMetas(eMeta::tech);
		for(Meta& m : mTech) 
			{ file << "Technique: " << m.time << " | " << m.text ENDLINE }
		
		/* file << "Phrases: " ENDLINE
		auto mPhrase = it->getMetas(eMeta::phrase);
		for(Meta& m : mPhrase) {
			file << "Phrase  Time: " << m.time << " | Name: " 
			<< m.text ENDLINE
		} */
		file << "END OF TRACK. \n" ENDLINE
	}
	file.close();
}

// Private ====

int MIDIRead::getMIDI(std::string midiName) {
	// Taking the content from a MIDI file.
	std::ifstream midi;
	std::ifstream::pos_type midiSize;
	midiName += ".mid";
	midi.open(midiName.c_str(), 
		std::ios::in | std::ios::binary | std::ios::ate);
	if(midi.is_open())
		{
		midiSize = midi.tellg();;
		memblock = new unsigned char[midiSize];
		// creating a (signed) buffer so we can just transfer stuff across.
		char* membuffer = new char[midiSize];
		midi.seekg(0);
		midi.read(membuffer, midiSize);
		// Inelegant transfer of signed to unsigned.
		for(int i = 0; i < midiSize; i++)
			{ memblock[i] = membuffer[i]; }
		}
	else { std::cout << "File could not be opened." ENDLINE } 
	midi.close();
	
	return (int)midiSize;
}	
	
// Read Process Mark 2
int MIDIRead::processDelta(const unsigned int& beginPoint) {
	Track nT;
	
	if(tracks.size() > 0) { 
		nT.setTempos(tracks.at(0).getTempos()); 
		nT.setMetas(tracks.at(0).getMetas(eMeta::marker),eMeta::marker);
	}
	std::vector<Tempo>::const_iterator tCount = nT.getTempos().begin();
	std::vector<Tempo>::const_iterator tEnd = nT.getTempos().end();
	currentTempo = 120.0;
	float timer = DEFAULTOFFSET;
	
	std::vector<unsigned char> time;

	unsigned int trackSize = (memblock[beginPoint + 6] * 256);
	trackSize += memblock[beginPoint + 7];
	unsigned int it = beginPoint + TRACKHEADERLENGTH;
	unsigned int endPoint = it + trackSize;
	
	/* std::cout << "Track " << tracks.size() << ": Begin: " << beginPoint 
	<< " Track size: " << trackSize << " End: " << endPoint ENDLINE */
	while( it < endPoint ) {
		// Unsigned chars are necessary for the hex-based comparisons work.
		unsigned char c = memblock[it]; // Read byte.
		time.push_back( c );		
		// Not the last delta-time byte
		if ( c > 0x7F ) {
			if( time.size() >= MAXDELTA ) { 
				// || convertBytes2VLQ2(time) > 0x0FFFFFFF || )  
				std::cout << "Time Float is too large at: " << it << " | " 
				<< c << "\n"; return endPoint; 
			} 
			++it;
		}
		// 'Should' be the last delta-time byte
		else if( c < 0x80 ) {
			int delta = convertBytes2VLQ( time );
			if( tracks.size() > 0 ) { 
				currentTempo = getCurrentTempo( tCount, tEnd, timer );
			}
			float deltaConv = convertDelta2Time( delta, currentTempo );
			timer += deltaConv;
			time.clear();
			
			processContent( it, timer, nT );
			} 
		}
	if( timer > endTime ) { endTime = timer; }
	nT.duration = timer;
	nT.sortNotes();
	tracks.push_back( nT );
	return endPoint;
}

void MIDIRead::processContent(unsigned int& it, float& timer, Track& nT) {
	++it;
	unsigned char c = memblock[it]; // Read byte.
	std::vector<unsigned char> contents;
	if ( c == 0xFF ) { 
		// std::cout << "\n" << it;
		++it;
		c = memblock[it];
		if( c == 0x2F ) { it += 2; return; }
		processMeta(it, timer, nT); 
	} else if( c == 0xF0 || c == 0xF7 ) {
		// SysEx. Not used by us. Because they are variable length, we're
		// kind of fucked until I bother do this shit.
	} else {
		// std::cout << "\t" << it;
		if ( c >= 0x80 && c < 0xA0 ) {
			// Note-on and -off messages.
			unsigned int range = it + MIDICONTROLLENGTH;
			for(it = it; it != range; ++it) 
				{ contents.push_back(memblock[it]); }
			processNote(contents, timer, nT);
			lastEvent = c;
		} else if( c >= 0xB0 && c < 0xC0 ) {
			/* Controller events. Don't really know how this will affect
			our charts but they may have implications down the line. */
		} else if( c >= 0xC0 && c < 0xE0 ) {
			// Program change and aftertouch events. Irrelevant to us.
		} else if( c >= 0xE0 && c < 0xF0 ) {
			/* Pitch Bend events. These will be necessary to parse to get
			a direct line to string-bend events. An intermediary for now 
			is using the 'T' text event to specify bends. */
			unsigned int range = it + MIDICONTROLLENGTH;
			for(it = it; it != range; ++it) 
				{ contents.push_back(memblock[it]); }
			lastEvent = c;
		} else if( c < 0x80 ) {
			/* This one requires an explanation. The 4-bit event-type sequence 
			doesn't show up if there is a zero-value delta-time AND the 
			event-type repeats. As such, this throws the otherwise 
			well-established pattern off. Fucking 80's. */
			contents.push_back(lastEvent);
			unsigned int range = it + MIDICONTROLLENGTH - 1;
			for(it = it; it != range; ++it) 
				{ contents.push_back(memblock[it]); }
			if( lastEvent >= 0x80 && lastEvent < 0xA0 ) { 
				processNote(contents, timer, nT);
			} else if ( lastEvent >= 0xE0 && lastEvent < 0xF0 ) { }
		}
	} /* else {
		std::cout << "BORK BORK BORK at: " 
		<< it << " | " << c << "\n"; 
	} */
}

void MIDIRead::processNote(const std::vector<unsigned char>& contents, 
	const float& timer, Track& nT)
	{
	unsigned char channel, pitch, velocity;
	channel = contents.at(0);
	pitch = contents.at(1);
	velocity = contents.at(2);
	unsigned int string = 0;
	if(channel >= 0x90)	{ string = channel - 0x90; }
	else if(channel >= 0x80) { string = channel - 0x80; }
	Note n(timer, string, pitch, velocity);
	
	/* Putting it into a vector.
	
	Logic, at the very least, ignores the dedicated
	note-off message and instead uses a note-on message
	with a velocity of '0'. Thankfully, note-off messages
	always seem to have a velocity of '0' anyway, so 
	mixing and matching won't kick us in the dick hopefully. */
	if(midiMode == eMidi::logic && velocity > 0) 
		{ nT.addNoteOn(n); }
	else if (midiMode == eMidi::logic && velocity == 0)
		{ nT.addNoteOff(n); }
	else if(channel >= 0x90) // Note-on
		{ nT.addNoteOn(n); }
	else if(channel >= 0x80) // Note-off
		{ nT.addNoteOff(n); }
						
	/* Debug corner
	std::cout << "\tChannel: " << (int)channel - 0x80 << " String: " << string 
	<< " Pitch: " << (int)pitch << " Velocity: " << (int)velocity 
	<< " Time: " << timer << " Tempo: " << currentTempo; */
}

void MIDIRead::processMeta(unsigned int& it, float& timer, Track& nT) {
	// Meta-event
	Meta m;
	m.time = timer;
	unsigned int type = it;
	// The next byte contains the length of the proceeding section.
	++it;
	unsigned int range = memblock[it];
	++it;
	range += it;
	std::vector<unsigned char> contents;
	for(it = it; it != range; ++it) { contents.push_back(memblock[it]); }
	
	switch(memblock[type]) { // Determines meta-event type					
		case 0x01: { // Text Event. Useful for RS-specific stuff.
			eMeta metaType;
			switch(contents.at(0)) {
				case 'A': metaType = eMeta::anchor; break;
				case 'C': metaType = eMeta::chord; break;
				case 'E': metaType = eMeta::event; break;
				case 'P': metaType = eMeta::phrase; break;
				case 'T': metaType = eMeta::tech; break;
				case 'X': metaType = eMeta::special; break;
			}	
			std::vector<unsigned char> contentsMod;
			for(auto jt = contents.begin() + 1; jt != contents.end(); ++jt) 
				{ contentsMod.push_back(*jt); } 	
			m.text = convertBytes2String(contentsMod);
			nT.addMeta(metaType, m);
		} break;
		// Track Name meta-event
		case 0x03: nT.name = convertBytes2String(contents); break;
		case 0x04: break; // Instrument Name meta-event. Not much use.
		case 0x05: // Lyrics.
			m.text = convertBytes2String(contents); 
			nT.addMeta(eMeta::lyrics, m); break;
		case 0x06: // Marker meta-event.
			m.text = convertBytes2String(contents);
			nT.addMeta(eMeta::marker, m); break;
		case 0x51: { // Set Tempo.
			float t = convertBytes2Float(contents);
			currentTempo = ONEMINUTEMICRO / t; 
			nT.addTempo(timer, currentTempo);
			/* Debug Corner
			std::cout << i << " " << currentTempo << " " << t 
			<< " " << deltaConv ENDLINE
			std::cout << i << " " << currentTempo << " " << t 
			<< " " << timer ENDLINE */
		} break;
		// SMPTE offset.
		case 0x54: timer += convertSMPTE2Time(contents); break; 
		case 0x58: { // Time Signature. Good to know at some point.
			TimeSig time;
			time.num = contents.at(0);
			time.denom = contents.at(1);
			time.clock = contents.at(2);
			time.quart = contents.at(3);
			nT.addTimeSig(time);
		} break;
		case 0x59: break; // Key Signature.	
		case 0x2F: break; // End of Track.
	}
	/* Debug corner
	std::cout << "\tit: " << type << " Type: " << std::hex 
	<< (int)memblock[type] << " Range: " << std::dec << range 
	<< " Contents size: " << contents.size() << " Time: " << timer ENDLINE */
}

// Converters and stuff
float MIDIRead::getCurrentTempo(std::vector<Tempo>::const_iterator& tCount, 
	const std::vector<Tempo>::const_iterator& tEnd, const float& timer) 
	{
	float tempo = currentTempo;
	if(tCount != tEnd && timer >= tCount->time) 
		{ tempo = tCount->tempo; ++tCount; }
	return tempo;
}

float MIDIRead::convertDelta2Time(const int& delta, const float& tempo) {
	float length = (float)delta / (float)division;
	return convertTempo2Beat(tempo) * length;
}

int MIDIRead::convertBytes2VLQ(const std::vector<unsigned char>& vlq) {
	int buffer = vlq.back();
	for(auto it = vlq.begin(); it != vlq.end() - 1; ++it) 
		{ buffer += (int)(*it - 0x80) * pow(128,(vlq.end() - 1 - it)); }
	return buffer;
}

float MIDIRead::convertBytes2Float(const std::vector<unsigned char>& b) {
	float buffer = 0;
	for(auto it = b.begin(); it != b.end(); ++it) 
		{ buffer += (int)*it * pow(256.0,(b.end() - 1 - it)); }
	return buffer;
}

float MIDIRead::convertSMPTE2Time(const std::vector<unsigned char>& b) {
	float time = 0;
	// time += (b.at(0) * 60);
	time += b.at(1);
	time += (b.at(2) / division);
	time += ((b.at(3) / 100) / division);
	return time;
}

std::string MIDIRead::convertBytes2String(const std::vector<unsigned char>& b) 
	{ 
	std::string s = "";
	for(auto& c : b) { s += c; }
	return s;
}



