#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <stdlib.h>

#include "midi.h"
#include "constdef.h"

using namespace std;

typedef unsigned char uchar;

// CHANNEL CLASS
channel::channel() {
	name = "";
	size = 0;
}

channel::channel(string __str) {
	name = __str;
}

void channel::setName(string __str) {
	name = __str;
}

void channel::setSize(unsigned int __val) {
	size = __val;
}

void channel::setStart(unsigned int __val) {
	pos_start = __val;
}

string channel::getName() {
	return name;
}

unsigned int channel::getSize() {
	return size;
}

unsigned int channel::getStart() {
	return pos_start;
}


// MIDI CLASS
midi::midi() {
	filename = "";
}

midi::midi(string __str) {
	filename = __str;
	open();
}

void midi::open(string __str) {
	filename = __str;
	open();
}

void midi::process() {
	//Process basic information about MIDI file.
	type = (data[8] * (2 << 7)) + data[9];
	
	//Get number of tracks
	num_channel = (data[10] * (2 << 7)) + data[11];
	channels.resize(num_channel);
	
	//Get Delta Time
	delta_time = (data[12] * (2 << 7)) + data[13];
}

void midi::open() {
	//Read MIDI file.
	ifstream fp(filename.c_str());
	
	size = 0;
	fp.seekg(0, ios::end);
	size = fp.tellg(); //Get the length of the file.
	
	//Create new Data Array.
	pre_data = new  char[size];
	data     = new uchar[size];
	
	//Read all data.
	fp.seekg(0, ios::beg);
	fp.read(pre_data, size);
	
	//Cast all entries to a uchar. This prevents errors with negative numbers up to -128.
	for (int i = 0; i < size; i++)
		data[i] = (uchar) pre_data[i];
	
	//Clean up
	delete[] pre_data;
	
	fp.close();
	
	//Header Check
	if (data[0] != 0x4D || data[1] != 0x54 || data[2] != 0x68 || data[3] != 0x64) {
		//The hell are you throwing into this application?
		cout << "Not a MIDI File" << endl;
		exit(1);
	}
}

unsigned int midi::getSize() {
	return size;
}

unsigned int midi::getType() {
	return type;
}

unsigned int midi::getChannelCount() {
	return num_channel;
}

unsigned int midi::getDeltaTime() {
	return delta_time;
}

void midi::splice_channels() {
	//Read data from array and then extract instrument notes.
	unsigned int pos = 14;
	
	for (int i = 0; i < num_channel; i++) {
		unsigned int trk_pos = pos;
		pos += 8;
		
		//Set the size of the channel.
		channels[i].setSize((data[pos - 4] * (2 << 23))
                          + (data[pos - 3] * (2 << 15))
                          + (data[pos - 2] * (2 <<  7))
                          +  data[pos - 1]);
        
        pos++;
        
        if (data[pos] == 0xFF && data[pos + 1] == 0x03) {
        	pos += 2;
        	string __tmp_name = "";
        	uchar length = 0;
        	
        	//Get name
        	length = data[pos++];
        	for (int _i = 0; _i < length; _i++)
        		__tmp_name += data[pos++];
        	
        	//Set the channel name respectively.
        	//cout << __tmp_name << endl;
        	channels[i].setName (__tmp_name);
        	channels[i].setStart(trk_pos);
        }
        
        pos = trk_pos + 8 + channels[i].getSize();
	}
}

void midi::filter_channels() {
	for (int a = num_channel - 1; a > 1; a--) {
		bool cut = true;
		for (int i = 0; i < num_of_ins; i++) {
			if (channels[a].getName() == corris_inst[i])
				cut = false;
		}
		
		if (cut == true) {
			//Unrecognized track.
			cout << "Unrecognized track: \"" << channels[a].getName() << "\" removed" << endl;
			channels.erase(channels.begin() + a);
			num_channel--;
			//a -= 1;
		}
	}
}

string midi::to_string(const int& str) {
	ostringstream ss;
	ss << str;
	return ss.str();
}

