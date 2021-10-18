#pragma once

#include "helpers.h"


class Group
{
	private:
		static const uint8_t maxGroupNameLength = 40;

	public:
		Group () :
			id (0),
			pageId (0),
			colour (ElectraColours::white)
			//groupGraphics (nullptr)
		{
		}

		Group (uint8_t newId,
			   uint8_t newPageId,
			   Rectangle newBounds,
			   const char *newLabel,
			   Colour newColour) :
			id (newId),
			pageId (newPageId),
			colour (newColour),
			bounds (newBounds)
			//groupGraphics (nullptr)
		{
			setLabel (newLabel);
		}

		void setId (uint8_t newId)
		{
			id = newId;
		}

		uint8_t getId (void) const
		{
			return (id);
		}

		void setLabel (const char* newLabel)
		{
			copyString (label, newLabel, maxGroupNameLength);
		}

		const char* getLabel (void) const
		{
			return (label);
		}

		uint8_t getPageId (void) const
		{
			return (pageId);
		}

		void setColour (Colour newColour)
		{
			colour = newColour;
		}

		Colour getColour (void) const
		{
			return (colour);
		}

		Rectangle getBounds (void) const
		{
			return (bounds);
		}

/*
		void assignGroupGraphics (GroupGraphics* newGroupGraphics)
		{
			groupGraphics = newGroupGraphics;
		}

		GroupGraphics* getGroupGraphics (void)
		{
			return (groupGraphics);
		}
*/

	private:
		uint8_t id;
		uint8_t pageId;
		Colour colour;
		char label[maxGroupNameLength + 1];
		Rectangle bounds;
		//GroupGraphics *groupGraphics;
};
