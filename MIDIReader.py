from MIDIObjects import *

'''
MIDIReader takes a MIDI file and, hopefully, produces a one-to-one Python object representation of it.

It was designed to be used in the creation of custom .xml charts for Rocksmith (2012/2014), which may help 
explain the inevitable limitations of it. I do intend for it to be MIDI spec-compliant at some point, but
oversights will occur given the limited needs of the intended application.

I aim to catalogue its shortcomings in the following list. I hope that it is comprehensive, though I wouldn't
count on it. It is worth noting that where possible, I've attempted at least a basic safety net for unused events.

Issues:
	- The Track() and Global() objects should really have separate attributes for each type of meta-event, rather than the 
		current safety net.
	- The length of meta-events must be under 128 bytes, as the current system assumes a single byte (only 7-bit) for length.
	- No SysEx support at all. Raises exception (probably).

Potential issues:
	- Format 0 and format 2 files are untested. Defaults to format 1 process, which *may* be sufficient.

'''

def ConvertBytesToFloat( bytes = bytearray() ):
	if bytes is None:
		return 0.0

	buffer = 0.0
	for index, value in enumerate( bytes ):
		buffer += value * pow( 256, ( len( bytes ) - 1 - index ) )
	return buffer;

def ConvertBytesToVLQOld( bytes = bytearray() ):
	if bytes is None:
		return 0

	buffer = bytes[ -1 ]
	#debug print( "VLQ Last: " + str( buffer ) )
	for index, value in enumerate( bytes[:-1] ):
		buffer += ( value - 0x80 ) * pow( 128, ( len( bytes ) - 1 - index ) )
		# debug print( "Byte: " + str( value ) + " " + str( buffer ) )

	return buffer;

def ConvertBytesToVLQ( bytes = bytearray() ):
	if bytes is None:
		return 0

	buffer = 0
	for byte in bytes:
		buffer = ( buffer << 7 ) + ( byte & 0x7F )

	return buffer

def ConvertDeltaToTime( delta = 0, division = Default._DIVISION, tempo = Default._TEMPO):
	length = delta / division
	return ( Default._ONEMINUTE / tempo ) * length

def ConvertParametersToPitchBend( param1 = int, param2 = int ):
	# http://www.blitter.com/~russtopia/MIDI/~jglatt/tech/midispec/wheel.htm
	# unsigned short CombineBytes(unsigned char First, unsigned char Second) {
	#	unsigned short _14bit;
	#	_14bit = (unsigned short)Second;
	#	_14bit <<= 7;
	#	_14bit |= (unsigned short)First;
	#	return(_14bit);
	# }
	bend = param2
	bend <<= 7
	bend |= param1
	return bend

def ConvertSMPTEToTime( bytes = bytearray(3), division = Default._DIVISION ):
	time = 0.0;
	time = time + bytes[1]
	time = time + ( bytes[2] / division )
	time = time + ( ( bytes[3] / 100 ) / division )
	return time

def GetMIDI( filePath = None ):
	if filePath is None:	
		print("No MIDI file was selected to be read.")
		return None
	
	memblock = bytearray()
	
	try:
		# http://stackoverflow.com/questions/1035340/reading-binary-file-in-python
		with open( filePath, "rb" ) as f:
			byte = f.read( 1 )
			while byte:
				# Do stuff with byte.
				memblock.extend( byte )
				byte = f.read( 1 )		
	except Exception as e:
		raise Exception( "Failed to create memory block of file contents. " + str( e ) )

	# 'Sorting' the memblock
	format = memblock[ Default._FORMATBYTE ]
	tracks = memblock[ Default._TRACKSBYTE ]
	division = ( memblock[ 12 ] << 8 ) + memblock[ 13 ] # Magic numbers, but more readable than the alternative. See Default._DIVISIONBYTES
	file = BinaryFile( format, division )

	readIndex = Default._HEADERLENGTH # readIndex tracks progress through the memblock
	isFinished = False
	while isFinished is False:
		trackSize = readIndex + Default._CHUNKIDLENGTH # Start of the size bytes
		trackStart = readIndex + Default._TRACKHEADERLENGTH # Start of the track contents
		trackLength = 0
		for byte in memblock[ trackSize : trackStart ]:
			trackLength = ( trackLength << 8 ) + byte

		readIndex = trackStart + trackLength
		newTrack = memblock[ trackStart : readIndex ]

		#print( "Track " + str( len( file.tracks ) ) + " Size: " + str( trackLength ) + "\tActual Track Size: " + str( len( newTrack ) ) )

		file.tracks.append( newTrack )

		if readIndex >= len( memblock ):
			isFinished = True

	return file

