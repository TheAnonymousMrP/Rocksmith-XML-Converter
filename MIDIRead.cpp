#include "MIDIRead.h"
using namespace std;

// Public ====

void MIDIRead::process(int arrN)
	{
	if(fileName == "")
		{ cout << "No MIDI file was selected to be read." ENDLINE }
	else
		{
		int fileSize = getMIDI(fileName);
	
		division = (memblock[12] * 256) + memblock[13]; /* The time division 
		is stored as a short in bytes 12 and 13. This is an inelegant - yet 
		accurate - solution, code-reusability be damned. */
		float currentTempo = DEFAULTTEMPO; /* '120' is the default tempo in 
		MIDI. Useful safety net if for some reason there's no tempo meta-event 
		at the start of a track. */
		
		// Single-track case.
		if(memblock[9] == 0) { int filePos = processTrack(HEADER); }
		// Multiple tracks.
		else
			{
			// Global track. Holds information such as tempo and time signature.
			int filePos = processTrack(HEADER);
			cout << "File Position: " << filePos << " | File Size: "
			<< fileSize ENDLINE
			bool moarTrack = true;
			while(filePos < fileSize && moarTrack)
				{
				// Resets the tempo to the first value.
				currentTempo = tracks.at(0).getTempo(0).tempo;
				cout << "Track Count: " << tracks.size() ENDLINE
				// cout << "Current Tempo: " << currentTempo << " Division: "
				// << division ENDLINE
				filePos = processTrack(filePos);
				if(arrN != -1 && tracks.size() >= arrN)
					{ moarTrack = false; }
				}
			}	
		}
	}
	
void MIDIRead::testFile(string s)
	{
	string fileName = "testFile.txt";
	ofstream file;
	file.open(fileName.c_str()); // This is the file we're writing to.
	
	file << "Test File for " << s << ".\n" ENDLINE
	
	cout << "Tracks: " << tracks.size() ENDLINE
	
	for(vector<Track>::iterator it = tracks.begin(); it != tracks.end(); ++it) 
	// for(int it = 0; it < tracks.size(); ++it)
		{
		file << "Track " << (it - tracks.begin()) << ":" ENDLINE
		Track& cT = *it;
		// file << "Track" << it << ":" ENDLINE
		// Track& cT = tracks.at(it);
		// Should print all notes in a track.
		cout << "Number of Notes: " << cT.getNoteSize() ENDLINE
		int i = 0;
		while(i >= 0)
			{
			Note cN = cT.getNote(i);
			if(cN.getPitch() >= 0)
				{
				file << "Note " << i << " - Time: " 
				<< cN.getTime() << " | String: " << cN.getString() 
				<< " | Pitch: " << cN.getPitch() ENDLINE
				++i;
				}
			else { i = -1; }
			}
		file << "\nMetadata:" ENDLINE
		i = 0;
		while(i >= 0)
			{
			
			}
		file << "END OF TRACK. \n" ENDLINE
		}
	
	file.close();
	}

// Private ====

int MIDIRead::getMIDI(string midiName)
	{
	// Taking the content from a MIDI file.
	ifstream midi;
	ifstream::pos_type midiSize;
	midiName += ".mid";
	midi.open(midiName.c_str(), ios::in | ios::binary | ios::ate);
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
	else { cout << "File could not be opened." ENDLINE } 
	midi.close();
	
	return (int)midiSize;
	}
	
