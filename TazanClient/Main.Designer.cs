namespace TazanClient
{
    partial class Main
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다.
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.renderTimer = new System.Windows.Forms.Timer(this.components);
            this.textBox_CurrentClientCount = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.textBox_ControledDirection = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.textBox_ControledPos = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.textBox_ControledUserID = new System.Windows.Forms.TextBox();
            this.label11 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.textBox_SelectedDirection = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.textBox_SelectedPos = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.textBox_SelectedUserID = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.button_SelectedControl = new System.Windows.Forms.Button();
            this.radioButton_AutoSetting = new System.Windows.Forms.RadioButton();
            this.radioButton_ManualSetting = new System.Windows.Forms.RadioButton();
            this.button_Stop = new System.Windows.Forms.Button();
            this.button_AI = new System.Windows.Forms.Button();
            this.button_Broadcast = new System.Windows.Forms.Button();
            this.button_Echo = new System.Windows.Forms.Button();
            this.textBox_Status = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.textBox_MaxClientCount = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.textBox_AverageElapsed = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.textBox_SendBufferSize = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.textBox_SendPerASecond = new System.Windows.Forms.TextBox();
            this.panel_Render = new TazanClient.DXPanel();
            this.groupBox1.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // renderTimer
            // 
            this.renderTimer.Tick += new System.EventHandler(this.renderTimer_Tick);
            // 
            // textBox_CurrentClientCount
            // 
            this.textBox_CurrentClientCount.Location = new System.Drawing.Point(8, 71);
            this.textBox_CurrentClientCount.Name = "textBox_CurrentClientCount";
            this.textBox_CurrentClientCount.ReadOnly = true;
            this.textBox_CurrentClientCount.Size = new System.Drawing.Size(80, 21);
            this.textBox_CurrentClientCount.TabIndex = 3;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 56);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(168, 12);
            this.label1.TabIndex = 4;
            this.label1.Text = "Client Count (Current / Max)";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.groupBox3);
            this.groupBox1.Controls.Add(this.groupBox2);
            this.groupBox1.Controls.Add(this.radioButton_AutoSetting);
            this.groupBox1.Controls.Add(this.radioButton_ManualSetting);
            this.groupBox1.Controls.Add(this.button_Stop);
            this.groupBox1.Controls.Add(this.button_AI);
            this.groupBox1.Controls.Add(this.button_Broadcast);
            this.groupBox1.Controls.Add(this.button_Echo);
            this.groupBox1.Controls.Add(this.textBox_Status);
            this.groupBox1.Controls.Add(this.label5);
            this.groupBox1.Controls.Add(this.textBox_MaxClientCount);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.textBox_AverageElapsed);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.textBox_SendBufferSize);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.textBox_SendPerASecond);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.textBox_CurrentClientCount);
            this.groupBox1.Location = new System.Drawing.Point(606, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(182, 576);
            this.groupBox1.TabIndex = 5;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Clients Management";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.textBox_ControledDirection);
            this.groupBox3.Controls.Add(this.label9);
            this.groupBox3.Controls.Add(this.textBox_ControledPos);
            this.groupBox3.Controls.Add(this.label10);
            this.groupBox3.Controls.Add(this.textBox_ControledUserID);
            this.groupBox3.Controls.Add(this.label11);
            this.groupBox3.Location = new System.Drawing.Point(10, 345);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(164, 95);
            this.groupBox3.TabIndex = 29;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Controled Client";
            // 
            // textBox_ControledDirection
            // 
            this.textBox_ControledDirection.Location = new System.Drawing.Point(74, 68);
            this.textBox_ControledDirection.Name = "textBox_ControledDirection";
            this.textBox_ControledDirection.ReadOnly = true;
            this.textBox_ControledDirection.Size = new System.Drawing.Size(84, 21);
            this.textBox_ControledDirection.TabIndex = 28;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(6, 71);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(62, 12);
            this.label9.TabIndex = 27;
            this.label9.Text = "Direction :";
            // 
            // textBox_ControledPos
            // 
            this.textBox_ControledPos.Location = new System.Drawing.Point(74, 41);
            this.textBox_ControledPos.Name = "textBox_ControledPos";
            this.textBox_ControledPos.ReadOnly = true;
            this.textBox_ControledPos.Size = new System.Drawing.Size(84, 21);
            this.textBox_ControledPos.TabIndex = 26;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(6, 44);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(35, 12);
            this.label10.TabIndex = 25;
            this.label10.Text = "Pos :";
            // 
            // textBox_ControledUserID
            // 
            this.textBox_ControledUserID.Location = new System.Drawing.Point(74, 14);
            this.textBox_ControledUserID.Name = "textBox_ControledUserID";
            this.textBox_ControledUserID.ReadOnly = true;
            this.textBox_ControledUserID.Size = new System.Drawing.Size(84, 21);
            this.textBox_ControledUserID.TabIndex = 24;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(6, 17);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(50, 12);
            this.label11.TabIndex = 23;
            this.label11.Text = "UserID :";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.textBox_SelectedDirection);
            this.groupBox2.Controls.Add(this.label8);
            this.groupBox2.Controls.Add(this.textBox_SelectedPos);
            this.groupBox2.Controls.Add(this.label7);
            this.groupBox2.Controls.Add(this.textBox_SelectedUserID);
            this.groupBox2.Controls.Add(this.label6);
            this.groupBox2.Controls.Add(this.button_SelectedControl);
            this.groupBox2.Location = new System.Drawing.Point(10, 215);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(164, 124);
            this.groupBox2.TabIndex = 21;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Selected Client";
            // 
            // textBox_SelectedDirection
            // 
            this.textBox_SelectedDirection.Location = new System.Drawing.Point(74, 68);
            this.textBox_SelectedDirection.Name = "textBox_SelectedDirection";
            this.textBox_SelectedDirection.ReadOnly = true;
            this.textBox_SelectedDirection.Size = new System.Drawing.Size(84, 21);
            this.textBox_SelectedDirection.TabIndex = 28;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(6, 71);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(62, 12);
            this.label8.TabIndex = 27;
            this.label8.Text = "Direction :";
            // 
            // textBox_SelectedPos
            // 
            this.textBox_SelectedPos.Location = new System.Drawing.Point(74, 41);
            this.textBox_SelectedPos.Name = "textBox_SelectedPos";
            this.textBox_SelectedPos.ReadOnly = true;
            this.textBox_SelectedPos.Size = new System.Drawing.Size(84, 21);
            this.textBox_SelectedPos.TabIndex = 26;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(6, 44);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(35, 12);
            this.label7.TabIndex = 25;
            this.label7.Text = "Pos :";
            // 
            // textBox_SelectedUserID
            // 
            this.textBox_SelectedUserID.Location = new System.Drawing.Point(74, 14);
            this.textBox_SelectedUserID.Name = "textBox_SelectedUserID";
            this.textBox_SelectedUserID.ReadOnly = true;
            this.textBox_SelectedUserID.Size = new System.Drawing.Size(84, 21);
            this.textBox_SelectedUserID.TabIndex = 24;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(6, 17);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(50, 12);
            this.label6.TabIndex = 23;
            this.label6.Text = "UserID :";
            // 
            // button_SelectedControl
            // 
            this.button_SelectedControl.Location = new System.Drawing.Point(6, 95);
            this.button_SelectedControl.Name = "button_SelectedControl";
            this.button_SelectedControl.Size = new System.Drawing.Size(152, 23);
            this.button_SelectedControl.TabIndex = 22;
            this.button_SelectedControl.Text = "Control";
            this.button_SelectedControl.UseVisualStyleBackColor = true;
            // 
            // radioButton_AutoSetting
            // 
            this.radioButton_AutoSetting.AutoSize = true;
            this.radioButton_AutoSetting.Location = new System.Drawing.Point(8, 467);
            this.radioButton_AutoSetting.Name = "radioButton_AutoSetting";
            this.radioButton_AutoSetting.Size = new System.Drawing.Size(90, 16);
            this.radioButton_AutoSetting.TabIndex = 20;
            this.radioButton_AutoSetting.Text = "Auto Setting";
            this.radioButton_AutoSetting.UseVisualStyleBackColor = true;
            // 
            // radioButton_ManualSetting
            // 
            this.radioButton_ManualSetting.AutoSize = true;
            this.radioButton_ManualSetting.Checked = true;
            this.radioButton_ManualSetting.Location = new System.Drawing.Point(8, 445);
            this.radioButton_ManualSetting.Name = "radioButton_ManualSetting";
            this.radioButton_ManualSetting.Size = new System.Drawing.Size(107, 16);
            this.radioButton_ManualSetting.TabIndex = 19;
            this.radioButton_ManualSetting.TabStop = true;
            this.radioButton_ManualSetting.Text = "Manual Setting";
            this.radioButton_ManualSetting.UseVisualStyleBackColor = true;
            // 
            // button_Stop
            // 
            this.button_Stop.Location = new System.Drawing.Point(8, 547);
            this.button_Stop.Name = "button_Stop";
            this.button_Stop.Size = new System.Drawing.Size(170, 23);
            this.button_Stop.TabIndex = 18;
            this.button_Stop.Text = "Stop";
            this.button_Stop.UseVisualStyleBackColor = true;
            this.button_Stop.Click += new System.EventHandler(this.button_Stop_Click);
            // 
            // button_AI
            // 
            this.button_AI.Location = new System.Drawing.Point(8, 518);
            this.button_AI.Name = "button_AI";
            this.button_AI.Size = new System.Drawing.Size(170, 23);
            this.button_AI.TabIndex = 17;
            this.button_AI.Text = "AI";
            this.button_AI.UseVisualStyleBackColor = true;
            // 
            // button_Broadcast
            // 
            this.button_Broadcast.Location = new System.Drawing.Point(98, 489);
            this.button_Broadcast.Name = "button_Broadcast";
            this.button_Broadcast.Size = new System.Drawing.Size(80, 23);
            this.button_Broadcast.TabIndex = 16;
            this.button_Broadcast.Text = "Broadcast";
            this.button_Broadcast.UseVisualStyleBackColor = true;
            // 
            // button_Echo
            // 
            this.button_Echo.Location = new System.Drawing.Point(8, 489);
            this.button_Echo.Name = "button_Echo";
            this.button_Echo.Size = new System.Drawing.Size(80, 23);
            this.button_Echo.TabIndex = 15;
            this.button_Echo.Text = "Echo";
            this.button_Echo.UseVisualStyleBackColor = true;
            // 
            // textBox_Status
            // 
            this.textBox_Status.Location = new System.Drawing.Point(8, 32);
            this.textBox_Status.Name = "textBox_Status";
            this.textBox_Status.ReadOnly = true;
            this.textBox_Status.Size = new System.Drawing.Size(170, 21);
            this.textBox_Status.TabIndex = 13;
            this.textBox_Status.Text = "Stop";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(6, 17);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(40, 12);
            this.label5.TabIndex = 12;
            this.label5.Text = "Status";
            // 
            // textBox_MaxClientCount
            // 
            this.textBox_MaxClientCount.Location = new System.Drawing.Point(98, 71);
            this.textBox_MaxClientCount.Name = "textBox_MaxClientCount";
            this.textBox_MaxClientCount.Size = new System.Drawing.Size(80, 21);
            this.textBox_MaxClientCount.TabIndex = 11;
            this.textBox_MaxClientCount.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textbox_OnlyNum_KeyPress);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(6, 173);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(101, 12);
            this.label4.TabIndex = 10;
            this.label4.Text = "Average Elapsed";
            // 
            // textBox_AverageElapsed
            // 
            this.textBox_AverageElapsed.Location = new System.Drawing.Point(8, 188);
            this.textBox_AverageElapsed.Name = "textBox_AverageElapsed";
            this.textBox_AverageElapsed.ReadOnly = true;
            this.textBox_AverageElapsed.Size = new System.Drawing.Size(170, 21);
            this.textBox_AverageElapsed.TabIndex = 9;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(6, 134);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(99, 12);
            this.label3.TabIndex = 8;
            this.label3.Text = "Send Buffer Size";
            // 
            // textBox_SendBufferSize
            // 
            this.textBox_SendBufferSize.Location = new System.Drawing.Point(8, 149);
            this.textBox_SendBufferSize.Name = "textBox_SendBufferSize";
            this.textBox_SendBufferSize.Size = new System.Drawing.Size(170, 21);
            this.textBox_SendBufferSize.TabIndex = 7;
            this.textBox_SendBufferSize.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textbox_OnlyNum_KeyPress);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 95);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(116, 12);
            this.label2.TabIndex = 6;
            this.label2.Text = "Send Per A Second";
            // 
            // textBox_SendPerASecond
            // 
            this.textBox_SendPerASecond.Location = new System.Drawing.Point(8, 110);
            this.textBox_SendPerASecond.Name = "textBox_SendPerASecond";
            this.textBox_SendPerASecond.Size = new System.Drawing.Size(170, 21);
            this.textBox_SendPerASecond.TabIndex = 5;
            this.textBox_SendPerASecond.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textbox_OnlyNum_KeyPress);
            // 
            // panel_Render
            // 
            this.panel_Render.BackColor = System.Drawing.SystemColors.Control;
            this.panel_Render.Location = new System.Drawing.Point(0, 0);
            this.panel_Render.Name = "panel_Render";
            this.panel_Render.Size = new System.Drawing.Size(600, 600);
            this.panel_Render.TabIndex = 2;
            this.panel_Render.MouseMove += new System.Windows.Forms.MouseEventHandler(this.panel_Render_MouseMove);
            // 
            // Main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 600);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.panel_Render);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "Main";
            this.Text = "Tazan Client Miner Ver";
            this.Load += new System.EventHandler(this.Main_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Timer renderTimer;
        private DXPanel panel_Render;
        private System.Windows.Forms.TextBox textBox_CurrentClientCount;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.TextBox textBox_ControledDirection;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox textBox_ControledPos;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox textBox_ControledUserID;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.TextBox textBox_SelectedDirection;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox textBox_SelectedPos;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox textBox_SelectedUserID;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button button_SelectedControl;
        private System.Windows.Forms.RadioButton radioButton_AutoSetting;
        private System.Windows.Forms.RadioButton radioButton_ManualSetting;
        private System.Windows.Forms.Button button_Stop;
        private System.Windows.Forms.Button button_AI;
        private System.Windows.Forms.Button button_Broadcast;
        private System.Windows.Forms.Button button_Echo;
        private System.Windows.Forms.TextBox textBox_Status;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox textBox_MaxClientCount;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox textBox_AverageElapsed;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox textBox_SendBufferSize;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textBox_SendPerASecond;

    }
}

