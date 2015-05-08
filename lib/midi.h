#ifndef MIDI_H
#define MIDI_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <map>

using namespace std;

typedef unsigned char uchar;

class channel {
	public:
		//Constructor
		channel();
		channel(string);
		
		//Get Functions
		string getName();
		unsigned int getSize();
		unsigned int getStart();
		
		//Set Functions
		void setName(string);
		void setSize (unsigned int);
		void setStart(unsigned int);
	private:
		unsigned int pos_start;
		unsigned int size;
		string name;
};

class midi {
	public:
		//Constructors
		midi();
		midi(string);
		
		//Functions
		void open(string);
		void open();
		
		void process();
		
		//Get Functions
		unsigned int getSize();
		unsigned int getType();
		unsigned int getChannelCount();
		unsigned int getDeltaTime();
		
		//Actions
		void splice_channels();
		void filter_channels();
		void process_notes_in_channels();
		void write_chart(string);
		
		//Operator Overload
		channel *operator[] (int);
		
	private:
		//Hidden Functions
		string to_string(const int&);
		unsigned int VLQ_to_Int(unsigned int&);
		void write_section(ofstream &, string);
		
		//Data needed
		string filename;
		char* pre_data;
		uchar* data;
		unsigned int type;
		unsigned int size;
		unsigned int num_channel;
		unsigned int delta_time;
		vector<channel> channels;
		vector< vector<bool> > trk_exists;
		map<string, string> file;
};

#endif