int MIDIRead::processTrack(int beginPoint) // Returns end-position
	{
	/* All MIDI tracks have a 14-byte header, so we know more or less ignore
	them. It does contain the 'division' information we need, though.
	
	We can ignore a further 4 bytes, as it is simply the marker for the 
	track section. Another 4 bytes reveals the length of the track section.
	
	Events begin with a delta-time followed by a byte which reveals the 
	nature of the event in question. Delta-times are of variable length,
	as are many of the meta-events.
	
	Further complicating matters is that the channel byte only seems to be
	present after a delta-time of non-zero. At least, this seems to be the
	case in Logic. */
	
	Track currentTrack;
	
	timer = DEFAULTOFFSET; // Reset the timer.
	currentTempo = DEFAULTTEMPO; // Reset the tempo.
	int delta = 0; int deltaCount = 0; float deltaConv = 0;
	float trackSize = convertBytes2Int((beginPoint + 5),2);
	beginPoint += TRACKHEAD; // Now we have the track size, we can move on.
	/* Debug variables
	int count = 0, countOn = 0, countOff = 0; */
	for(int i = beginPoint; i < trackSize + beginPoint;) 
		{
		// Unsigned chars are necessary for the hex-based comparisons work.
		unsigned char currentByte = memblock[i]; // Read byte.
		// This is some dumbass, convoluted shit.
		if(currentByte < 0x80) // 'Should' be the last delta-time byte
			{
			delta = convertBytes2VLQ(i,deltaCount);
			deltaConv = convertDelta2TimeFloat(delta);
			// cout << i << " " << deltaConv << " | ";
			if(memblock[i+1] < 0x80)
			/* This one requires an explanation. As mentioned above, the 
			event-type/channel byte would appear to only show up if there 
			is a non-zero delta-time. Presumably, different event types 
			would show up if relevant, but there might be some sorting done
			by Logic. Different pitches don't cause the event byte to 
			pop up again.
			
			This probably shouldn't cause any conflicts, as the values 
			reserved for notes (0x0-0x7F) don't coincide with event values.
			*/
				{ 
				addNote(i, deltaConv, currentTrack);	
				i += METALENGTH-1; // 
				}
			else if (memblock[i+1] == 0xFF)
			// Meta-event
				{
				Meta m;
				m.time = deltaConv;
				switch(memblock[i+2]) // Determines meta-event type
					{					
					case 0x01: // Text Event. Useful for RS-specific stuff.
						{
						int metaType = 0;
						m.text = convertBytes2String(i+4,(int)memblock[i+3]-1);
						switch(memblock[i+4])
							{
							case 'A': // Anchor
								metaType = eAnchor; break;
							case 'C': // Chord name
								metaType = eChord; break;
							case 'P': // Phrase name
								metaType = ePhrase; break;
							case 'T': // Technique
								metaType = eTech; break;
							case 'X': // Special cases
								metaType = eSpecial; break;
							}
						currentTrack.addMeta(metaType, m);
						}
						break;
					case 0x03: // Track Name meta-event
						{
						string s = convertBytes2String(i+3,(int)memblock[i+3]);
						currentTrack.setName(s);
						
						/* string arr = "";
						int length = (int)memblock[i+3];
						for(int j = 0; j <= length; j++) 
							{ arr += memblock[i+4+j]; }
						arrName.push_back(arr);
						cout << "Arrangement name: " << arr ENDLINE */
						}
						break;
					case 0x04: // Instrument Name meta-event. Not much use.
						break;
					case 0x05: // Lyrics.
						m.text = convertBytes2String(i+3,(int)memblock[i+3]); 
						currentTrack.addMeta(eLyrics, m); break;
					case 0x06: // Marker meta-event.
						m.text = convertBytes2String(i+3,(int)memblock[i+3]);
						currentTrack.addMeta(eMarker, m); break;
					case 0x51: // Set Tempo. Very necessary.
					// Tempo changes shouldn't break everything, hopefully.
						{
						float t = convertBytes2Int(i+3,(int)memblock[i+3]);
						currentTempo = ONEMINUTEMICRO / t; 
						currentTrack.addTempo(deltaConv, currentTempo);
						/* Debug Corner
						cout << i << " " << currentTempo << " " << t 
						<< " " << deltaConv ENDLINE
						cout << i << " " << currentTempo << " " << t 
						<< " " << timer ENDLINE */
						}
						break;
					case 0x54: // SMPTE offset. Also necessary.
						int hour, minute, second, frame, fraction;
						// hour = memblock[i+4]; Not playing nicely.
						hour = 0;
						minute = memblock[i+5];
						second = memblock[i+6];
						frame = memblock[i+7];
						fraction = memblock[i+8];
						timer += convertSMPTE2TimeFloat(hour, minute, 
										second, frame, fraction);
						break;
					case 0x58: // Time Signature. Good to know at some point.
						{
						TimeSig time;
						int num, denom, clock, quart;
						time.num = (int)memblock[i+4];
						time.denom = (int)memblock[i+5];
						time.clock = (int)memblock[i+6];
						time.quart = (int)memblock[i+7];
						currentTrack.addTimeSig(time);
						}
						break;
					case 0x59: // Key Signature meta-event. Irrelevant.
						break;
					case 0x2F: // 'End of Track' meta-event
						if(timer > endTime)
							{ endTime = timer; }
						/* cout << "File Position: " << i+3 
						<< " | End Time: " << endTime << "\n"; */
						currentTrack.setDuration(timer);
						// We want a single note vector, so we sort now.
						currentTrack.sortNotes();
						tracks.push_back(currentTrack); 
						return trackSize + beginPoint;
						break;
					}
				/* Debug corner
				cout << "Delta: " << delta 
				<< " | Event Type and Channel: " << (int)memblock[i+1] 
				<< " " << (int)memblock[i+2] << " " << (int)memblock[i+3] 
				<< " | Position: " << i << "\n"; */
					
				i += ((int)memblock[i+3] + METALENGTH); 
				/* Offsets the pointer past the meta-event.
				The first value offsets past the "contents" of the
				meta-event (revealed in byte +3). The second value offsets 
				past the delta-time, meta-event and length bytes. */ 
				}
			else if (memblock[i+1] >= 0x80 && memblock[i+1] < 0xA0)
			// Note-on and -off messages.
				{
				addNote(i, deltaConv, currentTrack);
				i += METALENGTH;
				}
			else if(memblock[i+1] >= 0xB0 && memblock[i+1] < 0xC0)
			/* Controller events. Don't really know how this will affect
			our charts but they may have implications down the line. */
				{
				// All controller events are 3 bytes in length.
				// cout << i << " - Controller event." ENDLINE
				i += METALENGTH;
				}
			else if(memblock[i+1] >= 0xC0 && memblock[i+1] < 0xE0)
			// Program change and aftertouch events. Irrelevant to us.
				{
				// cout << i << " - Program Change event." ENDLINE
				i += METALENGTH-1;
				}
			else if(memblock[i+1] >= 0xE0 && memblock[i+1] < 0xF0)
			/* Pitch Bend events. These will be necessary to parse to get
			a direct line to string-bend events. An intermediary for now is
			using the 'T' text event to specify bends. */
				{
				// cout << i << " " << timer << " - Pitch Bend event." ENDLINE
				i += METALENGTH;
				}
			else
				{
				cout << "BORK BORK BORK at: " 
				<< i << " | " << memblock[i] << "\n"; 
				i++; 
				}
			delta = 0; deltaConv = 0; deltaCount = 0;
			}
		else if (currentByte > 0x7F) // Not the last delta-time byte
			{
			deltaCount++;
			if(delta > 0x0FFFFFFF || deltaCount == MAXDELTA)
				{ cout << "Time Float is too large at: " 
				<< i << " | " << memblock[i] << "\n"; }
			i++;
			}
		}
	}
	
