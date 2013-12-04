import MIDIObjects as MIDI
import RSXMLObjects as RSXML
from itertools import zip_longest
import fractions

class Compatibility:
	_TRACKNAMES		= { "Chord": "Rhythm", "Combo": "Lead", "Combo 2": "Rhythm", "Single Note": "Lead", "Bass": "Bass" }
	_TUNINGS		= { "StandardE": "E Standard", "DropD": "Drop D", "OpenG": "Open G", "StandardEb": "Eb Standard" }

	_PITCHBENDLOGIC	= float( fractions.Fraction( 128, 11 ) ) # What the fuck is this shit.

class Default:
	_ARRANGEMENTTYPES		= [ "Lead", "Rhythm", "Bass" ]
	_MAXSTRINGS				= { "Guitar": 6, "Bass": 4 }
	_MAXFRETS				= 24
	_PITCHESLOW				= [ 40, 45, 50, 55, 59, 64 ]
	_PITCHESHIGH			= [ 52, 57, 62, 67, 71, 76 ]
	_PITCHBEND				= 12
	# Consider modulation for vibrato and portamento for slide.
	_TECHNIQUECONTROLLER	= { "bend": 29, "hopo": 68, "hammerOn": 68, "pullOff": 68, "harmonic": 20, "harmonicPinch": 21, "mute": 22,
								"palmMute": 23, "pluck": 30, "slap": 31, "slideTo": 24, "slideUnpitchTo": 25, "tap": 26, "tremolo": 27, "vibrato": 28 }	

def ConvertPitchBendToFloat( bend = 8192, semiTones = Compatibility._PITCHBENDLOGIC ):
	# Assumes a +- 12 semitone range for pitch wheel*. Will add parameter in future.
	halfRange = int( MIDI.Default._PITCHBENDRANGE / 2 )	
	bend = bend - halfRange									# Orients the value around the midpoint.
	bend = int( bend / int( halfRange / semiTones * 2 ) )	# Should slice bend value to quarter tones
	bend = float( bend / 2 )								# Converts back to semitones, now as a neat float in 0.5 increments.
	return bend

def YieldBend( time = 0.0, bends = RSXML.Track().bends ):
	pass

def ArrangeEbeats( file = MIDI.File() ):
	ebeats = []

	tempoIter = iter( file.globalTrack.tempo )
	try:
		nextTempo = next( tempoIter )
	except StopIteration as e:
		nextTempo = MIDI.Tempo( -1, 120 )
			
	timeSigIter = iter( file.globalTrack.timeSig )
	try:
		nextTimeSig = next( timeSigIter )
	except StopIteration as e:
		nextTimeSig = MIDI.TimeSig( -1 )

	currentBar = 0
	currentBeat = 0
	currentTime = file.globalTrack.smpteOffset
	currentTempo = nextTempo
	currentTimeSig = nextTimeSig
	while currentTime < file.length:
		if nextTempo.time > -1 and currentTime > nextTempo.time:
			currentTempo = nextTempo
			try:
				nextTempo = next( tempoIter )
			except StopIteration as e:
				nextTempo = MIDI.Tempo( -1, 120 )
		if nextTimeSig.time > -1 and currentTime > nextTimeSig.time:
			currentTimeSig = nextTimeSig
			try:
				nextTimeSig = next( timeSigIter )
				currentBeat = 0
			except StopIteration as e:
				nextTimeSig = MIDI.TimeSig( -1 )

		measure = -1
		# The first beat of a bar holds the bar number in the 'measure' attribute.
		if currentBeat % currentTimeSig.numerator == 0:
			currentBar += 1
			measure = currentBar

		ebeats.append( RSXML.Ebeat( currentTime, measure ) )

		currentBeat += 1
		currentTime += ( 60.0 / currentTempo.tempo )

	return ebeats

