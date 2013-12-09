from copy import deepcopy
from decimal import Decimal
from fractions import Fraction
from itertools import zip_longest
from operator import attrgetter

import MIDIObjects as MIDI
import RSXMLObjects as RSXML

class RSXMLArrangerFlags( object ):
	class _DEFAULT:
		pass

	def __init__( self ):
		self.quantise			= 2
		self.hasChanneledBends	= True
		self.hasManualAnchors	= True

class Compatibility:
	_TRACKNAMES		= { "Chord": "Rhythm", "Combo": "Lead", "Combo 2": "Rhythm", "Single Note": "Lead", "Bass": "Bass" }
	_TUNINGS		= { "StandardE": "E Standard", "DropD": "Drop D", "OpenG": "Open G", "StandardEb": "Eb Standard" }

	_PITCHBENDLOGIC	= float( Fraction( 128, 11 ) ) # What the fuck is this shit.

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

def YieldBends( note = RSXML.Note(), bends = RSXML.Track().helper.bends, isChanneled = True ):
	# Horribly inefficient.
	bendValues = []
	if note.time is None:
		note.time = -1.0
	start = note.time
	end = start + note.techniques[ "sustain" ]

	bendsIter = None
	if isChanneled is True:
		bendsIter = iter( bends[ note.string ] )
	else:
		bendsIter = iter( bends )

	try:
		bend = next( bendsIter )
	except StopIteration as e:
		return None

	while bend.time != None and bend.time < end:
		if bend.time >= start:
			bendValues.append( bend )

		try:
			bend = next( bendsIter )
		except StopIteration as e:
			bend = RSXML.BendValue()

	return bendValues

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
		if nextTempo is not None and currentTime > nextTempo.time:
			currentTempo = nextTempo
			try:
				nextTempo = next( tempoIter )
			except StopIteration as e:
				nextTempo = None
		if nextTimeSig is not None and currentTime > nextTimeSig.time:
			currentTimeSig = nextTimeSig
			try:
				nextTimeSig = next( timeSigIter )
				currentBeat = 0
			except StopIteration as e:
				nextTimeSig = None

		measure = -1
		# The first beat of a bar holds the bar number in the 'measure' attribute.
		if currentBeat % currentTimeSig.numerator == 0:
			currentBar += 1
			measure = currentBar

		ebeats.append( RSXML.Ebeat( currentTime, measure ) )

		currentBeat += 1
		currentTime += Decimal( ( MIDI.Default._ONEMINUTE / currentTempo.tempo ) ).quantize( Decimal( '0.001' ) )

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

def ProcessMetaEvents( midi = MIDI.Track(), rsxml = RSXML.Track() ):
	# This function covers the various Text meta-events stored per-Track.

	# Phrase preparation
	phrases = []
	phraseIterations = []
	phraseIDs = {}
	phraseVariations = {}

	# Chord preparation
	chordNames = {}

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
			# Redundancy is acceptable at this time, as we cannot yet substantiate potential fret/finger differences between chord inversions.
			chordNames[ event.time ] = str( message )

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
				rsxml.helper.quantise = int( message )
			except ValueError as e:
				print( str( e ) )
				pass

		# Tuning - Will probably have to think of a solution for variable tunings (ints), but for now strings suffice.
		elif textType == 'T':
			if message in Compatibility._TUNINGS:
				message = Compatibility._TUNINGS[ message ]
			if message in RSXML.Tuning._DEFAULT:
				rsxml.meta[ "tuning" ] = RSXML.Tuning._DEFAULT[ message ]

	rsxml.structure[ "phrases" ] = phrases
	rsxml.structure[ "phraseIterations" ] = phraseIterations
	rsxml.helper.chordNames = chordNames

