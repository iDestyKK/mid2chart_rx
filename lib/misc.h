#ifndef MISC_H
#define MISC_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

//Include our custom libraries. :)
#include "midi.h"

using namespace std;

void display_MIDI_information(midi &convert) {
	//Display size of MIDI.
	cout << "MIDI is " << convert.getSize() << " bytes." << endl;
	
	//Get type of MIDI.
	cout << "MIDI is type " << convert.getType() << " (";
	switch (convert.getType()) {
		case 0 : cout << "Single Track MIDI"; break;
		case 1 : cout << "Multi Synchronous Track MIDI"; break;
		case 2 : cout << "Multi Asynchronous Track MIDI"; break;
		default: cout << "Unknown Track Type MIDI"; break;
	}
	cout << ")" << endl;
	
	//Number of Channels
	cout << "MIDI has " << convert.getChannelCount() << " channels" << endl;
	
	//Spit out the Delta Time too
	cout << "MIDI delta time is " << convert.getDeltaTime() << endl;
}

void display_MIDI_tracks(midi &convert) {
	unsigned int cnt = convert.getChannelCount();
	for (int i = 0; i < cnt; i++) {
		cout << convert[i]->getName() << endl;
	}
}

#endif
