using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProjectCreatorCore
{
    /// <summary>
    /// Text contents filtering processor
    /// </summary>
    internal class FilterProcessor
    {
        public FilterProcessor(CreationVariables variables)
        {
            m_variables = variables;
        }


        private CreationVariables m_variables;


        public string Filter(string contentsToFilter)
        {
            if (string.IsNullOrEmpty(contentsToFilter))
            {
                return contentsToFilter;
            }

            int stateType = 0;  // 1: $, 2: ${, 3: var mode, 4: var }

            StringBuilder builder = new StringBuilder();
            builder.EnsureCapacity(contentsToFilter.Length * 3 / 2);

            List<char> variableBuffer = new List<char>();
            for (int i = 0; i < contentsToFilter.Length; ++i)
            {
                char currentChar = contentsToFilter[i];
                switch (stateType)
                {
                    case 0:
                        {
                            switch (currentChar)
                            {
                                case '$':
                                    {
                                        stateType = 1;
                                    }
                                    break;

                                default:
                                    {
                                        builder.Append(currentChar);
                                    }
                                    break;
                            }
                        }
                        break;

                    case 1:
                        {
                            switch (currentChar)
                            {
                                case '{':
                                    {
                                        stateType = 2;
                                    }
                                    break;

                                case '$':
                                    {
                                        builder.Append('$');
                                        stateType = 1;
                                    }
                                    break;

                                default:
                                    {
                                        builder.Append('$');
                                        builder.Append(currentChar);
                                        stateType = 0;
                                    }
                                    break;
                            }
                        }
                        break;

                    case 2:
                        {
                            switch (currentChar)
                            {
                                case '{':
                                    {
                                        stateType = 3;
                                    }
                                    break;

                                case '$':
                                    {
                                        builder.Append("${");
                                        stateType = 1;
                                    }
                                    break;

                                default:
                                    {
                                        builder.Append("${");
                                        builder.Append(currentChar);
                                        stateType = 0;
                                    }
                                    break;
                            }
                        }
                        break;

                    case 3:
                        {
                            switch (currentChar)
                            {
                                case '}':
                                    {
                                        stateType = 4;
                                    }
                                    break;

                                default:
                                    {
                                        variableBuffer.Add(currentChar);
                                    }
                                    break;
                            }
                        }
                        break;

                    case 4:
                        {
                            switch (currentChar)
                            {
                                case '}':
                                    {
                                        string parsedVariable = new string(variableBuffer.ToArray());
                                        variableBuffer.Clear();

                                        builder.Append(m_variables.GetValue(parsedVariable));
                                        stateType = 0;
                                    }
                                    break;

                                default:
                                    {
                                        variableBuffer.Add('}');
                                        variableBuffer.Add(currentChar);
                                        stateType = 3;
                                    }
                                    break;
                            }
                        }
                        break;
                }
            }
            switch (stateType)
            {
                case 1:
                    {
                        builder.Append('$');
                    }
                    break;

                case 2:
                    {
                        builder.Append("${");
                    }
                    break;

                case 3:
                    {
                        builder.Append("${{");
                        for (int j = 0; j < variableBuffer.Count; ++j)
                        {
                            builder.Append(variableBuffer[j]);
                        }
                    }
                    break;

                case 4:
                    {
                        builder.Append("${{");
                        for (int j = 0; j < variableBuffer.Count; ++j)
                        {
                            builder.Append(variableBuffer[j]);
                        }
                        builder.Append('}');
                    }
                    break;
            }

            return builder.ToString();
        }
    }
}
