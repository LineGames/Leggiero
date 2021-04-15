using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Text.RegularExpressions;

namespace ProjectCreatorTool
{
    /// <summary>
    /// Commandline Option Parser
    /// </summary>
    class OptionParser
    {
        /// <summary>
        /// Create an option object from command line arguments
        /// </summary>
        /// <param name="commandlineArgs">Command line arguments</param>
        public OptionParser(string[] commandlineArgs)
        {
            m_isValid = false;
            IsNeedToShowHelp = false;

            _SetInitialValues();
            if (!_ParseCommandlineArgs(commandlineArgs))
            {
                return;
            }
            if (IsNeedToShowHelp)
            {
                m_isValid = true;
                return;
            }
            m_isValid = _PostProcessValues();
        }

        /// <summary>
        /// Creating Options
        /// </summary>
        public ProjectCreatorCore.CreationOptions creatingOptions;

        /// <summary>
        /// Creating Environment
        /// </summary>
        public ProjectCreatorCore.EnvironmentSetting environment;


        /// <summary>
        /// Check whether the option is valid
        /// </summary>
        /// <returns></returns>
        public bool IsValid()
        {
            return m_isValid;
        }


        private bool m_isValid;


        /// <summary>
        /// Whether it is needed to show help or not
        /// </summary>
        public bool IsNeedToShowHelp
        {
            get;
            protected set;
        }


        private void _SetInitialValues()
        {
            creatingOptions = new ProjectCreatorCore.CreationOptions();
            creatingOptions.appOrientation = ProjectCreatorCore.CreationOptions.AppOrientationType.kLandscape;
            creatingOptions.creatingScene = ProjectCreatorCore.CreationOptions.SceneGenerationType.kEmptyScene;

            environment = new ProjectCreatorCore.EnvironmentSetting();
            environment.engineRootPath = "../..";
            environment.templatePath = "Template";
        }

        private bool _ParseCommandlineArgs(string[] commandlineArgs)
        {
            CmdArgsParser parser = new CmdArgsParser(new List<string>() {
                "appid", "app-id", "a",
                "title", "t",
                "project-dir", "dir", "p",
                "orientation", "o",
                "scene", "s",
                "engine", "engine-path", "e",
                "template","template-path", "d",
            });
            CmdArgsParser.ParseResult options = parser.Parse(commandlineArgs);

            if (options.normalArgs.Count > 0)
            {
                creatingOptions.programName = options.normalArgs[0].Trim();
            }

            foreach (var currentOption in options.options)
            {
                switch (currentOption.optionString)
                {
                    case "appid":
                    case "app-id":
                    case "a":
                        {
                            if (!string.IsNullOrWhiteSpace(currentOption.valueString))
                            {
                                creatingOptions.appIdentifier = currentOption.valueString.Trim();
                            }
                        }
                        break;

                    case "title":
                    case "t":
                        {
                            if (!string.IsNullOrWhiteSpace(currentOption.valueString))
                            {
                                creatingOptions.gameTitle = currentOption.valueString.Trim();
                            }
                        }
                        break;

                    case "project-dir":
                    case "dir":
                    case "p":
                        {
                            if (!string.IsNullOrWhiteSpace(currentOption.valueString))
                            {
                                creatingOptions.projectDirctoryName = currentOption.valueString.Trim();
                            }
                        }
                        break;

                    case "orientation":
                    case "o":
                        {
                            if (!string.IsNullOrWhiteSpace(currentOption.valueString))
                            {
                                var parsedOrientation = _ParseOrientation(currentOption.valueString);
                                if (parsedOrientation.Item1)
                                {
                                    creatingOptions.appOrientation = parsedOrientation.Item2;
                                }
                            }
                        }
                        break;

                    case "scene":
                    case "s":
                        {
                            if (!string.IsNullOrWhiteSpace(currentOption.valueString))
                            {
                                var parsedScene = _ParseScene(currentOption.valueString);
                                if (parsedScene.Item1)
                                {
                                    creatingOptions.creatingScene = parsedScene.Item2;
                                }
                            }
                        }
                        break;

                    case "engine":
                    case "engine-path":
                    case "e":
                        {
                            if (!string.IsNullOrWhiteSpace(currentOption.valueString))
                            {
                                environment.engineRootPath = currentOption.valueString.Trim();
                            }
                        }
                        break;

                    case "template":
                    case "template-path":
                    case "d":
                        {
                            if (!string.IsNullOrWhiteSpace(currentOption.valueString))
                            {
                                environment.templatePath = currentOption.valueString.Trim();
                            }
                        }
                        break;

                    case "help":
                    case "h":
                        {
                            IsNeedToShowHelp = true;
                        }
                        break;

                    default:
                        {
                            Console.Error.WriteLine("  [W] Not a valid option \"{0}\" specified(ignored).", currentOption.optionString);
                        }
                        break;
                }
            }

            return true;
        }