def CompileBends( midi = MIDI.Track(), rsxml = RSXML.Track() ):
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
			rsxml.helper.bends[ bend.channel ].append( RSXML.BendValue( bend.time, ConvertPitchBendToFloat( bend.bend ) ) )
			rsxml.helper.bendsOmni.append( RSXML.BendValue( bend.time, ConvertPitchBendToFloat( bend.bend ), bend.channel ) )
		elif ( last.bend <= bend.bend > next.bend ) or ( last.bend >= bend.bend < next.bend ):
			# Should be the end of a peak or trough.
			rsxml.helper.bends[ bend.channel ].append( RSXML.BendValue( bend.time, ConvertPitchBendToFloat( bend.bend ) ) )
			rsxml.helper.bendsOmni.append( RSXML.BendValue( bend.time, ConvertPitchBendToFloat( bend.bend ), bend.channel ) )
		
		last = bend	
		
def CompileNotes( midi = MIDI.Track(), rsxml = RSXML.Track() ):
	''' Converts MIDI notes to RSXML notes.

	The mismatch-correction logic needs to be comprehensively tested. 
	A mismatch between note-on and -off events will be resolved as follows:
		- Produce a functional <note> sans sustain.
		- Add off-event to buffer.
		- For off-event buffer:
			- Reverse-iterate over the <note> list (until off.time is less than on.time) to find a note with sustain="0".
			- Check if the last buffered off event matches the string and fret, and if so pop it from the buffer.

	Theoretically, a mismatched on-off sequence should not exist in isolation.
	By checking the entire buffer each time it happens, the buffer should stay empty across instances of mismatching.

	Theoretically.
	'''
	tuning = rsxml.meta[ "tuning" ]
	strings = Default._MAXSTRINGS[ "Guitar" ]
	if rsxml.meta[ "arrangement" ] == Default._ARRANGEMENTTYPES[ 2 ]: # Is this arrangement for the bass? Not terribly reliable.
		strings = Default._MAXSTRINGS[ "Bass" ]

	# Mismatches between notes-on and notes-off could be fatal. Little buffer for confidence.
	buffer = []
	failNotes = []

	for on, off in zip_longest( midi.noteOn, midi.noteOff, fillvalue = None ):
		string = on.channel
		fret = on.primary - tuning[ string ] - Default._PITCHESHIGH[ string ]
		note = RSXML.Note( on.time, string, fret )

		# Note difficulty, based on velocity. Gets added to a track-wide list of velocities if not yet present.
		note.minDifficulty = on.secondary
		if note.minDifficulty not in rsxml.helper.difficulties:
			rsxml.helper.difficulties.append( note.minDifficulty )

		if on.channel == off.channel:
			duration = off.time - on.time
			note.techniques[ "sustain" ] = duration
			if ( fret < 0 or fret > Default._MAXFRETS ) or ( string < 0 or string >= strings ):
				failNotes.append( note )
			else:
				rsxml.transcriptionTrack.notes.append( note )
		else:
			if ( fret < 0 or fret > Default._MAXFRETS ) or ( string < 0 or string >= strings ):
				failNotes.append( note )
			else:
				rsxml.transcriptionTrack.notes.append( note )
			buffer.append( off )

			# Process for inserting popped buffer notes into transcription track.
			for onRev in reversed( rsxml.transcriptionTrack.notes ):
				# Buffer[0] should be the earliest
				if len( buffer ) > 0:
					i = 0
					while i < len( buffer ):
						string = buffer[ i ].channel
						if onRev.time < buffer[ i ].time and onRev.string == string:
							fret = buffer[ i ].primary - tuning[ string ] - Default._PITCHESHIGH[ string ]
							if onRev.fret == fret:
								duration = buffer[ i ].time - onRev.time
								onRev.techniques[ "sustain" ] = duration
								buffer.pop( i )
								break
						i += 1
				else:
					break		

	for off in buffer:
		print( "Note Off Time: " + str( off.time ) + " String: " + str( off.channel ) + " Pitch: " + str( off.primary ) )

	for note in failNotes:
		print( "Couldn't add note to list - Time: " + str( note.time ) + " String: " + str( note.string ) + " Fret: " + str( note.fret ) )