def ArrangeSections( file = MIDI.File() ):
	sections = []

	sectionFrequency = { }

	for midi in file.globalTrack.metaEvent:
		if midi.type == MIDI.Event.eMeta._MARKER:
			try:
				sectionFrequency[ midi.message ] += 1
			except KeyError as e:
				sectionFrequency[ midi.message ] = 1
			
			sections.append( RSXML.Section( midi.message, sectionFrequency[ midi.message ], midi.time ) )

	return sections

def ArrangeMetaEvents( midi = MIDI.Track(), rsxml = RSXML.Track() ):
	# This function covers the various Text meta-events stored per-Track.

	# Phrase preparation
	phrases = []
	phraseIterations = []
	phraseIDs = {}
	phraseVariations = {}

	# Chord preparation
	chordNames = []

	for event in midi.metaEvent:
		textType = event.message[0] 
		message = event.message[1:]

		# Anchors - There should be a better way to do this. No support for variable width at present.
		if textType == 'A':
			rsxml.transcriptionTrack.anchors.append( RSXML.Anchor( event.time, message ) )

		# Bends - Provided for compatibility purposes. Will likely deprecate as soon as pitch bend events are working and tested.
		elif textType == 'B':
			pass

		# Chord Names - Not sure what alternative there is, here.
		elif textType == 'C':
			pass

		elif textType == 'E':
			rsxml.structure[ "events" ].append( RSXML.Event( event.time, message ) )

		# Phrases
		elif textType == 'P':
			if message in phraseIDs:
				phraseVariations[ message ] += 1
			else:
				phrases.append( RSXML.Phrase( message ) )
				phraseIDs[ message ] = len( phraseIDs )
				phraseVariations[ message ] = 0
			
			phraseIterations.append( RSXML.PhraseIteration( event.time, phraseIDs[ message ], phraseVariations[ message ] ) )

		# "Quantize Duration" - this specifies a minimum duration, as a fraction of beats (what was I thinking?), for when notes are sustained.
		elif textType == 'Q':
			try:
				rsxml.quantize = int( message )
			except ValueError:
				pass

		# Tuning - Will probably have to think of a solution for variable tunings (ints), but for now strings suffice.
		elif textType == 'T':
			if message in Compatibility._TUNINGS:
				message = Compatibility._TUNINGS[ message ]
			if message in RSXML.Tuning._DEFAULT:
				rsxml.meta[ "tuning" ] = RSXML.Tuning._DEFAULT[ message ]

	rsxml.structure[ "phrases" ] = phrases
	rsxml.structure[ "phraseIterations" ] = phraseIterations

def ArrangeBends( midi = MIDI.Track(), rsxml = RSXML.Track() ):
	last = MIDI.PitchBend( None, 0, MIDI.Default._PITCHBENDRANGE / 2 )
	next = None
	for index, bend in enumerate( midi.pitchBend ):
		try:
			next = midi.pitchBend[ index + 1 ]
		except IndexError as e:
			next = MIDI.PitchBend( None, 0, MIDI.Default._PITCHBENDRANGE / 2 )
		
		# print( "Time: " + str( bend.time ) + " Channel: " + str( bend.channel ) + " Bend: " + str( bend.bend ) )
		# print( "Last: " + str( last.bend ) + "\tThis: " + str( bend.bend ) + "\tNext: " + str( next.bend ) )

		# We try to filter bends to just the peaks and troughs. 
		
		if index == 0:
			pass
		elif ( last.bend < bend.bend >= next.bend ) or ( last.bend > bend.bend <= next.bend ):
			# Should be the start of a peak or trough.
			rsxml.bends.append( RSXML.BendValue( bend.time, ConvertPitchBendToFloat( bend.bend ), bend.channel ) )
		elif ( last.bend <= bend.bend > next.bend ) or ( last.bend >= bend.bend < next.bend ):
			# Should be the end of a peak or trough.
			rsxml.bends.append( RSXML.BendValue( bend.time, ConvertPitchBendToFloat( bend.bend ), bend.channel ) )
		
		last = bend	
			
