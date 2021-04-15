using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProjectCreatorCore
{
    /// <summary>
    /// Project Creating Environment Setting
    /// </summary>
    public class EnvironmentSetting
    {
        /// <summary>
        /// Engine root directory path.
        /// The root directory contains Engine and Tools directories.
        /// </summary>
        public string engineRootPath;

        /// <summary>
        /// Template data path.
        /// </summary>
        public string templatePath;
    }
}
