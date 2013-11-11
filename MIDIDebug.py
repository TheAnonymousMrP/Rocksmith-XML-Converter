from MIDIObjects import Track, TrackGlobal

def GOTHERE( location = None ):
	input( "Got here. " + str( location ) )

def DebugGlobalTrack( track = TrackGlobal() ):
	print( "Global Track: " )
	print( "SMPTE Offset: " + str( track.smpteOffset ) )

	print( "Meta Events: " )
	for m in track.metaEvent:
		print( "\tTime: " + str( m.time ) + "\tType: " + str( m.type ) + "\tMessage: " + str( m.message ) )

	print( "Tempos: " )
	for t in track.tempo:
		print( "\tTime: " + str( t.time ) + "\tTempo: " + str( t.tempo ) )
		
	print( "Time Signatures: " )
	for ts in track.timeSig:
		print( "\tTime: " + str( ts.time ) )

	print( "Key Signatures: " )
	for ks in track.keySig:
		print( "\tTime: " + str( ks.time ) )

def DebugTrack( track = Track() ):
	print( "Track: " )
	print( "Meta Events: " )
	for i in track.metaEvent:
		print( "\tTime: " + str( i.time ) + "\tType: " + str( i.type ) + "\tMessage: " + str( i.message ) )

	print( "Notes On: " )
	for i in track.noteOn:
		print( "\tTime: " + str( i.time ) )

	print( "Pitch Bend: " )
	for i in track.pitchBend:
		print( "\tTime: " + str( i.time ) + "\tBend: " + str( i.bend ) )