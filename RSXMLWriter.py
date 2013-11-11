from RSXMLObjects import *

def RSXMLWriter( track = Track() ):
	try:					
		pass
	except Exception as e:	
		print( "Error: " + str( e ) )

newNote = Note( 10.5, 2, 18 )
print( str( newNote ) )