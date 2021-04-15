////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionExpression_Reader.inl (Leggiero/Modules - LegacyUI)
//
// Reader Implementation of Expressions
// Included in UIDescriptionExpression.cpp
////////////////////////////////////////////////////////////////////////////////


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			namespace Expression
			{
				namespace Reader
				{
					//------------------------------------------------------------------------------
					// Read inline-expression from the given string
					std::shared_ptr<IArbitrarilyTypedExpression> ParseInlineExpression(const char *expressionString, UIDescriptionValueType hintedType)
					{
						if (expressionString == nullptr)
						{
							return nullptr;
						}

						std::string expStringCopy(expressionString);
						if (expStringCopy.length() > 2 && expStringCopy[0] == 'v' && expStringCopy[1] == '@')
						{
							std::vector<std::string> tokenized;

							size_t lastStartIndex = 2;
							for (size_t i = 2; i < expStringCopy.length(); ++i)
							{
								if (expStringCopy[i] == StringFormat::kExpressionTypeDelimiter)
								{
									tokenized.push_back(std::string(expStringCopy, lastStartIndex, i - lastStartIndex));
									lastStartIndex = i + 1;
									break;
								}
							}
							tokenized.push_back(std::string(expStringCopy, lastStartIndex));

							UIDescriptionValueType valueType = UIDescriptionValueType::kINVALID;
							if (tokenized.size() > 1)
							{
								valueType = Expression::ParseTypeFromString(tokenized[1].c_str());
								if (valueType == UIDescriptionValueType::kINVALID)
								{
									// Invalid Type
									return nullptr;
								}
							}
							if (valueType == UIDescriptionValueType::kINVALID)
							{
								valueType = hintedType;
							}

							VariablePathType targetPath = TypeUtility::ParseVariablePath(tokenized[0]);

							// Typed Variables
							switch (valueType)
							{
								case UIDescriptionValueType::kBoolean:
									return std::make_shared<Dereference<BooleanValueType> >(targetPath);

								case UIDescriptionValueType::kInteger:
									return std::make_shared<Dereference<IntegerValueType> >(targetPath);

								case UIDescriptionValueType::kFloatingPoint:
									return std::make_shared<Dereference<FloatingPointValueType> >(targetPath);

								case UIDescriptionValueType::kColor:
									return std::make_shared<Dereference<ColorARGBValueType> >(targetPath);

								case UIDescriptionValueType::kVector2D:
									return std::make_shared<Dereference<Vector2DValueType> >(targetPath);

								case UIDescriptionValueType::kVector3D:
									return std::make_shared<Dereference<Vector3DValueType> >(targetPath);

								case UIDescriptionValueType::kString:
									return std::make_shared<Dereference<StringValueType> >(targetPath);
							}

							return nullptr;
						}
						else
						{
							if (hintedType == UIDescriptionValueType::kString)
							{
								return std::make_shared<Constant<StringValueType> >(expStringCopy);
							}

							std::vector<std::string> tokenized;

							size_t lastStartIndex = 0;
							for (size_t i = 0; i < expStringCopy.length(); ++i)
							{
								if (expStringCopy[i] == StringFormat::kExpressionTypeDelimiter)
								{
									tokenized.push_back(std::string(expStringCopy, lastStartIndex, i - lastStartIndex));
									lastStartIndex = i + 1;
									break;
								}
							}
							tokenized.push_back(std::string(expStringCopy, lastStartIndex));

							UIDescriptionValueType valueType = UIDescriptionValueType::kINVALID;
							if (tokenized.size() > 1)
							{
								valueType = Expression::ParseTypeFromString(tokenized[1].c_str());
								if (valueType == UIDescriptionValueType::kINVALID)
								{
									// Invalid Type
									tokenized[0] = expStringCopy;
								}
							}
							if (valueType == UIDescriptionValueType::kINVALID)
							{
								if (hintedType == UIDescriptionValueType::kINVALID)
								{
									valueType = Expression::DetectTypeFromString(tokenized[0].c_str());
									if (valueType == UIDescriptionValueType::kINVALID)
									{
										// Invalid Value
										return nullptr;
									}
								}
								else
								{
									valueType = hintedType;
								}
							}

							switch (valueType)
							{
								case UIDescriptionValueType::kBoolean:
									return std::make_shared<Constant<BooleanValueType> >(Expression::ParseBooleanValueFromString(tokenized[0].c_str()));

								case UIDescriptionValueType::kInteger:
									return std::make_shared<Constant<IntegerValueType> >(Expression::ParseIntegerValueFromString(tokenized[0].c_str()));

								case UIDescriptionValueType::kFloatingPoint:
									return std::make_shared<Constant<FloatingPointValueType> >(Expression::ParseFloatingPointValueFromString(tokenized[0].c_str()));

								case UIDescriptionValueType::kColor:
									return std::make_shared<Constant<ColorARGBValueType> >(Expression::ParseColorValueFromString(tokenized[0].c_str()));

								case UIDescriptionValueType::kVector2D:
									return std::make_shared<Constant<Vector2DValueType> >(Expression::ParseVector2DValueFromString(tokenized[0].c_str()));

								case UIDescriptionValueType::kVector3D:
									return std::make_shared<Constant<Vector3DValueType> >(Expression::ParseVector3DValueFromString(tokenized[0].c_str()));

								case UIDescriptionValueType::kString:
									return std::make_shared<Constant<StringValueType> >(Expression::ParseStringValueFromString(tokenized[0].c_str()));
							}

							return nullptr;
						}
					}


					//////////////////////////////////////////////////////////////////////////////// Reader from XML Data

					namespace XML
					{
						namespace _Internal
						{
							namespace TagNames
							{
								const char *kConstantValue = "Constant";

								const char *kVariableDereference = "Reference";
								const char *kCasting = "Cast";

								const char *kOperationMinus = "Minus";
								const char *kOperationAdd = "Add";
								const char *kOperationSubtract = "Subtract";
								const char *kOperationMultiply = "Multiply";
								const char *kOperationDivide = "Divide";

								const char *kOperationModulus = "Modulus";
								const char *kOperationRemainder = "Remainder";

								const char *kOperationLerp = "Lerp";

								const char *kLogicalNot = "Not";
								const char *kLogicalAnd = "And";
								const char *kLogicalOr = "Or";
								const char *kLogicalXor = "Xor";

								const char *kConditionEqual = "IsEqual";
								const char *kConditionNotEqual = "IsNotEqual";
								const char *kConditionGreater = "IsGreater";
								const char *kConditionLess = "IsLess";
								const char *kConditionGreaterEqual = "IsGreaterEqual";
								const char *kConditionLessEqual = "IsLessEqual";
								const char *kConditionApproxEqual = "IsApproxEqual";

								const char *kSelectIf = "If";
								const char *kSelectMin = "Min";
								const char *kSelectMax = "Max";

								const char *kFunctionAbsolute = "Abs";
								const char *kFunctionSquareRoot = "Sqrt";
								const char *kFunctionPower = "Power";
								const char *kFunctionLog = "Log";
								const char *kFunctionSin = "Sin";
								const char *kFunctionCos = "Cos";
								const char *kFunctionTan = "Tan";
								const char *kFunctionArcSin = "ArcSin";
								const char *kFunctionArcCos = "ArcCos";
								const char *kFunctionArcTan = "ArcTan";

								const char *kFunctionRound = "Round";
								const char *kFunctionFloor = "Floor";
								const char *kFunctionCeil = "Ceil";

								const char *kFunctionClamp = "Clamp";
								const char *kFunctionClampLow = "ClampLow";
								const char *kFunctionClampHigh = "ClampHigh";

								const char *kTextureGetWidth = "TexGetWidth";
								const char *kTextureGetHeight = "TexGetHeight";

								const char *kConstructColorARGB = "ColorARGB";
								const char *kConstructColorAHSL = "ColorAHSL";

								const char *kConstructVector2D = "Vector2D";
								const char *kConstructVector3D = "Vector3D";

								const char *kComponentOf = "Component";

								const char *kVectorDot = "VDot";
								const char *kVectorCross = "VCross";
								const char *kVectorScalarMultiply = "VScalarMultiply";
								const char *kVectorNormalize = "VNormal";
								const char *kVectorLength = "VLength";
								const char *kVectorSquareLength = "VSquareLength";
							}

							//------------------------------------------------------------------------------
							std::shared_ptr<IArbitrarilyTypedExpression> ReadConstantValueExpressionFromElement(tinyxml2::XMLElement *elem, UIDescriptionValueType hintedType)
							{
								const char *valueAttribute = elem->Attribute("value");

								UIDescriptionValueType valueType = UIDescriptionValueType::kINVALID;

								const char *typeAttribute = elem->Attribute("type");
								if (typeAttribute != nullptr)
								{
									valueType = Expression::ParseTypeFromString(typeAttribute);
									if (valueType == UIDescriptionValueType::kINVALID)
									{
										// Invalid Type
										return nullptr;
									}
								}

								if (valueAttribute == nullptr)
								{
									// Default Values
									switch (valueType)
									{
										case UIDescriptionValueType::kBoolean: return std::make_shared<Constant<BooleanValueType> >(false);
										case UIDescriptionValueType::kInteger: return std::make_shared<Constant<IntegerValueType> >(0);
										case UIDescriptionValueType::kFloatingPoint: return std::make_shared<Constant<FloatingPointValueType> >(std::numeric_limits<FloatingPointValueType>::quiet_NaN());
										case UIDescriptionValueType::kColor: return std::make_shared<Constant<ColorARGBValueType> >(ColorARGBValueType::kBlack);
										case UIDescriptionValueType::kVector2D: return std::make_shared<Constant<Vector2DValueType> >(Vector2DValueType::kInvalid);
										case UIDescriptionValueType::kVector3D: return std::make_shared<Constant<Vector3DValueType> >(Vector3DValueType::kInvalid);
										case UIDescriptionValueType::kString: return std::make_shared<Constant<StringValueType> >("");
									}

									// No Value
									return nullptr;
								}

								if (valueType == UIDescriptionValueType::kINVALID)
								{
									if (hintedType == UIDescriptionValueType::kINVALID)
									{
										valueType = Expression::DetectTypeFromString(valueAttribute);
										if (valueType == UIDescriptionValueType::kINVALID)
										{
											// Invalid Value
											return nullptr;
										}
									}
									else
									{
										valueType = hintedType;
									}
								}

								switch (valueType)
								{
									case UIDescriptionValueType::kBoolean:
										return std::make_shared<Constant<BooleanValueType> >(Expression::ParseBooleanValueFromString(valueAttribute));

									case UIDescriptionValueType::kInteger:
										return std::make_shared<Constant<IntegerValueType> >(Expression::ParseIntegerValueFromString(valueAttribute));

									case UIDescriptionValueType::kFloatingPoint:
										return std::make_shared<Constant<FloatingPointValueType> >(Expression::ParseFloatingPointValueFromString(valueAttribute));

									case UIDescriptionValueType::kColor:
										return std::make_shared<Constant<ColorARGBValueType> >(Expression::ParseColorValueFromString(valueAttribute));

									case UIDescriptionValueType::kVector2D:
										return std::make_shared<Constant<Vector2DValueType> >(Expression::ParseVector2DValueFromString(valueAttribute));

									case UIDescriptionValueType::kVector3D:
										return std::make_shared<Constant<Vector3DValueType> >(Expression::ParseVector3DValueFromString(valueAttribute));

									case UIDescriptionValueType::kString:
										return std::make_shared<Constant<StringValueType> >(Expression::ParseStringValueFromString(valueAttribute));
								}

								return nullptr;
							}

							//------------------------------------------------------------------------------
							std::shared_ptr<IArbitrarilyTypedExpression> ReadVariableDereferenceExpressionFromElement(tinyxml2::XMLElement *elem, UIDescriptionValueType hintedType)
							{
								UIDescriptionValueType valueType = UIDescriptionValueType::kINVALID;

								const char *typeAttribute = elem->Attribute("type");
								if (typeAttribute != nullptr)
								{
									valueType = Expression::ParseTypeFromString(typeAttribute);
									if (valueType == UIDescriptionValueType::kINVALID)
									{
										// Invalid Type
										return nullptr;
									}
								}
								if (valueType == UIDescriptionValueType::kINVALID)
								{
									valueType = hintedType;
								}

								const char *referencedTarget = elem->Attribute("var");
								if (referencedTarget == nullptr)
								{
									return nullptr;
								}
								VariablePathType targetPath = TypeUtility::ParseVariablePath(referencedTarget);

								// Typed Variables
								switch (valueType)
								{
									case UIDescriptionValueType::kBoolean:
										return std::make_shared<Dereference<BooleanValueType> >(targetPath);

									case UIDescriptionValueType::kInteger:
										return std::make_shared<Dereference<IntegerValueType> >(targetPath);

									case UIDescriptionValueType::kFloatingPoint:
										return std::make_shared<Dereference<FloatingPointValueType> >(targetPath);

									case UIDescriptionValueType::kColor:
										return std::make_shared<Dereference<ColorARGBValueType> >(targetPath);

									case UIDescriptionValueType::kVector2D:
										return std::make_shared<Dereference<Vector2DValueType> >(targetPath);

									case UIDescriptionValueType::kVector3D:
										return std::make_shared<Dereference<Vector3DValueType> >(targetPath);

									case UIDescriptionValueType::kString:
										return std::make_shared<Dereference<StringValueType> >(targetPath);
								}

								return nullptr;
							}

							//------------------------------------------------------------------------------
							std::shared_ptr<IArbitrarilyTypedExpression> ReadCastingExpressionFromElement(tinyxml2::XMLElement *elem, UIDescriptionValueType hintedType)
							{
								UIDescriptionValueType valueType = UIDescriptionValueType::kINVALID;

								const char *typeAttribute = elem->Attribute("type");
								if (typeAttribute != nullptr)
								{
									valueType = Expression::ParseTypeFromString(typeAttribute);
									if (valueType == UIDescriptionValueType::kINVALID)
									{
										// Invalid Type
										return nullptr;
									}
								}
								if (valueType == UIDescriptionValueType::kINVALID)
								{
									valueType = hintedType;
								}

								tinyxml2::XMLElement *expressionNode = elem->FirstChildElement();
								if (expressionNode == nullptr)
								{
									// No Expression
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> variableExpression(ReadExpressionFromElement(expressionNode, valueType));
								if (!variableExpression)
								{
									// Invalid Expression
									return nullptr;
								}

								if (variableExpression->GetValueType() != valueType)
								{
									variableExpression = Expression::_Internal::CastExpression(variableExpression, valueType);
								}
								return variableExpression;
							}

							//------------------------------------------------------------------------------
							std::shared_ptr<IArbitrarilyTypedExpression> ReadMinusExpressionFromElement(tinyxml2::XMLElement *elem)
							{
								tinyxml2::XMLElement *expressionNode = elem->FirstChildElement();
								if (expressionNode == nullptr)
								{
									// No Expression
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> subExpression(ReadExpressionFromElement(expressionNode));
								if (!subExpression)
								{
									// Invalid Expression
									return nullptr;
								}

								switch (subExpression->GetValueType())
								{
									case UIDescriptionValueType::kBoolean: return std::make_shared<Minus<BooleanValueType> >(std::dynamic_pointer_cast<IExpression<BooleanValueType> >(subExpression));
									case UIDescriptionValueType::kInteger: return std::make_shared<Minus<IntegerValueType> >(std::dynamic_pointer_cast<IExpression<IntegerValueType> >(subExpression));
									case UIDescriptionValueType::kFloatingPoint: return std::make_shared<Minus<FloatingPointValueType> >(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression));
									case UIDescriptionValueType::kVector2D: return std::make_shared<Minus<Vector2DValueType> >(std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(subExpression));
									case UIDescriptionValueType::kVector3D: return std::make_shared<Minus<Vector3DValueType> >(std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(subExpression));
								}

								// Not-Minusable Type
								throw new UIDescriptionExpressionTypeErrorException();
							}

							//------------------------------------------------------------------------------
							template <template<typename ValueT> class OperationT>
							std::shared_ptr<IArbitrarilyTypedExpression> ReadMultiOperandOperationExpressionFromElement(tinyxml2::XMLElement *elem)
							{
								UIDescriptionValueType primaryType = UIDescriptionValueType::kINVALID;
								std::vector<std::shared_ptr<IArbitrarilyTypedExpression> > primaryTypedExpressions;
								std::vector<std::shared_ptr<IArbitrarilyTypedExpression> > secondaryTypedExpressions;
								std::vector<tinyxml2::XMLElement *> unTypedElements;
								for (tinyxml2::XMLElement *currentNode = elem->FirstChildElement(); currentNode != nullptr; currentNode = currentNode->NextSiblingElement())
								{
									bool isTypeError = false;
									try
									{
										std::shared_ptr<IArbitrarilyTypedExpression> currentSubExpression(ReadExpressionFromElement(currentNode, primaryType));
										if (primaryType == UIDescriptionValueType::kINVALID)
										{
											if (!currentSubExpression)
											{
												unTypedElements.push_back(currentNode);
												continue;
											}
											primaryType = currentSubExpression->GetValueType();
											primaryTypedExpressions.push_back(currentSubExpression);
										}
										else
										{
											if (currentSubExpression)
											{
												if (currentSubExpression->GetValueType() == primaryType)
												{
													primaryTypedExpressions.push_back(currentSubExpression);
												}
												else
												{
													switch (primaryType)
													{
														case UIDescriptionValueType::kInteger:
															{
																if (currentSubExpression->GetValueType() == UIDescriptionValueType::kFloatingPoint)
																{
																	secondaryTypedExpressions.push_back(currentSubExpression);
																}
																else
																{
																	isTypeError = true;
																}
															}
															break;

														case UIDescriptionValueType::kFloatingPoint:
															{
																if (currentSubExpression->GetValueType() == UIDescriptionValueType::kInteger)
																{
																	secondaryTypedExpressions.push_back(currentSubExpression);
																}
																else
																{
																	isTypeError = true;
																}
															}
															break;

														default:
															// Cannot add expressions of diffrent types
															isTypeError = true;
															break;
													}
												}
											}
											else
											{
												isTypeError = true;
											}
										}
									}
									catch (UIDescriptionExpressionTypeErrorException)
									{
										unTypedElements.push_back(currentNode);
									}
									if (isTypeError)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}
								}
								if (primaryType == UIDescriptionValueType::kINVALID || primaryTypedExpressions.empty())
								{
									// No Expression
									return nullptr;
								}

								std::vector<std::shared_ptr<IArbitrarilyTypedExpression> > operandExpressions;
								operandExpressions.reserve(primaryTypedExpressions.size() + secondaryTypedExpressions.size() + unTypedElements.size());
								if (secondaryTypedExpressions.empty())
								{
									operandExpressions.insert(operandExpressions.end(), primaryTypedExpressions.begin(), primaryTypedExpressions.end());
								}
								else
								{
									if (primaryType == UIDescriptionValueType::kInteger)
									{
										primaryType = UIDescriptionValueType::kFloatingPoint;
										operandExpressions.insert(operandExpressions.end(), secondaryTypedExpressions.begin(), secondaryTypedExpressions.end());
										for (const std::shared_ptr<IArbitrarilyTypedExpression> &expressionToCast : primaryTypedExpressions)
										{
											operandExpressions.push_back(Expression::_Internal::CastExpression(expressionToCast, UIDescriptionValueType::kFloatingPoint));
										}
									}
									else if (primaryType == UIDescriptionValueType::kFloatingPoint)
									{
										operandExpressions.insert(operandExpressions.end(), primaryTypedExpressions.begin(), primaryTypedExpressions.end());
										for (const std::shared_ptr<IArbitrarilyTypedExpression> &expressionToCast : secondaryTypedExpressions)
										{
											operandExpressions.push_back(Expression::_Internal::CastExpression(expressionToCast, UIDescriptionValueType::kFloatingPoint));
										}
									}
									else
									{
										// Impossible!
										throw new UIDescriptionExpressionTypeErrorException();
									}
								}
								for (tinyxml2::XMLElement *pUnTypedElement : unTypedElements)
								{
									std::shared_ptr<IArbitrarilyTypedExpression> currentSubExpression(ReadExpressionFromElement(pUnTypedElement, primaryType));
									if (currentSubExpression->GetValueType() != primaryType)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}
									operandExpressions.push_back(currentSubExpression);
								}

								switch (primaryType)
								{
									case UIDescriptionValueType::kBoolean:
										{
											std::vector<std::shared_ptr<IExpression<BooleanValueType> > > typedSubExpressions;
											typedSubExpressions.reserve(operandExpressions.size());
											for (const std::shared_ptr<IArbitrarilyTypedExpression> &expressionToCast : operandExpressions)
											{
												typedSubExpressions.push_back(std::dynamic_pointer_cast<IExpression<BooleanValueType> >(expressionToCast));
											}
											return std::make_shared<OperationT<BooleanValueType> >(typedSubExpressions);
										}
									
									case UIDescriptionValueType::kInteger:
										{
											std::vector<std::shared_ptr<IExpression<IntegerValueType> > > typedSubExpressions;
											typedSubExpressions.reserve(operandExpressions.size());
											for (const std::shared_ptr<IArbitrarilyTypedExpression> &expressionToCast : operandExpressions)
											{
												typedSubExpressions.push_back(std::dynamic_pointer_cast<IExpression<IntegerValueType> >(expressionToCast));
											}
											return std::make_shared<OperationT<IntegerValueType> >(typedSubExpressions);
										}

									case UIDescriptionValueType::kFloatingPoint:
										{
											std::vector<std::shared_ptr<IExpression<FloatingPointValueType> > > typedSubExpressions;
											typedSubExpressions.reserve(operandExpressions.size());
											for (const std::shared_ptr<IArbitrarilyTypedExpression> &expressionToCast : operandExpressions)
											{
												typedSubExpressions.push_back(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(expressionToCast));
											}
											return std::make_shared<OperationT<FloatingPointValueType> >(typedSubExpressions);
										}

									case UIDescriptionValueType::kColor:
										{
											std::vector<std::shared_ptr<IExpression<ColorARGBValueType> > > typedSubExpressions;
											typedSubExpressions.reserve(operandExpressions.size());
											for (const std::shared_ptr<IArbitrarilyTypedExpression> &expressionToCast : operandExpressions)
											{
												typedSubExpressions.push_back(std::dynamic_pointer_cast<IExpression<ColorARGBValueType> >(expressionToCast));
											}
											return std::make_shared<OperationT<ColorARGBValueType> >(typedSubExpressions);
										}

									case UIDescriptionValueType::kVector2D:
										{
											std::vector<std::shared_ptr<IExpression<Vector2DValueType> > > typedSubExpressions;
											typedSubExpressions.reserve(operandExpressions.size());
											for (const std::shared_ptr<IArbitrarilyTypedExpression> &expressionToCast : operandExpressions)
											{
												typedSubExpressions.push_back(std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(expressionToCast));
											}
											return std::make_shared<OperationT<Vector2DValueType> >(typedSubExpressions);
										}

									case UIDescriptionValueType::kVector3D:
										{
											std::vector<std::shared_ptr<IExpression<Vector3DValueType> > > typedSubExpressions;
											typedSubExpressions.reserve(operandExpressions.size());
											for (const std::shared_ptr<IArbitrarilyTypedExpression> &expressionToCast : operandExpressions)
											{
												typedSubExpressions.push_back(std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(expressionToCast));
											}
											return std::make_shared<OperationT<Vector3DValueType> >(typedSubExpressions);
										}

									case UIDescriptionValueType::kString:
										{
											std::vector<std::shared_ptr<IExpression<StringValueType> > > typedSubExpressions;
											typedSubExpressions.reserve(operandExpressions.size());
											for (const std::shared_ptr<IArbitrarilyTypedExpression> &expressionToCast : operandExpressions)
											{
												typedSubExpressions.push_back(std::dynamic_pointer_cast<IExpression<StringValueType> >(expressionToCast));
											}
											return std::make_shared<OperationT<StringValueType> >(typedSubExpressions);
										}
								}

								// Invalid Type
								throw new UIDescriptionExpressionTypeErrorException();
							}

							//------------------------------------------------------------------------------
							template <template<typename ValueT> class OperationT>
							std::shared_ptr<IArbitrarilyTypedExpression> ReadBinaryOperationExpressionFromElement(tinyxml2::XMLElement *elem, UIDescriptionValueType hintedType)
							{
								tinyxml2::XMLElement *expressionNode1 = elem->FirstChildElement();
								if (expressionNode1 == nullptr)
								{
									// No Expression
									return nullptr;
								}
								tinyxml2::XMLElement *expressionNode2 = expressionNode1->NextSiblingElement();
								if (expressionNode2 == nullptr)
								{
									// Not-Enough Expression
									return nullptr;
								}

								UIDescriptionValueType primaryType = UIDescriptionValueType::kINVALID;

								std::shared_ptr<IArbitrarilyTypedExpression> subExpression1;
								try
								{
									subExpression1 = ReadExpressionFromElement(expressionNode1);
								}
								catch (UIDescriptionExpressionTypeErrorException)
								{
								}

								if (subExpression1)
								{
									primaryType = subExpression1->GetValueType();
								}
								std::shared_ptr<IArbitrarilyTypedExpression> subExpression2;
								try
								{
									subExpression2 = ReadExpressionFromElement(expressionNode2, primaryType);
								}
								catch (UIDescriptionExpressionTypeErrorException)
								{
								}

								if (!subExpression1)
								{
									if (subExpression2)
									{
										primaryType = subExpression2->GetValueType();
										subExpression1 = ReadExpressionFromElement(expressionNode1, primaryType);
									}
									else
									{
										subExpression1 = ReadExpressionFromElement(expressionNode1, hintedType);
										subExpression2 = ReadExpressionFromElement(expressionNode2, hintedType);
									}
								}

								if (!subExpression1 || !subExpression2)
								{
									// Invalid Operands
									throw new UIDescriptionExpressionTypeErrorException();
								}

								if (subExpression1->GetValueType() == subExpression2->GetValueType())
								{
									primaryType = subExpression1->GetValueType();
								}
								else
								{
									if (subExpression1->GetValueType() == UIDescriptionValueType::kInteger && subExpression2->GetValueType() == UIDescriptionValueType::kFloatingPoint)
									{
										primaryType = UIDescriptionValueType::kFloatingPoint;
										subExpression1 = Expression::_Internal::CastExpression(subExpression1, UIDescriptionValueType::kFloatingPoint);
									}
									else if (subExpression1->GetValueType() == UIDescriptionValueType::kFloatingPoint && subExpression2->GetValueType() == UIDescriptionValueType::kInteger)
									{
										primaryType = UIDescriptionValueType::kFloatingPoint;
										subExpression2 = Expression::_Internal::CastExpression(subExpression2, UIDescriptionValueType::kFloatingPoint);
									}
									else
									{
										// Incompatible Type
										throw new UIDescriptionExpressionTypeErrorException();
									}
								}

								switch (primaryType)
								{
									case UIDescriptionValueType::kBoolean:
										return std::make_shared<OperationT<BooleanValueType> >(std::dynamic_pointer_cast<IExpression<BooleanValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<BooleanValueType> >(subExpression2));

									case UIDescriptionValueType::kInteger:
										return std::make_shared<OperationT<IntegerValueType> >(std::dynamic_pointer_cast<IExpression<IntegerValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<IntegerValueType> >(subExpression2));

									case UIDescriptionValueType::kFloatingPoint:
										return std::make_shared<OperationT<FloatingPointValueType> >(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression2));

									case UIDescriptionValueType::kColor:
										return std::make_shared<OperationT<ColorARGBValueType> >(std::dynamic_pointer_cast<IExpression<ColorARGBValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<ColorARGBValueType> >(subExpression2));

									case UIDescriptionValueType::kVector2D:
										return std::make_shared<OperationT<Vector2DValueType> >(std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(subExpression2));

									case UIDescriptionValueType::kVector3D:
										return std::make_shared<OperationT<Vector3DValueType> >(std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(subExpression2));

									case UIDescriptionValueType::kString:
										return std::make_shared<OperationT<StringValueType> >(std::dynamic_pointer_cast<IExpression<StringValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<StringValueType> >(subExpression2));
								}

								// Invalid Type
								throw new UIDescriptionExpressionTypeErrorException();
							}

							//------------------------------------------------------------------------------
							template <template<typename ValueT> class OperationT>
							std::shared_ptr<IArbitrarilyTypedExpression> ReadBinaryNumberOperationExpressionFromElement(tinyxml2::XMLElement *elem, UIDescriptionValueType hintedType)
							{
								tinyxml2::XMLElement *expressionNode1 = elem->FirstChildElement();
								if (expressionNode1 == nullptr)
								{
									// No Expression
									return nullptr;
								}
								tinyxml2::XMLElement *expressionNode2 = expressionNode1->NextSiblingElement();
								if (expressionNode2 == nullptr)
								{
									// Not-Enough Expression
									return nullptr;
								}

								UIDescriptionValueType primaryType = UIDescriptionValueType::kINVALID;

								std::shared_ptr<IArbitrarilyTypedExpression> subExpression1;
								try
								{
									subExpression1 = ReadExpressionFromElement(expressionNode1);
								}
								catch (UIDescriptionExpressionTypeErrorException)
								{
								}

								if (subExpression1)
								{
									primaryType = subExpression1->GetValueType();
								}
								std::shared_ptr<IArbitrarilyTypedExpression> subExpression2;
								try
								{
									subExpression2 = ReadExpressionFromElement(expressionNode2, primaryType);
								}
								catch (UIDescriptionExpressionTypeErrorException)
								{
								}

								if (!subExpression1)
								{
									if (subExpression2)
									{
										primaryType = subExpression2->GetValueType();
										subExpression1 = ReadExpressionFromElement(expressionNode1, primaryType);
									}
									else
									{
										subExpression1 = ReadExpressionFromElement(expressionNode1, hintedType);
										subExpression2 = ReadExpressionFromElement(expressionNode2, hintedType);
									}
								}

								if (!subExpression1 || !subExpression2)
								{
									// Invalid Operands
									throw new UIDescriptionExpressionTypeErrorException();
								}

								if (subExpression1->GetValueType() == subExpression2->GetValueType())
								{
									primaryType = subExpression1->GetValueType();
								}
								else
								{
									if (subExpression1->GetValueType() == UIDescriptionValueType::kInteger && subExpression2->GetValueType() == UIDescriptionValueType::kFloatingPoint)
									{
										primaryType = UIDescriptionValueType::kFloatingPoint;
										subExpression1 = Expression::_Internal::CastExpression(subExpression1, UIDescriptionValueType::kFloatingPoint);
									}
									else if (subExpression1->GetValueType() == UIDescriptionValueType::kFloatingPoint && subExpression2->GetValueType() == UIDescriptionValueType::kInteger)
									{
										primaryType = UIDescriptionValueType::kFloatingPoint;
										subExpression2 = Expression::_Internal::CastExpression(subExpression2, UIDescriptionValueType::kFloatingPoint);
									}
									else
									{
										// Incompatible Type
										throw new UIDescriptionExpressionTypeErrorException();
									}
								}

								switch (primaryType)
								{
									case UIDescriptionValueType::kInteger:
										return std::make_shared<OperationT<IntegerValueType> >(std::dynamic_pointer_cast<IExpression<IntegerValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<IntegerValueType> >(subExpression2));

									case UIDescriptionValueType::kFloatingPoint:
										return std::make_shared<OperationT<FloatingPointValueType> >(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression2));
								}

								// Invalid Type
								throw new UIDescriptionExpressionTypeErrorException();
							}

							//------------------------------------------------------------------------------
							template <template<typename ValueT> class OperationT>
							std::shared_ptr<IArbitrarilyTypedExpression> ReadBinaryOrderingOperationExpressionFromElement(tinyxml2::XMLElement *elem, UIDescriptionValueType hintedType)
							{
								tinyxml2::XMLElement *expressionNode1 = elem->FirstChildElement();
								if (expressionNode1 == nullptr)
								{
									// No Expression
									return nullptr;
								}
								tinyxml2::XMLElement *expressionNode2 = expressionNode1->NextSiblingElement();
								if (expressionNode2 == nullptr)
								{
									// Not-Enough Expression
									return nullptr;
								}

								UIDescriptionValueType primaryType = UIDescriptionValueType::kINVALID;

								std::shared_ptr<IArbitrarilyTypedExpression> subExpression1;
								try
								{
									subExpression1 = ReadExpressionFromElement(expressionNode1);
								}
								catch (UIDescriptionExpressionTypeErrorException)
								{
								}

								if (subExpression1)
								{
									primaryType = subExpression1->GetValueType();
								}
								std::shared_ptr<IArbitrarilyTypedExpression> subExpression2;
								try
								{
									subExpression2 = ReadExpressionFromElement(expressionNode2, primaryType);
								}
								catch (UIDescriptionExpressionTypeErrorException)
								{
								}

								if (!subExpression1)
								{
									if (subExpression2)
									{
										primaryType = subExpression2->GetValueType();
										subExpression1 = ReadExpressionFromElement(expressionNode1, primaryType);
									}
									else
									{
										subExpression1 = ReadExpressionFromElement(expressionNode1, hintedType);
										subExpression2 = ReadExpressionFromElement(expressionNode2, hintedType);
									}
								}

								if (!subExpression1 || !subExpression2)
								{
									// Invalid Operands
									throw new UIDescriptionExpressionTypeErrorException();
								}

								if (subExpression1->GetValueType() == subExpression2->GetValueType())
								{
									primaryType = subExpression1->GetValueType();
								}
								else
								{
									if (subExpression1->GetValueType() == UIDescriptionValueType::kInteger && subExpression2->GetValueType() == UIDescriptionValueType::kFloatingPoint)
									{
										primaryType = UIDescriptionValueType::kFloatingPoint;
										subExpression1 = Expression::_Internal::CastExpression(subExpression1, UIDescriptionValueType::kFloatingPoint);
									}
									else if (subExpression1->GetValueType() == UIDescriptionValueType::kFloatingPoint && subExpression2->GetValueType() == UIDescriptionValueType::kInteger)
									{
										primaryType = UIDescriptionValueType::kFloatingPoint;
										subExpression2 = Expression::_Internal::CastExpression(subExpression2, UIDescriptionValueType::kFloatingPoint);
									}
									else
									{
										// Incompatible Type
										throw new UIDescriptionExpressionTypeErrorException();
									}
								}

								switch (primaryType)
								{
									case UIDescriptionValueType::kInteger:
										return std::make_shared<OperationT<IntegerValueType> >(std::dynamic_pointer_cast<IExpression<IntegerValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<IntegerValueType> >(subExpression2));

									case UIDescriptionValueType::kFloatingPoint:
										return std::make_shared<OperationT<FloatingPointValueType> >(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression2));

									case UIDescriptionValueType::kVector2D:
										return std::make_shared<OperationT<Vector2DValueType> >(std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(subExpression2));

									case UIDescriptionValueType::kVector3D:
										return std::make_shared<OperationT<Vector3DValueType> >(std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(subExpression2));

									case UIDescriptionValueType::kString:
										return std::make_shared<OperationT<StringValueType> >(std::dynamic_pointer_cast<IExpression<StringValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<StringValueType> >(subExpression2));
								}

								// Invalid Type
								throw new UIDescriptionExpressionTypeErrorException();
							}

							//------------------------------------------------------------------------------
							template <class OperationT, typename OperandT>
							std::shared_ptr<OperationT> ReadTypedUnaryOperationExpressionFromElement(tinyxml2::XMLElement *elem)
							{
								tinyxml2::XMLElement *expressionNode = elem->FirstChildElement();
								if (expressionNode == nullptr)
								{
									// No Expression
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> subExpression(ReadExpressionFromElement(expressionNode, GetValueTypeTag<OperandT>()));
								if (!subExpression)
								{
									// Invalid Operand
									return nullptr;
								}

								if (subExpression->GetValueType() != GetValueTypeTag<OperandT>())
								{
									subExpression = Expression::_Internal::CastExpression(subExpression, GetValueTypeTag<OperandT>());
								}

								return std::make_shared<OperationT>(std::dynamic_pointer_cast<IExpression<OperandT> >(subExpression));
							}

							//------------------------------------------------------------------------------
							template <template<typename ValueT> class OperationT>
							std::shared_ptr<IArbitrarilyTypedExpression> ReadVectorUnaryOperationExpressionFromElement(tinyxml2::XMLElement *elem, UIDescriptionValueType hintedType = UIDescriptionValueType::kINVALID)
							{
								std::shared_ptr<IArbitrarilyTypedExpression> subExpression;
								tinyxml2::XMLElement *expressionNode = elem->FirstChildElement();
								if (expressionNode == nullptr)
								{
									const char *vecAttribute = elem->Attribute("vec");
									if (vecAttribute == nullptr)
									{
										vecAttribute = elem->Attribute("vector");
									}
									if (vecAttribute == nullptr)
									{
										// No target
										return nullptr;
									}
									subExpression = ParseInlineExpression(vecAttribute, hintedType);
								}
								else
								{
									subExpression = ReadExpressionFromElement(expressionNode, hintedType);
								}
								if (!subExpression)
								{
									// Invalid Operand
									return nullptr;
								}

								switch (subExpression->GetValueType())
								{
									case UIDescriptionValueType::kVector2D:
										return std::make_shared<OperationT<Vector2DValueType> >(std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(subExpression));

									case UIDescriptionValueType::kVector3D:
										return std::make_shared<OperationT<Vector3DValueType> >(std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(subExpression));
								}

								throw new UIDescriptionExpressionTypeErrorException();
							}

							//------------------------------------------------------------------------------
							template <class OperationT, typename Operand1T, typename Operand2T>
							std::shared_ptr<OperationT> ReadTypedBinaryOperationExpressionFromElement(tinyxml2::XMLElement *elem)
							{
								tinyxml2::XMLElement *expressionNode1 = elem->FirstChildElement();
								if (expressionNode1 == nullptr)
								{
									// No Expression
									return nullptr;
								}
								tinyxml2::XMLElement *expressionNode2 = expressionNode1->NextSiblingElement();
								if (expressionNode2 == nullptr)
								{
									// Not-Enough Expression
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> subExpression1(ReadExpressionFromElement(expressionNode1, GetValueTypeTag<Operand1T>()));
								std::shared_ptr<IArbitrarilyTypedExpression> subExpression2(ReadExpressionFromElement(expressionNode2, GetValueTypeTag<Operand2T>()));
								if (!subExpression1 || !subExpression2)
								{
									// Invalid Operand
									return nullptr;
								}

								if (subExpression1->GetValueType() != GetValueTypeTag<Operand1T>())
								{
									subExpression1 = Expression::_Internal::CastExpression(subExpression1, GetValueTypeTag<Operand1T>());
								}
								if (subExpression2->GetValueType() != GetValueTypeTag<Operand2T>())
								{
									subExpression2 = Expression::_Internal::CastExpression(subExpression2, GetValueTypeTag<Operand2T>());
								}

								return std::make_shared<OperationT>(std::dynamic_pointer_cast<IExpression<Operand1T> >(subExpression1), std::dynamic_pointer_cast<IExpression<Operand2T> >(subExpression2));
							}

							//------------------------------------------------------------------------------
							template <class OperationT>
							std::shared_ptr<OperationT> ReadVector3DBinaryOperationExpressionFromElement(tinyxml2::XMLElement *elem)
							{
								tinyxml2::XMLElement *currentChildElement = elem->FirstChildElement();

								std::shared_ptr<IArbitrarilyTypedExpression> subExpression1;
								if (currentChildElement == nullptr)
								{
									const char *vec1Attribute = elem->Attribute("vec1");
									if (vec1Attribute == nullptr)
									{
										vec1Attribute = elem->Attribute("vector1");
									}
									if (vec1Attribute == nullptr)
									{
										// No target
										return nullptr;
									}
									subExpression1 = ParseInlineExpression(vec1Attribute, UIDescriptionValueType::kVector3D);
								}
								else
								{
									subExpression1 = ReadExpressionFromElement(currentChildElement, UIDescriptionValueType::kVector3D);
									currentChildElement = currentChildElement->NextSiblingElement();
								}
								if (!subExpression1)
								{
									// Invalid Operand
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> subExpression2;
								if (currentChildElement == nullptr)
								{
									const char *vec2Attribute = elem->Attribute("vec2");
									if (vec2Attribute == nullptr)
									{
										vec2Attribute = elem->Attribute("vector2");
									}
									if (vec2Attribute == nullptr)
									{
										// No target
										return nullptr;
									}
									subExpression2 = ParseInlineExpression(vec2Attribute, UIDescriptionValueType::kVector3D);
								}
								else
								{
									subExpression2 = ReadExpressionFromElement(currentChildElement, UIDescriptionValueType::kVector3D);
									currentChildElement = currentChildElement->NextSiblingElement();
								}
								if (!subExpression2)
								{
									// Invalid Operand
									return nullptr;
								}

								// Cast to Vector3D
								if (subExpression1->GetValueType() == UIDescriptionValueType::kVector2D)
								{
									subExpression1 = Expression::_Internal::CastExpression(subExpression1, UIDescriptionValueType::kVector3D);
								}
								if (subExpression2->GetValueType() == UIDescriptionValueType::kVector2D)
								{
									subExpression2 = Expression::_Internal::CastExpression(subExpression2, UIDescriptionValueType::kVector3D);
								}

								// Type Check
								if (subExpression1->GetValueType() != UIDescriptionValueType::kVector3D || subExpression2->GetValueType() != UIDescriptionValueType::kVector3D)
								{
									throw new UIDescriptionExpressionTypeErrorException();
								}

								return std::make_shared<OperationT>(std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(subExpression2));
							}

							//------------------------------------------------------------------------------
							template <template<typename ValueT> class OperationT, typename ParameterT>
							std::shared_ptr<IArbitrarilyTypedExpression> ReadTrinaryOperationExpressionFromElement(tinyxml2::XMLElement *elem, UIDescriptionValueType hintedType)
							{
								tinyxml2::XMLElement *expressionNode1 = elem->FirstChildElement();
								if (expressionNode1 == nullptr)
								{
									// No Expression
									return nullptr;
								}
								tinyxml2::XMLElement *expressionNode2 = expressionNode1->NextSiblingElement();
								if (expressionNode2 == nullptr)
								{
									// Not-Enough Expression
									return nullptr;
								}
								tinyxml2::XMLElement *expressionNode3 = expressionNode2->NextSiblingElement();
								if (expressionNode3 == nullptr)
								{
									// Not-Enough Expression
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> parameterExpression(ReadExpressionFromElement(expressionNode1, GetValueTypeTag<ParameterT>()));
								if (!parameterExpression)
								{
									// Invalid Parameter Expression
									return nullptr;
								}
								if (parameterExpression->GetValueType() != GetValueTypeTag<ParameterT>())
								{
									parameterExpression = Expression::_Internal::CastExpression(parameterExpression, GetValueTypeTag<ParameterT>());
								}

								UIDescriptionValueType primaryType = UIDescriptionValueType::kINVALID;

								std::shared_ptr<IArbitrarilyTypedExpression> subExpression1;
								try
								{
									subExpression1 = ReadExpressionFromElement(expressionNode2);
								}
								catch (UIDescriptionExpressionTypeErrorException)
								{
								}

								if (subExpression1)
								{
									primaryType = subExpression1->GetValueType();
								}
								std::shared_ptr<IArbitrarilyTypedExpression> subExpression2;
								try
								{
									subExpression2 = ReadExpressionFromElement(expressionNode3, primaryType);
								}
								catch (UIDescriptionExpressionTypeErrorException)
								{
								}

								if (!subExpression1)
								{
									if (subExpression2)
									{
										primaryType = subExpression2->GetValueType();
										subExpression1 = ReadExpressionFromElement(expressionNode2, primaryType);
									}
									else
									{
										subExpression1 = ReadExpressionFromElement(expressionNode2, hintedType);
										subExpression2 = ReadExpressionFromElement(expressionNode3, hintedType);
									}
								}

								if (!subExpression1 || !subExpression2)
								{
									// Invalid Operands
									throw new UIDescriptionExpressionTypeErrorException();
								}

								if (subExpression1->GetValueType() == subExpression2->GetValueType())
								{
									primaryType = subExpression1->GetValueType();
								}
								else
								{
									if (subExpression1->GetValueType() == UIDescriptionValueType::kInteger && subExpression2->GetValueType() == UIDescriptionValueType::kFloatingPoint)
									{
										primaryType = UIDescriptionValueType::kFloatingPoint;
										subExpression1 = Expression::_Internal::CastExpression(subExpression1, UIDescriptionValueType::kFloatingPoint);
									}
									else if (subExpression1->GetValueType() == UIDescriptionValueType::kFloatingPoint && subExpression2->GetValueType() == UIDescriptionValueType::kInteger)
									{
										primaryType = UIDescriptionValueType::kFloatingPoint;
										subExpression2 = Expression::_Internal::CastExpression(subExpression2, UIDescriptionValueType::kFloatingPoint);
									}
									else
									{
										// Incompatible Type
										throw new UIDescriptionExpressionTypeErrorException();
									}
								}

								switch (primaryType)
								{
									case UIDescriptionValueType::kBoolean:
										return std::make_shared<OperationT<BooleanValueType> >(std::dynamic_pointer_cast<IExpression<ParameterT> >(parameterExpression), std::dynamic_pointer_cast<IExpression<BooleanValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<BooleanValueType> >(subExpression2));

									case UIDescriptionValueType::kInteger:
										return std::make_shared<OperationT<IntegerValueType> >(std::dynamic_pointer_cast<IExpression<ParameterT> >(parameterExpression), std::dynamic_pointer_cast<IExpression<IntegerValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<IntegerValueType> >(subExpression2));

									case UIDescriptionValueType::kFloatingPoint:
										return std::make_shared<OperationT<FloatingPointValueType> >(std::dynamic_pointer_cast<IExpression<ParameterT> >(parameterExpression), std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression2));

									case UIDescriptionValueType::kColor:
										return std::make_shared<OperationT<ColorARGBValueType> >(std::dynamic_pointer_cast<IExpression<ParameterT> >(parameterExpression), std::dynamic_pointer_cast<IExpression<ColorARGBValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<ColorARGBValueType> >(subExpression2));

									case UIDescriptionValueType::kVector2D:
										return std::make_shared<OperationT<Vector2DValueType> >(std::dynamic_pointer_cast<IExpression<ParameterT> >(parameterExpression), std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(subExpression2));

									case UIDescriptionValueType::kVector3D:
										return std::make_shared<OperationT<Vector3DValueType> >(std::dynamic_pointer_cast<IExpression<ParameterT> >(parameterExpression), std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(subExpression2));

									case UIDescriptionValueType::kString:
										return std::make_shared<OperationT<StringValueType> >(std::dynamic_pointer_cast<IExpression<ParameterT> >(parameterExpression), std::dynamic_pointer_cast<IExpression<StringValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<StringValueType> >(subExpression2));
								}

								// Invalid Type
								throw new UIDescriptionExpressionTypeErrorException();
							}

							//------------------------------------------------------------------------------
							std::shared_ptr<IArbitrarilyTypedExpression> ReadLerpExpressionFromElement(tinyxml2::XMLElement *elem, UIDescriptionValueType hintedType)
							{
								tinyxml2::XMLElement *currentChildElement = elem->FirstChildElement();

								std::shared_ptr<IArbitrarilyTypedExpression> parameterExpression;
								const char *parameterAttribute = elem->Attribute("parameter");
								if (parameterAttribute == nullptr)
								{
									if (currentChildElement == nullptr)
									{
										// No Parameter Expression
										return nullptr;
									}
									parameterExpression = ReadExpressionFromElement(currentChildElement, UIDescriptionValueType::kFloatingPoint);
									currentChildElement = currentChildElement->NextSiblingElement();
								}
								else
								{
									parameterExpression = ParseInlineExpression(parameterAttribute, UIDescriptionValueType::kFloatingPoint);
								}
								if (!parameterExpression)
								{
									// Invalid Parameter
									return nullptr;
								}
								if (parameterExpression->GetValueType() != UIDescriptionValueType::kFloatingPoint)
								{
									parameterExpression = Expression::_Internal::CastExpression(parameterExpression, UIDescriptionValueType::kFloatingPoint);
								}

								tinyxml2::XMLElement *expressionNode1 = currentChildElement;
								if (expressionNode1 == nullptr)
								{
									// No Expression
									return nullptr;
								}
								tinyxml2::XMLElement *expressionNode2 = expressionNode1->NextSiblingElement();
								if (expressionNode2 == nullptr)
								{
									// Not-Enough Expression
									return nullptr;
								}
							
								UIDescriptionValueType primaryType = UIDescriptionValueType::kINVALID;

								std::shared_ptr<IArbitrarilyTypedExpression> subExpression1;
								try
								{
									subExpression1 = ReadExpressionFromElement(expressionNode1);
								}
								catch (UIDescriptionExpressionTypeErrorException)
								{
								}

								if (subExpression1)
								{
									primaryType = subExpression1->GetValueType();
								}
								std::shared_ptr<IArbitrarilyTypedExpression> subExpression2;
								try
								{
									subExpression2 = ReadExpressionFromElement(expressionNode2, primaryType);
								}
								catch (UIDescriptionExpressionTypeErrorException)
								{
								}

								if (!subExpression1)
								{
									if (subExpression2)
									{
										primaryType = subExpression2->GetValueType();
										subExpression1 = ReadExpressionFromElement(expressionNode1, primaryType);
									}
									else
									{
										subExpression1 = ReadExpressionFromElement(expressionNode1, hintedType);
										subExpression2 = ReadExpressionFromElement(expressionNode2, hintedType);
									}
								}

								if (!subExpression1 || !subExpression2)
								{
									// Invalid Operands
									throw new UIDescriptionExpressionTypeErrorException();
								}

								if (subExpression1->GetValueType() == subExpression2->GetValueType())
								{
									primaryType = subExpression1->GetValueType();
								}
								else
								{
									if (subExpression1->GetValueType() == UIDescriptionValueType::kInteger && subExpression2->GetValueType() == UIDescriptionValueType::kFloatingPoint)
									{
										primaryType = UIDescriptionValueType::kFloatingPoint;
										subExpression1 = Expression::_Internal::CastExpression(subExpression1, UIDescriptionValueType::kFloatingPoint);
									}
									else if (subExpression1->GetValueType() == UIDescriptionValueType::kFloatingPoint && subExpression2->GetValueType() == UIDescriptionValueType::kInteger)
									{
										primaryType = UIDescriptionValueType::kFloatingPoint;
										subExpression2 = Expression::_Internal::CastExpression(subExpression2, UIDescriptionValueType::kFloatingPoint);
									}
									else
									{
										// Incompatible Type
										throw new UIDescriptionExpressionTypeErrorException();
									}
								}

								switch (primaryType)
								{
									case UIDescriptionValueType::kBoolean:
										return std::make_shared<Lerp<BooleanValueType> >(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(parameterExpression), std::dynamic_pointer_cast<IExpression<BooleanValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<BooleanValueType> >(subExpression2));

									case UIDescriptionValueType::kInteger:
										return std::make_shared<Lerp<IntegerValueType> >(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(parameterExpression), std::dynamic_pointer_cast<IExpression<IntegerValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<IntegerValueType> >(subExpression2));

									case UIDescriptionValueType::kFloatingPoint:
										return std::make_shared<Lerp<FloatingPointValueType> >(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(parameterExpression), std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression2));

									case UIDescriptionValueType::kColor:
										return std::make_shared<Lerp<ColorARGBValueType> >(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(parameterExpression), std::dynamic_pointer_cast<IExpression<ColorARGBValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<ColorARGBValueType> >(subExpression2));

									case UIDescriptionValueType::kVector2D:
										return std::make_shared<Lerp<Vector2DValueType> >(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(parameterExpression), std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(subExpression2));

									case UIDescriptionValueType::kVector3D:
										return std::make_shared<Lerp<Vector3DValueType> >(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(parameterExpression), std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(subExpression2));
								}

								// Invalid Type
								throw new UIDescriptionExpressionTypeErrorException();
							}

							//------------------------------------------------------------------------------
							std::shared_ptr<IExpression<BooleanValueType> > ReadApproxCompareExpressionFromElement(tinyxml2::XMLElement *elem)
							{
								tinyxml2::XMLElement *currentChildElement = elem->FirstChildElement();

								std::shared_ptr<IArbitrarilyTypedExpression> errorLimitExpression;
								const char *errorAttribute = elem->Attribute("error");
								if (errorAttribute == nullptr)
								{
									if (currentChildElement != nullptr && currentChildElement->NextSiblingElement() != nullptr && currentChildElement->NextSiblingElement()->NextSiblingElement() != nullptr)
									{
										errorLimitExpression = ReadExpressionFromElement(currentChildElement, UIDescriptionValueType::kFloatingPoint);
										currentChildElement = currentChildElement->NextSiblingElement();
									}
									else
									{
										// Default Limit
										constexpr FloatingPointValueType kDefaultErrorLimit = static_cast<FloatingPointValueType>(0.000001f);
										errorLimitExpression = std::make_shared<Constant<FloatingPointValueType> >(kDefaultErrorLimit);
									}
								}
								else
								{
									errorLimitExpression = ParseInlineExpression(errorAttribute, UIDescriptionValueType::kFloatingPoint);
								}
								if (!errorLimitExpression)
								{
									// Invalid Limit
									return nullptr;
								}

								tinyxml2::XMLElement *expressionNode1 = currentChildElement;
								if (expressionNode1 == nullptr)
								{
									// No Expression
									return nullptr;
								}
								tinyxml2::XMLElement *expressionNode2 = expressionNode1->NextSiblingElement();
								if (expressionNode2 == nullptr)
								{
									// Not-Enough Expression
									return nullptr;
								}

								UIDescriptionValueType primaryType = UIDescriptionValueType::kINVALID;

								std::shared_ptr<IArbitrarilyTypedExpression> subExpression1;
								try
								{
									subExpression1 = ReadExpressionFromElement(expressionNode1);
								}
								catch (UIDescriptionExpressionTypeErrorException)
								{
								}

								if (subExpression1)
								{
									primaryType = subExpression1->GetValueType();
								}
								std::shared_ptr<IArbitrarilyTypedExpression> subExpression2;
								try
								{
									subExpression2 = ReadExpressionFromElement(expressionNode2, primaryType);
								}
								catch (UIDescriptionExpressionTypeErrorException)
								{
								}

								if (!subExpression1)
								{
									if (subExpression2)
									{
										primaryType = subExpression2->GetValueType();
										subExpression1 = ReadExpressionFromElement(expressionNode1, primaryType);
									}
								}

								if (!subExpression1 || !subExpression2)
								{
									// Invalid Operands
									throw new UIDescriptionExpressionTypeErrorException();
								}

								if (subExpression1->GetValueType() == UIDescriptionValueType::kInteger)
								{
									subExpression1 = Expression::_Internal::CastExpression(subExpression1, UIDescriptionValueType::kFloatingPoint);
								}
								if (subExpression2->GetValueType() == UIDescriptionValueType::kInteger)
								{
									subExpression2 = Expression::_Internal::CastExpression(subExpression2, UIDescriptionValueType::kFloatingPoint);
								}

								if (subExpression1->GetValueType() == subExpression2->GetValueType())
								{
									primaryType = subExpression1->GetValueType();
								}
								else
								{
									// Incompatible Type
									throw new UIDescriptionExpressionTypeErrorException();
								}

								switch (primaryType)
								{
									case UIDescriptionValueType::kFloatingPoint:
										{
											if (errorLimitExpression->GetValueType() != UIDescriptionValueType::kFloatingPoint)
											{
												throw new UIDescriptionExpressionTypeErrorException();
											}
											return std::make_shared<IsApproxEqual<FloatingPointValueType> >(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(errorLimitExpression),
												std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression2));
										}

									case UIDescriptionValueType::kVector2D:
										{
											if (errorLimitExpression->GetValueType() == UIDescriptionValueType::kFloatingPoint)
											{
												return std::make_shared<IsApproxFloatEqual<Vector2DValueType> >(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(errorLimitExpression),
													std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(subExpression2));
											}
											else if (errorLimitExpression->GetValueType() == UIDescriptionValueType::kVector2D)
											{
												return std::make_shared<IsApproxEqual<Vector2DValueType> >(std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(errorLimitExpression),
													std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(subExpression2));
											}
										}
										break;
								
									case UIDescriptionValueType::kVector3D:
										{
											if (errorLimitExpression->GetValueType() == UIDescriptionValueType::kFloatingPoint)
											{
												return std::make_shared<IsApproxFloatEqual<Vector3DValueType> >(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(errorLimitExpression),
													std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(subExpression2));
											}
											else if (errorLimitExpression->GetValueType() == UIDescriptionValueType::kVector2D)
											{
												return std::make_shared<IsApproxEqual<Vector3DValueType> >(std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(errorLimitExpression),
													std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(subExpression2));
											}
										}
										break;
								}

								// Invalid Type
								throw new UIDescriptionExpressionTypeErrorException();
							}

							//------------------------------------------------------------------------------
							std::shared_ptr<IArbitrarilyTypedExpression> ReadIfExpressionFromElement(tinyxml2::XMLElement *elem, UIDescriptionValueType hintedType)
							{
								tinyxml2::XMLElement *currentChildElement = elem->FirstChildElement();

								std::shared_ptr<IArbitrarilyTypedExpression> conditionExpression;
								const char *conditionAttribute = elem->Attribute("condition");
								if (conditionAttribute == nullptr)
								{
									if (currentChildElement == nullptr)
									{
										// No Condition Expression
										return nullptr;
									}
									conditionExpression = ReadExpressionFromElement(currentChildElement, UIDescriptionValueType::kBoolean);
									currentChildElement = currentChildElement->NextSiblingElement();
								}
								else
								{
									conditionExpression = ParseInlineExpression(conditionAttribute, UIDescriptionValueType::kBoolean);
								}
								if (!conditionExpression)
								{
									// Invalid Parameter
									return nullptr;
								}
								if (conditionExpression->GetValueType() != UIDescriptionValueType::kBoolean)
								{
									conditionExpression = Expression::_Internal::CastExpression(conditionExpression, UIDescriptionValueType::kBoolean);
								}

								tinyxml2::XMLElement *expressionNode1 = currentChildElement;
								if (expressionNode1 == nullptr)
								{
									// No Expression
									return nullptr;
								}
								tinyxml2::XMLElement *expressionNode2 = expressionNode1->NextSiblingElement();
								if (expressionNode2 == nullptr)
								{
									// Not-Enough Expression
									return nullptr;
								}

								UIDescriptionValueType primaryType = UIDescriptionValueType::kINVALID;

								std::shared_ptr<IArbitrarilyTypedExpression> subExpression1;
								try
								{
									subExpression1 = ReadExpressionFromElement(expressionNode1);
								}
								catch (UIDescriptionExpressionTypeErrorException)
								{
								}

								if (subExpression1)
								{
									primaryType = subExpression1->GetValueType();
								}
								std::shared_ptr<IArbitrarilyTypedExpression> subExpression2;
								try
								{
									subExpression2 = ReadExpressionFromElement(expressionNode2, primaryType);
								}
								catch (UIDescriptionExpressionTypeErrorException)
								{
								}

								if (!subExpression1)
								{
									if (subExpression2)
									{
										primaryType = subExpression2->GetValueType();
										subExpression1 = ReadExpressionFromElement(expressionNode1, primaryType);
									}
									else
									{
										subExpression1 = ReadExpressionFromElement(expressionNode1, hintedType);
										subExpression2 = ReadExpressionFromElement(expressionNode2, hintedType);
									}
								}

								if (!subExpression1 || !subExpression2)
								{
									// Invalid Operands
									throw new UIDescriptionExpressionTypeErrorException();
								}

								if (subExpression1->GetValueType() == subExpression2->GetValueType())
								{
									primaryType = subExpression1->GetValueType();
								}
								else
								{
									if (subExpression1->GetValueType() == UIDescriptionValueType::kInteger && subExpression2->GetValueType() == UIDescriptionValueType::kFloatingPoint)
									{
										primaryType = UIDescriptionValueType::kFloatingPoint;
										subExpression1 = Expression::_Internal::CastExpression(subExpression1, UIDescriptionValueType::kFloatingPoint);
									}
									else if (subExpression1->GetValueType() == UIDescriptionValueType::kFloatingPoint && subExpression2->GetValueType() == UIDescriptionValueType::kInteger)
									{
										primaryType = UIDescriptionValueType::kFloatingPoint;
										subExpression2 = Expression::_Internal::CastExpression(subExpression2, UIDescriptionValueType::kFloatingPoint);
									}
									else
									{
										// Incompatible Type
										throw new UIDescriptionExpressionTypeErrorException();
									}
								}

								switch (primaryType)
								{
									case UIDescriptionValueType::kBoolean:
										return std::make_shared<Iff<BooleanValueType> >(std::dynamic_pointer_cast<IExpression<BooleanValueType> >(conditionExpression), std::dynamic_pointer_cast<IExpression<BooleanValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<BooleanValueType> >(subExpression2));

									case UIDescriptionValueType::kInteger:
										return std::make_shared<Iff<IntegerValueType> >(std::dynamic_pointer_cast<IExpression<BooleanValueType> >(conditionExpression), std::dynamic_pointer_cast<IExpression<IntegerValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<IntegerValueType> >(subExpression2));

									case UIDescriptionValueType::kFloatingPoint:
										return std::make_shared<Iff<FloatingPointValueType> >(std::dynamic_pointer_cast<IExpression<BooleanValueType> >(conditionExpression), std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression2));

									case UIDescriptionValueType::kColor:
										return std::make_shared<Iff<ColorARGBValueType> >(std::dynamic_pointer_cast<IExpression<BooleanValueType> >(conditionExpression), std::dynamic_pointer_cast<IExpression<ColorARGBValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<ColorARGBValueType> >(subExpression2));

									case UIDescriptionValueType::kVector2D:
										return std::make_shared<Iff<Vector2DValueType> >(std::dynamic_pointer_cast<IExpression<BooleanValueType> >(conditionExpression), std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(subExpression2));

									case UIDescriptionValueType::kVector3D:
										return std::make_shared<Iff<Vector3DValueType> >(std::dynamic_pointer_cast<IExpression<BooleanValueType> >(conditionExpression), std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(subExpression2));

									case UIDescriptionValueType::kString:
										return std::make_shared<Iff<StringValueType> >(std::dynamic_pointer_cast<IExpression<BooleanValueType> >(conditionExpression), std::dynamic_pointer_cast<IExpression<StringValueType> >(subExpression1), std::dynamic_pointer_cast<IExpression<StringValueType> >(subExpression2));
								}

								// Invalid Type
								throw new UIDescriptionExpressionTypeErrorException();
							}

							//------------------------------------------------------------------------------
							std::shared_ptr<IArbitrarilyTypedExpression> ReadAbsExpressionFromElement(tinyxml2::XMLElement *elem, UIDescriptionValueType hintedType)
							{
								tinyxml2::XMLElement *expressionNode = elem->FirstChildElement();
								if (expressionNode == nullptr)
								{
									// No Expression
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> subExpression(ReadExpressionFromElement(expressionNode, hintedType));
								if (!subExpression)
								{
									// Invalid Expression
									return nullptr;
								}

								switch (subExpression->GetValueType())
								{
									case UIDescriptionValueType::kInteger:
										{
											if (hintedType == UIDescriptionValueType::kFloatingPoint)
											{
												return Expression::_Internal::CastExpression(std::make_shared<Abs<IntegerValueType, IntegerValueType> >(std::dynamic_pointer_cast<IExpression<IntegerValueType> >(subExpression)), UIDescriptionValueType::kFloatingPoint);;
											}
											return std::make_shared<Abs<IntegerValueType, IntegerValueType> >(std::dynamic_pointer_cast<IExpression<IntegerValueType> >(subExpression));
										}

									case UIDescriptionValueType::kFloatingPoint:
										return std::make_shared<Abs<FloatingPointValueType, FloatingPointValueType> >(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression));

									case UIDescriptionValueType::kVector2D:
										return std::make_shared<Abs<Vector2DValueType, FloatingPointValueType> >(std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(subExpression));

									case UIDescriptionValueType::kVector3D:
										return std::make_shared<Abs<Vector3DValueType, FloatingPointValueType> >(std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(subExpression));
								}

								// Invalid Type
								throw new UIDescriptionExpressionTypeErrorException();
							}

							//------------------------------------------------------------------------------
							std::shared_ptr<IExpression<FloatingPointValueType> > ReadFloatingPointFunctionExpressionFromElement(tinyxml2::XMLElement *elem, std::function<FloatingPointValueType(FloatingPointValueType)> applyingFunction)
							{
								tinyxml2::XMLElement *expressionNode = elem->FirstChildElement();
								if (expressionNode == nullptr)
								{
									// No Expression
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> subExpression(ReadExpressionFromElement(expressionNode, UIDescriptionValueType::kFloatingPoint));
								if (!subExpression)
								{
									// Invalid Expression
									return nullptr;
								}
								if (subExpression->GetValueType() != UIDescriptionValueType::kFloatingPoint)
								{
									subExpression = Expression::_Internal::CastExpression(subExpression, UIDescriptionValueType::kFloatingPoint);
								}

								return std::make_shared<AuxFloatingPointFunction>(applyingFunction, std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression));
							}

							//------------------------------------------------------------------------------
							std::shared_ptr<IExpression<FloatingPointValueType> > ReadPowerExpressionFromElement(tinyxml2::XMLElement *elem)
							{
								std::shared_ptr<IArbitrarilyTypedExpression> exponentExpression;
								const char *exponentAttribute = elem->Attribute("exponent");
								if (exponentAttribute == nullptr)
								{
									if (elem->FirstChildElement() == nullptr || elem->FirstChildElement()->NextSiblingElement() == nullptr)
									{
										// No Condition Expression
										return nullptr;
									}
									exponentExpression = ReadExpressionFromElement(elem->FirstChildElement()->NextSiblingElement(), UIDescriptionValueType::kFloatingPoint);
								}
								else
								{
									exponentExpression = ParseInlineExpression(exponentAttribute, UIDescriptionValueType::kFloatingPoint);
								}
								if (!exponentExpression)
								{
									// Invalid Exponent
									return nullptr;
								}

								tinyxml2::XMLElement *expressionNode = elem->FirstChildElement();
								if (expressionNode == nullptr)
								{
									// No Expression
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> subExpression(ReadExpressionFromElement(expressionNode, UIDescriptionValueType::kFloatingPoint));
								if (!subExpression)
								{
									// Invalid Expression
									return nullptr;
								}

								if (exponentExpression->GetValueType() == UIDescriptionValueType::kInteger && subExpression->GetValueType() == UIDescriptionValueType::kInteger)
								{
									return std::make_shared<Power<IntegerValueType> >(std::dynamic_pointer_cast<IExpression<IntegerValueType> >(subExpression), std::dynamic_pointer_cast<IExpression<IntegerValueType> >(exponentExpression));
								}
							
								if (subExpression->GetValueType() != UIDescriptionValueType::kFloatingPoint)
								{
									subExpression = Expression::_Internal::CastExpression(subExpression, UIDescriptionValueType::kFloatingPoint);
								}
								if (exponentExpression->GetValueType() != UIDescriptionValueType::kFloatingPoint)
								{
									exponentExpression = Expression::_Internal::CastExpression(exponentExpression, UIDescriptionValueType::kFloatingPoint);
								}
								return std::make_shared<Power<FloatingPointValueType> >(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression), std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(exponentExpression));
							}

							//------------------------------------------------------------------------------
							std::shared_ptr<IExpression<FloatingPointValueType> > ReadLogExpressionFromElement(tinyxml2::XMLElement *elem)
							{
								const char *baseAttribute = elem->Attribute("base");
								if (baseAttribute == nullptr)
								{
									// ln for Default
									return _Internal::ReadFloatingPointFunctionExpressionFromElement(elem, [](FloatingPointValueType x) { return static_cast<FloatingPointValueType>(log(x)); });
								}

								UIDescriptionValueType baseType = DetectTypeFromString(baseAttribute);
								if (baseType == UIDescriptionValueType::kInteger)
								{
									IntegerValueType baseValue = ParseIntegerValueFromString(baseAttribute);
									if (baseValue == 10)
									{
										return _Internal::ReadFloatingPointFunctionExpressionFromElement(elem, [](FloatingPointValueType x) { return static_cast<FloatingPointValueType>(log10(x)); });
									}
									else if (baseValue == 2)
									{
										return _Internal::ReadFloatingPointFunctionExpressionFromElement(elem, [](FloatingPointValueType x) { return static_cast<FloatingPointValueType>(std::log2(x)); });
									}

									return _Internal::ReadFloatingPointFunctionExpressionFromElement(elem, [baseValue](FloatingPointValueType x) { return static_cast<FloatingPointValueType>(log(x) / log((float)baseValue)); });
								}

								FloatingPointValueType baseFloatValue = ParseFloatingPointValueFromString(baseAttribute);
								return _Internal::ReadFloatingPointFunctionExpressionFromElement(elem, [baseFloatValue](FloatingPointValueType x) { return static_cast<FloatingPointValueType>(log(x) / log(baseFloatValue)); });
							}

							//------------------------------------------------------------------------------
							template <class OperationT>
							std::shared_ptr<IExpression<IntegerValueType> > ReadToIntegerExpressionFromElement(tinyxml2::XMLElement *elem)
							{
								tinyxml2::XMLElement *expressionNode = elem->FirstChildElement();
								if (expressionNode == nullptr)
								{
									// No Expression
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> subExpression(ReadExpressionFromElement(expressionNode));
								if (!subExpression)
								{
									// Invalid Expression
									return nullptr;
								}

								switch (subExpression->GetValueType())
								{
									case UIDescriptionValueType::kInteger: return std::dynamic_pointer_cast<IExpression<IntegerValueType> >(subExpression);
									case UIDescriptionValueType::kFloatingPoint: return std::make_shared<OperationT>(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression));
								}

								// Not-Minusable Type
								throw new UIDescriptionExpressionTypeErrorException();
							}

							//------------------------------------------------------------------------------
							std::shared_ptr<IArbitrarilyTypedExpression> ReadClampExpressionFromElement(tinyxml2::XMLElement *elem, UIDescriptionValueType hintedType)
							{
								tinyxml2::XMLElement *currentChildElement = elem->FirstChildElement();

								std::shared_ptr<IArbitrarilyTypedExpression> lowClampExpression;
								const char *lowClampAttribute = elem->Attribute("low");
								if (lowClampAttribute == nullptr)
								{
									if (currentChildElement == nullptr)
									{
										// No Clamp Expression
										return nullptr;
									}
									lowClampExpression = ReadExpressionFromElement(currentChildElement, hintedType);
									currentChildElement = currentChildElement->NextSiblingElement();
								}
								else
								{
									lowClampExpression = ParseInlineExpression(lowClampAttribute, hintedType);
								}
								if (!lowClampExpression)
								{
									// Invalid Parameter
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> highClampExpression;
								const char *highClampAttribute = elem->Attribute("high");
								if (highClampAttribute == nullptr)
								{
									if (currentChildElement == nullptr)
									{
										// No Clamp Expression
										return nullptr;
									}
									highClampExpression = ReadExpressionFromElement(currentChildElement, hintedType);
									currentChildElement = currentChildElement->NextSiblingElement();
								}
								else
								{
									highClampExpression = ParseInlineExpression(highClampAttribute, hintedType);
								}
								if (!highClampExpression)
								{
									// Invalid Parameter
									return nullptr;
								}

								tinyxml2::XMLElement *expressionNode = currentChildElement;
								if (expressionNode == nullptr)
								{
									// No Expression
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> subExpression(ReadExpressionFromElement(expressionNode, hintedType));
								if (!subExpression)
								{
									// Invalid Expression
									return nullptr;
								}

								if (lowClampExpression->GetValueType() == UIDescriptionValueType::kFloatingPoint || highClampExpression->GetValueType() == UIDescriptionValueType::kFloatingPoint || subExpression->GetValueType() == UIDescriptionValueType::kFloatingPoint)
								{
									if (lowClampExpression->GetValueType() == UIDescriptionValueType::kInteger)
									{
										lowClampExpression = Expression::_Internal::CastExpression(lowClampExpression, UIDescriptionValueType::kFloatingPoint);
									}
									if (subExpression->GetValueType() == UIDescriptionValueType::kInteger)
									{
										subExpression = Expression::_Internal::CastExpression(subExpression, UIDescriptionValueType::kFloatingPoint);
									}
									if (highClampExpression->GetValueType() == UIDescriptionValueType::kInteger)
									{
										highClampExpression = Expression::_Internal::CastExpression(highClampExpression, UIDescriptionValueType::kFloatingPoint);
									}
								}

								switch (subExpression->GetValueType())
								{
									case UIDescriptionValueType::kInteger:
										return std::make_shared<Clamp<IntegerValueType> >(std::dynamic_pointer_cast<IExpression<IntegerValueType> >(subExpression), std::dynamic_pointer_cast<IExpression<IntegerValueType> >(lowClampExpression), std::dynamic_pointer_cast<IExpression<IntegerValueType> >(highClampExpression));

									case UIDescriptionValueType::kFloatingPoint:
										return std::make_shared<Clamp<FloatingPointValueType> >(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression), std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(lowClampExpression), std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(highClampExpression));

									case UIDescriptionValueType::kString:
										return std::make_shared<Clamp<StringValueType> >(std::dynamic_pointer_cast<IExpression<StringValueType> >(subExpression), std::dynamic_pointer_cast<IExpression<StringValueType> >(lowClampExpression), std::dynamic_pointer_cast<IExpression<StringValueType> >(highClampExpression));
								}

								// Invalid Type
								throw new UIDescriptionExpressionTypeErrorException();
							}

							//------------------------------------------------------------------------------
							std::shared_ptr<IArbitrarilyTypedExpression> ReadClampHighExpressionFromElement(tinyxml2::XMLElement *elem, UIDescriptionValueType hintedType)
							{
								tinyxml2::XMLElement *currentChildElement = elem->FirstChildElement();

								std::shared_ptr<IArbitrarilyTypedExpression> highClampExpression;
								const char *highClampAttribute = elem->Attribute("high");
								if (highClampAttribute == nullptr)
								{
									if (currentChildElement == nullptr)
									{
										// No Clamp Expression
										return nullptr;
									}
									highClampExpression = ReadExpressionFromElement(currentChildElement, hintedType);
									currentChildElement = currentChildElement->NextSiblingElement();
								}
								else
								{
									highClampExpression = ParseInlineExpression(highClampAttribute, hintedType);
								}
								if (!highClampExpression)
								{
									// Invalid Parameter
									return nullptr;
								}

								tinyxml2::XMLElement *expressionNode = currentChildElement;
								if (expressionNode == nullptr)
								{
									// No Expression
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> subExpression(ReadExpressionFromElement(expressionNode, hintedType));
								if (!subExpression)
								{
									// Invalid Expression
									return nullptr;
								}

								if (highClampExpression->GetValueType() == UIDescriptionValueType::kFloatingPoint || subExpression->GetValueType() == UIDescriptionValueType::kFloatingPoint)
								{
									if (subExpression->GetValueType() == UIDescriptionValueType::kInteger)
									{
										subExpression = Expression::_Internal::CastExpression(subExpression, UIDescriptionValueType::kFloatingPoint);
									}
									if (highClampExpression->GetValueType() == UIDescriptionValueType::kInteger)
									{
										highClampExpression = Expression::_Internal::CastExpression(highClampExpression, UIDescriptionValueType::kFloatingPoint);
									}
								}

								switch (subExpression->GetValueType())
								{
									case UIDescriptionValueType::kInteger:
										return std::make_shared<ClampHigh<IntegerValueType> >(std::dynamic_pointer_cast<IExpression<IntegerValueType> >(subExpression), std::dynamic_pointer_cast<IExpression<IntegerValueType> >(highClampExpression));

									case UIDescriptionValueType::kFloatingPoint:
										return std::make_shared<ClampHigh<FloatingPointValueType> >(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression), std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(highClampExpression));

									case UIDescriptionValueType::kString:
										return std::make_shared<ClampHigh<StringValueType> >(std::dynamic_pointer_cast<IExpression<StringValueType> >(subExpression), std::dynamic_pointer_cast<IExpression<StringValueType> >(highClampExpression));
								}

								// Invalid Type
								throw new UIDescriptionExpressionTypeErrorException();
							}

							//------------------------------------------------------------------------------
							std::shared_ptr<IArbitrarilyTypedExpression> ReadClampLowExpressionFromElement(tinyxml2::XMLElement *elem, UIDescriptionValueType hintedType)
							{
								tinyxml2::XMLElement *currentChildElement = elem->FirstChildElement();

								std::shared_ptr<IArbitrarilyTypedExpression> lowClampExpression;
								const char *lowClampAttribute = elem->Attribute("low");
								if (lowClampAttribute == nullptr)
								{
									if (currentChildElement == nullptr)
									{
										// No Clamp Expression
										return nullptr;
									}
									lowClampExpression = ReadExpressionFromElement(currentChildElement, hintedType);
									currentChildElement = currentChildElement->NextSiblingElement();
								}
								else
								{
									lowClampExpression = ParseInlineExpression(lowClampAttribute, hintedType);
								}
								if (!lowClampExpression)
								{
									// Invalid Parameter
									return nullptr;
								}
							
								tinyxml2::XMLElement *expressionNode = currentChildElement;
								if (expressionNode == nullptr)
								{
									// No Expression
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> subExpression(ReadExpressionFromElement(expressionNode, hintedType));
								if (!subExpression)
								{
									// Invalid Expression
									return nullptr;
								}

								if (lowClampExpression->GetValueType() == UIDescriptionValueType::kFloatingPoint || subExpression->GetValueType() == UIDescriptionValueType::kFloatingPoint)
								{
									if (lowClampExpression->GetValueType() == UIDescriptionValueType::kInteger)
									{
										lowClampExpression = Expression::_Internal::CastExpression(lowClampExpression, UIDescriptionValueType::kFloatingPoint);
									}
									if (subExpression->GetValueType() == UIDescriptionValueType::kInteger)
									{
										subExpression = Expression::_Internal::CastExpression(subExpression, UIDescriptionValueType::kFloatingPoint);
									}
								}

								switch (subExpression->GetValueType())
								{
									case UIDescriptionValueType::kInteger:
										return std::make_shared<ClampLow<IntegerValueType> >(std::dynamic_pointer_cast<IExpression<IntegerValueType> >(subExpression), std::dynamic_pointer_cast<IExpression<IntegerValueType> >(lowClampExpression));

									case UIDescriptionValueType::kFloatingPoint:
										return std::make_shared<ClampLow<FloatingPointValueType> >(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(subExpression), std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(lowClampExpression));

									case UIDescriptionValueType::kString:
										return std::make_shared<ClampLow<StringValueType> >(std::dynamic_pointer_cast<IExpression<StringValueType> >(subExpression), std::dynamic_pointer_cast<IExpression<StringValueType> >(lowClampExpression));
								}

								// Invalid Type
								throw new UIDescriptionExpressionTypeErrorException();
							}

							//------------------------------------------------------------------------------
							template <template<typename ValueT> class ExpressionT>
							std::shared_ptr<IArbitrarilyTypedExpression> ReadGetTargetNumericValueExpressionFromElement(tinyxml2::XMLElement *elem, UIDescriptionValueType hintedType = UIDescriptionValueType::kINVALID)
							{
								UIDescriptionValueType valueType = UIDescriptionValueType::kINVALID;

								const char *typeAttribute = elem->Attribute("type");
								if (typeAttribute != nullptr)
								{
									valueType = Expression::ParseTypeFromString(typeAttribute);
									if (valueType == UIDescriptionValueType::kINVALID)
									{
										// Invalid Type
										return nullptr;
									}
								}
								if (valueType == UIDescriptionValueType::kINVALID)
								{
									if (hintedType == UIDescriptionValueType::kINVALID)
									{
										// Default Float
										valueType = UIDescriptionValueType::kFloatingPoint;
									}
									else
									{
										valueType = hintedType;
									}
								}

								const char *referencedTarget = elem->Attribute("target");
								if (referencedTarget == nullptr)
								{
									return nullptr;
								}
								VariablePathType targetPath = TypeUtility::ParseVariablePath(referencedTarget);

								// Typed Variables
								switch (valueType)
								{
									case UIDescriptionValueType::kInteger:
										return std::make_shared<ExpressionT<IntegerValueType> >(targetPath);

									case UIDescriptionValueType::kFloatingPoint:
										return std::make_shared<ExpressionT<FloatingPointValueType> >(targetPath);
								}

								// Invalid Type
								throw new UIDescriptionExpressionTypeErrorException();
							}

							//------------------------------------------------------------------------------
							std::shared_ptr<IExpression<ColorARGBValueType> > ReadConstructColorARGBExpressionFromElement(tinyxml2::XMLElement *elem)
							{
								tinyxml2::XMLElement *childrenComponents[4];

								size_t childrenCount = 0;
								tinyxml2::XMLElement *currentChildElement = elem->FirstChildElement();
								while (currentChildElement != nullptr)
								{
									childrenComponents[childrenCount] = currentChildElement;
									++childrenCount;

									if (childrenCount >= 4)
									{
										break;
									}

									currentChildElement = currentChildElement->NextSiblingElement();
								}

								bool isFloatMode = false;
								const char *isFloatAttribute = elem->Attribute("float");
								if (isFloatAttribute != nullptr)
								{
									isFloatMode = ParseBooleanValueFromString(isFloatAttribute);
								}

								bool isConstantAlpha = false;
								std::shared_ptr<IArbitrarilyTypedExpression> alphaExpression;
								const char *alphaAttribute = elem->Attribute("alpha");
								if (alphaAttribute == nullptr)
								{
									alphaAttribute = elem->Attribute("a");
								}
								if (alphaAttribute == nullptr)
								{
									if (childrenCount >= 4)
									{
										alphaExpression = ReadExpressionFromElement(childrenComponents[0], (isFloatMode ? UIDescriptionValueType::kFloatingPoint : UIDescriptionValueType::kInteger));
									}
									else
									{
										alphaExpression = std::make_shared<Constant<IntegerValueType> >(255);
										isConstantAlpha = true;
									}
								}
								else
								{
									alphaExpression = ParseInlineExpression(alphaAttribute, (isFloatMode ? UIDescriptionValueType::kFloatingPoint : UIDescriptionValueType::kInteger));
								}
								if (!alphaExpression)
								{
									// Invalid Expression Parameter
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> redExpression;
								const char *redAttribute = elem->Attribute("red");
								if (redAttribute == nullptr)
								{
									redAttribute = elem->Attribute("r");
								}
								if (redAttribute == nullptr)
								{
									if (childrenCount >= 3)
									{
										redExpression = ReadExpressionFromElement(childrenComponents[(childrenCount == 3) ? 0 : 1], (isFloatMode ? UIDescriptionValueType::kFloatingPoint : UIDescriptionValueType::kInteger));
									}
								}
								else
								{
									redExpression = ParseInlineExpression(redAttribute, (isFloatMode ? UIDescriptionValueType::kFloatingPoint : UIDescriptionValueType::kInteger));
								}
								if (!redExpression)
								{
									// Invalid Expression Parameter
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> greenExpression;
								const char *greenAttribute = elem->Attribute("green");
								if (greenAttribute == nullptr)
								{
									greenAttribute = elem->Attribute("g");
								}
								if (greenAttribute == nullptr)
								{
									if (childrenCount >= 3)
									{
										greenExpression = ReadExpressionFromElement(childrenComponents[(childrenCount == 3) ? 1 : 2], (isFloatMode ? UIDescriptionValueType::kFloatingPoint : UIDescriptionValueType::kInteger));
									}
								}
								else
								{
									greenExpression = ParseInlineExpression(greenAttribute, (isFloatMode ? UIDescriptionValueType::kFloatingPoint : UIDescriptionValueType::kInteger));
								}
								if (!greenExpression)
								{
									// Invalid Expression Parameter
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> blueExpression;
								const char *blueAttribute = elem->Attribute("blue");
								if (blueAttribute == nullptr)
								{
									blueAttribute = elem->Attribute("b");
								}
								if (blueAttribute == nullptr)
								{
									if (childrenCount >= 3)
									{
										blueExpression = ReadExpressionFromElement(childrenComponents[(childrenCount == 3) ? 2 : 3], (isFloatMode ? UIDescriptionValueType::kFloatingPoint : UIDescriptionValueType::kInteger));
									}
								}
								else
								{
									blueExpression = ParseInlineExpression(blueAttribute, (isFloatMode ? UIDescriptionValueType::kFloatingPoint : UIDescriptionValueType::kInteger));
								}
								if (!blueExpression)
								{
									// Invalid Expression Parameter
									return nullptr;
								}

								if (isFloatMode)
								{
									if (!isConstantAlpha)
									{
										if (alphaExpression->GetValueType() != UIDescriptionValueType::kFloatingPoint)
										{
											alphaExpression = Expression::_Internal::CastExpression(alphaExpression, UIDescriptionValueType::kFloatingPoint);
										}
										alphaExpression = Expression::_Internal::CastExpression(std::make_shared<Multiply<FloatingPointValueType> >(std::vector<std::shared_ptr<IExpression<FloatingPointValueType> > >({ std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(alphaExpression), std::make_shared<Constant<FloatingPointValueType> >(255.0f) })), UIDescriptionValueType::kInteger);
									}

									if (redExpression->GetValueType() != UIDescriptionValueType::kFloatingPoint)
									{
										redExpression = Expression::_Internal::CastExpression(redExpression, UIDescriptionValueType::kFloatingPoint);
									}
									redExpression = Expression::_Internal::CastExpression(std::make_shared<Multiply<FloatingPointValueType> >(std::vector<std::shared_ptr<IExpression<FloatingPointValueType> > >({ std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(redExpression), std::make_shared<Constant<FloatingPointValueType> >(255.0f) })), UIDescriptionValueType::kInteger);

									if (greenExpression->GetValueType() != UIDescriptionValueType::kFloatingPoint)
									{
										greenExpression = Expression::_Internal::CastExpression(greenExpression, UIDescriptionValueType::kFloatingPoint);
									}
									greenExpression = Expression::_Internal::CastExpression(std::make_shared<Multiply<FloatingPointValueType> >(std::vector<std::shared_ptr<IExpression<FloatingPointValueType> > >({ std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(greenExpression), std::make_shared<Constant<FloatingPointValueType> >(255.0f) })), UIDescriptionValueType::kInteger);

									if (blueExpression->GetValueType() != UIDescriptionValueType::kFloatingPoint)
									{
										blueExpression = Expression::_Internal::CastExpression(blueExpression, UIDescriptionValueType::kFloatingPoint);
									}
									blueExpression = Expression::_Internal::CastExpression(std::make_shared<Multiply<FloatingPointValueType> >(std::vector<std::shared_ptr<IExpression<FloatingPointValueType> > >({ std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(blueExpression), std::make_shared<Constant<FloatingPointValueType> >(255.0f) })), UIDescriptionValueType::kInteger);
								}

								if (alphaExpression->GetValueType() != UIDescriptionValueType::kInteger)
								{
									alphaExpression = Expression::_Internal::CastExpression(alphaExpression, UIDescriptionValueType::kInteger);
								}
								if (redExpression->GetValueType() != UIDescriptionValueType::kInteger)
								{
									redExpression = Expression::_Internal::CastExpression(redExpression, UIDescriptionValueType::kInteger);
								}
								if (greenExpression->GetValueType() != UIDescriptionValueType::kInteger)
								{
									greenExpression = Expression::_Internal::CastExpression(greenExpression, UIDescriptionValueType::kInteger);
								}
								if (blueExpression->GetValueType() != UIDescriptionValueType::kInteger)
								{
									blueExpression = Expression::_Internal::CastExpression(blueExpression, UIDescriptionValueType::kInteger);
								}

								return std::make_shared<ConstructColorARGB>(
									std::dynamic_pointer_cast<IExpression<IntegerValueType> >(alphaExpression),
									std::dynamic_pointer_cast<IExpression<IntegerValueType> >(redExpression),
									std::dynamic_pointer_cast<IExpression<IntegerValueType> >(greenExpression),
									std::dynamic_pointer_cast<IExpression<IntegerValueType> >(blueExpression));
							}

							//------------------------------------------------------------------------------
							std::shared_ptr<IExpression<ColorARGBValueType> > ReadConstructColorAHSLExpressionFromElement(tinyxml2::XMLElement *elem)
							{
								tinyxml2::XMLElement *childrenComponents[4];

								size_t childrenCount = 0;
								tinyxml2::XMLElement *currentChildElement = elem->FirstChildElement();
								while (currentChildElement != nullptr)
								{
									childrenComponents[childrenCount] = currentChildElement;
									++childrenCount;

									if (childrenCount >= 4)
									{
										break;
									}

									currentChildElement = currentChildElement->NextSiblingElement();
								}

								bool isDegreeHue = false;
								const char *isDegreeAttribute = elem->Attribute("degree");
								if (isDegreeAttribute != nullptr)
								{
									isDegreeHue = ParseBooleanValueFromString(isDegreeAttribute);
								}

								std::shared_ptr<IArbitrarilyTypedExpression> alphaExpression;
								const char *alphaAttribute = elem->Attribute("alpha");
								if (alphaAttribute == nullptr)
								{
									alphaAttribute = elem->Attribute("a");
								}
								if (alphaAttribute == nullptr)
								{
									if (childrenCount >= 4)
									{
										alphaExpression = ReadExpressionFromElement(childrenComponents[0], UIDescriptionValueType::kFloatingPoint);
									}
									else
									{
										alphaExpression = std::make_shared<Constant<FloatingPointValueType> >(1.0f);
									}
								}
								else
								{
									alphaExpression = ParseInlineExpression(alphaAttribute, UIDescriptionValueType::kFloatingPoint);
								}
								if (!alphaExpression)
								{
									// Invalid Expression Parameter
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> hueExpression;
								const char *hueAttribute = elem->Attribute("hue");
								if (hueAttribute == nullptr)
								{
									hueAttribute = elem->Attribute("h");
								}
								if (hueAttribute == nullptr)
								{
									hueAttribute = elem->Attribute("dh");
									if (hueAttribute != nullptr)
									{
										isDegreeHue = true;
									}
								}
								if (hueAttribute == nullptr)
								{
									if (childrenCount >= 3)
									{
										hueExpression = ReadExpressionFromElement(childrenComponents[(childrenCount == 3) ? 0 : 1], UIDescriptionValueType::kFloatingPoint);
									}
								}
								else
								{
									hueExpression = ParseInlineExpression(hueAttribute, UIDescriptionValueType::kFloatingPoint);
								}
								if (!hueExpression)
								{
									// Invalid Expression Parameter
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> saturationExpression;
								const char *saturationAttribute = elem->Attribute("saturation");
								if (saturationAttribute == nullptr)
								{
									saturationAttribute = elem->Attribute("s");
								}
								if (saturationAttribute == nullptr)
								{
									if (childrenCount >= 3)
									{
										saturationExpression = ReadExpressionFromElement(childrenComponents[(childrenCount == 3) ? 1 : 2], UIDescriptionValueType::kFloatingPoint);
									}
								}
								else
								{
									saturationExpression = ParseInlineExpression(saturationAttribute, UIDescriptionValueType::kFloatingPoint);
								}
								if (!saturationExpression)
								{
									// Invalid Expression Parameter
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> lightnessExpression;
								const char *lightnessAttribute = elem->Attribute("lightness");
								if (lightnessAttribute == nullptr)
								{
									lightnessAttribute = elem->Attribute("l");
								}
								if (lightnessAttribute == nullptr)
								{
									if (childrenCount >= 3)
									{
										lightnessExpression = ReadExpressionFromElement(childrenComponents[(childrenCount == 3) ? 2 : 3], UIDescriptionValueType::kFloatingPoint);
									}
								}
								else
								{
									lightnessExpression = ParseInlineExpression(lightnessAttribute, UIDescriptionValueType::kFloatingPoint);
								}
								if (!lightnessExpression)
								{
									// Invalid Expression Parameter
									return nullptr;
								}

								if (alphaExpression->GetValueType() != UIDescriptionValueType::kFloatingPoint)
								{
									alphaExpression = Expression::_Internal::CastExpression(alphaExpression, UIDescriptionValueType::kFloatingPoint);
								}
								if (hueExpression->GetValueType() != UIDescriptionValueType::kFloatingPoint)
								{
									hueExpression = Expression::_Internal::CastExpression(hueExpression, UIDescriptionValueType::kFloatingPoint);
								}
								if (saturationExpression->GetValueType() != UIDescriptionValueType::kFloatingPoint)
								{
									saturationExpression = Expression::_Internal::CastExpression(saturationExpression, UIDescriptionValueType::kFloatingPoint);
								}
								if (lightnessExpression->GetValueType() != UIDescriptionValueType::kFloatingPoint)
								{
									lightnessExpression = Expression::_Internal::CastExpression(lightnessExpression, UIDescriptionValueType::kFloatingPoint);
								}

								return std::make_shared<ConstructColorAHSL>(isDegreeHue, 
									std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(alphaExpression),
									std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(hueExpression),
									std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(saturationExpression),
									std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(lightnessExpression));
							}

							//------------------------------------------------------------------------------
							std::shared_ptr<IExpression<Vector2DValueType> > ReadConstructVector2DExpressionFromElement(tinyxml2::XMLElement *elem)
							{
								tinyxml2::XMLElement *currentChildElement = elem->FirstChildElement();

								std::shared_ptr<IArbitrarilyTypedExpression> xExpression;
								const char *xAttribute = elem->Attribute("x");
								if (xAttribute == nullptr)
								{
									if (currentChildElement == nullptr)
									{
										// No X Expression
										return nullptr;
									}
									xExpression = ReadExpressionFromElement(currentChildElement, UIDescriptionValueType::kFloatingPoint);
									currentChildElement = currentChildElement->NextSiblingElement();
								}
								else
								{
									xExpression = ParseInlineExpression(xAttribute, UIDescriptionValueType::kFloatingPoint);
								}
								if (!xExpression)
								{
									// Invalid Expression
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> yExpression;
								const char *yAttribute = elem->Attribute("y");
								if (yAttribute == nullptr)
								{
									if (currentChildElement == nullptr)
									{
										// No Y Expression
										return nullptr;
									}
									yExpression = ReadExpressionFromElement(currentChildElement, UIDescriptionValueType::kFloatingPoint);
									currentChildElement = currentChildElement->NextSiblingElement();
								}
								else
								{
									yExpression = ParseInlineExpression(yAttribute, UIDescriptionValueType::kFloatingPoint);
								}
								if (!yExpression)
								{
									// Invalid Expression
									return nullptr;
								}

								if (xExpression->GetValueType() != UIDescriptionValueType::kFloatingPoint)
								{
									xExpression = Expression::_Internal::CastExpression(xExpression, UIDescriptionValueType::kFloatingPoint);
								}
								if (yExpression->GetValueType() != UIDescriptionValueType::kFloatingPoint)
								{
									yExpression = Expression::_Internal::CastExpression(yExpression, UIDescriptionValueType::kFloatingPoint);
								}
								return std::make_shared<ConstructVector2D>(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(xExpression), std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(yExpression));
							}

							//------------------------------------------------------------------------------
							std::shared_ptr<IExpression<Vector3DValueType> > ReadConstructVector3DExpressionFromElement(tinyxml2::XMLElement *elem)
							{
								tinyxml2::XMLElement *currentChildElement = elem->FirstChildElement();

								std::shared_ptr<IArbitrarilyTypedExpression> xExpression;
								const char *xAttribute = elem->Attribute("x");
								if (xAttribute == nullptr)
								{
									if (currentChildElement == nullptr)
									{
										// No X Expression
										return nullptr;
									}
									xExpression = ReadExpressionFromElement(currentChildElement, UIDescriptionValueType::kFloatingPoint);
									currentChildElement = currentChildElement->NextSiblingElement();
								}
								else
								{
									xExpression = ParseInlineExpression(xAttribute, UIDescriptionValueType::kFloatingPoint);
								}
								if (!xExpression)
								{
									// Invalid Expression
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> yExpression;
								const char *yAttribute = elem->Attribute("y");
								if (yAttribute == nullptr)
								{
									if (currentChildElement == nullptr)
									{
										// No Y Expression
										return nullptr;
									}
									yExpression = ReadExpressionFromElement(currentChildElement, UIDescriptionValueType::kFloatingPoint);
									currentChildElement = currentChildElement->NextSiblingElement();
								}
								else
								{
									yExpression = ParseInlineExpression(yAttribute, UIDescriptionValueType::kFloatingPoint);
								}
								if (!yExpression)
								{
									// Invalid Expression
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> zExpression;
								const char *zAttribute = elem->Attribute("z");
								if (zAttribute == nullptr)
								{
									if (currentChildElement == nullptr)
									{
										// No Y Expression
										return nullptr;
									}
									zExpression = ReadExpressionFromElement(currentChildElement, UIDescriptionValueType::kFloatingPoint);
									currentChildElement = currentChildElement->NextSiblingElement();
								}
								else
								{
									zExpression = ParseInlineExpression(zAttribute, UIDescriptionValueType::kFloatingPoint);
								}
								if (!zExpression)
								{
									// Invalid Expression
									return nullptr;
								}

								if (xExpression->GetValueType() != UIDescriptionValueType::kFloatingPoint)
								{
									xExpression = Expression::_Internal::CastExpression(xExpression, UIDescriptionValueType::kFloatingPoint);
								}
								if (yExpression->GetValueType() != UIDescriptionValueType::kFloatingPoint)
								{
									yExpression = Expression::_Internal::CastExpression(yExpression, UIDescriptionValueType::kFloatingPoint);
								}
								if (zExpression->GetValueType() != UIDescriptionValueType::kFloatingPoint)
								{
									zExpression = Expression::_Internal::CastExpression(zExpression, UIDescriptionValueType::kFloatingPoint);
								}
								return std::make_shared<ConstructVector3D>(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(xExpression), std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(yExpression), std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(zExpression));
							}

							//------------------------------------------------------------------------------
							std::shared_ptr<IArbitrarilyTypedExpression> ReadComponentOfExpressionFromElement(tinyxml2::XMLElement *elem, UIDescriptionValueType hintedType)
							{
								tinyxml2::XMLElement *currentChildElement = elem->FirstChildElement();

								std::shared_ptr<IArbitrarilyTypedExpression> subExpression;
								const char *expAttribute = elem->Attribute("value");
								if (expAttribute == nullptr)
								{
									if (currentChildElement == nullptr)
									{
										// No Sub Expression
										return nullptr;
									}
									subExpression = ReadExpressionFromElement(currentChildElement, ((hintedType == UIDescriptionValueType::kInteger) ? UIDescriptionValueType::kColor : UIDescriptionValueType::kVector3D));
									currentChildElement = currentChildElement->NextSiblingElement();
								}
								else
								{
									subExpression = ParseInlineExpression(expAttribute, ((hintedType == UIDescriptionValueType::kInteger) ? UIDescriptionValueType::kColor : UIDescriptionValueType::kVector3D));
								}
								if (!subExpression)
								{
									// Invalid Expression
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> indexExpression;
								const char *indexAttribute = elem->Attribute("index");
								if (indexAttribute == nullptr)
								{
									indexAttribute = elem->Attribute("idx");
								}
								if (indexAttribute == nullptr)
								{
									if (currentChildElement == nullptr)
									{
										// No Index Expression
										return nullptr;
									}
									indexExpression = ReadExpressionFromElement(currentChildElement, UIDescriptionValueType::kInteger);
									currentChildElement = currentChildElement->NextSiblingElement();
								}
								else
								{
									indexExpression = ParseInlineExpression(indexAttribute, UIDescriptionValueType::kInteger);
								}
								if (!indexExpression)
								{
									// Invalid Expression
									return nullptr;
								}

								if (indexExpression->GetValueType() != UIDescriptionValueType::kInteger)
								{
									indexExpression = Expression::_Internal::CastExpression(indexExpression, UIDescriptionValueType::kInteger);
								}

								switch (subExpression->GetValueType())
								{
									case UIDescriptionValueType::kColor:
										return std::make_shared<ComponentOf<ColorARGBValueType, IntegerValueType> >(std::dynamic_pointer_cast<IExpression<ColorARGBValueType> >(subExpression), std::dynamic_pointer_cast<IExpression<IntegerValueType> >(indexExpression));

									case UIDescriptionValueType::kVector2D:
										return std::make_shared<ComponentOf<Vector2DValueType, FloatingPointValueType> >(std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(subExpression), std::dynamic_pointer_cast<IExpression<IntegerValueType> >(indexExpression));

									case UIDescriptionValueType::kVector3D:
										return std::make_shared<ComponentOf<Vector3DValueType, FloatingPointValueType> >(std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(subExpression), std::dynamic_pointer_cast<IExpression<IntegerValueType> >(indexExpression));
								}

								// Invalid Type
								throw new UIDescriptionExpressionTypeErrorException();
							}

							//------------------------------------------------------------------------------
							std::shared_ptr<IArbitrarilyTypedExpression> ReadVectorScalarMultiplicationExpressionFromElement(tinyxml2::XMLElement *elem, UIDescriptionValueType hintedType)
							{
								tinyxml2::XMLElement *currentChildElement = elem->FirstChildElement();

								std::shared_ptr<IArbitrarilyTypedExpression> vectorExpression;
								if (currentChildElement == nullptr)
								{
									const char *vecAttribute = elem->Attribute("vec");
									if (vecAttribute == nullptr)
									{
										vecAttribute = elem->Attribute("vector");
									}
									if (vecAttribute == nullptr)
									{
										// No target
										return nullptr;
									}
									vectorExpression = ParseInlineExpression(vecAttribute, hintedType);
								}
								else
								{
									vectorExpression = ReadExpressionFromElement(currentChildElement, hintedType);
									currentChildElement = currentChildElement->NextSiblingElement();
								}
								if (!vectorExpression)
								{
									// Invalid Operand
									return nullptr;
								}

								std::shared_ptr<IArbitrarilyTypedExpression> scalarExpression;
								if (currentChildElement == nullptr)
								{
									const char *scalarAttribute = elem->Attribute("mul");
									if (scalarAttribute == nullptr)
									{
										scalarAttribute = elem->Attribute("multiplier");
									}
									if (scalarAttribute == nullptr)
									{
										scalarAttribute = elem->Attribute("scalar");
									}
									if (scalarAttribute == nullptr)
									{
										// No target
										return nullptr;
									}
									scalarExpression = ParseInlineExpression(scalarAttribute, UIDescriptionValueType::kFloatingPoint);
								}
								else
								{
									scalarExpression = ReadExpressionFromElement(currentChildElement, UIDescriptionValueType::kFloatingPoint);
									currentChildElement = currentChildElement->NextSiblingElement();
								}
								if (!scalarExpression)
								{
									// Invalid Operand
									return nullptr;
								}

								if (scalarExpression->GetValueType() != UIDescriptionValueType::kFloatingPoint)
								{
									scalarExpression = Expression::_Internal::CastExpression(scalarExpression, UIDescriptionValueType::kFloatingPoint);
								}

								switch (vectorExpression->GetValueType())
								{
									case UIDescriptionValueType::kVector2D:
										return std::make_shared<VectorScalarMultiply<Vector2DValueType> >(std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(vectorExpression), std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(scalarExpression));

									case UIDescriptionValueType::kVector3D:
										return std::make_shared<VectorScalarMultiply<Vector3DValueType> >(std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(vectorExpression), std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(scalarExpression));
								}

								// Invalid Type
								throw new UIDescriptionExpressionTypeErrorException();
							}
						}

					
						//------------------------------------------------------------------------------
						// Read varaible contains an expression from given XML Element
						std::shared_ptr<IDescriptionObject> ReadExpressionVariableFromElement(ParsingXMLElementType *elem)
						{
							UIDescriptionValueType valueType = UIDescriptionValueType::kINVALID;
							const char *typeAttribute = elem->Attribute("type");
							if (typeAttribute != nullptr)
							{
								valueType = Expression::ParseTypeFromString(typeAttribute);
								if (valueType == UIDescriptionValueType::kINVALID)
								{
									// Invalid Type
									return nullptr;
								}
							}

							tinyxml2::XMLElement *expressionNode = elem->FirstChildElement();
							if (expressionNode == nullptr)
							{
								// No Expression
								return nullptr;
							}

							std::shared_ptr<IArbitrarilyTypedExpression> variableExpression(ReadExpressionFromElement(expressionNode, valueType));
							if (!variableExpression)
							{
								// Invalid Expression
								return nullptr;
							}
							if (valueType == UIDescriptionValueType::kINVALID)
							{
								valueType = variableExpression->GetValueType();
							}
							if (variableExpression->GetValueType() != valueType)
							{
								variableExpression = Expression::_Internal::CastExpression(variableExpression, valueType);
								if (!variableExpression)
								{
									// Invalid Type
									return nullptr;
								}
							}

							switch (valueType)
							{
								case UIDescriptionValueType::kBoolean:
									return std::make_shared<ExpressionVariable<BooleanValueType> >(std::dynamic_pointer_cast<IExpression<BooleanValueType> >(variableExpression));

								case UIDescriptionValueType::kInteger:
									return std::make_shared<ExpressionVariable<IntegerValueType> >(std::dynamic_pointer_cast<IExpression<IntegerValueType> >(variableExpression));

								case UIDescriptionValueType::kFloatingPoint:
									return std::make_shared<ExpressionVariable<FloatingPointValueType> >(std::dynamic_pointer_cast<IExpression<FloatingPointValueType> >(variableExpression));

								case UIDescriptionValueType::kColor:
									return std::make_shared<ExpressionVariable<ColorARGBValueType> >(std::dynamic_pointer_cast<IExpression<ColorARGBValueType> >(variableExpression));

								case UIDescriptionValueType::kVector2D:
									return std::make_shared<ExpressionVariable<Vector2DValueType> >(std::dynamic_pointer_cast<IExpression<Vector2DValueType> >(variableExpression));

								case UIDescriptionValueType::kVector3D:
									return std::make_shared<ExpressionVariable<Vector3DValueType> >(std::dynamic_pointer_cast<IExpression<Vector3DValueType> >(variableExpression));

								case UIDescriptionValueType::kString:
									return std::make_shared<ExpressionVariable<StringValueType> >(std::dynamic_pointer_cast<IExpression<StringValueType> >(variableExpression));
							}

							return nullptr;
						}

						//------------------------------------------------------------------------------
						// Read an expression from given XML Element
						std::shared_ptr<IArbitrarilyTypedExpression> ReadExpressionFromElement(ParsingXMLElementType *elem, UIDescriptionValueType hintedType)
						{
							// Tabled Creator Definition
							static const std::unordered_map<std::string, std::function<std::shared_ptr<IArbitrarilyTypedExpression>(ParsingXMLElementType *, UIDescriptionValueType)> > factoryMap({

								{ std::string(_Internal::TagNames::kConstantValue), 
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadConstantValueExpressionFromElement(elem, hintedType); } }, 

								{ std::string(_Internal::TagNames::kVariableDereference), 
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadVariableDereferenceExpressionFromElement(elem, hintedType); } },

								{ std::string(_Internal::TagNames::kCasting),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadCastingExpressionFromElement(elem, hintedType); } },

								{ std::string(_Internal::TagNames::kOperationMinus),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadMinusExpressionFromElement(elem); } },

								{ std::string(_Internal::TagNames::kOperationAdd),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadMultiOperandOperationExpressionFromElement<Add>(elem); } },

								{ std::string(_Internal::TagNames::kOperationSubtract),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadBinaryNumberOperationExpressionFromElement<Subtract>(elem, hintedType); } },

								{ std::string(_Internal::TagNames::kOperationMultiply),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadMultiOperandOperationExpressionFromElement<Multiply>(elem); } },

								{ std::string(_Internal::TagNames::kOperationDivide),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadBinaryNumberOperationExpressionFromElement<Divide>(elem, hintedType); } },

								{ std::string(_Internal::TagNames::kOperationModulus),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadTypedBinaryOperationExpressionFromElement<Modulus, IntegerValueType, IntegerValueType>(elem); } },

								{ std::string(_Internal::TagNames::kOperationRemainder),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadTypedBinaryOperationExpressionFromElement<Remainder, FloatingPointValueType, FloatingPointValueType>(elem); } },

								{ std::string(_Internal::TagNames::kOperationLerp),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadLerpExpressionFromElement(elem, hintedType); } },

								{ std::string(_Internal::TagNames::kLogicalNot),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadTypedUnaryOperationExpressionFromElement<Not, BooleanValueType>(elem); } },

								{ std::string(_Internal::TagNames::kLogicalAnd),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadTypedBinaryOperationExpressionFromElement<And, BooleanValueType, BooleanValueType>(elem); } },

								{ std::string(_Internal::TagNames::kLogicalOr),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadTypedBinaryOperationExpressionFromElement<Or, BooleanValueType, BooleanValueType>(elem); } },

								{ std::string(_Internal::TagNames::kLogicalXor),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadTypedBinaryOperationExpressionFromElement<Xor, BooleanValueType, BooleanValueType>(elem); } },

								{ std::string(_Internal::TagNames::kConditionEqual),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadBinaryOperationExpressionFromElement<IsEqual>(elem, UIDescriptionValueType::kINVALID); } },

								{ std::string(_Internal::TagNames::kConditionNotEqual),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadBinaryOperationExpressionFromElement<IsNotEqual>(elem, UIDescriptionValueType::kINVALID); } },

								{ std::string(_Internal::TagNames::kConditionGreater),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadBinaryOrderingOperationExpressionFromElement<IsGreater>(elem, UIDescriptionValueType::kINVALID); } },

								{ std::string(_Internal::TagNames::kConditionLess),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadBinaryOrderingOperationExpressionFromElement<IsLess>(elem, UIDescriptionValueType::kINVALID); } },

								{ std::string(_Internal::TagNames::kConditionGreaterEqual),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadBinaryOrderingOperationExpressionFromElement<IsGreaterEqual>(elem, UIDescriptionValueType::kINVALID); } },

								{ std::string(_Internal::TagNames::kConditionLessEqual),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadBinaryOrderingOperationExpressionFromElement<IsLessEqual>(elem, UIDescriptionValueType::kINVALID); } },

								{ std::string(_Internal::TagNames::kConditionApproxEqual),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadApproxCompareExpressionFromElement(elem); } },

								{ std::string(_Internal::TagNames::kSelectIf),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadIfExpressionFromElement(elem, hintedType); } },

								{ std::string(_Internal::TagNames::kSelectMin),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadMultiOperandOperationExpressionFromElement<Min>(elem); } },

								{ std::string(_Internal::TagNames::kSelectMax),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadMultiOperandOperationExpressionFromElement<Max>(elem); } },

								{ std::string(_Internal::TagNames::kFunctionAbsolute),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadAbsExpressionFromElement(elem, hintedType); } },

								{ std::string(_Internal::TagNames::kFunctionSquareRoot),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadFloatingPointFunctionExpressionFromElement(elem, [](FloatingPointValueType x) { return static_cast<FloatingPointValueType>(sqrt(x)); }); } },

								{ std::string(_Internal::TagNames::kFunctionPower),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadPowerExpressionFromElement(elem); } },

								{ std::string(_Internal::TagNames::kFunctionLog),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadLogExpressionFromElement(elem); } },

								{ std::string(_Internal::TagNames::kFunctionSin),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadFloatingPointFunctionExpressionFromElement(elem, [](FloatingPointValueType x) { return static_cast<FloatingPointValueType>(sin(x)); }); } },

								{ std::string(_Internal::TagNames::kFunctionCos),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadFloatingPointFunctionExpressionFromElement(elem, [](FloatingPointValueType x) { return static_cast<FloatingPointValueType>(cos(x)); }); } },

								{ std::string(_Internal::TagNames::kFunctionTan),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadFloatingPointFunctionExpressionFromElement(elem, [](FloatingPointValueType x) { return static_cast<FloatingPointValueType>(tan(x)); }); } },

								{ std::string(_Internal::TagNames::kFunctionArcSin),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadFloatingPointFunctionExpressionFromElement(elem, [](FloatingPointValueType x) { return static_cast<FloatingPointValueType>(asin(x)); }); } },

								{ std::string(_Internal::TagNames::kFunctionArcCos),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadFloatingPointFunctionExpressionFromElement(elem, [](FloatingPointValueType x) { return static_cast<FloatingPointValueType>(acos(x)); }); } },

								{ std::string(_Internal::TagNames::kFunctionArcTan),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadFloatingPointFunctionExpressionFromElement(elem, [](FloatingPointValueType x) { return static_cast<FloatingPointValueType>(atan(x)); }); } },

								{ std::string(_Internal::TagNames::kFunctionRound),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadToIntegerExpressionFromElement<Round>(elem); } },

								{ std::string(_Internal::TagNames::kFunctionFloor),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadToIntegerExpressionFromElement<Floor>(elem); } },

								{ std::string(_Internal::TagNames::kFunctionCeil),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadToIntegerExpressionFromElement<Ceil>(elem); } },

								{ std::string(_Internal::TagNames::kFunctionClamp),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadClampExpressionFromElement(elem, hintedType); } },

								{ std::string(_Internal::TagNames::kFunctionClampLow),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadClampLowExpressionFromElement(elem, hintedType); } },

								{ std::string(_Internal::TagNames::kFunctionClampHigh),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadClampHighExpressionFromElement(elem, hintedType); } },

								{ std::string(_Internal::TagNames::kTextureGetWidth),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadGetTargetNumericValueExpressionFromElement<TextureGetWidth>(elem, hintedType); } },

								{ std::string(_Internal::TagNames::kTextureGetHeight),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadGetTargetNumericValueExpressionFromElement<TextureGetHeight>(elem, hintedType); } },

								{ std::string(_Internal::TagNames::kConstructColorARGB),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadConstructColorARGBExpressionFromElement(elem); } },

								{ std::string(_Internal::TagNames::kConstructColorAHSL),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadConstructColorAHSLExpressionFromElement(elem); } },

								{ std::string(_Internal::TagNames::kConstructVector2D),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadConstructVector2DExpressionFromElement(elem); } },

								{ std::string(_Internal::TagNames::kConstructVector3D),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadConstructVector3DExpressionFromElement(elem); } },

								{ std::string(_Internal::TagNames::kComponentOf),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadComponentOfExpressionFromElement(elem, hintedType); } },

								{ std::string(_Internal::TagNames::kVectorDot),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadVector3DBinaryOperationExpressionFromElement<VectorDot>(elem); } },

								{ std::string(_Internal::TagNames::kVectorCross),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadVector3DBinaryOperationExpressionFromElement<VectorCross>(elem); } },

								{ std::string(_Internal::TagNames::kVectorScalarMultiply),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadVectorScalarMultiplicationExpressionFromElement(elem, hintedType); } },

								{ std::string(_Internal::TagNames::kVectorNormalize),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadVectorUnaryOperationExpressionFromElement<VectorNormal>(elem, hintedType); } },

								{ std::string(_Internal::TagNames::kVectorLength),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadVectorUnaryOperationExpressionFromElement<VectorLength>(elem); } },

								{ std::string(_Internal::TagNames::kVectorSquareLength),
									[](ParsingXMLElementType *elem, UIDescriptionValueType hintedType) { return _Internal::ReadVectorUnaryOperationExpressionFromElement<VectorSquareLength>(elem); } },

								});

							// Find in the table
							std::string elementNameString = std::string(elem->Value());

							std::unordered_map<std::string, std::function<std::shared_ptr<IArbitrarilyTypedExpression>(ParsingXMLElementType *, UIDescriptionValueType)> >::const_iterator findIt = factoryMap.find(elementNameString);
							if (findIt != factoryMap.cend())
							{
								return findIt->second(elem, hintedType);
							}

							return nullptr;
						}
					}
				}
			}
		}
	}
}
