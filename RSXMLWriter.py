from RSXMLObjects import *

def ProcessMeta( track = Track() ):
	meta = ""
	for key, value in track.meta.items():
		if key == "version":
			pass
		elif key == "tuning":
			meta += "\t" + str( Tuning( value ) ) + "\n"
		elif key == "arrangementProperties":
			meta += "\t<" + key + " "
			for property, value in track.meta[ key ].items():
				meta += property + "=\"" + str( value ) + "\" "
			meta += "/>\n"
		else:
			meta += "\t<" + str( key ) + ">" + str( value ) + "</" + str( key ) + ">\n"
	
	return meta

def ProcessStructure( track = Track() ):
	structure = ""
	for key, value in track.structure.items():
		structure += "\t<" + key + " count=\"" + str( len( value ) ) + "\"" 
		if len( value ) == 0:
			 structure += "/>\n"
		else:
			structure += ">\n"
			for x in value:
				structure += "\t\t" + str( x ) + "\n"
			structure += "\t</" + key + ">\n"
	return structure

def ProcessContent( track = Track() ):
	content = ""
	content += WriteLevel( track.transcriptionTrack, 1 ) + "\n"
	content += "\t<levels count=\"" + str( len( track.levels ) ) + "\""
	if len( track.levels ) == 0:
		content += " />\n"
	else:
		content += ">\n"
		for level in track.levels:
			content += WriteLevel( level, 2 ) + "\n"
		content += "\t</levels>\n"
	return content

def WriteBend( bend = BendValue(), tabs = 0 ):
	if bend.time is None:
			bend.time = -1.0
	indent = ""
	for i in range( 0, tabs ):
		indent += "\t"
	return indent + "<bendValue time=\"" + str( bend.time ) + "\" step=\"" + str( bend.step ) + "\" />"

def WriteNote( note = Note(), tabs = 0 ):
	if note.time is None:
		note.time = -1.0
	if note.sustain is None:
		note.sustain = 0.0

	indent = ""
	for i in range( 0, tabs ):
		indent += "\t"

	output = ""
	if note.isChord is True:
		output = indent + "<chordNote "
	else:
		output = indent + "<note "
	output += "time=\"" + str( note.time ) + "\" string=\"" + str( note.string ) + "\" fret=\"" + str( note.fret )
	output += "\" sustain=\"" + str ( note.sustain ) + "\" "
	for type, value in note.techniques.items():
		output += type + "=\"" + str( value ) + "\" "
	if len( note.bendValues ) == 0:
		output += "/>"
	else:
		output += ">\n"
		output += indent + "<bendValues count=\"" + str( len( note.bendValues ) ) + "\">"
		for bend in note.bendValues:
			output += WriteBend( bend, tabs + 1 ) + "\n"
		output += indent + "</bendValues>"
		output += indent + "</note>"
	return output

def WriteChord( chord = Chord(), tabs = 0 ):
	#print( "Time: " + str( chord.time ) + " " + str( len( chord.notes ) ) )
	if chord.time is None:
		chord.time = -1.0

	indent = ""
	for i in range( 0, tabs ):
		indent += "\t"

	output = indent + "<chord " + "time=\"" + str( chord.time ) + "\" chordID=\"" + str( chord.chordID ) + "\" "
	for type, value in chord.techniques.items():
		output += type + "=\"" + str( value ) + "\" "
	if len( chord.notes ) is 0:
		output += "/>"
	else:
		output += ">\n"
		for note in chord.notes:
			output += WriteNote( note, tabs + 1 ) + "\n"
		output += indent + "</chord>"
	return output

def WriteAnchor( anchor = Anchor(), tabs = 0 ):
	if anchor.time is None:
			anchor.time = -1.0

	indent = ""
	for i in range( 0, tabs ):
		indent += "\t"

	return indent + "<anchor time=\"" + str( anchor.time ) + "\" fret=\"" + str( anchor.fret ) + "\" width=\"" + str( anchor.width ) + "\" />"

def WriteHandShape( handShape = HandShape(), tabs = 0 ):
	if handShape.startTime is None:
		handShape.startTime = -1.0
	if handShape.endTime is None:
		handShape.endTime = -1.0

	indent = ""
	for i in range( 0, tabs ):
		indent += "\t"
		
	buffer = indent + "<handShape startTime=\"" + str( handShape.startTime )
	buffer += "\" endTime=\"" + str( handShape.endTime ) + "\" chordID=\"" + str( handShape.fret ) + "\" />"
	return buffer

def WriteLevel( level = Level(), tabs = 0 ):
	indent = ""
	for i in range( 0, tabs + 1 ):
		indent += "\t"

	content = ""
	# Notes
	if len( level.notes ) == 0:
		content += indent + "<notes count=\"0\" />\n"
	else:
		content += indent + "<notes count=\"" + str( len( level.notes ) ) + "\">\n"
		for i in level.notes:
			content += WriteNote( i, tabs + 2 ) + "\n"
		content += indent + "</notes>\n"
	# Chords
	if len( level.chords ) == 0:
		content += indent + "<chords count=\"0\" />\n"
	else:
		content += indent + "<chords count=\"" + str( len( level.chords ) ) + "\">\n"
		for i in level.chords:
			content += WriteChord( i, tabs + 2 ) + "\n"
		content += indent + "</chords>\n"
	# Anchors
	if len( level.anchors ) == 0:
		content += indent + "<anchors count=\"0\" />\n"
	else:
		content += indent + "<anchors count=\"" + str( len( level.anchors ) ) + "\">\n"
		for i in level.anchors:
			content += WriteAnchor( i, tabs + 2 ) + "\n"
		content += indent + "</anchors>\n"
	# HandShapes
	if len( level.handShapes ) == 0:
		content += indent + "<handShapes count=\"0\" />\n"
	else:
		content += indent + "<handShapes count=\"" + str( len( level.handShapes ) ) + "\">\n"
		for i in level.handShapes:
			content += WriteHandShape( i, tabs + 2 ) + "\n"
		content += indent + "\t</handShapes>\n"

	indent = ""
	for i in range( 0, tabs ):
		indent += "\t"

	output = ""
	if level.isTranscription == False:
		output = indent + "<level difficulty=\"" + str( level.difficulty ) + "\">\n"
		output += content
		output += indent + "</level>"
	else:
		output = indent + "<transcriptionTrack difficulty=\"-1\">\n"
		output += content
		output += indent + "</transcriptionTrack>"
	return output

def Write( track = Track() ):
	trackName = track.meta[ "title" ]
	arrangementName = track.meta[ "arrangement" ]
	filePath = trackName + " - " + arrangementName + ".xml"	
	with open( filePath, 'w' ) as f:
		meta = ProcessMeta( track )
		structure = ProcessStructure( track )
		content = ProcessContent( track )

		f.write( "<?xml version='1.0' encoding='UTF-8'?>\n" )
		f.write( "<song version=\"" + str( track.meta[ "version" ] ) + "\">\n" )
		f.write( meta )
		f.write( structure )
		f.write( content )
		f.write( "</song>" )

def RSXMLWriter( tracks = [] ):
	for track in tracks:
		Write( track )