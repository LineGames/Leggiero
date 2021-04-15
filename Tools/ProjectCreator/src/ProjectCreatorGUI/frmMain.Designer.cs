
namespace ProjectCreatorGUI
{
    partial class frmMain
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.grpEnv = new System.Windows.Forms.GroupBox();
            this.lblfTemplatePath = new System.Windows.Forms.Label();
            this.lblfEnginePath = new System.Windows.Forms.Label();
            this.btnBrowseTemplate = new System.Windows.Forms.Button();
            this.txtTemplatePath = new System.Windows.Forms.TextBox();
            this.btnBrowseEngine = new System.Windows.Forms.Button();
            this.txtEnginePath = new System.Windows.Forms.TextBox();
            this.grpOpt = new System.Windows.Forms.GroupBox();
            this.btnCreate = new System.Windows.Forms.Button();
            this.grpScene = new System.Windows.Forms.GroupBox();
            this.radioSceneUI = new System.Windows.Forms.RadioButton();
            this.radioSceneEmpty = new System.Windows.Forms.RadioButton();
            this.radioSceneNo = new System.Windows.Forms.RadioButton();
            this.grpOrientation = new System.Windows.Forms.GroupBox();
            this.radioOrientPortrait = new System.Windows.Forms.RadioButton();
            this.radioOrientLandscape = new System.Windows.Forms.RadioButton();
            this.txtProjectDir = new System.Windows.Forms.TextBox();
            this.lblfDir = new System.Windows.Forms.Label();
            this.lblfTitle = new System.Windows.Forms.Label();
            this.txtGameTitle = new System.Windows.Forms.TextBox();
            this.txtAppId = new System.Windows.Forms.TextBox();
            this.lblfAppId = new System.Windows.Forms.Label();
            this.txtProgramName = new System.Windows.Forms.TextBox();
            this.lblfName = new System.Windows.Forms.Label();
            this.pathBrowser = new System.Windows.Forms.FolderBrowserDialog();
            this.lbliStatus = new System.Windows.Forms.Label();
            this.grpEnv.SuspendLayout();
            this.grpOpt.SuspendLayout();
            this.grpScene.SuspendLayout();
            this.grpOrientation.SuspendLayout();
            this.SuspendLayout();
            // 
            // grpEnv
            // 
            this.grpEnv.Controls.Add(this.lblfTemplatePath);
            this.grpEnv.Controls.Add(this.lblfEnginePath);
            this.grpEnv.Controls.Add(this.btnBrowseTemplate);
            this.grpEnv.Controls.Add(this.txtTemplatePath);
            this.grpEnv.Controls.Add(this.btnBrowseEngine);
            this.grpEnv.Controls.Add(this.txtEnginePath);
            this.grpEnv.Location = new System.Drawing.Point(12, 12);
            this.grpEnv.Name = "grpEnv";
            this.grpEnv.Size = new System.Drawing.Size(754, 134);
            this.grpEnv.TabIndex = 2;
            this.grpEnv.TabStop = false;
            this.grpEnv.Text = "Environment";
            // 
            // lblfTemplatePath
            // 
            this.lblfTemplatePath.AutoSize = true;
            this.lblfTemplatePath.Location = new System.Drawing.Point(20, 83);
            this.lblfTemplatePath.Name = "lblfTemplatePath";
            this.lblfTemplatePath.Size = new System.Drawing.Size(135, 25);
            this.lblfTemplatePath.TabIndex = 5;
            this.lblfTemplatePath.Text = "Template Data:";
            // 
            // lblfEnginePath
            // 
            this.lblfEnginePath.AutoSize = true;
            this.lblfEnginePath.Location = new System.Drawing.Point(44, 39);
            this.lblfEnginePath.Name = "lblfEnginePath";
            this.lblfEnginePath.Size = new System.Drawing.Size(111, 25);
            this.lblfEnginePath.TabIndex = 4;
            this.lblfEnginePath.Text = "Engine Path:";
            // 
            // btnBrowseTemplate
            // 
            this.btnBrowseTemplate.Location = new System.Drawing.Point(618, 78);
            this.btnBrowseTemplate.Name = "btnBrowseTemplate";
            this.btnBrowseTemplate.Size = new System.Drawing.Size(112, 34);
            this.btnBrowseTemplate.TabIndex = 3;
            this.btnBrowseTemplate.Text = "Browse..";
            this.btnBrowseTemplate.UseVisualStyleBackColor = true;
            this.btnBrowseTemplate.Click += new System.EventHandler(this.btnBrowseTemplate_Click);
            // 
            // txtTemplatePath
            // 
            this.txtTemplatePath.Location = new System.Drawing.Point(161, 80);
            this.txtTemplatePath.Name = "txtTemplatePath";
            this.txtTemplatePath.Size = new System.Drawing.Size(451, 31);
            this.txtTemplatePath.TabIndex = 2;
            // 
            // btnBrowseEngine
            // 
            this.btnBrowseEngine.Location = new System.Drawing.Point(618, 34);
            this.btnBrowseEngine.Name = "btnBrowseEngine";
            this.btnBrowseEngine.Size = new System.Drawing.Size(112, 34);
            this.btnBrowseEngine.TabIndex = 1;
            this.btnBrowseEngine.Text = "Browse..";
            this.btnBrowseEngine.UseVisualStyleBackColor = true;
            this.btnBrowseEngine.Click += new System.EventHandler(this.btnBrowseEngine_Click);
            // 
            // txtEnginePath
            // 
            this.txtEnginePath.Location = new System.Drawing.Point(161, 36);
            this.txtEnginePath.Name = "txtEnginePath";
            this.txtEnginePath.Size = new System.Drawing.Size(451, 31);
            this.txtEnginePath.TabIndex = 0;
            // 
            // grpOpt
            // 
            this.grpOpt.Controls.Add(this.btnCreate);
            this.grpOpt.Controls.Add(this.grpScene);
            this.grpOpt.Controls.Add(this.grpOrientation);
            this.grpOpt.Controls.Add(this.txtProjectDir);
            this.grpOpt.Controls.Add(this.lblfDir);
            this.grpOpt.Controls.Add(this.lblfTitle);
            this.grpOpt.Controls.Add(this.txtGameTitle);
            this.grpOpt.Controls.Add(this.txtAppId);
            this.grpOpt.Controls.Add(this.lblfAppId);
            this.grpOpt.Controls.Add(this.txtProgramName);
            this.grpOpt.Controls.Add(this.lblfName);
            this.grpOpt.Location = new System.Drawing.Point(12, 152);
            this.grpOpt.Name = "grpOpt";
            this.grpOpt.Size = new System.Drawing.Size(754, 315);
            this.grpOpt.TabIndex = 3;
            this.grpOpt.TabStop = false;
            this.grpOpt.Text = "Options";
            // 
            // btnCreate
            // 
            this.btnCreate.Font = new System.Drawing.Font("맑은 고딕", 14F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.btnCreate.Location = new System.Drawing.Point(562, 221);
            this.btnCreate.Name = "btnCreate";
            this.btnCreate.Size = new System.Drawing.Size(168, 79);
            this.btnCreate.TabIndex = 10;
            this.btnCreate.Text = "Create";
            this.btnCreate.UseVisualStyleBackColor = true;
            this.btnCreate.Click += new System.EventHandler(this.btnCreate_Click);
            // 
            // grpScene
            // 
            this.grpScene.Controls.Add(this.radioSceneUI);
            this.grpScene.Controls.Add(this.radioSceneEmpty);
            this.grpScene.Controls.Add(this.radioSceneNo);
            this.grpScene.Location = new System.Drawing.Point(230, 150);
            this.grpScene.Name = "grpScene";
            this.grpScene.Size = new System.Drawing.Size(225, 150);
            this.grpScene.TabIndex = 9;
            this.grpScene.TabStop = false;
            this.grpScene.Text = "Scene Setting";
            // 
            // radioSceneUI
            // 
            this.radioSceneUI.AutoSize = true;
            this.radioSceneUI.Location = new System.Drawing.Point(30, 106);
            this.radioSceneUI.Name = "radioSceneUI";
            this.radioSceneUI.Size = new System.Drawing.Size(110, 29);
            this.radioSceneUI.TabIndex = 2;
            this.radioSceneUI.TabStop = true;
            this.radioSceneUI.Text = "UI Scene";
            this.radioSceneUI.UseVisualStyleBackColor = true;
            // 
            // radioSceneEmpty
            // 
            this.radioSceneEmpty.AutoSize = true;
            this.radioSceneEmpty.Checked = true;
            this.radioSceneEmpty.Location = new System.Drawing.Point(30, 71);
            this.radioSceneEmpty.Name = "radioSceneEmpty";
            this.radioSceneEmpty.Size = new System.Drawing.Size(143, 29);
            this.radioSceneEmpty.TabIndex = 1;
            this.radioSceneEmpty.TabStop = true;
            this.radioSceneEmpty.Text = "Empty Scene";
            this.radioSceneEmpty.UseVisualStyleBackColor = true;
            // 
            // radioSceneNo
            // 
            this.radioSceneNo.AutoSize = true;
            this.radioSceneNo.Location = new System.Drawing.Point(30, 36);
            this.radioSceneNo.Name = "radioSceneNo";
            this.radioSceneNo.Size = new System.Drawing.Size(117, 29);
            this.radioSceneNo.TabIndex = 0;
            this.radioSceneNo.TabStop = true;
            this.radioSceneNo.Text = "No Scene";
            this.radioSceneNo.UseVisualStyleBackColor = true;
            // 
            // grpOrientation
            // 
            this.grpOrientation.Controls.Add(this.radioOrientPortrait);
            this.grpOrientation.Controls.Add(this.radioOrientLandscape);
            this.grpOrientation.Location = new System.Drawing.Point(20, 150);
            this.grpOrientation.Name = "grpOrientation";
            this.grpOrientation.Size = new System.Drawing.Size(204, 150);
            this.grpOrientation.TabIndex = 8;
            this.grpOrientation.TabStop = false;
            this.grpOrientation.Text = "Orientation";
            // 
            // radioOrientPortrait
            // 
            this.radioOrientPortrait.AutoSize = true;
            this.radioOrientPortrait.Location = new System.Drawing.Point(30, 94);
            this.radioOrientPortrait.Name = "radioOrientPortrait";
            this.radioOrientPortrait.Size = new System.Drawing.Size(95, 29);
            this.radioOrientPortrait.TabIndex = 1;
            this.radioOrientPortrait.TabStop = true;
            this.radioOrientPortrait.Text = "Portrait";
            this.radioOrientPortrait.UseVisualStyleBackColor = true;
            // 
            // radioOrientLandscape
            // 
            this.radioOrientLandscape.AutoSize = true;
            this.radioOrientLandscape.Checked = true;
            this.radioOrientLandscape.Location = new System.Drawing.Point(30, 48);
            this.radioOrientLandscape.Name = "radioOrientLandscape";
            this.radioOrientLandscape.Size = new System.Drawing.Size(123, 29);
            this.radioOrientLandscape.TabIndex = 0;
            this.radioOrientLandscape.TabStop = true;
            this.radioOrientLandscape.Text = "Landscape";
            this.radioOrientLandscape.UseVisualStyleBackColor = true;
            // 
            // txtProjectDir
            // 
            this.txtProjectDir.Location = new System.Drawing.Point(562, 37);
            this.txtProjectDir.Name = "txtProjectDir";
            this.txtProjectDir.Size = new System.Drawing.Size(168, 31);
            this.txtProjectDir.TabIndex = 3;
            // 
            // lblfDir
            // 
            this.lblfDir.AutoSize = true;
            this.lblfDir.Location = new System.Drawing.Point(349, 40);
            this.lblfDir.Name = "lblfDir";
            this.lblfDir.Size = new System.Drawing.Size(207, 25);
            this.lblfDir.TabIndex = 0;
            this.lblfDir.Text = "Project Directory Name:";
            // 
            // lblfTitle
            // 
            this.lblfTitle.AutoSize = true;
            this.lblfTitle.Location = new System.Drawing.Point(51, 90);
            this.lblfTitle.Name = "lblfTitle";
            this.lblfTitle.Size = new System.Drawing.Size(104, 25);
            this.lblfTitle.TabIndex = 0;
            this.lblfTitle.Text = "Game Title:";
            // 
            // txtGameTitle
            // 
            this.txtGameTitle.Location = new System.Drawing.Point(161, 87);
            this.txtGameTitle.Name = "txtGameTitle";
            this.txtGameTitle.Size = new System.Drawing.Size(150, 31);
            this.txtGameTitle.TabIndex = 2;
            // 
            // txtAppId
            // 
            this.txtAppId.Location = new System.Drawing.Point(427, 87);
            this.txtAppId.Name = "txtAppId";
            this.txtAppId.Size = new System.Drawing.Size(303, 31);
            this.txtAppId.TabIndex = 4;
            // 
            // lblfAppId
            // 
            this.lblfAppId.AutoSize = true;
            this.lblfAppId.Location = new System.Drawing.Point(349, 90);
            this.lblfAppId.Name = "lblfAppId";
            this.lblfAppId.Size = new System.Drawing.Size(72, 25);
            this.lblfAppId.TabIndex = 0;
            this.lblfAppId.Text = "App Id:";
            // 
            // txtProgramName
            // 
            this.txtProgramName.Location = new System.Drawing.Point(161, 37);
            this.txtProgramName.Name = "txtProgramName";
            this.txtProgramName.Size = new System.Drawing.Size(150, 31);
            this.txtProgramName.TabIndex = 1;
            this.txtProgramName.Validating += new System.ComponentModel.CancelEventHandler(this.txtProgramName_Validating);
            this.txtProgramName.Validated += new System.EventHandler(this.txtProgramName_Validated);
            // 
            // lblfName
            // 
            this.lblfName.AutoSize = true;
            this.lblfName.Location = new System.Drawing.Point(15, 40);
            this.lblfName.Name = "lblfName";
            this.lblfName.Size = new System.Drawing.Size(140, 25);
            this.lblfName.TabIndex = 0;
            this.lblfName.Text = "Program Name:";
            // 
            // lbliStatus
            // 
            this.lbliStatus.AutoSize = true;
            this.lbliStatus.Location = new System.Drawing.Point(12, 491);
            this.lbliStatus.Name = "lbliStatus";
            this.lbliStatus.Size = new System.Drawing.Size(61, 25);
            this.lbliStatus.TabIndex = 4;
            this.lbliStatus.Text = "Status";
            // 
            // frmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 25F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(778, 544);
            this.Controls.Add(this.lbliStatus);
            this.Controls.Add(this.grpOpt);
            this.Controls.Add(this.grpEnv);
            this.Name = "frmMain";
            this.Text = "Leggiero Project Creator";
            this.grpEnv.ResumeLayout(false);
            this.grpEnv.PerformLayout();
            this.grpOpt.ResumeLayout(false);
            this.grpOpt.PerformLayout();
            this.grpScene.ResumeLayout(false);
            this.grpScene.PerformLayout();
            this.grpOrientation.ResumeLayout(false);
            this.grpOrientation.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox grpEnv;
        private System.Windows.Forms.GroupBox grpOpt;
        private System.Windows.Forms.Label lblfTemplatePath;
        private System.Windows.Forms.Label lblfEnginePath;
        private System.Windows.Forms.Button btnBrowseTemplate;
        private System.Windows.Forms.TextBox txtTemplatePath;
        private System.Windows.Forms.Button btnBrowseEngine;
        private System.Windows.Forms.TextBox txtEnginePath;
        private System.Windows.Forms.FolderBrowserDialog pathBrowser;
        private System.Windows.Forms.TextBox txtProjectDir;
        private System.Windows.Forms.Label lblfDir;
        private System.Windows.Forms.Label lblfTitle;
        private System.Windows.Forms.TextBox txtGameTitle;
        private System.Windows.Forms.TextBox txtAppId;
        private System.Windows.Forms.Label lblfAppId;
        private System.Windows.Forms.TextBox txtProgramName;
        private System.Windows.Forms.Label lblfName;
        private System.Windows.Forms.GroupBox grpScene;
        private System.Windows.Forms.RadioButton radioSceneUI;
        private System.Windows.Forms.RadioButton radioSceneEmpty;
        private System.Windows.Forms.RadioButton radioSceneNo;
        private System.Windows.Forms.GroupBox grpOrientation;
        private System.Windows.Forms.RadioButton radioOrientPortrait;
        private System.Windows.Forms.RadioButton radioOrientLandscape;
        private System.Windows.Forms.Button btnCreate;
        private System.Windows.Forms.Label lbliStatus;
    }
}

