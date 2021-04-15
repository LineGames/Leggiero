using System;
using System.Text.RegularExpressions;

namespace ProjectCreatorTool
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Leggiero Project Creator - Command-line Tool");

            Console.WriteLine("Reading settings ...");
            OptionParser options = new OptionParser(args);
            if (!options.IsValid())
            {
                Console.WriteLine("ERROR: Project creation option is not valid.");
                _PrintHelp();
                return;
            }

            if (options.IsNeedToShowHelp)
            {
                _PrintHelp();
                return;
            }

            Console.WriteLine("Creating project ...");
            bool isCreationSucceed = ProjectCreatorCore.ProjectCreator.CreateProject(options.creatingOptions, options.environment);
            if (!isCreationSucceed)
            {
                Console.WriteLine("ERROR: Project creation FAILED.");
                return;
            }

            Console.WriteLine("Created a new Leggiero game project [{0}]", options.creatingOptions.gameTitle);
        }

        static void _PrintHelp()
        {
            Console.WriteLine("Usage: {0} [-h] ProgramName [-a appid] [-t title] [-p name] [-o orient] [-s scene] [-e path] [-d path]", AppDomain.CurrentDomain.FriendlyName);
            Console.WriteLine("Create a new Leggiero game project.");
            Console.WriteLine();
            Console.WriteLine("  -h, --help\t\t\tshow usages");
            Console.WriteLine();
            Console.WriteLine("  -a, --appid\t\tappid\tset app identifier of iOS/Android platform. e.g. com.leggiero.LegToy");
            Console.WriteLine("  \t\t\t\t(com.yourcompany.{ProgramName} when not specified)");
            Console.WriteLine();
            Console.WriteLine("  -t, --title\t\ttitle\tset the title of the game");
            Console.WriteLine("  \t\t\t\t({ProgramName} when not specified)");
            Console.WriteLine();
            Console.WriteLine("  -p, --project-dir\tname\tset the game project directory name");
            Console.WriteLine("  \t\t\t\t({ProgramName} when not specified)");
            Console.WriteLine();
            Console.WriteLine("  -o, --orientation\torient\tset orientation of the game app (kLandscape, kPortrait)");
            Console.WriteLine("  \t\t\t\t(default orientation is kLandscape)");
            Console.WriteLine();
            Console.WriteLine("  -s, --scene\t\tscene\tset game scene setting of generated game source code (kNone, kEmptyScene, kUIBaseScene)");
            Console.WriteLine("  \t\t\t\t(default orientation is kEmptyScene)");
            Console.WriteLine("  \t\t\t\t  * kNone: \t\tjust set game application code; no scene structure");
            Console.WriteLine("  \t\t\t\t  * kEmptyScene: \tset empty game scene");
            Console.WriteLine("  \t\t\t\t  * kUIBaseScene: \tset a game scene with LegacyUI functionality");
            Console.WriteLine();
            Console.WriteLine("  -e, --engine-path\tpath\tset engine root directory path; the project will be created here");
            Console.WriteLine("  \t\t\t\t(default path is ../../)");
            Console.WriteLine();
            Console.WriteLine("  -d, --template-path\tpath\tset project template data path");
            Console.WriteLine("  \t\t\t\t(default path is ./Template)");
            Console.WriteLine();
        }
    }
}