unsigned int midi::VLQ_to_Int(unsigned int &pos) {
	unsigned int total = 0;
	string bits = "";
	do {
		string seq = "";
		for (int i = 0; i < 7; i++) {
			seq = to_string((data[pos] & (1 << i)) != 0) + seq;
		}
		bits = bits + seq;
		pos++;
	} while ((data[pos - 1] & (1 << 7)) != 0);
	//Combine the bits in the most inefficient way possible.
	for (unsigned int i = 0; i < bits.length(); i++) {
		total += pow((double)2, (int)((bits.length() - i) - 1)) * (bits[i] != '0');
	}

	//Save the day.
	return total;
}

void midi::process_notes_in_channels() {
	file.clear(); //Erase the map entirely.
	
	//Fill in what is guaranteed to be there already.
	file["Song"] = "";
	file["SyncTrack"] = "";
	file["Events"] = "";
	
	//Generate the Song string.
	file["Song"] += "\tName = \"" + channels[0].getName() + "\"\n";
	file["Song"] += "\tArtist = \"\"\n";
	file["Song"] += "\tCharter = \"\"\n";
	file["Song"] += "\tOffset = 0\n";
	file["Song"] += "\tResolution = " + to_string(delta_time) + "\n";
	file["Song"] += "\tPlayer2 = bass\n";
	file["Song"] += "\tDifficulty = 0\n";
	file["Song"] += "\tPreviewStart = 0.00\n";
	file["Song"] += "\tPreviewEnd = 0.00\n";
	file["Song"] += "\tGenre = unknown\n";
	file["Song"] += "\tMediaType = \"cd\"\n";
	file["Song"] += "\tMusicStream = \"song.ogg\"\n";
	file["Song"] += "\tGuitarStream = \"guitar.ogg\"\n";
	file["Song"] += "\tBassStream = \"rhythm.ogg\"\n";
	
	//That includes the instruments... yes.
	for (int a = 0; a < num_of_ins; a++)
		for (int b = 0; b < num_of_difficulties; b++)
			file[ difficulties[b] + instruments[a] ] = "";
	
	//Now begin deciphering the MIDI bytes.
	unsigned int pos;
	unsigned int pos_max;
	
	//Define a map for easy access.
	map<string, int> midi2ins;
	/*midi2ins["PART GUITAR"] = 0;
	midi2ins["PART BASS"  ] = 1;
	midi2ins["PART DRUMS" ] = 2;
	midi2ins["PART KEYS"  ] = 3;*/
	for (int i = 0; i < num_of_ins; i++) {
		midi2ins[corris_inst[i]] = i;
	}
	
	for (int i = 0; i < num_channel; i++) {
		cout << "lol " << channels[i].getName() << endl;
		//For each channel, we are going to scan through the bytes and do magic.
		pos = channels[i].getStart() + 12 + channels[i].getName().length();
		cout << pos << endl;
		
		unsigned int cur_pos = 0; //Yes.
		
		//Set max.
		pos_max = channels[i].getStart() + channels[i].getSize() - (channels[i].getName().length() + 1);
		
		//Set Note Queues. This is for timing accordingly.
		int note_queue[num_of_difficulties][5];
		for (int _i = 0; _i < num_of_difficulties; _i++)
			for (int _a = 0; _a < 5; _a++)
				note_queue[_i][_a] = 0;
		
		unsigned int track_pos[num_of_difficulties];
		for (int _i = 0; _i < num_of_difficulties; _i++)
			track_pos[_i] = 0;
		
		for (; pos < pos_max;) {
			//Unorthodox For-Loop ftw.
		 	cur_pos += VLQ_to_Int(pos);
		 	
			int head_chk = data[pos++];
			int head_type;
			unsigned int tmp_value;
			string tmp_str;
			
			//Notes!
			if (head_chk >= 0x80 && head_chk <= 0x9F) {
				int note = data[pos++];
				
				for (int a = 0; a < num_of_difficulties; a++) {
					for (int b = 0; b < 5; b++) {
						if (note_hex[a][b] == note) {
							//cout << (difficulties[a] + instruments[ midi2ins[channels[i].getName()] ]) << endl;
							//We have a match.
							if (note_queue[a][b] == 0)
								note_queue[a][b] = cur_pos;
							else {
								if (cur_pos - note_queue[a][b] > delta_time / 4) {
									file[ difficulties[a] + instruments[ midi2ins[channels[i].getName()] ] ] += "\t" + to_string(note_queue[a][b]) + " = N " + to_string(b) + " " + to_string(cur_pos - note_queue[a][b]) + "\n";
								}
								else {
									file[ difficulties[a] + instruments[ midi2ins[channels[i].getName()] ] ] += "\t" + to_string(note_queue[a][b]) + " = N " + to_string(b) + " 0\n";
								}
								note_queue[a][b] = 0;
							}
						}
					}
				}
				
				pos++;
			}
			
			//Events.
 			if (head_chk == 0xFF) {
 				head_type = data[pos++];
 				switch (head_type) {
 					case 0x01:
 						//Text Event.
 						tmp_str = "";
 						tmp_value = data[pos++];
 						
 						for (int _i = 0; _i < tmp_value; _i++) {
 							tmp_str += data[pos++];
 						}
 						
 						//cout << tmp_str << endl;
 						//Write this to whatever, depending on what it is.
 						if (i != 0) {
 							//It isn't the initial track. Good.
 							//Insert the event string into EVERY other track... beautiful.
 							for (int _i = 0; _i < num_of_difficulties; _i++)
	 							file[ difficulties[_i] + instruments[ midi2ins[channels[i].getName()] ] ] += "\t" + to_string(cur_pos) + " = E " + tmp_str + "\n";
 							
 						} else {
 							file["Events"] += "\t" + to_string(cur_pos) + " = E " + tmp_str + "\n";
 						}
	 					break;
	 					
 					case 0x58:
 						//Time Signature Change
		 				pos++; //Skip this byte.
		 				tmp_value = data[pos++];
		 				
		 				//Add to the SyncTrack section.
		 				file["SyncTrack"] += "\t" + to_string(cur_pos) + " = TS " + to_string(tmp_value) + "\n";
		 				
		 				pos += 3; //Skip the rest. The next three bytes are the denominator, MIDI clocks per metronome tick, and Number of 1/32 notes per 24 MIDI clocks
		 				break;
		 				
 					case 0x51:
 						//BPM Change
		 				pos++; //Skip this byte.
		 				tmp_value = 60000000000 / ((data[pos++] * 65536) + (data[pos++] * 256) + (data[pos++]));
		 				
		 				//Add to the SyncTrack section.
		 				file["SyncTrack"] += "\t" + to_string(cur_pos) + " = B " + to_string(tmp_value) + "\n";
		 				break;
		 				
		 			case 0x2F:
		 				//The end cut off the damn loop.
		 				pos = pos_max;
		 				break;
 				}
 			}
		}
	}
	//cout << file["ExpertDrums"] << endl;
}

void midi::write_chart(string filename) {
	ofstream op(filename.c_str());
	
	write_section(op, "Song");
	write_section(op, "SyncTrack");
	write_section(op, "Events");
	
	/*for (map<string, string>::iterator ii = file.begin(); ii != file.end(); ii++)
		if (ii->first != "Song" && ii->first != "SyncTrack" && ii->first != "Events")
			write_section(op, ii->first);
	*/
	
	for (int a = 0; a < num_of_ins; a++)
		for (int b = 0; b < num_of_difficulties; b++)
			write_section(op, difficulties[b] + instruments[a]);
	
	op.close();
}

void midi::write_section(ofstream &op, string section) {
	op << "[" << section << "]\n{\n";
	op << file[section];
	op << "}\n";
}

channel *midi::operator[] (int i) {
	return &channels[i];
}