# Returns delta (int).
def ProcessDelta( memblock, index ):
	bytes = bytearray()
	while len( bytes ) < Default._MAXDELTASIZE:
		if index >= len( memblock ) - 1:
			raise EOFError( "Reached end of file at unexpected point during delta processing." )

		bytes.append( memblock[ index ] )
		index += 1
		# This should happen on the last delta byte.
		if memblock[ index - 1 ] < Event.eType._NOTE_OFF:
			return ConvertBytesToVLQ( bytes ), index

	raise Exception( "Delta length exceeded max." )

def ProcessDebug( memblock, readPosition, division = Default._DIVISION, file = File() ):	
	globalTrack = file.globalTrack
	currentTime = 0.0
	currentTempo = Default._TEMPO
	globalTempo = iter( globalTrack.tempo )
	try:
		nextTempo = next( globalTempo )
	except StopIteration as e:
		nextTempo = Tempo( 0, -1 )
	
	lastEvent = 0x00

	finished = False	
	while finished == False:
		start = readPosition
		delta = 0
		try:
			delta, readPosition = ProcessDelta( memblock, readPosition )
			if nextTempo.tempo != -1 and currentTime >= nextTempo.time:
				input( "Current time: " + str( currentTime ) + "\n\t" + str( nextTempo.time ) + " " + str( nextTempo.tempo ) )
				currentTempo = nextTempo.tempo
				try:
					nextTempo = next( globalTempo )
					input( str( nextTempo.time ) + " " + str( nextTempo.tempo ) )
				except StopIteration as e:
					nextTempo = Tempo( 0, -1 )
			currentTime = currentTime + ConvertDeltaToTime( delta, division, currentTempo )
		except EOFError as e:
			print( e )
			print( "AAAAAAAAAAAAAAAAAAAH" )
		except Exception as e:
			raise e

		status = memblock[ readPosition ]
		readPosition += 1

		print( "Start: " + str( start ) + "\t" + hex( status ))
		if status < Event.eType._NOTE_OFF:
			status = lastEvent
			print( "\tNo Status Byte." )
			readPosition -= 1
		if status >= Event.eType._NOTE_OFF and status < Event.eType._SYSEX:
			eventType = status & 0xF0
			channel = status & 0x0F
			primary = memblock[ readPosition ]

			# 3 bytes
			if eventType >= Event.eType._NOTE_OFF and eventType < Event.eType._PROGRAM or eventType == Event.eType._PITCH_WHEEL:
				readPosition += 1
				secondary = memblock[ readPosition ]

				print( "\tEvent Type: " + hex( eventType ) + "\tChannel: " + hex( channel ) + " Pitch: " + hex( primary ) + " Velocity: " + hex( secondary ) )
			# 2 bytes
			else:
				print( "\tEvent Type: " + hex( eventType ) + "\tChannel: " + hex( channel ) + " Primary: " + hex( primary ) )

		elif status >= Event.eType._SYSEX and status < Event.eType._META:
			print( "\tSysEx." )

		else:
			metaType = memblock[ readPosition ]
			readPosition += 1
			contentStart = readPosition + 1
			range = readPosition + memblock[ readPosition ] + 1
			readPosition += 1

			contents = bytearray()
			for byte in memblock[ readPosition : range ]:
				contents.append( byte )	
			readPosition = range - 1

			if metaType == Event.eMeta._END_OF_TRACK: 
				input( "> " )
				finished = True
			#elif metaType == Event.eMeta._TEMPO: break
			#elif metaType == Event.eMeta._SMPTE_OFFSET: break
			#elif metaType == Event.eMeta._TIME_SIGNATURE: break
			#elif metaType == Event.eMeta._KEY_SIGNATURE: break
			#elif metaType == Event.eMeta._SEQUENCER_SPECIFIC: break

			print( "\tMeta Type: " + hex( metaType) + "\tContent Start: " + str( contentStart) + " Range: " + str( range ) + "\n\tContents: " + str( contents ) )
	
		debugStart = 999999999
		if start >= debugStart:			
			input( "> " )

		readPosition += 1
		lastEvent = status

	return readPosition

