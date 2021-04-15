using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ProjectCreatorGUI
{
    public partial class frmMain : Form
    {
        public frmMain()
        {
            InitializeComponent();

            _FillInitialValues();
            lbliStatus.Text = string.Empty;
        }


        private void _FillInitialValues()
        {
            string currentDirectory = Directory.GetCurrentDirectory();
            DirectoryInfo currentDirInfo = new DirectoryInfo(currentDirectory);

            // Engine Directory as ../../
            DirectoryInfo engineDirInfo = currentDirInfo;
            if (engineDirInfo.Parent != null)
            {
                engineDirInfo = engineDirInfo.Parent;
            }
            if (engineDirInfo.Parent != null)
            {
                engineDirInfo = engineDirInfo.Parent;
            }
            txtEnginePath.Text = engineDirInfo.FullName;

            // Template Directory as ./Template
            DirectoryInfo templateDirInfo = currentDirInfo;
            DirectoryInfo[] matchedSubDirs = templateDirInfo.GetDirectories("Template");
            if (matchedSubDirs.Length > 0)
            {
                templateDirInfo = matchedSubDirs[0];
            }
            txtTemplatePath.Text = templateDirInfo.FullName;
        }


        private void btnBrowseEngine_Click(object sender, EventArgs e)
        {
            pathBrowser.SelectedPath = txtEnginePath.Text;
            if (pathBrowser.ShowDialog() == DialogResult.OK)
            {
                txtEnginePath.Text = pathBrowser.SelectedPath;
            }
        }

        private void btnBrowseTemplate_Click(object sender, EventArgs e)
        {
            pathBrowser.SelectedPath = txtTemplatePath.Text;
            if (pathBrowser.ShowDialog() == DialogResult.OK)
            {
                txtTemplatePath.Text = pathBrowser.SelectedPath;
            }
        }

        private void txtProgramName_Validated(object sender, EventArgs e)
        {
            string trimmedText = txtProgramName.Text.Trim();
            if (txtProgramName.Text != trimmedText)
            {
                txtProgramName.Text = trimmedText;
            }
            if (!Regex.IsMatch(txtProgramName.Text, "^[A-Za-z][0-9A-Za-z]*$"))
            {
                return;
            }

            if (string.IsNullOrWhiteSpace(txtGameTitle.Text))
            {
                txtGameTitle.Text = trimmedText;
            }
            if (string.IsNullOrWhiteSpace(txtProjectDir.Text))
            {
                txtProjectDir.Text = trimmedText;
            }
            if (string.IsNullOrWhiteSpace(txtAppId.Text))
            {
                txtAppId.Text = string.Format("com.yourcompany.{0}", trimmedText);
            }
        }

        private void txtProgramName_Validating(object sender, CancelEventArgs e)
        {
            if (!Regex.IsMatch(txtProgramName.Text, "^[A-Za-z][0-9A-Za-z]*$"))
            {
                if (txtProgramName.Text.Length > 0)
                {
                    e.Cancel = true;
                    txtProgramName.SelectAll();
                }
                lbliStatus.Text = "Program name should be an alphanumeric string without space starts with an alphabet.";
            }
        }

        private void btnCreate_Click(object sender, EventArgs e)
        {
            if (!_CheckOptions(true))
            {
                return;
            }

            ProjectCreatorCore.CreationOptions options = _CollectOptions();
            ProjectCreatorCore.EnvironmentSetting envs = _CollectEnvironments();

            lbliStatus.Text = "Creating project ...";

            bool isCreationSucceed = ProjectCreatorCore.ProjectCreator.CreateProject(options, envs);
            if (!isCreationSucceed)
            {
                lbliStatus.Text = "ERROR: Project creation FAILED.";
                MessageBox.Show("Error!\nAn error occured during project creation.", "Creation Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            lbliStatus.Text = string.Format("Created a new Leggiero game project [{0}]", options.gameTitle);
            MessageBox.Show(string.Format("Created a new Leggiero game project [{0}]", options.gameTitle), "Success", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }


        private bool _CheckOptions(bool isShowError)
        {
            if (!Regex.IsMatch(txtProgramName.Text, "^[A-Za-z][0-9A-Za-z]*$"))
            {
                if (isShowError)
                {
                    MessageBox.Show("Error!\nProgram name should be an alphanumeric string without space starts with an alphabet.", "Option Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                return false;
            }

            if (string.IsNullOrWhiteSpace(txtGameTitle.Text))
            {
                if (isShowError)
                {
                    MessageBox.Show("Error!\nNo game title specified.", "Option Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                return false;
            }

            if (string.IsNullOrWhiteSpace(txtAppId.Text))
            {
                if (isShowError)
                {
                    MessageBox.Show("Error!\nNo app identifier specified.", "Option Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                return false;
            }
            else if (!Regex.IsMatch(txtAppId.Text, "^([A-Za-z][0-9A-Za-z]*)(\\.([A-Za-z][0-9A-Za-z]*))+$"))
            {
                if (isShowError)
                {
                    MessageBox.Show("Error!\nInvalid app identifier.\nApp Id should be fulfill both iOS and Android Id requirements.", "Option Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                return false;
            }

            if (string.IsNullOrWhiteSpace(txtEnginePath.Text))
            {
                if (isShowError)
                {
                    MessageBox.Show("Error!\nNo engine root path specified.", "Option Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                return false;
            }
            string engineRootPath = Path.GetFullPath(txtEnginePath.Text);
            if (!Directory.Exists(engineRootPath))
            {
                if (isShowError)
                {
                    MessageBox.Show("Error!\nEngine root directory NOT exists.", "Option Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                return false;
            }

            if (string.IsNullOrWhiteSpace(txtTemplatePath.Text))
            {
                if (isShowError)
                {
                    MessageBox.Show("Error!\nNo template data path specified.", "Option Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                return false;
            }
            string templateDataPath = Path.GetFullPath(txtTemplatePath.Text);
            if (!Directory.Exists(templateDataPath))
            {
                if (isShowError)
                {
                    MessageBox.Show("Error!\nTemplate data directory NOT exists.", "Option Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                return false;
            }

            return true;
        }

        private ProjectCreatorCore.CreationOptions _CollectOptions()
        {
            ProjectCreatorCore.CreationOptions readingOptions = new ProjectCreatorCore.CreationOptions();

            readingOptions.programName = txtProgramName.Text.Trim();
            readingOptions.appIdentifier = txtAppId.Text.Trim();
            readingOptions.gameTitle = txtGameTitle.Text.Trim();
            readingOptions.projectDirctoryName = txtProjectDir.Text.Trim();

            if (radioOrientLandscape.Checked)
            {
                readingOptions.appOrientation = ProjectCreatorCore.CreationOptions.AppOrientationType.kLandscape;
            }
            else if (radioOrientPortrait.Checked)
            {
                readingOptions.appOrientation = ProjectCreatorCore.CreationOptions.AppOrientationType.kPortrait;
            }

            if (radioSceneEmpty.Checked)
            {
                readingOptions.creatingScene = ProjectCreatorCore.CreationOptions.SceneGenerationType.kEmptyScene;
            }
            else if (radioSceneNo.Checked)
            {
                readingOptions.creatingScene = ProjectCreatorCore.CreationOptions.SceneGenerationType.kNone;
            }
            else if (radioSceneUI.Checked)
            {
                readingOptions.creatingScene = ProjectCreatorCore.CreationOptions.SceneGenerationType.kUIBaseScene;
            }

            return readingOptions;
        }

        private ProjectCreatorCore.EnvironmentSetting _CollectEnvironments()
        {
            ProjectCreatorCore.EnvironmentSetting readingEnvs = new ProjectCreatorCore.EnvironmentSetting();

            readingEnvs.engineRootPath = txtEnginePath.Text.Trim();
            readingEnvs.templatePath = txtTemplatePath.Text.Trim();

            return readingEnvs;
        }
    }
}
