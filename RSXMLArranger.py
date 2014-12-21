from copy import copy, deepcopy
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

def YieldOmniBends( note = RSXML.Note(), bends = RSXML.Track().helper.bendsOmni ):
	# Seems inefficient. Learn to use actual generators.
	start = None
	end = start
	if note.time is not None:
		start = note.time
		if note.sustain is not None:
			end = start + note.sustain

	if start is None:
		return None

	bendsIter = iter( bends )

	try:
		bend = next( bendsIter )
	except StopIteration as e:
		return None

	bendValues = []
	while bend is not None and bend.time is not None and bend.time < end:
		if bend.time >= start:
			bendValues.append( bend )

		try:
			bend = next( bendsIter )
		except StopIteration as e:
			bend = None

	if len( bendValues ) == 0:
		return None
	else:
		return tuple( bendValues )

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
	'''	This function covers the various Text meta-events stored per-Track. Most of this will likely be deprecated in the future. '''

	# Phrase preparation
	phrases = []
	phraseIterations = []
	phraseIDs = {}
	phraseVariations = {}

	# ChordTemplate preparation
	arpeggios = {}
	arpStart = None
	chordNames = {}

	for event in midi.metaEvent:
		textType = event.message[0] 
		message = event.message[1:]

		# Anchors - There should be a better way to do this. No support for variable width at present.
		if textType == 'A':
			rsxml.transcriptionTrack.anchors.append( RSXML.Anchor( event.time, message ) )
		# Getting replaced with arpeggios.
			

		# Chord Names - Not sure what alternative there is, here.
		elif textType == 'C':
			if message == "ArpStart":
				arpStart = event.time
			elif message == "ArpEnd":
				if arpStart is not None:
					arpeggios[ arpStart ] = event.time
				else:
					assert "Potential problem with Arpeggio-flagging Text event at %f: No identified start event." % event.time
			else:
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
	rsxml.helper.arpeggios = arpeggios
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
	'''	Converts MIDI notes to RSXML notes.

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
			note.sustain = duration
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

		if start is not None:
			bendValues = []
			while currentBend[ note.string ] != None and currentBend[ note.string ].time <= end:
				if currentBend[ note.string ].time >= start:
					bendValues.append( currentBend[ note.string ] )

				try:
					currentBend[ note.string ] = next( bendIter[ note.string ] )
				except StopIteration as e:
					currentBend[ note.string ] = None

			note.bend = 0.0
			for i in bendValues:
				if i.step > note.bend: 
					note.bend = i.step

			if note.bend < 1.0:
				note.bend = 1.0

			note.bendValues = tuple( bendValues )

def ApplyTechniques( midi = MIDI.Track(), rsxml = RSXML.Track() ):
	events = iter( midi.controller )
	try:
		event = next( events )
	except StopIteration as e:
		event = MIDI.Event()

	flags = RSXML.Note._TECHNIQUES.copy()
	for note in rsxml.transcriptionTrack.notes:
		while event.time is not None and note.time >= event.time:
			if event.primary == Default._TECHNIQUECONTROLLER[ "bend" ] and note.bendValues is not None:
				# A flag for bends when Pitch Wheel doesn't want to play with channels.
				note.bendValues = YieldOmniBends( note, rsxml.helper.bendsOmni )
				
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
					flags[ "rightHand" ] = 1
				else:
					flags[ "tap" ] = 0
					flags[ "righthand" ] = -1
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
				if event.secondary >= 64:
					flags[ "pluck" ] = 1
				else:
					flags[ "pluck" ] = 0
			elif event.primary == Default._TECHNIQUECONTROLLER[ "slap" ]:
				if event.secondary >= 64:
					flags[ "slap" ] = 1
				else:
					flags[ "slap" ] = 0	

			try:
				event = next( events )
			except StopIteration as e:
				event = MIDI.Event()

		# Apply to note.		
		note.techniques = flags.copy()

def QuantiseNotes( rsxml = RSXML.Track() ):
	'''	If a note's duration is less than a specified quantisation amount, the value is reset to 0. '''
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
	'''	Process Note list to identify notes within a chord, separating the two as the RSXML format requires.. '''
	notes = []
	chords = []

	buffer = []
	bufferTime = 0
	for note in rsxml.transcriptionTrack.notes:
		if note.time > bufferTime:
			if len( buffer ) > 1:
				# Assemble chord, and attempt to derive chord ID (creating template if necessary).
				chord = RSXML.Chord( bufferTime )
				chord.notes = tuple( buffer )
				chord.chordID = CompileChordID( chord.GetFrets(), chord.time, rsxml, False, True )
				fingers = rsxml.structure[ "chordTemplates" ][ chord.chordID ].fingers
				for index, note in enumerate( chord.notes ):
					note.techniques[ "leftHand" ] = fingers[ index ]

					if note.minDifficulty > chord.minDifficulty:
						chord.minDifficulty = note.minDifficulty

				chords.append( chord )
			elif len( buffer ) == 0:
				pass
			else:
				notes.append( buffer[ 0 ] )

			buffer.clear()

		buffer.append( note )
		bufferTime = note.time

	rsxml.transcriptionTrack.notes = notes
	rsxml.transcriptionTrack.chords = chords		

