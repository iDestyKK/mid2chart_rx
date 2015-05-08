#ifndef CONSTDEF_H
#define CONSTDEF_H

#include <string>
#include <map>

using namespace std;

//These are GLOBAL application settings. Change them if you want to add in extra difficulties, instruments, etc.
const int num_of_ins          = 4;
const int num_of_difficulties = 4;

//Define all of the names here.
const string difficulties[num_of_difficulties] = { "Expert"     , "Hard"      , "Medium"    , "Easy"      };
const string instruments [num_of_ins         ] = { "Single"     , "DoubleBass", "Drums"     , "Keyboard"  };
const string corris_inst [num_of_ins         ] = { "PART GUITAR", "PART BASS" , "PART DRUMS", "PART KEYS" };


//Tell the application what values are actually notes...
const unsigned char note_hex[num_of_difficulties][5] = { { 0x60, 0x61, 0x62, 0x63, 0x64 },
                                                         { 0x54, 0x55, 0x56, 0x57, 0x58 },
                                                         { 0x48, 0x49, 0x4A, 0x4B, 0x4C },
                                                         { 0x3C, 0x3D, 0x3E, 0x3F, 0x40 } };
//ORDER
//Expert: Green, Red, Yellow, Blue, Orange
//Hard  : Green, Red, Yellow, Blue, Orange
//etc.

#endif
