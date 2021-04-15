////////////////////////////////////////////////////////////////////////////////
// Sugar/Finally.h (Leggiero - Utility)
//
// Block Finally which is not provided by standard C++
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__SUGAR__FINALLY_H
#define __UTILITY__SUGAR__FINALLY_H


namespace Leggiero
{
	namespace Utility
	{
		namespace SyntacticSugar
		{
			// Finalizer Object Class
			template <typename FinalFunctionT>
			class SectionFinalizer
			{
			public:
				SectionFinalizer(const FinalFunctionT &finalFunction) : m_finalFunction(finalFunction) { }
				SectionFinalizer() = delete;
				~SectionFinalizer() { m_finalFunction(); }

			private:
				FinalFunctionT m_finalFunction;
			};
		}
	}
}

// Block Finalizer Macro
#define FINALLY_OF_BLOCK(NAME, DO_LAMBDA) Leggiero::Utility::SyntacticSugar::SectionFinalizer<decltype(DO_LAMBDA)> NAME(DO_LAMBDA)

#endif