def ArrangeNotes( midi = MIDI.Track(), rsxml = RSXML.Track() ):
	# Converts MIDI notes to RSXML notes. Poorly.
	tuning = rsxml.meta[ "tuning" ]
	strings = Default._MAXSTRINGS[ "Guitar" ]
	if rsxml.meta[ "arrangement" ] == Default._ARRANGEMENTTYPES[ 2 ]: # Is this arrangement for the bass? Not terribly reliable.
		strings = Default._MAXSTRINGS[ "Bass" ]

	# Mismatches between notes-on and notes-off could be fatal. Little buffer for confidence.
	bufferOn = []
	bufferOff = []
	failNotes = []

	for index, ( on, off )in enumerate( zip_longest( midi.noteOn, midi.noteOff, fillvalue = MIDI.ChannelEvent( -1.0, 0, 0, 0 ) ) ):
		if on.channel == off.channel:
			string = on.channel
			fret = on.primary - tuning[ string ] - Default._PITCHESHIGH[ string ]

			note = RSXML.Note( on.time, string, fret )
			note.difficulty = on.secondary

			if ( fret < 0 or fret > Default._MAXFRETS ) or ( string < 0 or string >= strings ):
				failNotes.append( note )
			else:
				rsxml.transcriptionTrack.notes.append( note )

		else:
			# Some process for inserting popped buffer notes into transcription track.

			# Otherwise, append the buffer and move on.
			bufferOn.append( [ on, index ] )
			bufferOff.append( [ off, index ] )

	for on, off in zip_longest( bufferOn, bufferOff ):
		print( "Time A: " + str( on[0].time ) + " String: " + str( on[0].channel ) + " Pitch: " + str( on[0].primary ) )
		print( "Time B: " + str( off[0].time ) + " String: " + str( off[0].channel ) + " Pitch: " + str( off[0].primary ) )

	for note in failNotes:
		print( "Time: " + str( note.time ) + " String: " + str( note.string ) + " Fret: " + str( note.fret ) )

def ArrangeTechniques( midi = MIDI.Track(), rsxml = RSXML.Track() ):
	events = iter( midi.controller )
	try:
		event = next( events )
	except StopIteration as e:
		event = MIDI.Event( -1.0 )

	flags = RSXML.Note._TECHNIQUES
	for note in rsxml.transcriptionTrack.notes:
		if note.time > event.time and event.time != -1.0:
			try:
				event = next( events )
			except StopIteration as e:
				event = MIDI.Event( -1.0 )

			# _TECHNIQUES = OrderedDict( [ ( "linkNext", 0 ), ( "bend", 0 ), ( "hopo", 0 ), ( "hammerOn", 0 ), ( "pullOff", 0 ), 
			#		( "harmonic", 0 ), ( "harmonicPinch", 0 ), ( "ignore", 0 ), ( "leftHand", 1 ), ( "rightHand", 0 ), 
			#		( "mute", 0 ), ( "palmMute", 0 ), ( "pickDirection", 0 ), ( "pluck", -1 ), ( "slap", -1 ), 
			#		( "slideTo", -1 ), ( "slideUnpitchTo", -1 ), ( "sustain", float( 0 ) ), ( "tap", 0 ), ( "tremolo", 0 ), ( "vibrato", 0 ) ] )

			if event.primary == Default._TECHNIQUECONTROLLER[ "bend" ]:
				# A flag for bends when Pitch Wheel doesn't want to play with channels.
				bend = YieldBend( note.time, rsxml.bends )

			elif event.primary == Default._TECHNIQUECONTROLLER[ "hopo" ]:
				# Covers both hammer on and pull off.
				if event.secondary >= 64:
					flags[ "hopo" ] = 1
					flags[ "hammerOn" ] = 1
					flags[ "hammerOff" ] = 1
				else:
					flags[ "hopo" ] = 0
					flags[ "hammerOn" ] = 0
					flags[ "hammerOff" ] = 0
			elif event.primary == Default._TECHNIQUECONTROLLER[ "harmonic" ]:
				if event.secondary >= 64:
					flags[ "harmonic" ] = 1
				else:
					flags[ "harmonic" ] = 0
			elif event.primary == Default._TECHNIQUECONTROLLER[ "harmonicPinch" ]:
				if event.secondary >= 64:
					flags[ "harmonicPinch" ] = 1
				else:
					flags[ "harmonicPinch" ] = 0
			elif event.primary == Default._TECHNIQUECONTROLLER[ "mute" ]:
				if event.secondary >= 64:
					flags[ "mute" ] = 1
				else:
					flags[ "mute" ] = 0
			elif event.primary == Default._TECHNIQUECONTROLLER[ "palmMute" ]:
				if event.secondary >= 64:
					flags[ "palmMute" ] = 1
				else:
					flags[ "palmMute" ] = 0
			elif event.primary == Default._TECHNIQUECONTROLLER[ "slideTo" ]:
				if 0 < event.secondary <= Default._MAXFRETS:
					flags[ "slideTo" ] = event.secondary
				else:
					flags[ "slideTo" ] = -1
			elif event.primary == Default._TECHNIQUECONTROLLER[ "slideUnpitchTo" ]:
				if 0 < event.secondary <= Default._MAXFRETS:
					flags[ "slideUnpitchTo" ] = event.secondary
				else:
					flags[ "slideUnpitchTo" ] = -1
			elif event.primary == Default._TECHNIQUECONTROLLER[ "tap" ]:
				if event.secondary >= 64:
					flags[ "tap" ] = 1
				else:
					flags[ "tap" ] = 0
			elif event.primary == Default._TECHNIQUECONTROLLER[ "tremolo" ]:
				if event.secondary >= 64:
					flags[ "tremolo" ] = 1
				else:
					flags[ "tremolo" ] = 0
			elif event.primary == Default._TECHNIQUECONTROLLER[ "vibrato" ]:
				if event.secondary >= 64:
					flags[ "vibrato" ] = 1
				else:
					flags[ "vibrato" ] = 0
			# Bass
			elif event.primary == Default._TECHNIQUECONTROLLER[ "pluck" ]:
				pass
			elif event.primary == Default._TECHNIQUECONTROLLER[ "slap" ]:
				pass

		# Apply to note.

