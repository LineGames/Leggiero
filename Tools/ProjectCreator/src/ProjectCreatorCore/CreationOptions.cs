using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProjectCreatorCore
{
    /// <summary>
    /// Project Creation Options
    /// </summary>
    public class CreationOptions
    {
        #region Types

        /// <summary>
        /// Orientation of the App
        /// </summary>
        public enum AppOrientationType
        {
            kLandscape, 
            kPortrait,
        }

        /// <summary>
        /// Initial Scene Type to Create
        /// </summary>
        public enum SceneGenerationType
        {
            /// <summary>
            /// Do not create a scene. Just set a game object.
            /// </summary>
            kNone, 

            /// <summary>
            /// Create an empty scene.
            /// </summary>
            kEmptyScene, 

            /// <summary>
            /// Create an UI scene.
            /// </summary>
            kUIBaseScene, 
        }

        #endregion


        /// <summary>
        /// Name of the program (in development).
        /// Use only alphanumeric characters, and cannot start with numeric characters.
        /// </summary>
        public string programName = null;

        /// <summary>
        /// Project directory name.
        /// </summary>
        public string projectDirctoryName = null;

        /// <summary>
        /// Game title to show.
        /// App caption will be gameTitle.
        /// </summary>
        public string gameTitle = null;

        /// <summary>
        /// App identifier in iOS/Android platform.
        /// for example, LegToy's app identifier is com.leggiero.LegToy.
        /// Should fulfill union of requirements of both platforms (only contains alphanumeric, ...)
        /// </summary>
        public string appIdentifier = null;

        /// <summary>
        /// Orientation of the app.
        /// </summary>
        public AppOrientationType appOrientation = AppOrientationType.kLandscape;

        /// <summary>
        /// Initial scene structure to create.
        /// </summary>
        public SceneGenerationType creatingScene = SceneGenerationType.kEmptyScene;
    }
}