void MIDIRead::addNote(int i, float deltaConv, Track& currentTrack)
	{
	unsigned char channel, pitch, vel;
	channel = memblock[i+1];
	pitch = memblock[i+2];
	vel = memblock[i+3];
	Note n(deltaConv, channel, pitch, vel);

	/* Putting it into a vector.
	
	Logic, at the very least, ignores the dedicated
	note-off message and instead uses a note-on message
	with a velocity of '0'. Thankfully, note-off messages
	always seem to have a velocity of '0' anyway, so 
	mixing and matching won't kick us in the dick hopefully. */
	if(midiMode == "logic" && vel > 0) 
		{ currentTrack.addNoteOn(n); }
	else if (midiMode == "logic" && vel == 0)
		{ currentTrack.addNoteOff(n); }
	else if(channel >= 0x90) // Note-on
		{ currentTrack.addNoteOn(n); }
	else // Note-off
		{ currentTrack.addNoteOff(n); }
						
	/* Debug corner
	cout << "ID: " << i << " | Delta: " << deltaConv 
	<< " | Event Type and Channel: " << (int)channel 
	<< " | Pitch: " << (int)pitch << " | Velocity: " 
	<< (int)vel << " | Position: " << i << "\n"; 
	
	cout << "Time: " << n.getTime() << " | Pitch: " << n.getPitch() ENDLINE */
	}
	
	
// Private ====	
// Conversion methods for track processing ===
// In goes delta, out goes TIMEFLOAT.
float MIDIRead::convertDelta2TimeFloat(int delta)
	{
	// If we're not dealing with the first track, we need to get the tempo.
	if(tracks.size() >= 1) 
		{ 
		Track& cTrack = tracks.at(0); 
		
		// We need to grab the next tempo.
		Tempo t = cTrack.getTempo(tempoCount+1);
		if(t.time > -1 && timer >= t.time)
			{
			++tempoCount;
			currentTempo = t.tempo;
			// cout << "Current Tempo: " << currentTempo ENDLINE
			}
		
		}
		
	// BPM to seconds
	float beat = ONEMINUTEMILLI / currentTempo; 
	float length = (float)delta / (float)division; 
	timer += (beat * length) / ONESECONDMILLI;
	return timer;
	}
		
float MIDIRead::convertSMPTE2TimeFloat(int hour, 
	int minute, int second, int frame, int fraction)
	{
	float time = 0;
	time += (hour * 60);
	time += second;
	time += (frame / division);
	time += ((fraction / 100) / division);
	return time;
	}

/* This probably already exists somewhere.
The use of pow() compensates for the exponential increase in the value of 
additional bytes. As MIDI uses big-endian numbers, the first byte is the 
largest, which is why the second parameter of pow() represents a decreasing 
number as we continue.
	- offset should be the location of byte BEFORE the first (largest) byte.
	Counterintuitive, but whatever.
	- length is the number of bytes we need to convert. */	
float MIDIRead::convertBytes2Int(int offset, int length) 
	{
	float buffer = 0;
	for(int i = 1; i <= length; i++)
		// 'byte' * pow(256.0, 'decrementing counter')
		{ buffer += (int)memblock[offset+i] * pow(256.0,(length - i)); }
	// It shouldn't be possible to have non-integral values, FYI.
	// cout << "Timer: " << timer << " | Buffer: " << buffer ENDLINE
	return buffer; 
	}
	
float MIDIRead::convertBytes2VLQ(int offset, int length)
	{
	float buffer = memblock[offset];
	for(int i = 1; i <= length; i++)
		{ buffer += ((int)memblock[offset-i] - 0x80) * pow(128.0,i); }
	// cout << (int)byte << " / " << f << " | ";
	return buffer;
	}
	
std::string MIDIRead::convertBytes2String(int offset, int length)
	{
	string s = "";
	// Adds each byte to a string. 
	for(int i = 1; i <= length; i++) // '1' puts us on the first text-byte.
		{ s += memblock[offset+i]; }
	// cout << "Time: " << time << " | Text: " << newText.text ENDLINE
	
	return s;
	}