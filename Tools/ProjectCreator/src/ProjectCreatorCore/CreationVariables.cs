using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProjectCreatorCore
{
    /// <summary>
    /// Creating Variable Environment
    /// </summary>
    internal class CreationVariables
    {
        internal CreationVariables(CreationOptions options, CreationContext ctx)
        {
            _InitializeVariables(options, ctx);
        }

        internal string GetValue(string accessString)
        {
            if (accessString == null)
            {
                return null;
            }

            string variableName = accessString;
            string modifiers = null;
            if (accessString.Contains(':'))
            {
                int separateIndex = accessString.IndexOf(':');
                variableName = accessString.Substring(0, separateIndex);
                modifiers = accessString.Substring(separateIndex + 1);
            }

            string regularizedVariableName = variableName.ToUpper();
            if (!m_variableTable.ContainsKey(regularizedVariableName))
            {
                // Not Found
                return null;
            }

            string variableValue = m_variableTable[regularizedVariableName];

            if (modifiers != null)
            {
                if (modifiers.Contains('U'))
                {
                    variableValue = variableValue.ToUpper();
                }
                if (modifiers.Contains('L'))
                {
                    variableValue = variableValue.ToLower();
                }
            }

            return variableValue;
        }


        private Dictionary<string, string> m_variableTable;

        private void _InitializeVariables(CreationOptions options, CreationContext ctx)
        {
            m_variableTable = new Dictionary<string, string>();

            // Token
            m_variableTable.Add("${{", "${{");

            m_variableTable.Add("ProgramName".ToUpper(), options.programName);
            m_variableTable.Add("ProgramName-Lower".ToUpper(), options.programName.ToLower());
            m_variableTable.Add("ProgramName-Upper".ToUpper(), options.programName.ToUpper());

            m_variableTable.Add("GameTitle".ToUpper(), options.gameTitle);

            m_variableTable.Add("AppIdentifier".ToUpper(), options.appIdentifier);

            switch (options.appOrientation)
            {
                case CreationOptions.AppOrientationType.kLandscape:
                    {
                        m_variableTable.Add("Orientation-IsLandscapeCppBool".ToUpper(), "true");
                        m_variableTable.Add("Orientation-Android".ToUpper(), "userLandscape");
                        m_variableTable.Add("Orientation-iOS".ToUpper(), "\t\t<string>UIInterfaceOrientationLandscapeLeft</string>\r\n\t\t<string>UIInterfaceOrientationLandscapeRight</string>");
                        m_variableTable.Add("Resolution-WinPC".ToUpper(), "\t\tscreenWidth = 1280;\r\n\t\tscreenHeight = 720;");
                    }
                    break;

                case CreationOptions.AppOrientationType.kPortrait:
                    {
                        m_variableTable.Add("Orientation-IsLandscapeCppBool".ToUpper(), "false");
                        m_variableTable.Add("Orientation-Android".ToUpper(), "userPortrait");
                        m_variableTable.Add("Orientation-iOS".ToUpper(), "\t\t<string>UIInterfaceOrientationPortrait</string>\r\n\t\t<string>UIInterfaceOrientationPortraitUpsideDown</string>");
                        m_variableTable.Add("Resolution-WinPC".ToUpper(), "\t\tscreenWidth = 540;\r\n\t\tscreenHeight = 960;");
                    }
                    break;
            }

            m_variableTable.Add("GUID-Solution".ToUpper(), ctx.guid_solution);
            m_variableTable.Add("GUID-GameProject".ToUpper(), ctx.guid_gameProject);
            m_variableTable.Add("GUID-AppAndroid".ToUpper(), ctx.guid_appAndroid);
            m_variableTable.Add("GUID-AppWinPC".ToUpper(), ctx.guid_appWinPC);
        }
    }
}