def BendNotes( rsxml = RSXML.Track() ):
	bendIter = []
	currentBend = []
	for i in Default._MAXSTRINGS[ "Guitar" ]:
		bendIter.append( iter( midi.controller ) )
		try:
			currentBend[ i ] = next( bendIter[ i ] )
		except StopIteration as e:
			currentBend[ i ] = None

	for note in rsxml.transcriptionTrack.notes:
		start = note.time
		end = note.time + note.sustain

		if start >= 0:
			while currentBend[ note.string ] != None and currentBend[ note.string ].time <= end:
				if currentBend[ note.string ].time >= start:
					note.bendValues.append( currentBend[ note.string ] )

				try:
					currentBend[ note.string ] = next( bendIter[ note.string ] )
				except StopIteration as e:
					currentBend[ note.string ] = None

def ApplyTechniques( midi = MIDI.Track(), rsxml = RSXML.Track() ):
	events = iter( midi.controller )
	try:
		event = next( events )
	except StopIteration as e:
		event = MIDI.Event()

	flags = RSXML.Note._TECHNIQUES
	for note in rsxml.transcriptionTrack.notes:
		if event.time != None and note.time > event.time:
			try:
				event = next( events )
			except StopIteration as e:
				event = MIDI.Event()

			if event.primary == Default._TECHNIQUECONTROLLER[ "bend" ] and len( note.bendValues ) == 0:
				# A flag for bends when Pitch Wheel doesn't want to play with channels.
				note.bendValues = YieldBends( note, rsxml.helper.bendsOmni, False )
				
				maxBend = 0.0
				for i in bends:
					if i.step > maxBend: 
						maxBend = i.step

				if maxBend < 1.0:
					maxBend = 1.0

				note.bend = maxBend
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
		note.techniques = flags.copy()

def QuantiseNotes( rsxml = RSXML.Track() ):
	''' If a note's duration is less than a specified quantisation amount, the value is reset to 0. '''
	thisBeat = None
	nextBeat = None

	beatsIter = iter( rsxml.structure[ "ebeats" ] )
	try:
		nextBeat = next( beatsIter )
	except StopIteration as e:
		nextBeat = None

	beat = None
	for note in rsxml.transcriptionTrack.notes:
		while nextBeat is not None and nextBeat.time <= note.time:
			try:
				thisBeat = nextBeat
				nextBeat = next( beatsIter )
			except StopIteration as e:
				nextBeat = None

		if nextBeat is not None:
			beat = nextBeat.time - thisBeat.time
			if note.sustain < ( beat / rsxml.helper.quantise ): 
				note.sustain = 0

def CompileChords( rsxml = RSXML.Track() ):
	''' Process Note list to identify notes within a chord, separating the two as the RSXML format requires.. '''
	notes = []
	chords = []

	buffer = []
	buffer.append( rsxml.transcriptionTrack.notes[ 0 ] )
	bufferTime = rsxml.transcriptionTrack.notes[ 0 ].time
	for note in rsxml.transcriptionTrack.notes[ 1: ]:
		if note.time > bufferTime:
			if len( buffer ) > 1:
				chord = RSXML.Chord( bufferTime )
				chord.notes = buffer.copy()

				for n in chord.notes:
					if n.minDifficulty > chord.minDifficulty:
						chord.minDifficulty = n.minDifficulty

				chords.append( chord )

			else:
				try:
					notes.append( buffer[ 0 ] )
				except IndexError as e:
					print( "Unexpected error during chord arrangement. Approx time: " + str( note.time ) )

			buffer.clear()

		buffer.append( note )
		bufferTime = note.time

	rsxml.transcriptionTrack.notes = notes
	rsxml.transcriptionTrack.chords = chords

	# Chord IDs, or an attempt at such.
	for chord in chords:
		CompileChordID( chord, rsxml, True )

