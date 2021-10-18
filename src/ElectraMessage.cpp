#include "ElectraMessage.h"
#include <string.h>

ElectraMessageType translateElectraMessageType (const char *typeText)
{
	if (typeText)
	{
		if (strcmp (typeText, "cc7") == 0)
		{
			return (ElectraMessageType::cc7);
		}
		else if (strcmp (typeText, "cc14") == 0)
		{
			return (ElectraMessageType::cc14);
		}
		else if (strcmp (typeText, "nrpn") == 0)
		{
			return (ElectraMessageType::nrpn);
		}
		else if (strcmp (typeText, "rpn") == 0)
		{
			return (ElectraMessageType::rpn);
		}
		else if (strcmp (typeText, "note") == 0)
		{
			return (ElectraMessageType::note);
		}
		else if (strcmp (typeText, "program") == 0)
		{
			return (ElectraMessageType::program);
		}
		else if (strcmp (typeText, "sysex") == 0)
		{
			return (ElectraMessageType::sysex);
		}
		else if (strcmp (typeText, "start") == 0)
		{
			return (ElectraMessageType::start);
		}
		else if (strcmp (typeText, "stop") == 0)
		{
			return (ElectraMessageType::stop);
		}
		else if (strcmp (typeText, "tune") == 0)
		{
			return (ElectraMessageType::tune);
		}
		else if (strcmp (typeText, "virtual") == 0)
		{
			return (ElectraMessageType::virt);
		}
	}
	else
	{
		return (ElectraMessageType::sysex); // TODO: this resolves situation when type is not specified
	}
	return (ElectraMessageType::invalid);
}

const char *translateElectraMessageTypeToText (ElectraMessageType messageType)
{

	if (messageType == ElectraMessageType::cc7)
	{
		return ("cc7");
	}
	else if (messageType == ElectraMessageType::cc14)
	{
		return ("cc14");
	}
	else if (messageType == ElectraMessageType::nrpn)
	{
		return ("nrpn");
	}
	else if (messageType == ElectraMessageType::rpn)
	{
		return ("rpn");
	}
	else if (messageType == ElectraMessageType::note)
	{
		return ("note");
	}
	else if (messageType == ElectraMessageType::program)
	{
		return ("program");
	}
	else if (messageType == ElectraMessageType::sysex)
	{
		return ("sysex");
	}
	else if (messageType == ElectraMessageType::start)
	{
		return ("start");
	}
	else if (messageType == ElectraMessageType::stop)
	{
		return ("stop");
	}
	else if (messageType == ElectraMessageType::tune)
	{
		return ("tune");
	}
	else if (messageType == ElectraMessageType::virt)
	{
		return ("virtual");
	}

	return ("invalid");
}
