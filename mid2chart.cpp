#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>

//Include our custom libraries. :)
#include "lib/midi.h"
#include "lib/misc.h"

using namespace std;

//Misc functions created to help display information.
void display_MIDI_information(midi &);
void display_MIDI_tracks(midi &);

int main(int argc, char* argv[]) {
	if (argc != 2) {
		//User didn't specify a file!!!
		cerr << "Usage: " << argv[0] << " midi" << endl;
		return 1;
	}
	
	//Open up the midi and configure it.
	midi convert(argv[1]);
	
	//Get generic information about MIDI.
	convert.process();
	
	//Display information
	display_MIDI_information(convert);
	
	//Splice the channels
	convert.splice_channels();
	convert.filter_channels();
	
	//Display the track names.
	display_MIDI_tracks(convert);
	
	//Process the remaining channels.
	convert.process_notes_in_channels();
	
	//Finalize the chart.
	convert.write_chart("test.chart");
}