def CompileChordID( frets = RSXML.ChordTemplate._DEFAULT, time = None, rsxml = RSXML.Track(), isArpeggio = False, isFirstSort = False ):
	'''	Derives a chordID (and if necessary, creates a template) from a set of frets.''' 
	for template in rsxml.structure[ "chordTemplates" ]:
		if frets == template.frets:
			return template.id
			
	else:
		chordName = None
		if isFirstSort:
			if time in rsxml.helper.chordNames.keys():
				# Chord names are only needed first time only, so we can safely pop it from the helper dict.
				# Ironically, they could have been useful later on, but I've already commited to it here, so fuck me.
				try:
					chordName = rsxml.helper.chordNames.pop( time )
				except KeyError as e:
					print( "Unexpected pop disaster at " + str( time ) )
		else:
			if sum( i >= 0 for i in frets ) > 2:
				# No need to name a 'double stop'.
				pass

		displayName = chordName # Use some kind of look-up table, if this works how I think it does.

		if isArpeggio:
			if displayName is None:
				displayName = "arp"
			else:
				displayName = chordName + "-arp"

		fingers = RSXML.ChordTemplate.ConvertFretsToFingers( frets )

		template = RSXML.ChordTemplate( chordName, displayName, fingers, frets )
		template.id = len( rsxml.structure[ "chordTemplates" ] )

		rsxml.structure[ "chordTemplates" ].append( template )

		return template.id

