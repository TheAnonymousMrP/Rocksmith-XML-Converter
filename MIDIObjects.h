#ifndef _MIDI_READ_OBJECTS_
#define _MIDI_READ_OBJECTS_

#ifndef _BASE_NOTE
#include "BaseNote.h"
#endif

#ifndef _BASE_META
#include "BaseMeta.h"
#endif

#include <vector>

namespace MIDI {
	class Note : public Base::GuitarNote { 
		public:
			Note( const float& tim = -1.0, const unsigned char& pit = 0x00, 
				const unsigned char& str = 0x00, const unsigned int& dif = 0 ) 
				: Base::GuitarNote( tim, pit, str ) { arbitraryDifficulty = dif; };
				
			const unsigned int&		GetArbitraryDifficulty() const 
										{ return arbitraryDifficulty; };
	
		private:
			unsigned int 	arbitraryDifficulty;
	};
	
	enum eTrackType {
		VOCAL,
		SINGLE,
		COMBO,
		CHORDS,
		BASS,
		BASS_PICK,
	};
	
	class Track {
		public:
			Track();
			
			eTrackType		type;
			std::string		name;
			float			duration;
			
			void 	AddNote( const MIDI::Note& note, bool on = 1 ) {
						if( on ) { noteOn.push_back( note ); }
						else { noteOff.push_back( note ); }	
					};
			void 	AddAnchor( const MetaUInt& meta ) { anchors.push_back( meta ); };
			void 	AddBend( const MetaFloat& meta ) { bends.push_back( meta ); };
			void	AddMetaString( const MetaString& meta, const eMeta& type );
			void 	AddTempo( const Tempo& meta ) { tempos.push_back( meta ); };
			void 	AddTimeSig( const TimeSig& meta ) { timeSigs.push_back( meta ); };
			
			const unsigned int&				GetMaxDifficulty() const 
												{ return maxDifficulty; };
			const std::vector<MIDI::Note>& 	GetNotes( bool on = 1 ) const {
												if( on ) { return noteOn; } 
												else { return noteOff; }	
											};
			const std::vector<MetaUInt>& 	GetAnchors() const { return anchors; };
			const std::vector<MetaFloat>& 	GetBends() const { return bends; };
			const std::vector<MetaString>&	GetMetaStrings( const eMeta& type ) const;
			const std::vector<Tempo>&		GetTempos() const { return tempos; };
			const std::vector<TimeSig>&		GetTimeSigs() const { return timeSigs; };
			
			/* Because tempo and marker events are in the first, note-less track of the 
			MIDI file, further tracks need to copy these vectors. Further investigation 
			should be made into the plausibility of making them static, but I suspect this
			would have an unintended effect if somewhere down the line multiple files are
			dealt with. */
			void	SetMarkers( const std::vector<MetaString>& mar ) { markers = mar; };
			void 	SetTempos( const std::vector<Tempo>& tem ) { tempos = tem; };
			
			void	NormaliseDifficulties();
			
		private:
			
			unsigned int					maxDifficulty;
		
			std::vector<MIDI::Note> 		noteOn;
			std::vector<MIDI::Note>			noteOff;
			
			std::vector<MetaUInt>			anchors;
			std::vector<MetaFloat> 			bends; 	
			std::vector<MetaString> 		chords; 	// Chord names.
			std::vector<MetaString> 		events;
			std::vector<MetaString> 		markers; 	// Markers; used for sections.
			std::vector<MetaString> 		phrases; 	
			std::vector<MetaString> 		lyrics;
			std::vector<MetaString> 		special;	// Miscellaneous.
			std::vector<MetaString> 		techniques; // Technique meta-events. 
			std::vector<Tempo> 				tempos; 	// Tempo changes.
			std::vector<TimeSig> 			timeSigs; 	// Time Signature changes.
	};
	
	void MIDI::Track::AddMetaString( const MetaString& meta, const eMeta& type ) {
		switch( type ) {
			case eMeta::CHORD: chords.push_back( meta ); break;
			case eMeta::EVENT: events.push_back( meta ); break;
			case eMeta::LYRICS: lyrics.push_back( meta ); break;
			case eMeta::MARKER: markers.push_back( meta ); break;
			case eMeta::PHRASE: phrases.push_back( meta ); break;
			case eMeta::TECHNIQUE: techniques.push_back( meta ); break;
			case eMeta::KEYSIG: 
			case eMeta::TUNING: 
			case eMeta::SPECIAL: special.push_back( meta ); break;
			case eMeta::ANCHOR: 
			case eMeta::BEND:
			case eMeta::TEMPO: 
			case eMeta::TIMESIG:
			default: throw metaException; break;
		}
	}
	
	const std::vector<MetaString>& MIDI::Track::GetMetaStrings( const eMeta& type ) 
		const {
		switch( type ) {
			case eMeta::CHORD: return chords; break;
			case eMeta::EVENT: return events; break;
			case eMeta::LYRICS: return lyrics; break;
			case eMeta::MARKER: return markers; break;
			case eMeta::PHRASE: return phrases; break;
			case eMeta::TECHNIQUE: return techniques;
				break;
			case eMeta::KEYSIG: 
			case eMeta::TUNING: 
			case eMeta::SPECIAL: return special; 
				break;
			case eMeta::ANCHOR: 
			case eMeta::BEND:
			case eMeta::TEMPO: 
			case eMeta::TIMESIG:
			default: throw metaException; break;
		}
	}
	
	void MIDI::Track::NormaliseDifficulties() {
		std::vector<unsigned int> difs;
		bool match = false;
		for( const MIDI::Note& n : noteOn ) {
			match = false;
			for( const unsigned int& i : difs ) {
				if( n.GetArbitraryDifficulty() == i ) { match = true; break; }
			}
			if(!match) { difs.push_back( n.GetArbitraryDifficulty() ); }
		}
		maxDifficulty = difs.size()-1;
	
		// Sorts the vector into order.
		sort(difs.begin(),difs.end());
		
		for( MIDI::Note& n : noteOn ) { 
			// Set a normalised difficulty.
			for( auto it = difs.begin(); it != difs.end(); ++it ) {
				if( n.GetArbitraryDifficulty() == *it ) 
					{ n.normalisedDifficulty = ( it - difs.begin() ); break; }
			}
		}
	}
	
	float ConvertDelta2Time( const int& delta, const int& division, const float& tempo ) {
		float length = (float)delta / (float)division;
		return ConvertTempo2Beat( tempo ) * length;
	}
	
	unsigned int ConvertBytes2VLQ( const std::vector<unsigned char>& vlq ) {
		unsigned int buffer = vlq.back();
		for(auto it = vlq.begin(); it != vlq.end() - 1; ++it) 
			{ buffer += (unsigned int)(*it - 0x80) * pow(128,(vlq.end() - 1 - it)); }
		return buffer;
	}
	
	float ConvertBytes2Float( const std::vector<unsigned char>& b ) {
		float buffer = 0;
		for(auto it = b.begin(); it != b.end(); ++it) 
			{ buffer += (int)*it * pow(256.0,(b.end() - 1 - it)); }
		return buffer;
	}
	
	float ConvertSMPTE2Time( const std::vector<unsigned char>& b, const int& division ) {
		float time = 0;
		// time += (b.at(0) * 60);
		time += b.at(1);
		time += ( b.at(2) / division );
		time += ( ( b.at(3) / 100 ) / division );
		return time;
	}
	
	std::string ConvertBytes2String( const std::vector<unsigned char>& b ) { 
		std::string s = "";
		for(auto& c : b) { s += c; }
		return s;
	}
};










#endif