#ifndef READ_MIDI
#include "MIDIReader.h"
#endif

#include <array>

#define MIDICONTROLLENGTH 3 // Length of MIDI Control Events.

namespace MIDI {
	class ReaderDefault : public MIDI::Reader {
		public:
			ReaderDefault( const std::string& file ) : MIDI::Reader( file ) { };
			/* void debug(); Will generate a file listing the contents of the 
			pulled tracks. For debug purposes. */
		
		private:
			unsigned int 		ProcessNote( unsigned int it, const float& timer, MIDI::Track& track, unsigned char c = 0 );
			unsigned int 		ProcessMeta( unsigned int it, float& timer, MIDI::Track& track );
			unsigned int 		ProcessSysEx( unsigned int it, const float& timer, MIDI::Track& track ) { return 0; };	
	};
	
	unsigned int ReaderDefault::ProcessNote( unsigned int it, const float& timer, MIDI::Track& track, unsigned char c ) {
		// 'c' is there as a safety net for the no-event-byte situation.
		unsigned char channel, pitch, velocity;
		if( c != 0 ) { channel = c; }
		else { channel = memblock[it]; ++it; }
		pitch = memblock[it];
		velocity = memblock[++it];
		unsigned char string = 0;
		if(channel >= 0x90)	{ string = channel - 0x90; }
		else if(channel >= 0x80) { string = channel - 0x80; }
		MIDI::Note n( timer, pitch, string, velocity );
		
		/* Putting it into a vector.
		
		Logic, at the very least, ignores the dedicated
		note-off message and instead uses a note-on message
		with a velocity of '0'. Thankfully, note-off messages
		always seem to have a velocity of '0' anyway, so 
		mixing and matching won't kick us in the dick hopefully. */
		if( 0 != 0 && velocity > 0 ) 
			{ track.AddNote( n ); }
		else if ( 0 != 0 && velocity == 0 )
			{ track.AddNote( n, 0 ); }
		else if( channel >= 0x90 ) // Note-on
			{ track.AddNote( n ); }
		else if( channel >= 0x80 ) // Note-off
			{ track.AddNote( n, 0 ); }
							
		/* Debug corner
		std::cout << "\tChannel: " << (int)channel - 0x80 << " String: " << string 
		<< " Pitch: " << (int)pitch << " Velocity: " << (int)velocity 
		<< " Time: " << timer << " Tempo: " << currentTempo; */
		
		return it;
	}
	
	unsigned int ReaderDefault::ProcessMeta( unsigned int it, float& timer, MIDI::Track& track ) {
		unsigned int type = it;
		// The next byte contains the length of the proceeding section.
		++it;
		unsigned int range = memblock[it];
		++it;
		range += it;
		std::vector<unsigned char> contents;
		for( it = it; it != range; ++it ) { contents.push_back(memblock[it]); }
		
		switch( memblock[type] ) { // Determines meta-event type					
			case 0x01: { // Text Event. Useful for RS-specific stuff.
				eMeta metaType;
				switch( contents.at(0) ) {
					case 'A': metaType = eMeta::ANCHOR; break;
					case 'C': metaType = eMeta::CHORD; break;
					case 'E': metaType = eMeta::EVENT; break;
					case 'P': metaType = eMeta::PHRASE; break;
					case 'T': metaType = eMeta::TECHNIQUE; break;
					case 'X': metaType = eMeta::SPECIAL; break;
				}	
				std::vector<unsigned char> contentsMod;
				for( auto jt = contents.begin() + 1; jt != contents.end(); ++jt ) 
					{ contentsMod.push_back(*jt); } 
				if( metaType == eMeta::ANCHOR ) {
					MetaUInt m( metaType, timer, 
						(unsigned int)ConvertBytes2Float(contentsMod) );
					track.AddAnchor( m );
				} else if ( metaType == eMeta::BEND ) {
					MetaFloat m( metaType, timer, ConvertBytes2Float(contentsMod) );
					track.AddBend( m );
				} else {
					MetaString m( metaType, timer, ConvertBytes2String(contentsMod) );
					track.AddMetaString( m, metaType );
				}
			} break;
			// Track Name meta-event
			case 0x03: track.name = ConvertBytes2String(contents); break;
			case 0x04: break; // Instrument Name meta-event. Not much use.
			case 0x05: { // Lyrics.
				MetaString m( eMeta::LYRICS, timer, ConvertBytes2String(contents) );  
				track.AddMetaString( m, eMeta::LYRICS ); 
			} break;
			case 0x06: { // Marker meta-event.
				MetaString m( eMeta::MARKER, timer, ConvertBytes2String(contents) );
				track.AddMetaString( m, eMeta::MARKER ); 
			} break;
			case 0x51: { // Set Tempo.
				float t = ConvertBytes2Float(contents);
				currentTempo = ONEMINUTEMICRO / t; 
				Tempo m( timer, t );
				track.AddTempo( m );
			} break;
			// SMPTE offset.
			case 0x54: timer += ConvertSMPTE2Time(contents, division); break; 
			case 0x58: { // Time Signature. Good to know at some point.
				std::array<unsigned int, 4> i = { { contents.at(0), contents.at(1), contents.at(2), contents.at(3) } };
				TimeSig time( timer, i );
				/* time.num = contents.at(0);
				time.denom = contents.at(1);
				time.clock = contents.at(2);
				time.quart = contents.at(3); */
				track.AddTimeSig( time );
			} break;
			case 0x59: break; // Key Signature.	
			case 0x2F: break; // End of Track.
		}
		/* Debug corner
		std::cout << "\tit: " << type << " Type: " << std::hex 
		<< (int)memblock[type] << " Range: " << std::dec << range 
		<< " Contents size: " << contents.size() << " Time: " << timer << "\n"; */
		return it;
	}
};

