#pragma once

#include "helpers.h"

class Page
{
	public:
		Page () :
			id (0),
			hasObjects (false)
		{
			copyString (name, "", MaxNameLength);
		}

		Page (uint8_t id, const char *newName) :
			id (id)
		{
			setName (newName);
		}

		uint8_t getId (void) const
		{
			return (id);
		}

		void setName (const char* newName)
		{
			if (newName)
			{
				copyString (name, newName, MaxNameLength);
			}
			else
			{
				*name = '\0';
			}
		}

		const char *getName (void) const
		{
			return (name);
		}

		bool getHasObjects (void) const
		{
			return (hasObjects);
		}

		void setHasObjects (bool shouldHaveObjects)
		{
			hasObjects = shouldHaveObjects;
		}

	private:
		static const uint8_t MaxNameLength = 20;

		uint8_t id;
		char name[MaxNameLength + 1];
		bool hasObjects;
};