def ArrangeChords( midi = MIDI.Track(), rsxml = RSXML.Track() ):
	pass

def ArrangeLevels( midi = MIDI.Track(), rsxml = RSXML.Track() ):
	pass

def RSXMLArranger( midi = MIDI.File() ):
	rsxml = RSXML.Track()
	helper = RSXML.Helper()

	# Start by assembling the applicable metadata from the global MIDI track.
	rsxml.structure[ "ebeats" ] = ArrangeEbeats( midi )
	rsxml.structure[ "sections" ] = ArrangeSections( midi )
	rsxml.meta[ "version" ] = 0.1
	rsxml.meta[ "title" ] = midi.name
	rsxml.meta[ "offset" ] = midi.globalTrack.smpteOffset
	rsxml.meta[ "songLength" ] = midi.length
	rsxml.meta[ "startBeat" ] = 0
	averageTempo = 0.0
	for tempo in midi.globalTrack.tempo:  
		averageTempo += tempo.tempo
	averageTempo /= len( midi.globalTrack.tempo )
	rsxml.meta[ "averageTempo" ] = averageTempo # 
	rsxml.meta[ "crowdSpeed" ] = 1

	tracks = []
	for midiTrack in midi.tracks:
		rsxmlTrack = RSXML.Track()
		rsxmlTrack.meta = rsxml.meta.copy()
		rsxmlTrack.structure = rsxml.structure.copy()

		trackName = midiTrack.nameTrack
		if trackName in Compatibility._TRACKNAMES:
			trackName = Compatibility._TRACKNAMES[ trackName ]
		rsxmlTrack.meta[ "arrangement" ] = trackName
		rsxmlTrack.meta[ "part" ] = 1 # Not sure what the relevence of this is.
		
		ArrangeMetaEvents( midiTrack, rsxmlTrack )

		ArrangeBends( midiTrack, rsxmlTrack )

		ArrangeNotes( midiTrack, rsxmlTrack )	

		print( "Work on YieldBends next." )
			
		
	return tracks