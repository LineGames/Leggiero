////////////////////////////////////////////////////////////////////////////////
// Description/UIDescription.h (Leggiero/Modules - LegacyUI)
//
// Definition of UI Description which represents predefined UI Data
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__DESCRIPTION__UI_DESCRIPTION_H
#define __LM_LUI__DESCRIPTION__UI_DESCRIPTION_H


// Standard Library
#include <initializer_list>
#include <memory>
#include <unordered_map>
#include <vector>

// Leggiero.LegacyUI
#include "../Rendering/UITexture.h"
#include "UIDescriptionTypes.h"
#include "UIDescriptionProcessingContext.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			// Forward Declarations
			class DescriptionManager;
			class DescriptionUnit;


			// Object consists UI Description
			class IDescriptionObject
			{
			public:
				virtual UIDescriptionObjectType GetType() const { return UIDescriptionObjectType::kINVALID; }

				virtual bool IsAlias() const { return false; }
			};


			// Interface for namespace context for UI Description
			class IDescriptionNameSpaceContext
			{
			public:
				virtual ObjectEnvironmentPair Select(const VariableNameType &name, DescriptionProcessingContext &processingContext) = 0;
				ObjectEnvironmentPair Select(const VariablePathType &path, DescriptionProcessingContext &processingContext) { return _Select(path, 0, processingContext); }

				virtual bool IsNeedRecursiveSearch(DescriptionProcessingContext &processingContext) const { return false; }

			protected:
				ObjectEnvironmentPair _Select(const VariablePathType &path, size_t offset, DescriptionProcessingContext &processingContext);
			};


			// Imported Description Unit
			class ImportedUnit
				: public IDescriptionObject
				, public IDescriptionNameSpaceContext
			{
			public:
				ImportedUnit(SavedUnitNameType targetUnitName);
				virtual ~ImportedUnit() { }

			public:	// DescriptionObject
				virtual UIDescriptionObjectType GetType() const override { return UIDescriptionObjectType::kImportedUnit; }

			public:	// IDescriptionNameSpaceContext
				virtual ObjectEnvironmentPair Select(const VariableNameType &name, DescriptionProcessingContext &processingContext) override;
				virtual bool IsNeedRecursiveSearch(DescriptionProcessingContext &processingContext) const override { return true; }

			protected:
				SavedUnitNameType m_targetUnitName;
			};


			// Namespace group object in a description unit
			class NameSpaceGroup
				: public IDescriptionObject
				, public IDescriptionNameSpaceContext
				, public std::enable_shared_from_this<NameSpaceGroup>
			{
			public:
				NameSpaceGroup() { }

				template <typename InputIterator>
				NameSpaceGroup(InputIterator first, InputIterator last)
					: m_descriptionTable(first, last)
				{ }

				NameSpaceGroup(const std::unordered_map<VariableNameType, std::shared_ptr<IDescriptionObject> > &original);
				NameSpaceGroup(std::unordered_map<VariableNameType, std::shared_ptr<IDescriptionObject> > &&rhs);

				virtual ~NameSpaceGroup() { }

			public:	// DescriptionObject
				virtual UIDescriptionObjectType GetType() const override { return UIDescriptionObjectType::kNameSpaceGroup; }

			public:	// IDescriptionNameSpaceContext
				virtual ObjectEnvironmentPair Select(const VariableNameType &name, DescriptionProcessingContext &processingContext) override;
				virtual bool IsNeedRecursiveSearch(DescriptionProcessingContext &processingContext) const override { return true; }

			public:
				void Register(const VariableNameType &name, std::shared_ptr<IDescriptionObject> value);
				std::shared_ptr<IDescriptionObject> UnRegister(const VariableNameType &name);

			protected:
				std::unordered_map<VariableNameType, std::shared_ptr<IDescriptionObject> > m_descriptionTable;
			};


			// Overridable namespaces
			class OverridingNameSpace
				: public IDescriptionObject
				, public IDescriptionNameSpaceContext
			{
			public:
				OverridingNameSpace() { }

				template <typename InputIterator>
				OverridingNameSpace(InputIterator first, InputIterator last)
					: m_priorityTable(first, last)
				{ }

				OverridingNameSpace(const std::vector<std::shared_ptr<IDescriptionNameSpaceContext> > &original);
				OverridingNameSpace(std::vector<std::shared_ptr<IDescriptionNameSpaceContext> > &&rhs);
				OverridingNameSpace(std::initializer_list<std::shared_ptr<IDescriptionNameSpaceContext> > il);

				virtual ~OverridingNameSpace() { }

			public:	// DescriptionObject
				virtual UIDescriptionObjectType GetType() const override { return UIDescriptionObjectType::kOverridingNameSpace; }

			public:	// IDescriptionNameSpaceContext
				virtual ObjectEnvironmentPair Select(const VariableNameType &name, DescriptionProcessingContext &processingContext) override;

			public:
				void Append(std::shared_ptr<IDescriptionNameSpaceContext> appendingNameSpace);
				void Override(std::shared_ptr<IDescriptionNameSpaceContext> overridingNameSpace);

			protected:
				std::vector<std::shared_ptr<IDescriptionNameSpaceContext> > m_priorityTable;

			public:	// Sub-types
				// Alias for a namespace by name
				class NameSpaceAliasByName
					: public IDescriptionNameSpaceContext
				{
				public:
					NameSpaceAliasByName(const VariableNameType &alias)
						: m_alias(alias)
					{ }

				public:	// IDescriptionNameSpaceContext
					virtual ObjectEnvironmentPair Select(const VariableNameType &name, DescriptionProcessingContext &processingContext) override;
					virtual bool IsNeedRecursiveSearch(DescriptionProcessingContext &processingContext) const override;

				public:
					const VariableNameType &GetAliasedName() const { return m_alias; }

				protected:
					VariableNameType m_alias;
				};


				// Alias for a namespace by path
				class NameSpaceAliasByPath
					: public IDescriptionNameSpaceContext
				{
				public:
					NameSpaceAliasByPath(const VariablePathType &alias)
						: m_alias(alias)
					{ }

				public:	// IDescriptionNameSpaceContexts
					virtual ObjectEnvironmentPair Select(const VariableNameType &name, DescriptionProcessingContext &processingContext) override;
					virtual bool IsNeedRecursiveSearch(DescriptionProcessingContext &processingContext) const override;

				public:
					const VariablePathType &GetAliasedPath() const { return m_alias; }

				protected:
					VariablePathType m_alias;
				};
			};


			// Alias for a Name
			class AliasName
				: public IDescriptionObject
			{
			public:
				AliasName(VariableNameType &&alias)
					: m_alias(alias)
				{ }

				AliasName(const VariableNameType &alias)
					: m_alias(alias)
				{ }

			public:	// DescriptionObject
				virtual UIDescriptionObjectType GetType() const override { return UIDescriptionObjectType::kAliasName; }
				virtual bool IsAlias() const override { return true; }

			public:
				const VariableNameType &GetAliasedName() const { return m_alias; }

			protected:
				VariableNameType m_alias;
			};


			// Alias for a Path
			class AliasPath
				: public IDescriptionObject
			{
			public:
				AliasPath(VariablePathType &&alias)
					: m_alias(alias)
				{ }

				AliasPath(const VariablePathType &alias)
					: m_alias(alias)
				{ }

			public:	// DescriptionObject
				virtual UIDescriptionObjectType GetType() const override { return UIDescriptionObjectType::kAliasPath; }
				virtual bool IsAlias() const override { return true; }

			public:
				const VariablePathType &GetAliasedPath() const { return m_alias; }

			protected:
				VariablePathType m_alias;
			};


			// Interface for a description element has type tag
			class ITypeTagged
			{
			public:
				virtual UIDescriptionValueType GetValueType() const { return UIDescriptionValueType::kINVALID; }
			};


			// Interface to get a value of a given type
			// UIDescriptionExpressionTypeErrorException will be raised for invalid type conversion
			class IValueGetter
			{
			public:
				virtual BooleanValueType GetValueBoolean(DescriptionProcessingContext &processingContext) = 0;
				virtual IntegerValueType GetValueInteger(DescriptionProcessingContext &processingContext) = 0;
				virtual FloatingPointValueType GetValueFloatingPoint(DescriptionProcessingContext &processingContext) = 0;
				virtual ColorARGBValueType GetValueColor(DescriptionProcessingContext &processingContext) = 0;
				virtual Vector2DValueType GetValueVector2D(DescriptionProcessingContext &processingContext) = 0;
				virtual Vector3DValueType GetValueVector3D(DescriptionProcessingContext &processingContext) = 0;
				virtual StringValueType GetValueString(DescriptionProcessingContext &processingContext) = 0;
			};


			// Variable in UI Description
			template <typename ValueT>
			class Variable
				: public IDescriptionObject
				, public ITypeTagged
				, public IValueGetter
			{
			public:
				using ValueType = ValueT;

			public:
				Variable() : m_isEvaluated(false) { }
				Variable(ValueT calculatedValue) : m_isEvaluated(true), m_value(calculatedValue) { }
				virtual ~Variable() { }

			public:	// DescriptionObject
				virtual UIDescriptionObjectType GetType() const override { return UIDescriptionObjectType::kVariable; }

			public:	// ITypeTagged
				virtual UIDescriptionValueType GetValueType() const override { return ms_valueType; }

			public:
				ValueT GetValue(DescriptionProcessingContext &processingContext)
				{
					if (!m_isEvaluated)
					{
						_Evaluate(processingContext);
					}
					return m_value;
				}

			public:	// IValueGetter
				virtual BooleanValueType GetValueBoolean(DescriptionProcessingContext &processingContext) override;
				virtual IntegerValueType GetValueInteger(DescriptionProcessingContext &processingContext) override;
				virtual FloatingPointValueType GetValueFloatingPoint(DescriptionProcessingContext &processingContext) override;
				virtual ColorARGBValueType GetValueColor(DescriptionProcessingContext &processingContext) override;
				virtual Vector2DValueType GetValueVector2D(DescriptionProcessingContext &processingContext) override;
				virtual Vector3DValueType GetValueVector3D(DescriptionProcessingContext &processingContext) override;
				virtual StringValueType GetValueString(DescriptionProcessingContext &processingContext) override;

			protected:
				virtual void _Evaluate(DescriptionProcessingContext &processingContext) { };

			protected:
				bool	m_isEvaluated;
				ValueT	m_value;

				const static UIDescriptionValueType ms_valueType;
			};

			// Typed Variable
			using BoolVariable = Variable<BooleanValueType>;
			using IntVariable = Variable<IntegerValueType>;
			using FloatVariable = Variable<FloatingPointValueType>;
			using ColorVariable = Variable<ColorARGBValueType>;
			using Vector2DVariable = Variable<Vector2DValueType>;
			using Vector3DVariable = Variable<Vector3DValueType>;
			using StringVariable = Variable<StringValueType>;

			template<> Variable<Vector2DValueType>::Variable();
			template<> Variable<Vector3DValueType>::Variable();
		}
	}
}

#endif