def CompileAnchors( rsxml = RSXML.Track() ):
	'''	Algorithm to automatically generate anchors. Not yet implemented.
	For now, assumes a width of 4 frets. May provide an exception for chords. '''
	# Clear any manually-added flags. This shouldn't be neccessary, really.
	rsxml.transcriptionTrack.anchors.clear()

	note = None
	chord = None

	noteIter = iter( rsxml.transcriptionTrack.notes )
	try:
		note = next( noteIter )
	except StopIteration:
		note = None

	chordIter = iter( rsxml.transcriptionTrack.chords )
	try:
		chord = next( chordIter )
	except StopIteration:
		chord = None

	anchor = None
	time = None
	low = -1
	high = 0
	isTapped = False
	isChordNext = False
	tapped = []
	while note is not None or chord is not None:
		if note is None:
			isChordNext = True			
		else:
			if note.time > chord.time:
				isChordNext = True
			elif note.time == chord.time:
				assert "Overlapping note and chord times at %f." % note.time

		if isChordNext:
			isTapped = False
			frets = sorted( chord.GetFrets() )
			i = 0
			while i < len( frets ):
				if frets[ i ] == -1:
					frets.pop( i )
				elif frets[ i ] == 0 and frets[ -1 ] > 5:
					frets.pop( i )
				else:
					break

			time = chord.time
			low = frets[ 0 ]
			high = frets[ -1 ]

			try:
				chord = next( chordIter )
			except StopIteration:
				chord = None

			isChordNext = False

		else:
			time = note.time
			if note.techniques[ "tap" ] == 1:
				isTapped = True
			elif note.techniques[ "hopo" ] == 0:
				isTapped = False

			low = note.fret
			high = note.fret
			if isTapped:
				tapped.append( note )

			try:
				note = next( noteIter )
			except StopIteration:
				note = None



		# Compare to last anchor.
		if anchor is None:
			width = high - low
			anchor = RSXML.Anchor( time, low, width )

			#print( "Time: " + str( time ) ) # Debug
			#print( "Anchor: " + str( anchor.time ) + " " + str( anchor.fret ) + " " + str( anchor.width ) ) # Debug
			#print( "\tThis note/chord: " + str( low ) + " " + str( high ) + " " + str( width ) ) # Debug

		elif isTapped:
			pass
					
		else:
			if len( tapped ) > 1:
				pass

			#print( "Time: " + str( time ) ) # Debug
			#print( "\tAnchor: " + str( anchor.time ) + " " + str( anchor.fret ) )
			#print( "\tThis note/chord: " + str( low ) + " " + str( high ) + " " + str( width ) )

			if low < anchor.fret or high > anchor.fret + anchor.width:
				if low < anchor.fret and high > anchor.fret + anchor.width:
					width = high - low
				elif low < anchor.fret:
					width = anchor.fret + anchor.width - low
				elif high > anchor.fret + anchor.width:
					width = high - anchor.fret
				
				if width > 4:
					#if width > 7 and low >= 15:
					#	anchor.fret = low
					#	anchor.width = width
					#elif width > 5 and low >= 10:
					#	anchor.fret = low
					#	anchor.width = width
					#else:
						# New anchor
						if anchor.width < 4: 
							anchor.width = 4
						rsxml.transcriptionTrack.anchors.append( anchor )
						#print( "\t\tAnchor appended: " + str( anchor.time ) + " " + str( anchor.fret ) + " " + str( anchor.width ) )
						anchor = RSXML.Anchor( time, low, 0 )
				elif width > anchor.width:
					anchor.width = width	

				if low != 0:
					anchor.fret = low		

			time = None
			low = -1
			high = 0

def CompileHandShapes( rsxml = RSXML.Track() ):
	'''	Algorithm to automatically generate handshapes. Somewhat implemented. '''
	noteIter = iter( rsxml.transcriptionTrack.notes )
	try:
		note = next( noteIter )
	except StopIteration:
		note = RSXML.Note( -1.0 )
	chordIter = iter( rsxml.transcriptionTrack.chords )
	try:
		chord = next( chordIter )
	except StopIteration:
		chord = RSXML.Chord( -1.0 )

	for start, end in rsxml.helper.arpeggios:
		frets = list( ChordTemplate._DEFAULT )

		while 0 <= note.time < start:
			try:
				note = next( noteIter )
			except StopIteration:
				note = Note( -1.0 )
		while 0 <= chord.time < start:
			'''	As each chord is meant to have a corresponding HandShape, we process that at the same time as arpeggios. To reduce redundancy, 
			if a chord is within the timeframe of an arpeggio, a corresponding HandShape is not produced. This should preserve chronology. '''
			rsxml.transcriptionTrack.handShapes.append( RSXML.HandShape( chord.time, chord.time + chord.GetDuration(), chord.chordID ) )
			try:
				chord = next( chordIter )
			except StopIteration:
				chord = Chord( -1.0 )

		''' The logic of this section could here splinter into providing the capacity to produce multiple arpeggios within a timeframe. '''
		while 0 <= note.time < end or 0 <= chord.time < end:
			if 0 <= note.time < chord.time:
				if frets[ note.string ] == -1:
					frets[ note.string ] = note.fret
				try:
					note = next( noteIter )
				except StopIteration:
					note = Note( -1.0 )
			elif 0 <= chord.time <= note.time:
				for index, fret in enumerate( chord.GetFrets() ):
					if frets[ index ] == -1:
						frets[ index ] = fret
				try:
					chord = next( chordIter )
				except StopIteration:
					chord = Chord( -1.0 )

			if sum( i == -1 for i in frets ) == 0: # If all 6 frets are accounted for, ceases searching for new notes.
				break
		else:
			assert "No notes or chords found within timeframe: %f, %f." % start, end
	
		rsxml.transcriptionTrack.handShapes.append( RSXML.HandShape( start, end, CompileChordID( frets, start, rsxml, True, True ) ) )
	else:
		firstChord = None
		endTime = None
		for chord in rsxml.transcriptionTrack.chords:
			if firstChord is not None:
				if chord.chordID == firstChord.chordID:
					chord.isRepeat = True
					endTime = chord.time + chord.GetDuration()
				else:
					rsxml.transcriptionTrack.handShapes.append( RSXML.HandShape( firstChord.time, endTime, chord.chordID ) )
					firstChord = chord
					endTime = chord.time + chord.GetDuration()
			else:
				firstChord = chord
				endTime = chord.time + chord.GetDuration()			

