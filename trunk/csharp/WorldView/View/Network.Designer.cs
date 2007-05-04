namespace WorldView
{
    partial class Network
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
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
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.dataToSend = new System.Windows.Forms.RichTextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.DataReved = new System.Windows.Forms.RichTextBox();
            this.Send = new System.Windows.Forms.Button();
            this.revEnable = new System.Windows.Forms.Button();
            this.sent_number = new System.Windows.Forms.TextBox();
            this.revNumber = new System.Windows.Forms.TextBox();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // dataToSend
            // 
            this.dataToSend.Location = new System.Drawing.Point(95, 40);
            this.dataToSend.Name = "dataToSend";
            this.dataToSend.Size = new System.Drawing.Size(300, 96);
            this.dataToSend.TabIndex = 0;
            this.dataToSend.Text = "";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(34, 71);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(55, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "发送数据";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(34, 196);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(55, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "接收数据";
            // 
            // DataReved
            // 
            this.DataReved.Location = new System.Drawing.Point(97, 155);
            this.DataReved.Name = "DataReved";
            this.DataReved.Size = new System.Drawing.Size(298, 96);
            this.DataReved.TabIndex = 3;
            this.DataReved.Text = "";
            // 
            // Send
            // 
            this.Send.Location = new System.Drawing.Point(97, 293);
            this.Send.Name = "Send";
            this.Send.Size = new System.Drawing.Size(75, 23);
            this.Send.TabIndex = 4;
            this.Send.Text = "发送";
            this.Send.UseVisualStyleBackColor = true;
            this.Send.Click += new System.EventHandler(this.Send_Click);
            // 
            // revEnable
            // 
            this.revEnable.Location = new System.Drawing.Point(178, 293);
            this.revEnable.Name = "revEnable";
            this.revEnable.Size = new System.Drawing.Size(75, 23);
            this.revEnable.TabIndex = 5;
            this.revEnable.Text = "启动接收";
            this.revEnable.UseVisualStyleBackColor = true;
            this.revEnable.Click += new System.EventHandler(this.revEnable_Click);
            // 
            // sent_number
            // 
            this.sent_number.Location = new System.Drawing.Point(401, 71);
            this.sent_number.Name = "sent_number";
            this.sent_number.Size = new System.Drawing.Size(129, 20);
            this.sent_number.TabIndex = 6;
            // 
            // revNumber
            // 
            this.revNumber.Location = new System.Drawing.Point(401, 196);
            this.revNumber.Name = "revNumber";
            this.revNumber.Size = new System.Drawing.Size(129, 20);
            this.revNumber.TabIndex = 7;
            // 
            // timer1
            // 
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // Network
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(542, 406);
            this.Controls.Add(this.revNumber);
            this.Controls.Add(this.sent_number);
            this.Controls.Add(this.revEnable);
            this.Controls.Add(this.Send);
            this.Controls.Add(this.DataReved);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.dataToSend);
            this.Name = "Network";
            this.Text = "Network";
            this.Load += new System.EventHandler(this.Network_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.RichTextBox dataToSend;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.RichTextBox DataReved;
        private System.Windows.Forms.Button Send;
        private System.Windows.Forms.Button revEnable;
        private System.Windows.Forms.TextBox sent_number;
        private System.Windows.Forms.TextBox revNumber;
        private System.Windows.Forms.Timer timer1;
    }
}