        private bool _PostProcessValues()
        {
            bool isErrorOccured = false;

            // Base Error Check
            if (string.IsNullOrWhiteSpace(creatingOptions.programName))
            {
                Console.Error.WriteLine("  [E] No program name specified!");
                isErrorOccured = true;
            }

            // Default Value Filling
            if (string.IsNullOrWhiteSpace(creatingOptions.projectDirctoryName))
            {
                creatingOptions.projectDirctoryName = creatingOptions.programName;
            }
            if (string.IsNullOrWhiteSpace(creatingOptions.gameTitle))
            {
                creatingOptions.gameTitle = creatingOptions.programName;
            }
            if (string.IsNullOrWhiteSpace(creatingOptions.appIdentifier))
            {
                if (creatingOptions.programName != null)
                {
                    creatingOptions.appIdentifier = string.Format("com.yourcompany.{0}", creatingOptions.programName);
                }
            }

            // Error Check
            if (creatingOptions.programName != null && !Regex.IsMatch(creatingOptions.programName, "^[A-Za-z][0-9A-Za-z]*$"))
            {
                Console.Error.WriteLine("  [E] Invalid program name: \"{0}\"", creatingOptions.programName);
                isErrorOccured = true;
            }

            if (string.IsNullOrWhiteSpace(creatingOptions.gameTitle))
            {
                Console.Error.WriteLine("  [E] No game title specified!");
                isErrorOccured = true;
            }

            if (string.IsNullOrWhiteSpace(creatingOptions.appIdentifier))
            {
                Console.Error.WriteLine("  [E] No app identifier specified!");
                isErrorOccured = true;
            }
            else if (!Regex.IsMatch(creatingOptions.appIdentifier, "^([A-Za-z][0-9A-Za-z]*)(\\.([A-Za-z][0-9A-Za-z]*))+$"))
            {
                Console.Error.WriteLine("  [E] Invalid app identifier: \"{0}\"", creatingOptions.appIdentifier);
                isErrorOccured = true;
            }

            environment.engineRootPath = Path.GetFullPath(environment.engineRootPath);
            if (!Directory.Exists(environment.engineRootPath))
            {
                Console.Error.WriteLine("  [E] Engine root directory NOT exists: {0}", environment.engineRootPath);
                isErrorOccured = true;
            }

            environment.templatePath = Path.GetFullPath(environment.templatePath);
            if (!Directory.Exists(environment.templatePath))
            {
                Console.Error.WriteLine("  [E] Template data NOT exists: {0}", environment.templatePath);
                isErrorOccured = true;
            }

            return !isErrorOccured;
        }

        Tuple<bool, ProjectCreatorCore.CreationOptions.AppOrientationType> _ParseOrientation(string orientationString)
        {
            string processingString = orientationString.Trim().ToLower();

            switch (processingString)
            {
                case "klandscape":
                case "landscape":
                case "l":
                case "horizontal":
                case "h":
                    return new Tuple<bool, ProjectCreatorCore.CreationOptions.AppOrientationType>(true, ProjectCreatorCore.CreationOptions.AppOrientationType.kLandscape);

                case "kportrait":
                case "portrait":
                case "p":
                case "vertical":
                case "v":
                    return new Tuple<bool, ProjectCreatorCore.CreationOptions.AppOrientationType>(true, ProjectCreatorCore.CreationOptions.AppOrientationType.kPortrait);
            }

            if ("horizontal".StartsWith(processingString))
            {
                return new Tuple<bool, ProjectCreatorCore.CreationOptions.AppOrientationType>(true, ProjectCreatorCore.CreationOptions.AppOrientationType.kLandscape);
            }
            if ("vertical".StartsWith(processingString))
            {
                return new Tuple<bool, ProjectCreatorCore.CreationOptions.AppOrientationType>(true, ProjectCreatorCore.CreationOptions.AppOrientationType.kPortrait);
            }
            if ("landscape".StartsWith(processingString))
            {
                return new Tuple<bool, ProjectCreatorCore.CreationOptions.AppOrientationType>(true, ProjectCreatorCore.CreationOptions.AppOrientationType.kLandscape);
            }
            if ("portrait".StartsWith(processingString))
            {
                return new Tuple<bool, ProjectCreatorCore.CreationOptions.AppOrientationType>(true, ProjectCreatorCore.CreationOptions.AppOrientationType.kPortrait);
            }

            Console.Error.WriteLine("  [W] Not a valid orientation value \"{0}\" (ignored).", orientationString);
            return new Tuple<bool, ProjectCreatorCore.CreationOptions.AppOrientationType>(false, ProjectCreatorCore.CreationOptions.AppOrientationType.kLandscape);
        }

        Tuple<bool, ProjectCreatorCore.CreationOptions.SceneGenerationType> _ParseScene(string sceneString)
        {
            string processingString = sceneString.Trim().ToLower();

            switch (processingString)
            {
                case "knone":
                case "none":
                case "no":
                case "null":
                case "n":
                case "gameonly":
                case "game-only":
                case "game":
                case "g":
                    return new Tuple<bool, ProjectCreatorCore.CreationOptions.SceneGenerationType>(true, ProjectCreatorCore.CreationOptions.SceneGenerationType.kNone);

                case "kemptyscene":
                case "emptyscene":
                case "empty":
                case "sceneonly":
                case "scene-only":
                case "scene":
                case "s":
                    return new Tuple<bool, ProjectCreatorCore.CreationOptions.SceneGenerationType>(true, ProjectCreatorCore.CreationOptions.SceneGenerationType.kEmptyScene);

                case "kuibasescene":
                case "uibasescene":
                case "ui-base":
                case "uibase":
                case "ui-scene":
                case "uiscene":
                case "ui":
                case "u":
                    return new Tuple<bool, ProjectCreatorCore.CreationOptions.SceneGenerationType>(true, ProjectCreatorCore.CreationOptions.SceneGenerationType.kUIBaseScene);
            }

            Console.Error.WriteLine("  [W] Not a valid project type value \"{0}\" (ignored).", sceneString);
            return new Tuple<bool, ProjectCreatorCore.CreationOptions.SceneGenerationType>(false, ProjectCreatorCore.CreationOptions.SceneGenerationType.kNone);
        }
    }
}
