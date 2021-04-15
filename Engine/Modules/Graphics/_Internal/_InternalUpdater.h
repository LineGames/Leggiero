////////////////////////////////////////////////////////////////////////////////
// _Internal/_InternalUpdater.h (Leggiero/Modules - Graphics)
//   * DO NOT directly include this header file outside of Graphics project
//
// Internal Update Function Definitions for Graphics Module
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS___INTERNAL___INTERNAL_UPDATER_H
#define __LM_GRAPHICS___INTERNAL___INTERNAL_UPDATER_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>


namespace Leggiero
{
	namespace Graphics
	{
		namespace ReferenceState
		{
			namespace _Internal
			{
				// Update Reference State at the start of Each Frame
				void _UpdateReferenceStateAtFrame();
			}
		}
	}
}

#endif
