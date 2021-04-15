////////////////////////////////////////////////////////////////////////////////
// Rendering/UIClipping.cpp (Leggiero/Modules - LegacyUI)
//
// UI Clipping Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIClipping.h"

// Leggiero.Utility
#include <Utility/Math/Vector.h>


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// Common Clippings

		//------------------------------------------------------------------------------
		// No-Clipping : Transparent Class
		class UIClippingPass
			: public IUIClipping
		{
		public:
			UIClippingPass() { }
			virtual ~UIClippingPass() { }

		public:
			virtual std::shared_ptr<const IUIClipping> Combine(const std::shared_ptr<const IUIClipping> &rhs) const override { return rhs; }

		public:
			virtual bool IsPointPass(const UIVector2D &position) const override { return true; }

			virtual std::vector<Graphics::ColoredVertex> ClipPolygon(const std::vector<Graphics::ColoredVertex> &polygon) const override { return polygon; }
			virtual std::vector<Graphics::TextureColoredVertex> ClipPolygon(const std::vector<Graphics::TextureColoredVertex> &polygon) const override { return polygon; }
			virtual std::vector<Graphics::PositionVertex> ClipPolygon(const std::vector<Graphics::PositionVertex> &polygon) const override { return polygon; }
		};


		//------------------------------------------------------------------------------
		// All-Clipping : Opaque Class
		class UIClippingReject
			: public IUIClipping
			, public std::enable_shared_from_this<UIClippingReject>
		{
		public:
			UIClippingReject() { }
			virtual ~UIClippingReject() { }

		public:
			virtual std::shared_ptr<const IUIClipping> Combine(const std::shared_ptr<const IUIClipping> &rhs) const override { return shared_from_this(); }

		public:
			virtual bool IsPointPass(const UIVector2D &position) const override { return false; }

			virtual std::vector<Graphics::ColoredVertex> ClipPolygon(const std::vector<Graphics::ColoredVertex> &polygon) const override { return std::vector<Graphics::ColoredVertex>(); }
			virtual std::vector<Graphics::TextureColoredVertex> ClipPolygon(const std::vector<Graphics::TextureColoredVertex> &polygon) const override { return std::vector<Graphics::TextureColoredVertex>(); }
			virtual std::vector<Graphics::PositionVertex> ClipPolygon(const std::vector<Graphics::PositionVertex> &polygon) const override { return std::vector<Graphics::PositionVertex>(); }

			virtual bool IsAllClipping() const override { return true; }
		};


		//------------------------------------------------------------------------------
		// Static Members
		const std::shared_ptr<const IUIClipping> IUIClipping::kAlwaysPass(std::make_shared<UIClippingPass>());
		const std::shared_ptr<const IUIClipping> IUIClipping::kAlwaysReject(std::make_shared<UIClippingReject>());


		//////////////////////////////////////////////////////////////////////////////// Polygon Clipping Utility

		namespace _Internal
		{
			template <typename PointT>
			UIVector2D UI2DVectorize(const PointT &targetPoint)
			{
				return UIVector2D(targetPoint);
			}

			template <>
			UIVector2D UI2DVectorize<Graphics::ColoredVertex>(const Graphics::ColoredVertex &targetPoint)
			{
				return UIVector2D(targetPoint.position[0], targetPoint.position[1]);
			}

			template <>
			UIVector2D UI2DVectorize<Graphics::TextureColoredVertex>(const Graphics::TextureColoredVertex &targetPoint)
			{
				return UIVector2D(targetPoint.position[0], targetPoint.position[1]);
			}

			template <>
			UIVector2D UI2DVectorize<Graphics::PositionVertex>(const Graphics::PositionVertex &targetPoint)
			{
				return UIVector2D(targetPoint.position[0], targetPoint.position[1]);
			}

			template <typename PointT>
			std::vector<PointT> ClipPointPolygon(const std::vector<UIVector2D> &clippingPolygon, const std::vector<PointT> &targetPolygon, const UIClippingConvexPolygon &clippingObject)
			{
				if (clippingPolygon.size() < 3 || targetPolygon.size() < 3)
				{
					return std::vector<PointT>();
				}

				// Structure to store point data
				struct PointCollectingEntry
				{
					PointT		point;
					UIVector2D	pointVector;
					bool		isInClippingArea;

				public:
					PointCollectingEntry(const PointT &pointData, bool isInClippingArea = false)
						: point(pointData), pointVector(UI2DVectorize(pointData)), isInClippingArea(isInClippingArea)
					{ }

					PointCollectingEntry(const PointT &pointData, const UIVector2D &pointVector, bool isInClippingArea = false)
						: point(pointData), pointVector(pointVector), isInClippingArea(isInClippingArea)
					{ }
				};

				bool isPassPointExists = false;
				bool isRejectPointExists = false;

				std::vector<PointCollectingEntry> processingPolygon;
				processingPolygon.reserve(targetPolygon.size() + 4);

				for (const PointT &currentPolygonPoint : targetPolygon)
				{
					PointCollectingEntry currentPolygonEntry(currentPolygonPoint);
					currentPolygonEntry.isInClippingArea = clippingObject.IsPointPass(currentPolygonEntry.pointVector);
					if (currentPolygonEntry.isInClippingArea)
					{
						isPassPointExists = true;
					}
					else
					{
						isRejectPointExists = true;
					}
					processingPolygon.push_back(currentPolygonEntry);
				}

				// Polygon is in clipping area Completly
				if (isPassPointExists && !isRejectPointExists)
				{
					return std::vector<PointT>(targetPolygon);
				}

				size_t clippingPolygonPointCount = clippingPolygon.size();

				// Check Whole-Clipping
				if (!isPassPointExists)
				{
					bool isExclusive = false;
					size_t targetPolygonPointCount = processingPolygon.size();
					for (size_t i = 0; i < clippingPolygonPointCount; ++i)
					{
						const UIVector2D &clipPoint1 = clippingPolygon[i];
						const UIVector2D &clipPoint2 = clippingPolygon[(i == (clippingPolygonPointCount - 1)) ? 0 : i + 1];
						Utility::Math::Vector3D<UIVector2D::ScalarType> referenceLine(Utility::Math::Vector3D<UIVector2D::ScalarType>::FromVector2D(clipPoint2 - clipPoint1));
						bool isSeparating = true;
						for (size_t j = 0; j < targetPolygonPointCount; ++j)
						{
							const PointCollectingEntry &targetPoint1 = processingPolygon[j];
							Utility::Math::Vector3D<UIVector2D::ScalarType> line1(Utility::Math::Vector3D<UIVector2D::ScalarType>::FromVector2D(targetPoint1.pointVector - clipPoint1));
							if (clippingObject.IsClockwiseWinding())
							{
								if (referenceLine.GetCrossWith(line1).z > static_cast<UIVector2D::ScalarType>(0.0))
								{
									isSeparating = false;
									break;
								}
							}
							else
							{
								if (referenceLine.GetCrossWith(line1).z < static_cast<UIVector2D::ScalarType>(0.0))
								{
									isSeparating = false;
									break;
								}
							}
						}
						if (isSeparating)
						{
							isExclusive = true;
							break;
						}
					}
					if (!isExclusive)
					{
						bool isTargetPolygonClockwiseWinding = (Utility::Math::Vector3D<UIVector2D::ScalarType>::FromVector2D(processingPolygon[1].pointVector - processingPolygon[0].pointVector).GetCrossWith(
							Utility::Math::Vector3D<UIVector2D::ScalarType>::FromVector2D(processingPolygon[2].pointVector - processingPolygon[0].pointVector)).z
								> static_cast<UIVector2D::ScalarType>(0.0));
						for (size_t j = 0; j < targetPolygonPointCount; ++j)
						{
							const UIVector2D &targetPoint1 = processingPolygon[j].pointVector;
							const UIVector2D &targetPoint2 = processingPolygon[(j == (targetPolygonPointCount - 1)) ? 0 : j + 1].pointVector;
							Utility::Math::Vector3D<UIVector2D::ScalarType> referenceLine(Utility::Math::Vector3D<UIVector2D::ScalarType>::FromVector2D(targetPoint2 - targetPoint1));
							bool isSeparating = true;
							for (size_t i = 0; i < clippingPolygonPointCount; ++i)
							{
								const UIVector2D &clipPoint1 = clippingPolygon[i];
								Utility::Math::Vector3D<UIVector2D::ScalarType> line1(Utility::Math::Vector3D<UIVector2D::ScalarType>::FromVector2D(clipPoint1 - targetPoint1));
								if (isTargetPolygonClockwiseWinding)
								{
									if (referenceLine.GetCrossWith(line1).z > static_cast<UIVector2D::ScalarType>(0.0))
									{
										isSeparating = false;
										break;
									}
								}
								else
								{
									if (referenceLine.GetCrossWith(line1).z < static_cast<UIVector2D::ScalarType>(0.0))
									{
										isSeparating = false;
										break;
									}
								}
							}
							if (isSeparating)
							{
								isExclusive = true;
								break;
							}
						}
					}
					if (isExclusive)
					{
						return std::vector<PointT>();
					}
				}

				// Cut by Edges
				std::vector<PointT> lerpReferencer;
				for (size_t i = 0; i < clippingPolygonPointCount; ++i)
				{
					const UIVector2D &clipPoint1 = clippingPolygon[i];
					size_t secondIndex = i + 1;
					if (secondIndex >= clippingPolygonPointCount)
					{
						secondIndex = 0;
					}
					const UIVector2D &clipPoint2 = clippingPolygon[secondIndex];
					Utility::Math::Vector3D<UIVector2D::ScalarType> referenceLine(Utility::Math::Vector3D<UIVector2D::ScalarType>::FromVector2D(clipPoint2 - clipPoint1));

					size_t targetPolygonPointCount = processingPolygon.size();
					if (targetPolygonPointCount < 3)
					{
						break;
					}
					for (size_t j = 0; j < targetPolygonPointCount - 1; ++j)
					{
						const PointCollectingEntry &targetPoint1 = processingPolygon[j];
						const PointCollectingEntry &targetPoint2 = processingPolygon[j + 1];
						if (targetPoint1.isInClippingArea && targetPoint2.isInClippingArea)
						{
							continue;
						}

						Utility::Math::Vector3D<UIVector2D::ScalarType> line1(Utility::Math::Vector3D<UIVector2D::ScalarType>::FromVector2D(targetPoint1.pointVector - clipPoint1));
						Utility::Math::Vector3D<UIVector2D::ScalarType> line2(Utility::Math::Vector3D<UIVector2D::ScalarType>::FromVector2D(targetPoint2.pointVector - clipPoint1));

						UIVector2D::ScalarType checkValue1 = referenceLine.GetCrossWith(line1).z;
						UIVector2D::ScalarType checkValue2 = referenceLine.GetCrossWith(line2).z;
						bool isPoint1Inside = false;
						bool isStartPointOnLine = false;
						if (checkValue1 == static_cast<UIVector2D::ScalarType>(0.0))
						{
							if (checkValue2 == static_cast<UIVector2D::ScalarType>(0.0))
							{
								// Meeting Line
								continue;
							}
							else
							{
								bool isPoint2Inside = false;
								if (clippingObject.IsClockwiseWinding())
								{
									isPoint2Inside = (checkValue2 >= static_cast<UIVector2D::ScalarType>(0.0));
								}
								else
								{
									isPoint2Inside = (checkValue2 <= static_cast<UIVector2D::ScalarType>(0.0));
								}
								if (isPoint2Inside)
								{
									// Inside line
									continue;
								}
							}

							// Clip at point 1
							isPoint1Inside = true;
							isStartPointOnLine = true;
						}
						else if (checkValue2 == static_cast<UIVector2D::ScalarType>(0.0))
						{
							if (clippingObject.IsClockwiseWinding())
							{
								isPoint1Inside = (checkValue1 >= static_cast<UIVector2D::ScalarType>(0.0));
							}
							else
							{
								isPoint1Inside = (checkValue1 <= static_cast<UIVector2D::ScalarType>(0.0));
							}
							if (isPoint1Inside)
							{
								// Inside line
								continue;
							}

							// Clip at point 2
							isStartPointOnLine = true;
						}
						else if (checkValue1 * checkValue2 < static_cast<UIVector2D::ScalarType>(0.0))
						{
							// Intersect Line
							if (clippingObject.IsClockwiseWinding())
							{
								isPoint1Inside = (checkValue1 >= static_cast<UIVector2D::ScalarType>(0.0));
							}
							else
							{
								isPoint1Inside = (checkValue1 <= static_cast<UIVector2D::ScalarType>(0.0));
							}
						}
						else
						{
							// Non-Clipping Line
							continue;
						}

						// Check from start point
						int targetPolygonPointCountSigned = (int)targetPolygonPointCount;
						int rejectingDirection = isPoint1Inside ? 1 : -1;
						int rejectCheckPointStartIndex = isPoint1Inside
							? (((int)j + 2) % targetPolygonPointCountSigned)
							: ((((int)(j + targetPolygonPointCount) - 1) % targetPolygonPointCountSigned) + targetPolygonPointCountSigned);
						for (int k = 0; k < targetPolygonPointCountSigned - 1; ++k)
						{
							const PointCollectingEntry &returnCheckPoint = processingPolygon[(size_t)((rejectCheckPointStartIndex + rejectingDirection * k) % targetPolygonPointCountSigned)];

							Utility::Math::Vector3D<UIVector2D::ScalarType> checkLine(Utility::Math::Vector3D<UIVector2D::ScalarType>::FromVector2D(returnCheckPoint.pointVector - clipPoint1));
							UIVector2D::ScalarType returnCheckValue = referenceLine.GetCrossWith(checkLine).z;

							bool isReturnPointInside = false;
							if (clippingObject.IsClockwiseWinding())
							{
								isReturnPointInside = (returnCheckValue >= static_cast<UIVector2D::ScalarType>(0.0));
							}
							else
							{
								isReturnPointInside = (returnCheckValue <= static_cast<UIVector2D::ScalarType>(0.0));
							}
							if (!isReturnPointInside)
							{
								continue;
							}
							bool isFinishPointOnLine = (returnCheckValue == static_cast<UIVector2D::ScalarType>(0.0));

							// Process Clipping
							int rejectSegment1Start = -1;
							int rejectSegment1Finish = -1;
							int rejectSegment2Start = -1;
							int rejectSegment2Finish = -1;
							int rejectStartIndex = -1;
							int rejectFinishIndex = -1;
							if (isPoint1Inside)
							{
								rejectStartIndex = (int)j + 1;
								rejectFinishIndex = (rejectStartIndex + k + 1) % targetPolygonPointCountSigned;
							}
							else
							{
								rejectFinishIndex = (rejectCheckPointStartIndex + 2) % targetPolygonPointCountSigned;
								rejectStartIndex = (rejectCheckPointStartIndex - k + 1) % targetPolygonPointCountSigned;
							}
							if (rejectFinishIndex < rejectStartIndex)
							{
								rejectSegment1Start = rejectStartIndex;
								rejectSegment1Finish = targetPolygonPointCountSigned;
								rejectSegment2Start = 0;
								rejectSegment2Finish = rejectFinishIndex;
							}
							else
							{
								rejectSegment1Start = rejectStartIndex;
								rejectSegment1Finish = rejectFinishIndex;
							}

							int startPointInsertIndex = (int)(j + 1);
							int finishPointInsertIndex = (rejectCheckPointStartIndex + rejectingDirection * k) % targetPolygonPointCountSigned;
							if (!isPoint1Inside)
							{
								++finishPointInsertIndex;
							}

							int finishingPeriod = targetPolygonPointCountSigned;
							if (!isStartPointOnLine)
							{
								UIVector2D::ScalarType divisor = (clipPoint1.x - clipPoint2.x) * (targetPoint1.pointVector.y - targetPoint2.pointVector.y) - (clipPoint1.y - clipPoint2.y) * (targetPoint1.pointVector.x - targetPoint2.pointVector.x);
								UIVector2D startCrossingPoint(
									((clipPoint1.x * clipPoint2.y - clipPoint1.y * clipPoint2.x) * (targetPoint1.pointVector.x - targetPoint2.pointVector.x) - (clipPoint1.x - clipPoint2.x) * (targetPoint1.pointVector.x * targetPoint2.pointVector.y - targetPoint1.pointVector.y * targetPoint2.pointVector.x)) / divisor,
									((clipPoint1.x * clipPoint2.y - clipPoint1.y * clipPoint2.x) * (targetPoint1.pointVector.y - targetPoint2.pointVector.y) - (clipPoint1.y - clipPoint2.y) * (targetPoint1.pointVector.x * targetPoint2.pointVector.y - targetPoint1.pointVector.y * targetPoint2.pointVector.x)) / divisor
								);

								float lerpParam = (startCrossingPoint - targetPoint1.pointVector).Length() / (targetPoint2.pointVector - targetPoint1.pointVector).Length();
								lerpReferencer.push_back(targetPoint1.point.LerpWith(targetPoint2.point, lerpParam));

								// Push After Start
								typename std::vector<PointCollectingEntry>::iterator insertPoitionIt = processingPolygon.begin() + startPointInsertIndex;
								processingPolygon.insert(insertPoitionIt, PointCollectingEntry(lerpReferencer[lerpReferencer.size() - 1], startCrossingPoint, true));

								++finishingPeriod;
								if (finishPointInsertIndex > startPointInsertIndex)
								{
									++finishPointInsertIndex;
								}
								else if (finishPointInsertIndex == startPointInsertIndex)
								{
									if (isPoint1Inside)
									{
										++finishPointInsertIndex;
									}
								}
								if (rejectSegment1Start >= startPointInsertIndex)
								{
									++rejectSegment1Start;
									++rejectSegment1Finish;
								}
							}
							if (!isFinishPointOnLine)
							{
								const PointCollectingEntry &finishLinePoint1 = processingPolygon[(size_t)((finishPointInsertIndex + finishingPeriod - 1) % finishingPeriod)];
								const PointCollectingEntry &finishLinePoint2 = processingPolygon[(size_t)(finishPointInsertIndex % finishingPeriod)];

								UIVector2D::ScalarType divisor = (clipPoint1.x - clipPoint2.x) * (finishLinePoint1.pointVector.y - finishLinePoint2.pointVector.y) - (clipPoint1.y - clipPoint2.y) * (finishLinePoint1.pointVector.x - finishLinePoint2.pointVector.x);
								UIVector2D finishCrossingPoint(
									((clipPoint1.x * clipPoint2.y - clipPoint1.y * clipPoint2.x) * (finishLinePoint1.pointVector.x - finishLinePoint2.pointVector.x) - (clipPoint1.x - clipPoint2.x) * (finishLinePoint1.pointVector.x * finishLinePoint2.pointVector.y - finishLinePoint1.pointVector.y * finishLinePoint2.pointVector.x)) / divisor,
									((clipPoint1.x * clipPoint2.y - clipPoint1.y * clipPoint2.x) * (finishLinePoint1.pointVector.y - finishLinePoint2.pointVector.y) - (clipPoint1.y - clipPoint2.y) * (finishLinePoint1.pointVector.x * finishLinePoint2.pointVector.y - finishLinePoint1.pointVector.y * finishLinePoint2.pointVector.x)) / divisor
								);

								float lerpParam = (finishCrossingPoint - finishLinePoint1.pointVector).Length() / (finishLinePoint2.pointVector - finishLinePoint1.pointVector).Length();
								lerpReferencer.push_back(finishLinePoint1.point.LerpWith(finishLinePoint2.point, lerpParam));

								// Push After Finish
								typename std::vector<PointCollectingEntry>::iterator insertPoitionIt = processingPolygon.begin() + finishPointInsertIndex;
								processingPolygon.insert(insertPoitionIt, PointCollectingEntry(lerpReferencer[lerpReferencer.size() - 1], finishCrossingPoint, true));

								if (rejectSegment1Start >= finishPointInsertIndex)
								{
									++rejectSegment1Start;
									++rejectSegment1Finish;
								}
							}

							processingPolygon.erase(processingPolygon.begin() + rejectSegment1Start, processingPolygon.begin() + rejectSegment1Finish);
							if (rejectSegment2Start > -1)
							{
								processingPolygon.erase(processingPolygon.begin() + rejectSegment2Start, processingPolygon.begin() + rejectSegment2Finish);
							}

							break;
						}

						break;
					}
				}

				if (processingPolygon.size() < 3)
				{
					return std::vector<PointT>();
				}

				std::vector<PointT> resultPolygon;
				resultPolygon.reserve(processingPolygon.size());
				for (const PointCollectingEntry &currentPolygonPoint : processingPolygon)
				{
					resultPolygon.push_back(currentPolygonPoint.point);
				}

				return resultPolygon;
			}
		}


		//////////////////////////////////////////////////////////////////////////////// UIClippingConvexPolygon

		//------------------------------------------------------------------------------
		UIClippingConvexPolygon::UIClippingConvexPolygon(std::vector<UIVector2D> initializingPolygon)
			: m_points(initializingPolygon)
		{
			_InitializePolygon();
		}

		//------------------------------------------------------------------------------
		UIClippingConvexPolygon::UIClippingConvexPolygon(std::initializer_list<UIVector2D> initializingPolygon)
			: m_points(initializingPolygon)
		{
			_InitializePolygon();
		}

		//------------------------------------------------------------------------------
		UIClippingConvexPolygon::UIClippingConvexPolygon(const UIClippingConvexPolygon &other)
			: m_points(other.m_points), m_isValid(other.m_isValid), m_isClockwiseWinding(other.m_isClockwiseWinding)
		{
		}

		//------------------------------------------------------------------------------
		UIClippingConvexPolygon::UIClippingConvexPolygon(UIClippingConvexPolygon &&other)
			: m_points(std::move(other.m_points)), m_isValid(other.m_isValid), m_isClockwiseWinding(other.m_isClockwiseWinding)
		{
		}

		//------------------------------------------------------------------------------
		UIClippingConvexPolygon &UIClippingConvexPolygon::operator=(const UIClippingConvexPolygon &rhs)
		{
			m_points.clear();
			m_points.reserve(rhs.m_points.size());
			m_points.insert(m_points.begin(), rhs.m_points.begin(), rhs.m_points.end());

			m_isValid = rhs.m_isValid;
			m_isClockwiseWinding = rhs.m_isClockwiseWinding;

			return *this;
		}

		//------------------------------------------------------------------------------
		UIClippingConvexPolygon &UIClippingConvexPolygon::operator=(UIClippingConvexPolygon &&rhs)
		{
			m_points.swap(rhs.m_points);

			bool oldIsValid = m_isValid;
			bool oldIsClockwiseWinding = m_isClockwiseWinding;

			m_isValid = rhs.m_isValid;
			m_isClockwiseWinding = rhs.m_isClockwiseWinding;

			rhs.m_isValid = oldIsValid;
			rhs.m_isClockwiseWinding = oldIsClockwiseWinding;

			return *this;
		}

		//------------------------------------------------------------------------------
		void UIClippingConvexPolygon::_InitializePolygon()
		{
			bool isCollapsed;
			do
			{
				isCollapsed = false;
				for (size_t i = 0; i < m_points.size(); ++i)
				{
					size_t idx1 = (i + 1) % m_points.size();
					size_t idx2 = (i + 2) % m_points.size();

					// Same Point
					UIVector2D d1(m_points[idx1] - m_points[i]);
					if (d1.SquareLength() < kFloatEpsilon)
					{
						isCollapsed = true;
						m_points.erase(m_points.begin() + idx1);
						break;
					}

					UIVector2D d2(m_points[idx2] - m_points[idx1]);
					if (d2.SquareLength() < kFloatEpsilon)
					{
						continue;
					}

					// Along the same line
					if ((d2.GetNormalized() - d1.GetNormalized()).SquareLength() < kFloatEpsilon)
					{
						isCollapsed = true;
						m_points.erase(m_points.begin() + idx1);
						break;
					}
				}
			} while (isCollapsed && m_points.size() > 2);

			if (m_points.size() < 3)
			{
				m_isValid = false;
				return;
			}
			m_isValid = true;

			Utility::Math::Vector3D<UIVector2D::ScalarType> line1(Utility::Math::Vector3D<UIVector2D::ScalarType>::FromVector2D(m_points[1] - m_points[0]));
			Utility::Math::Vector3D<UIVector2D::ScalarType> line2(Utility::Math::Vector3D<UIVector2D::ScalarType>::FromVector2D(m_points[2] - m_points[1]));
			m_isClockwiseWinding = line1.GetCrossWith(line2).z > static_cast<UIVector2D::ScalarType>(0.0);
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<const IUIClipping> UIClippingConvexPolygon::Combine(const std::shared_ptr<const IUIClipping> &rhs) const
		{
			if (!m_isValid)
			{
				return kAlwaysReject;
			}

			if (std::dynamic_pointer_cast<const UIClippingReject>(rhs))
			{
				return rhs;
			}
			if (std::dynamic_pointer_cast<const UIClippingPass>(rhs))
			{
				return shared_from_this();
			}

			std::shared_ptr<const UIClippingConvexPolygon> mergingPolygon(std::dynamic_pointer_cast<const UIClippingConvexPolygon>(rhs));
			if (!mergingPolygon)
			{
				// Something Wrong
				return shared_from_this();
			}
			if (!mergingPolygon->m_isValid)
			{
				return kAlwaysReject;
			}

			// Combine Polygons
			std::vector<UIVector2D> intersectPolygon = _Internal::ClipPointPolygon(m_points, mergingPolygon->m_points, *this);
			if (intersectPolygon.size() < 3)
			{
				return kAlwaysReject;
			}

			return std::make_shared<UIClippingConvexPolygon>(intersectPolygon);
		}

		//------------------------------------------------------------------------------
		bool UIClippingConvexPolygon::IsPointPass(const UIVector2D &position) const
		{
			if (!m_isValid)
			{
				return false;
			}

			size_t checkingSize = m_points.size() - 1;
			for (size_t i = 0; i < checkingSize; ++i)
			{
				Utility::Math::Vector3D<UIVector2D::ScalarType> line1(Utility::Math::Vector3D<UIVector2D::ScalarType>::FromVector2D(m_points[i + 1] - m_points[i]));
				Utility::Math::Vector3D<UIVector2D::ScalarType> line2(Utility::Math::Vector3D<UIVector2D::ScalarType>::FromVector2D(position - m_points[i]));
				if (m_isClockwiseWinding)
				{
					if (line1.GetCrossWith(line2).z < static_cast<UIVector2D::ScalarType>(0.0))
					{
						return false;
					}
				}
				else
				{
					if (line1.GetCrossWith(line2).z > static_cast<UIVector2D::ScalarType>(0.0))
					{
						return false;
					}
				}
			}
			{
				Utility::Math::Vector3D<UIVector2D::ScalarType> line1(Utility::Math::Vector3D<UIVector2D::ScalarType>::FromVector2D(m_points[0] - m_points[checkingSize]));
				Utility::Math::Vector3D<UIVector2D::ScalarType> line2(Utility::Math::Vector3D<UIVector2D::ScalarType>::FromVector2D(position - m_points[checkingSize]));
				if (m_isClockwiseWinding)
				{
					if (line1.GetCrossWith(line2).z < static_cast<UIVector2D::ScalarType>(0.0))
					{
						return false;
					}
				}
				else
				{
					if (line1.GetCrossWith(line2).z > static_cast<UIVector2D::ScalarType>(0.0))
					{
						return false;
					}
				}
			}

			return true;
		}

		//------------------------------------------------------------------------------
		std::vector<Graphics::ColoredVertex> UIClippingConvexPolygon::ClipPolygon(const std::vector<Graphics::ColoredVertex> &polygon) const
		{
			if (!m_isValid)
			{
				return std::vector<Graphics::ColoredVertex>();
			}

			return _Internal::ClipPointPolygon(m_points, polygon, *this);
		}

		//------------------------------------------------------------------------------
		std::vector<Graphics::TextureColoredVertex> UIClippingConvexPolygon::ClipPolygon(const std::vector<Graphics::TextureColoredVertex> &polygon) const
		{
			if (!m_isValid)
			{
				return std::vector<Graphics::TextureColoredVertex>();
			}

			return _Internal::ClipPointPolygon(m_points, polygon, *this);
		}

		//------------------------------------------------------------------------------
		std::vector<Graphics::PositionVertex> UIClippingConvexPolygon::ClipPolygon(const std::vector<Graphics::PositionVertex> &polygon) const
		{
			if (!m_isValid)
			{
				return std::vector<Graphics::PositionVertex>();
			}

			return _Internal::ClipPointPolygon(m_points, polygon, *this);
		}

		//------------------------------------------------------------------------------
		void UIClippingConvexPolygon::ForceUpdatePolygonStatus()
		{
			_InitializePolygon();
		}
	}
}
