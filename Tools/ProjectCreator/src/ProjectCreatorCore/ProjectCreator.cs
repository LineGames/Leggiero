using System;
using System.IO;

namespace ProjectCreatorCore
{
    /// <summary>
    /// Project Creator Library Class
    /// </summary>
    public static class ProjectCreator
    {
        /// <summary>
        /// Create a new project with given settings.
        /// </summary>
        /// <param name="options">Project creation option</param>
        /// <param name="environment">Creating environment</param>
        /// <returns>Whether if the creation is succeed</returns>
        public static bool CreateProject(CreationOptions options, EnvironmentSetting environment)
        {
            if (!_SanityCheck(options, environment))
            {
                return false;
            }

            CreationContext ctx = new CreationContext();
            ctx.InitializeGeneratingValues();

            CreationVariables variables = new CreationVariables(options, ctx);
            FilterProcessor filter = new FilterProcessor(variables);

            TemplateList templateListData = TemplateList.ReadFile(Path.Combine(environment.templatePath, "Template.txt"), filter);
            if (templateListData == null)
            {
                Console.Error.WriteLine("  [E] Cannot read template index data.");
                return false;
            }

            if (!_DoCopyWorks(templateListData, filter, options, environment))
            {
                Console.Error.WriteLine("  [E] Copy process failed!!");
                return false;
            }

            return true;
        }

        private static bool _SanityCheck(CreationOptions options, EnvironmentSetting environment)
        {
            if (options == null || environment == null)
            {
                return false;
            }

            return true;
        }

        private static bool _DoCopyWorks(TemplateList templateListData, FilterProcessor filter, CreationOptions options, EnvironmentSetting environment)
        {
            // Create Project Directory
            string projectRoot = Path.Combine(environment.engineRootPath, options.projectDirctoryName);
            try
            {
                Directory.CreateDirectory(projectRoot);
            }
            catch
            {
                Console.Error.WriteLine("  [E] Cannot create project directory.");
                return false;
            }

            string templateProjectPath = Path.Combine(environment.templatePath, "Project");
            if (!Directory.Exists(templateProjectPath))
            {
                Console.Error.WriteLine("  [E] Template project data not exists.");
            }

            templateListData.mainWorkSet.DoCopyWorks(templateProjectPath, projectRoot, filter);

            switch (options.creatingScene)
            {
                case CreationOptions.SceneGenerationType.kNone:
                    {
                        const string kNoneSceneGameSectionName = "None";
                        if (templateListData.conditionalWorkSets.ContainsKey(kNoneSceneGameSectionName))
                        {
                            templateListData.conditionalWorkSets[kNoneSceneGameSectionName].DoCopyWorks(templateProjectPath, projectRoot, filter);
                        }
                    }
                    break;

                case CreationOptions.SceneGenerationType.kEmptyScene:
                    {
                        const string kEmptySceneGameSectionName = "EmptyScene";
                        if (templateListData.conditionalWorkSets.ContainsKey(kEmptySceneGameSectionName))
                        {
                            templateListData.conditionalWorkSets[kEmptySceneGameSectionName].DoCopyWorks(templateProjectPath, projectRoot, filter);
                        }
                    }
                    break;

                case CreationOptions.SceneGenerationType.kUIBaseScene:
                    {
                        const string kUIBaseSceneGameSectionName = "UIBaseScene";
                        if (templateListData.conditionalWorkSets.ContainsKey(kUIBaseSceneGameSectionName))
                        {
                            templateListData.conditionalWorkSets[kUIBaseSceneGameSectionName].DoCopyWorks(templateProjectPath, projectRoot, filter);
                        }
                    }
                    break;
            }

            return true;
        }
    }
}
