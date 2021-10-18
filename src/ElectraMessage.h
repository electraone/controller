#pragma once

#define MIDI_VALUE_DO_NOT_SEND 16537
#define MIDI_VALUE_TO_IGNORE   16536

enum class ElectraMessageType
{
	virt = 0,
	cc7 =  1,
	cc14 = 2,
	nrpn = 3,
	rpn  = 4,
	note = 5,
	program = 6,
	sysex = 7,
	start = 8,
	stop = 9,
	tune = 10,
	invalid = 11
};

ElectraMessageType translateElectraMessageType (const char *typeText);
const char *translateElectraMessageTypeToText (ElectraMessageType messageType);
