using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProjectCreatorCore
{
    /// <summary>
    /// Project Creation Context
    /// </summary>
    internal class CreationContext
    {
        public CreationContext()
        {

        }

        public string guid_solution;
        public string guid_gameProject;
        public string guid_appAndroid;
        public string guid_appWinPC;


        public void InitializeGeneratingValues()
        {
            guid_solution = Guid.NewGuid().ToString().ToUpper();
            guid_gameProject = Guid.NewGuid().ToString().ToLower();
            guid_appAndroid = Guid.NewGuid().ToString().ToUpper();
            guid_appWinPC = Guid.NewGuid().ToString().ToUpper();
        }
    }
}
