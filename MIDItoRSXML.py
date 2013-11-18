from sys import argv
from MIDIReader import MIDIReader

def main():
	script, fileName = argv

	tracks = MIDIReader( fileName )

	print( str( len( tracks ) ) )

main()