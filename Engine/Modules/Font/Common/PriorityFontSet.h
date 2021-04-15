////////////////////////////////////////////////////////////////////////////////
// Common/PriorityFontSet.h (Leggiero/Modules - Font)
//
// Prioritized Fonts Set
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FONT__COMMON__PRIORITY_FONT_SET_H
#define __LM_FONT__COMMON__PRIORITY_FONT_SET_H


// Standard Library
#include <iterator>
#include <memory>
#include <vector>
#include <utility>

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Font
#include "FontTypes.h"
#include "IFontSet.h"
#include "SingleFontSet.h"


namespace Leggiero
{
	namespace Font
	{
		// Forward Declarations
		class IFontFace;


		// Font Set with Priority
		class PriorityFontSet
			: public IFontSet
		{
		protected:
			struct _IteratorTypeSpecializerGeneralTag { };
			struct _IteratorTypeSpecializerFontFaceTag { };

			template <typename IteratorContentsT>
			struct _IteratorTypeSpecializer
			{
			public:
				using ValueCategory = _IteratorTypeSpecializerGeneralTag;
			};

			template <typename InputIterator>
			PriorityFontSet(InputIterator first, InputIterator last, _IteratorTypeSpecializerGeneralTag)
				: m_baseSets(first, last)
			{
			}

			template <typename InputIterator>
			PriorityFontSet(InputIterator first, InputIterator last, _IteratorTypeSpecializerFontFaceTag)
			{
				for (InputIterator currentIter = first; currentIter != last; ++currentIter)
				{
					m_baseSets.push_back(std::make_shared<SingleFontSet>(*currentIter));
				}
			}

		public:
			PriorityFontSet(std::shared_ptr<IFontSet> baseSet);
			PriorityFontSet(std::shared_ptr<IFontFace> baseFace);
			PriorityFontSet(std::initializer_list<std::shared_ptr<IFontSet> > initializer);
			PriorityFontSet(std::initializer_list<std::shared_ptr<IFontFace> > initializer);

			template <typename InputIterator>
			PriorityFontSet(InputIterator first, InputIterator last)
				: PriorityFontSet(first, last, _IteratorTypeSpecializer<typename std::iterator_traits<InputIterator>::value_type >::ValueCategory())
			{
			}

			virtual ~PriorityFontSet() { }

		private:
			PriorityFontSet() = delete;

		public:
			// Get a real font to render a given glyph
			virtual EffectiveFontResult GetEffectiveFont(GlyphCodePointType glyph, GlyphStyleType style = GlyphStyleType::kNone) const override;

		protected:
			std::vector<std::shared_ptr<IFontSet> > m_baseSets;
		};


		// Template specializations
		template <>
		struct PriorityFontSet::_IteratorTypeSpecializer<std::shared_ptr<IFontFace> >
		{
		public:
			using ValueCategory = PriorityFontSet::_IteratorTypeSpecializerFontFaceTag;
		};
		template <>
		struct PriorityFontSet::_IteratorTypeSpecializer<const std::shared_ptr<IFontFace> >
		{
		public:
			using ValueCategory = PriorityFontSet::_IteratorTypeSpecializerFontFaceTag;
		};
	}
}

#endif
