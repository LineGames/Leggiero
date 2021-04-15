////////////////////////////////////////////////////////////////////////////////
// Sugar/NonCopyable.h (Leggiero - Utility)
//
// Non-Copyable Base Class
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__SUGAR__NON_COPYABLE_H
#define __UTILITY__SUGAR__NON_COPYABLE_H


namespace Leggiero
{
	namespace Utility
	{
		namespace SyntacticSugar
		{
			// Prevent Copy or Assignment
			class NonCopyable
			{
			public:
				NonCopyable() { }

				NonCopyable(NonCopyable const&) = delete;
				NonCopyable& operator=(NonCopyable const&) = delete;
			};
		}
	}
}

#endif
