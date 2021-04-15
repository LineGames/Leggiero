using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProjectCreatorTool
{
    /// <summary>
    /// Simple, ugly parser tool for C# commandline arguments
    /// </summary>
    public class CmdArgsParser
    {
        /// <summary>
        /// Create command-line argument parser object
        /// </summary>
        /// <param name="argumentedOptions">Options with an argument</param>
        /// <param name="flagStarterList">Flag starting strings. Use default -, --, / if null.</param>
        /// <param name="valueDelimiterList">Option-value delimiter strings. Use default =, : if null.</param>
        public CmdArgsParser(IEnumerable<string> argumentedOptions = null, IEnumerable<string> flagStarterList = null, IEnumerable<string> valueDelimiterList = null)
        {
            if (argumentedOptions != null)
            {
                m_argumentedOptions = new HashSet<string>(argumentedOptions);
            }
            if (flagStarterList != null)
            {
                flagStarters = flagStarterList.ToList();
            }
            if (valueDelimiterList != null)
            {
                valueDelimiters = valueDelimiterList.ToList();
            }
        }

        /// <summary>
        /// Do Parsing
        /// </summary>
        /// <param name="commandLineArgs">Target args</param>
        /// <returns>Parsing result</returns>
        public ParseResult Parse(string[] commandLineArgs)
        {
            ParseResult parsingResult = new ParseResult();

            //note: parsing automata will be faster, but just compare all by length order because this may not be performance critical routine
            List<string> orderedStartCharacters = new List<string>(flagStarters);
            orderedStartCharacters.Sort((x, y) => y.Length.CompareTo(x.Length));

            List<string> orderedValueDelimiters = new List<string>(valueDelimiters);
            orderedValueDelimiters.Sort((x, y) => y.Length.CompareTo(x.Length));

            List<string> orderedArgumentOptions = m_argumentedOptions.ToList();
            orderedArgumentOptions.Sort((x, y) => y.Length.CompareTo(x.Length));

            OptionEntry currentCapturing = null;

            int readingPointer = 0;
            while (readingPointer < commandLineArgs.Length)
            {
                string specialParsingString = null;
                foreach (string currentStartCheck in orderedStartCharacters)
                {
                    if (commandLineArgs[readingPointer].StartsWith(currentStartCheck))
                    {
                        specialParsingString = commandLineArgs[readingPointer].Substring(currentStartCheck.Length);
                        break;
                    }
                }

                if (specialParsingString == null)
                {
                    if (currentCapturing != null)
                    {
                        currentCapturing.valueString = commandLineArgs[readingPointer];
                        parsingResult.options.Add(currentCapturing);
                        currentCapturing = null;
                    }
                    else
                    {
                        parsingResult.normalArgs.Add(commandLineArgs[readingPointer]);
                    }
                }
                else
                {
                    // Invalidate capture after special arguments
                    if (currentCapturing != null)
                    {
                        parsingResult.options.Add(currentCapturing);
                        currentCapturing = null;
                    }

                    if (string.IsNullOrWhiteSpace(specialParsingString))
                    {
                        // Not a valid option. considered as normal parameter
                        parsingResult.normalArgs.Add(commandLineArgs[readingPointer]);
                    }
                    else
                    {
                        string valueCandidateString = null;
                        string capturedOption = null;
                        foreach (string currentOptionCheck in orderedArgumentOptions)
                        {
                            if (specialParsingString.StartsWith(currentOptionCheck))
                            {
                                valueCandidateString = specialParsingString.Substring(currentOptionCheck.Length);
                                capturedOption = currentOptionCheck;
                                break;
                            }
                        }

                        if (capturedOption == null)
                        {
                            // Non-argumented options
                            parsingResult.options.Add(new OptionEntry() { optionString = specialParsingString });
                        }
                        else
                        {
                            // Options with an argument
                            currentCapturing = new OptionEntry() { optionString = capturedOption };
                            if (valueCandidateString.Length > 0)
                            {
                                string assignedOptionValue = null;
                                foreach (string currentDelimiter in orderedValueDelimiters)
                                {
                                    if (valueCandidateString.StartsWith(currentDelimiter))
                                    {
                                        assignedOptionValue = valueCandidateString.Substring(currentDelimiter.Length);
                                        break;
                                    }
                                }

                                if (assignedOptionValue == null)
                                {
                                    currentCapturing.valueString = valueCandidateString;
                                }
                                else
                                {
                                    currentCapturing.valueString = assignedOptionValue;
                                }
                                parsingResult.options.Add(currentCapturing);
                                currentCapturing = null;
                            }
                        }
                    }
                }

                ++readingPointer;
            }
            if (currentCapturing != null)
            {
                parsingResult.options.Add(currentCapturing);
            }

            return parsingResult;
        }


        #region State Interfaces

        /// <summary>
        /// Register a new argumented option.
        /// </summary>
        /// <param name="optionString">Option string. Should not contains sub-strings in valueDelimiters.</param>
        public void RegisterArgumentedOption(string optionString)
        {
            if (string.IsNullOrWhiteSpace(optionString))
            {
                return;
            }

            //note: Not checking, but should not contains sub-strings in valueDelimiters

            m_argumentedOptions.Add(optionString);
        }

        /// <summary>
        /// Remove an option.
        /// </summary>
        /// <param name="optionString">Option to remove</param>
        public void UnRegisterArgumentedOption(string optionString)
        {
            m_argumentedOptions.Remove(optionString);
        }

        /// <summary>
        /// Get all current registered argumented options.
        /// </summary>
        /// <returns>IEnumerable for all registered options.</returns>
        public IEnumerable<string> GetAllRegisteredOptions()
        {
            return m_argumentedOptions;
        }

        #endregion


        #region Result Types

        /// <summary>
        /// Option Entry
        /// </summary>
        public class OptionEntry
        {
            /// <summary>
            /// Option
            /// </summary>
            public string optionString;

            /// <summary>
            /// Option value.
            /// null if non-argumented option or no value passed.
            /// </summary>
            public string valueString = null;
        }

        /// <summary>
        /// Result for parsing
        /// </summary>
        public class ParseResult
        {
            /// <summary>
            /// Non-option arguments
            /// </summary>
            public List<string>         normalArgs = new List<string>();

            /// <summary>
            /// Passed options
            /// </summary>
            public List<OptionEntry>    options = new List<OptionEntry>();
        }

        #endregion


        #region Exposed Values

        /// <summary>
        /// Flag starting characters
        /// </summary>
        public List<string> flagStarters = new List<string>() { "-", "--", "/" };

        /// <summary>
        /// Option-Value delimieters
        /// </summary>
        public List<string> valueDelimiters = new List<string>() { "=", ":" };

        #endregion


        private HashSet<string> m_argumentedOptions = new HashSet<string>();
    }
}