def ArrangeLevels( rsxml = RSXML.Track() ):
	'''	Arranges difficulty levels.
	The segregation of notes and chords as discrete concepts and lists presents a challenge to reducing redundant sorting.
	'''
	if rsxml.transcriptionTrack.hasTuples():
		assert "Immutability of level arrays is not guaranteed."

	rsxml.helper.difficulties.sort( reverse = True )

	notes = list( rsxml.transcriptionTrack.notes )
	#chords = deepcopy( rsxml.transcriptionTrack.chords )
	chords = list( rsxml.transcriptionTrack.chords )
	anchors = list( rsxml.transcriptionTrack.anchors )
	handShapes = list( rsxml.transcriptionTrack.handShapes )

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
				if chords[ i ].hasTuples():
					assert "Modifiable chords may cause unreliable output."

				chordNotes = list( chords[ i ].notes )
				# Need to reinsert smaller chord or single note in the event of cross-difficulty shenanigans.
				j = 0
				while j < len( chordNotes ):
					if velocity < chordNotes[ j ].minDifficulty:
						note = chordNotes.pop( j )
						# Need to handle removal of hopo flags in the event of cross-difficulty shenanigans.
					else:
						j += 1

				if len( chordNotes ) > 1:
					chords[ i ] = copy( chords[ i ] )
					chords[ i ].notes = tuple( chordNotes )

					oldChordName = rsxml.structure[ "chordTemplates" ][ chords[ i ].chordID ].chordName # Part of cheesy hack.

					chords[ i ].chordID = CompileChordID( chords[ i ].GetFrets(), chords[ i ].time, rsxml )

					i += 1
					
					# Cheesy hack to derive chord name for now.
					if len( chordNotes ) > 2:
						rsxml.structure[ "chordTemplates" ][ chords[ i ].chordID ].chordName = oldChordName

				else:
					if len( chordNotes ) == 1:
						note = chordNotes[ 0 ]
						note.isChord = False
						notes.append( note )

					chords.pop( i )

			try:
				if chords[ i ].chordID == chords[ i - 1 ].chordID:
					chords[ i ].isRepeat = True
				else:
					chords[ i ].isRepeat = False
			except IndexError:
				if i == 0:
					pass
				
			else:
				i += 1

		notes.sort( key = attrgetter( "time" ) ) # Sorts notes derived from chords list into notes list.

		# level = RSXML.Level( False, rsxml.helper.difficulties[ index ] )
		level = RSXML.Level( False, len( rsxml.helper.difficulties ) - 1 - index )
		level.notes = tuple( notes )
		#level.chords = deepcopy( chords )
		level.chords = tuple( chords )
		level.anchors = tuple( anchors )
		level.handShapes = tuple( handShapes )

		print( "Difficulty " + str( level.difficulty ) + " - Finished." )
		print( "\tNotes: " + str( len( level.notes ) ) + " | Chords: " + str( len( level.chords ) ) + " | Anchors: " + str( len( level.anchors ) ) )

		rsxml.levels.append( level )

	# 'Undo' reversal.
	rsxml.levels.reverse()