def CompileChordID( chord = RSXML.Chord(), rsxml = RSXML.Track(), isFirstSort = False ):
	frets = chord.GetFrets()

	for template in rsxml.structure[ "chordTemplates" ]:
		if frets == template.frets:
			chord.chordID = template.id
			break
			
	else:
		chordName = None
		if isFirstSort:
			if chord.time in rsxml.helper.chordNames.keys():
				# Chord names are only needed first time only, so we can safely pop it from the helper dict.
				# Ironically, they could have been useful, but I've already commited to it here, so fuck me.
				try:
					chordName = rsxml.helper.chordNames.pop( chord.time )
				except KeyError as e:
					print( "Unexpected pop disaster at " + str( chord.time ) )
		else:
			if len( chord.notes ) > 2:
				# No need to name a 'double stop'.
				pass

		displayName = None # Use some kind of look-up table, if this works how I think it does.

		fingers = RSXML.ChordTemplate.ConvertFretsToFingers( frets )

		template = RSXML.ChordTemplate( chordName, displayName, fingers.copy(), frets.copy() )
		template.id = len( rsxml.structure[ "chordTemplates" ] )

		rsxml.structure[ "chordTemplates" ].append( template )

		chord.chordID = template.id

def CompileAnchors( rsxml = RSXML.Track() ):
	''' Algorithm to automatically generate anchors. Not yet implemented. '''
	# Clear any manually-added flags. This shouldn't be neccessary, really.
	rsxml.transcriptionTrack.anchors.clear()

	lastNote = None
	thisNote = None

def CompileHandShapes( rsxml = RSXML.Track() ):
	pass

def ArrangeLevels( rsxml = RSXML.Track() ):
	''' Arranges difficulty levels.

	The segregation of notes and chords as discrete concepts and lists presents a challenge to reducing redundant sorting.
	'''
	rsxml.helper.difficulties.sort( reverse = True )

	# Experiment in redundancy-reduction.
	notes = deepcopy( rsxml.transcriptionTrack.notes )
	chords = deepcopy( rsxml.transcriptionTrack.chords )
	#notes = list( rsxml.transcriptionTrack.notes )
	#chords = list( rsxml.transcriptionTrack.chords )
	anchors = rsxml.transcriptionTrack.anchors.copy()
	handShapes = rsxml.transcriptionTrack.handShapes.copy()
	
	print( "Original: " + str( len( rsxml.transcriptionTrack.chords[ 0 ].notes ) ) )
	print( "Copy: " + str( len( chords[ 0 ].notes ) ) )

	for index, velocity in enumerate( rsxml.helper.difficulties ):
		i = 0
		while i < len( notes ):
			if velocity < notes[ i ].minDifficulty:
				note = notes.pop( i )
				# Need to handle removal of hopo flags in the event of cross-difficulty shenanigans.
			else:
				i += 1

		i = 0
		while i < len( chords ):
			if velocity < chords[ i ].minDifficulty:
				chord = chords[ i ]
				# Need to reinsert smaller chord or single note in the event of cross-difficulty shenanigans.
				j = 0
				while j < len( chord.notes ):
					if velocity < chord.notes[ j ].minDifficulty:
						note = chord.notes.pop( j )
						# Need to handle removal of hopo flags in the event of cross-difficulty shenanigans.
					else:
						j += 1

				if len( chord.notes ) > 1:
					oldChordName = rsxml.structure[ "chordTemplates" ][ chord.chordID ].chordName # Part of cheesy hack.

					chord.chordID = None
					CompileChordID( chord, rsxml )

					i += 1
					
					# Cheesy hack to derive chord name for now.
					if len( chord.notes ) > 2:
						rsxml.structure[ "chordTemplates" ][ chord.chordID ].chordName = oldChordName

				else:
					if len( chord.notes ) == 1:
						note = chord.notes[ 0 ]
						note.isChord = False
						notes.append( note )

					chords.pop( i )
				
			else:
				i += 1

		notes.sort( key = attrgetter( "time" ) ) # Sorts notes derived from chords list into notes list.

		# level = RSXML.Level( False, rsxml.helper.difficulties[ index ] )
		level = RSXML.Level( False, len( rsxml.helper.difficulties ) - 1 - index )
		level.notes = deepcopy( notes )
		level.chords = deepcopy( chords )
		#level.notes = list( notes )
		#level.chords = list( chords )
		level.anchors = anchors.copy()
		level.handShapes = handShapes.copy()

		try:
			print( "Difficulty: " + str( level.difficulty ) + " Time: " + str( chords [ 0 ].time ) + " Notes: " + str( len( chords[ 0 ].notes ) ) )
			print( "Difficulty: " + str( level.difficulty ) + " Time: " + str( level.chords [ 0 ].time ) + " Notes: " + str( len( level.chords[ 0 ].notes ) ) )
		except IndexError as e:
			pass

		rsxml.levels.append( level )
	
	try:
		print( "Original: " + str( len( rsxml.transcriptionTrack.chords[ 0 ].notes ) ) )
		print( "Copy: " + str( len( chords[ 0 ].notes ) ) )
	except IndexError as e:
			pass

	# 'Undo' reversal.
	rsxml.levels.reverse()

	# Attempt to remove redundant phrases or sections across levels.
	lastLevel = None
	for level in rsxml.levels:
		pass
		try:
			print( "Difficulty: " + str( level.difficulty ) + " Time: " + str( level.chords [ 0 ].time ) + " Notes: " + str( len( level.chords[ 0 ].notes ) ) )
		except IndexError as e:
			pass

