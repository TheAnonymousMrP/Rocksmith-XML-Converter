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
	content += str( track.transcriptionTrack ) + "\n"
	content += "\t<levels count=\"" + str( len( track.levels ) ) + "\""
	if len( track.levels ) == 0:
		content += " />\n"
	else:
		content += ">\n"
		for x in track.levels:
			content += str( x ) + "\n"
		content += "\t</levels>\n"
	return content

def Write( track = Track() ):
	try:	
		filePath = track.title + " - " + track.arrangementType + ".xml"	
		with open( filePath, 'w+' ) as f:
			meta = ProcessMeta( track )
			structure = ProcessStructure( track )
			content = ProcessStructure( track )

			f.write( "<?xml version='1.0' encoding='UTF-8'?>" )
			f.write( "<song version=\"" + str( track.version ) + "\">" )
			f.write( meta )
			f.write( structure )
			f.write( content )
			f.write( "</song>" )
	except Exception as e:	
		print( "Error: " + str( e ) )

def RSXMLWriter( tracks = [] ):
	for track in tracks:
		Write( track )

def test():
	i = OrderedDict([ ( "first", 1 ), ( "second", 2 ), ( "third", 3 )])
	print( str( i ) )
	print( str( i["second"] ) )

	t = Track()
	print( ProcessMeta( t ) )
	print( ProcessStructure( t ) )
	print( ProcessContent( t ) )