def ProcessFormat1( memblock, file = File() ):
	index = 0 
	globalTrack = file.globalTrack
	track = Track()
	division = file.division
	
	isGlobalPass = True
	if len( globalTrack.tempo ) > 0:
		isGlobalPass = False

	currentTime = 0.0
	currentTempo = Default._TEMPO

	globalTempo = iter( globalTrack.tempo )
	try:
		nextTempo = next( globalTempo )
	except StopIteration as e:
		nextTempo = Tempo( 0, -1 )

	lastEvent = 0x00
	
	isFinished = False	
	while isFinished == False:
		delta = 0
		try:
			delta, index = ProcessDelta( memblock, index )
			if nextTempo.tempo != -1 and currentTime >= nextTempo.time:
				currentTempo = nextTempo.tempo
				try:
					nextTempo = next( globalTempo )
				except StopIteration as e:
					nextTempo = Tempo( 0, -1 )
			currentTime = currentTime + ConvertDeltaToTime( delta, division, currentTempo )
		except EOFError as e:
			raise e
		except Exception as e:
			raise e

		status = memblock[ index ]
		index += 1
		if status < Event.eType._NOTE_OFF:
			status = lastEvent
			index -= 1
		if status >= Event.eType._NOTE_OFF and status < Event.eType._SYSEX:
			eventType = status & 0xF0
			channel = status & 0x0F
			primary = memblock[ index ]
			
			# 3 bytes
			if Event.eType._NOTE_OFF <= eventType < Event.eType._PROGRAM or eventType == Event.eType._PITCH_WHEEL:
				index += 1
				secondary = memblock[ index ]
				if eventType == Event.eType._NOTE_OFF:		track.noteOff.append( ChannelEvent( currentTime, status, primary, secondary ) )
				elif eventType == Event.eType._NOTE_ON:		track.noteOn.append( ChannelEvent( currentTime, status, primary, secondary ) )
				elif eventType == Event.eType._AFTERTOUCH:	track.aftertouch.append( ChannelEvent( currentTime, status, primary, secondary ) )
				elif eventType == Event.eType._CONTROLLER:	track.controller.append( ChannelEvent( currentTime, status, primary, secondary ) )
				elif eventType == Event.eType._PITCH_WHEEL:	
					bend = ConvertParametersToPitchBend( primary, secondary )
					track.pitchBend.append( PitchBend( currentTime, status, bend ) )

			# 2 bytes
			elif eventType == Event.eType._PROGRAM:			track.programChange.append( ChannelEvent( currentTime, status, primary ) )
			elif eventType == Event.eType._PRESSURE:		track.channelPressure.append( ChannelEvent( currentTime, status, primary ) )
		elif Event.eType._SYSEX <= status < Event.eType._META:
			raise ValueError( "SYSEX not yet supported." )
		elif status == Event.eType._META:
			# x bytes
			metaType = memblock[ index ]
			index += 1
			range = index + memblock[ index ] + 1
			index += 1

			contents = bytearray()
			for byte in memblock[ index : range ]:
				contents.append( byte )	
			index = range - 1

			# Broad if statement for text-based Meta Events:
			# if metaType == Event.eMeta._SEQUENCE or metaType == Event.eMeta._TEXT or metaType == Event.eMeta._COPYRIGHT
			# or metaType == Event.eMeta._LYRIC or metaType == Event.eMeta._MARKER or metaType == Event.eMeta._CUE:
			#	track.metaEvent.append( MetaEvent( currentTime, metaType, str( contents, "ascii" ) ) )

			if metaType == Event.eMeta._SEQUENCE:				
				if 0 <= file.format < 2:
					globalTrack.metaEvent.append( MetaEvent( currentTime, metaType, str( contents, "ascii" ) ) )			
				else:
					track.metaEvent.append( MetaEvent( currentTime, metaType, str( contents, "ascii" ) ) )			
			elif metaType == Event.eMeta._TEXT:					track.metaEvent.append( MetaEvent( currentTime, metaType, str( contents, "ascii" ) ) )		
			elif metaType == Event.eMeta._COPYRIGHT:			track.metaEvent.append( MetaEvent( currentTime, metaType, str( contents, "ascii" ) ) )			
			elif metaType == Event.eMeta._NAME_SEQUENCE:		track.nameTrack = str( contents, "ascii" )
			elif metaType == Event.eMeta._NAME_INSTRUMENT:		track.nameInstrument = str( contents, "ascii" )
			elif metaType == Event.eMeta._LYRIC:				track.metaEvent.append( MetaEvent( currentTime, metaType, str( contents, "ascii" ) ) )				
			elif metaType == Event.eMeta._MARKER:				globalTrack.metaEvent.append( MetaEvent( currentTime, Event.eMeta._MARKER, str( contents, "ascii" ) ) )
			elif metaType == Event.eMeta._CUE:					globalTrack.metaEvent.append( MetaEvent( currentTime, Event.eMeta._CUE, str( contents, "ascii" ) ) )
			elif metaType == Event.eMeta._NAME_PROGRAM:			track.metaEvent.append( MetaEvent( currentTime, metaType, str( contents, "ascii" ) ) )
			elif metaType == Event.eMeta._NAME_DEVICE:			track.metaEvent.append( MetaEvent( currentTime, metaType, str( contents, "ascii" ) ) )
			elif metaType == Event.eMeta._CHANNEL_PREFIX:		track.metaEvent.append( MetaEvent( currentTime, metaType, str( contents, "ascii" ) ) )
			elif metaType == Event.eMeta._PORT:					track.metaEvent.append( MetaEvent( currentTime, metaType, str( contents, "ascii" ) ) )
			elif metaType == Event.eMeta._END_OF_TRACK: 
				isFinished = True
			elif metaType == Event.eMeta._TEMPO: 
				currentTempo = ( Default._ONEMINUTEMICRO / ConvertBytesToFloat( contents ) )
				globalTrack.tempo.append( Tempo( currentTime, currentTempo ) )
			elif metaType == Event.eMeta._SMPTE_OFFSET: 
				offset = ConvertSMPTEToTime( contents, division )
				currentTime += offset
				globalTrack.smpteOffset = offset
			elif metaType == Event.eMeta._TIME_SIGNATURE: 
				if len( contents ) != 4:
					globalTrack.timeSig.append( MetaEvent( currentTime, Event.eMeta._TIME_SIGNATURE, str( contents, "ascii" ) ) )
				else:
					globalTrack.timeSig.append( TimeSig( currentTime, contents[0], contents[1], contents[2], contents[3] ) )
			elif metaType == Event.eMeta._KEY_SIGNATURE:		
				if len( contents ) != 2:
					globalTrack.keySig.append( MetaEvent( currentTime, Event.eMeta._KEY_SIGNATURE, str( contents, "ascii" ) ) )
				else:
					globalTrack.keySig.append( KeySig( currentTime, contents[0], contents[1] ) )
			elif metaType == Event.eMeta._SEQUENCER_SPECIFIC:	pass	# No relevence.

		index += 1
		lastEvent = status

	if currentTime > file.length:
		file.length = currentTime

	if isGlobalPass is False:
		file.tracks.append( track )

# Generates MIDIObjects.Track's from MIDI file.
def Process( filePath = None, numArrs = None ):
	binary = GetMIDI( filePath )
	file = File( binary.format, binary.division )

	if file.format == 0:
		ProcessFormat1( binary.tracks[0], file )
	elif file.format == 1:
		for track in binary.tracks:
			ProcessFormat1( track, file )
	elif file.format == 2:
		for track in binary.tracks:
			ProcessFormat1( track, file )
	else:
		raise BaseException( "Unexpected MIDI file format." )

	return file

def MIDIReader( fileName = str ):
	try:					
		return Process( fileName )
	except Exception as e:	
		print( "Error: " + str( e ) )