def RSXMLArranger( midi = MIDI.File(), flags = RSXMLArrangerFlags() ):
	rsxml = RSXML.Track()
	helper = flags

	# Start by assembling the applicable metadata from the global MIDI track.
	rsxml.structure[ "ebeats" ] = ArrangeEbeats( midi )
	rsxml.structure[ "sections" ] = ArrangeSections( midi )
	rsxml.meta[ "version" ] = 0.1
	rsxml.meta[ "title" ] = midi.name
	rsxml.meta[ "offset" ] = midi.globalTrack.smpteOffset
	rsxml.meta[ "songLength" ] = midi.length
	rsxml.meta[ "startBeat" ] = 0
	averageTempo = Decimal( 0 ).quantize( Decimal( '0.001' ) )
	for tempo in midi.globalTrack.tempo:  
		averageTempo += tempo.tempo
	averageTempo /= len( midi.globalTrack.tempo )
	rsxml.meta[ "averageTempo" ] = Decimal( averageTempo ).quantize( Decimal( '0.001' ) ) # 
	rsxml.meta[ "crowdSpeed" ] = 1

	tracks = []
	for midiTrack in midi.tracks:
		rsxmlTrack = RSXML.Track()
		rsxmlTrack.meta = rsxml.meta.copy()
		rsxmlTrack.structure = rsxml.structure.copy()
		rsxmlTrack.helper.quantise = flags.quantise

		trackName = midiTrack.nameTrack
		if trackName in Compatibility._TRACKNAMES:
			trackName = Compatibility._TRACKNAMES[ trackName ]
		rsxmlTrack.meta[ "arrangement" ] = trackName
		rsxmlTrack.meta[ "part" ] = 1 # Not sure what the relevence of this is.
		
		ProcessMetaEvents( midiTrack, rsxmlTrack )

		CompileBends( midiTrack, rsxmlTrack )

		CompileNotes( midiTrack, rsxmlTrack )
		if flags.hasChanneledBends is True:
			BendNotes( rsxmlTrack )
		ApplyTechniques( midiTrack, rsxmlTrack )			
		QuantiseNotes( rsxmlTrack )
		CompileChords( rsxmlTrack )		
		if flags.hasManualAnchors is False:
			CompileAnchors( rsxmlTrack )
		CompileHandShapes( rsxmlTrack )

		ArrangeLevels( rsxmlTrack )

		tracks.append( rsxmlTrack )

	return tracks