def YieldPhraseEvents( events = None, phrases = None, index = None ):
	if events is None or phrases is None or index is None:
		raise StopIteration

	try:
		start = phrases[ index ].time
		end = start
		if index + 1 < len( phrases ):
			end = phrases[ index + 1 ].time
		
		for event in events:
			if start <= event.time < end:
				yield event
			elif event.time >= end:
				break

	except AttributeError or TypeError or IndexError:
		raise StopIteration

def OptimiseLevels( rsxml = RSXML.Track() ):
	lastLevel = None
	lastPhraseContents = None

	for level in rsxml.levels:
		print( "Level " + str( level.difficulty ) )

		phraseContents = []
		for i in range( 0, len( rsxml.structure[ "phraseIterations" ] ) ):
			phraseContents.append( [ [], [], False ] ) # Notes, Chords, hasChanged
			for note in YieldPhraseEvents( level.notes, rsxml.structure[ "phraseIterations" ], i ):
				if note is None:
					break
				phraseContents[ i ][ 0 ].append( note )

			for chord in YieldPhraseEvents( level.chords, rsxml.structure[ "phraseIterations" ], i ):
				if chord is None:
					break
				phraseContents[ i ][ 1 ].append( chord )

			if lastPhraseContents is None:
				phraseContents[ i ][ 2 ] = True

			else:
				difficulty = level.difficulty - 1
				if difficulty == -1:
					difficulty = len( rsxml.levels )
				if phraseContents[ i ][ 0 ] != lastPhraseContents[ i ][ 0 ]:
					phraseContents[ i ][ 2 ] = True
					rsxml.structure[ "phrases" ][ rsxml.structure[ "phraseIterations" ][ i ].phraseID ].maxDifficulty = difficulty
				if phraseContents[ i ][ 1 ] != lastPhraseContents[ i ][ 1 ]:
					phraseContents[ i ][ 2 ] = True
					rsxml.structure[ "phrases" ][ rsxml.structure[ "phraseIterations" ][ i ].phraseID ].maxDifficulty = difficulty

			print( "\tPhrase " + str( i ) + " - Notes: " + str( len( phraseContents[ i ][ 0 ] ) ) + \
				" Chords: " + str( len( phraseContents[ i ][ 1 ] ) ) + " Changed: " + str( phraseContents[ i ][ 2 ] ) )

		level.notes = []
		level.chords = []
		for phrase in phraseContents:
			if phrase[ 2 ]:
				for note in phrase[ 0 ]:
					level.notes.append( note )
				for chord in phrase[ 1 ]:
					level.chords.append( chord )

		level.notes = tuple( level.notes )
		level.chords = tuple( level.chords )

		print( "Difficulty " + str( level.difficulty ) + " - Finished." )
		print( "\tNotes: " + str( len( level.notes ) ) + " | Chords: " + str( len( level.chords ) ) + " | Anchors: " + str( len( level.anchors ) ) )

		lastLevel = level
		lastPhraseContents = phraseContents

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

		# Prepare transcription track for writing, specifically by making its contents read-only.
		rsxmlTrack.transcriptionTrack.notes = tuple( rsxmlTrack.transcriptionTrack.notes )
		rsxmlTrack.transcriptionTrack.chords = tuple( rsxmlTrack.transcriptionTrack.chords )
		rsxmlTrack.transcriptionTrack.anchors = tuple( rsxmlTrack.transcriptionTrack.anchors )
		rsxmlTrack.transcriptionTrack.handShapes = tuple( rsxmlTrack.transcriptionTrack.handShapes )

		ArrangeLevels( rsxmlTrack )
		OptimiseLevels( rsxmlTrack )

		tracks.append( rsxmlTrack )

	return tracks