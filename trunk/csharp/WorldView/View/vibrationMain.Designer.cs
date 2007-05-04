namespace WorldView
{
    partial class vibrationMain
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
            this.queryTimer = new System.Windows.Forms.Timer(this.components);
            this.checkRouteTimer = new System.Windows.Forms.Timer(this.components);
            this.nodeList = new System.Windows.Forms.ComboBox();
            this.listViewDataRev = new System.Windows.Forms.ListView();
            this.columnNodeid = new System.Windows.Forms.ColumnHeader();
            this.datatype = new System.Windows.Forms.ColumnHeader();
            this.columnContent = new System.Windows.Forms.ColumnHeader();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.listViewRouteInfo = new System.Windows.Forms.ListView();
            this.srcnodeid = new System.Windows.Forms.ColumnHeader();
            this.dstnodeid = new System.Windows.Forms.ColumnHeader();
            this.route = new System.Windows.Forms.ColumnHeader();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.sendCmd = new System.Windows.Forms.Button();
            this.dispData = new System.Windows.Forms.Button();
            this.dispRoute = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.cmdList = new System.Windows.Forms.ComboBox();
            this.stopRev = new System.Windows.Forms.Button();
            this.timer_cmd_send_period = new System.Windows.Forms.Timer(this.components);
            this.check_periodical_cmd_sent = new System.Windows.Forms.CheckBox();
            this.label3 = new System.Windows.Forms.Label();
            this.maskedTextBox_send_cmd_period = new System.Windows.Forms.MaskedTextBox();
            this.maskedTextBox_route_retrieve_period = new System.Windows.Forms.MaskedTextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.checkBox_periodical_route_retrieve = new System.Windows.Forms.CheckBox();
            this.maskedTextBox_data_retrieve_period = new System.Windows.Forms.MaskedTextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.checkBox_periodical_data_retrieve = new System.Windows.Forms.CheckBox();
            this.timer_route_retrieve_period = new System.Windows.Forms.Timer(this.components);
            this.timer_data_retrieve_period = new System.Windows.Forms.Timer(this.components);
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // queryTimer
            // 
            this.queryTimer.Interval = 1000;
            this.queryTimer.Tick += new System.EventHandler(this.queryTimer_Tick);
            // 
            // checkRouteTimer
            // 
            this.checkRouteTimer.Interval = 5000;
            this.checkRouteTimer.Tick += new System.EventHandler(this.checkRouteTimer_Tick);
            // 
            // nodeList
            // 
            this.nodeList.FormattingEnabled = true;
            this.nodeList.Location = new System.Drawing.Point(511, 38);
            this.nodeList.Name = "nodeList";
            this.nodeList.Size = new System.Drawing.Size(75, 21);
            this.nodeList.TabIndex = 1;
            // 
            // listViewDataRev
            // 
            this.listViewDataRev.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnNodeid,
            this.datatype,
            this.columnContent});
            this.listViewDataRev.FullRowSelect = true;
            this.listViewDataRev.GridLines = true;
            this.listViewDataRev.Location = new System.Drawing.Point(5, 15);
            this.listViewDataRev.Name = "listViewDataRev";
            this.listViewDataRev.Size = new System.Drawing.Size(390, 146);
            this.listViewDataRev.Sorting = System.Windows.Forms.SortOrder.Ascending;
            this.listViewDataRev.TabIndex = 3;
            this.listViewDataRev.UseCompatibleStateImageBehavior = false;
            this.listViewDataRev.View = System.Windows.Forms.View.Details;
            // 
            // columnNodeid
            // 
            this.columnNodeid.Text = "节点号";
            this.columnNodeid.Width = 92;
            // 
            // datatype
            // 
            this.datatype.Text = "数据类型";
            this.datatype.Width = 74;
            // 
            // columnContent
            // 
            this.columnContent.Text = "内容";
            this.columnContent.Width = 215;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.listViewDataRev);
            this.groupBox1.Location = new System.Drawing.Point(42, 26);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(402, 162);
            this.groupBox1.TabIndex = 4;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "数据采集信息";
            // 
            // listViewRouteInfo
            // 
            this.listViewRouteInfo.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.srcnodeid,
            this.dstnodeid,
            this.route});
            this.listViewRouteInfo.FullRowSelect = true;
            this.listViewRouteInfo.GridLines = true;
            this.listViewRouteInfo.Location = new System.Drawing.Point(0, 14);
            this.listViewRouteInfo.Name = "listViewRouteInfo";
            this.listViewRouteInfo.Size = new System.Drawing.Size(396, 159);
            this.listViewRouteInfo.TabIndex = 5;
            this.listViewRouteInfo.UseCompatibleStateImageBehavior = false;
            this.listViewRouteInfo.View = System.Windows.Forms.View.Details;
            this.listViewRouteInfo.SelectedIndexChanged += new System.EventHandler(this.listViewRouteInfo_SelectedIndexChanged);
            // 
            // srcnodeid
            // 
            this.srcnodeid.Text = "源节点号";
            this.srcnodeid.Width = 66;
            // 
            // dstnodeid
            // 
            this.dstnodeid.Text = "目标节点号";
            this.dstnodeid.Width = 82;
            // 
            // route
            // 
            this.route.Text = "路由信息";
            this.route.Width = 244;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.listViewRouteInfo);
            this.groupBox2.Location = new System.Drawing.Point(42, 194);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(402, 173);
            this.groupBox2.TabIndex = 6;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "路由信息";
            // 
            // sendCmd
            // 
            this.sendCmd.Location = new System.Drawing.Point(483, 275);
            this.sendCmd.Name = "sendCmd";
            this.sendCmd.Size = new System.Drawing.Size(99, 23);
            this.sendCmd.TabIndex = 7;
            this.sendCmd.Text = "发布命令";
            this.sendCmd.UseVisualStyleBackColor = true;
            this.sendCmd.Click += new System.EventHandler(this.sendCmd_Click);
            // 
            // dispData
            // 
            this.dispData.Location = new System.Drawing.Point(484, 346);
            this.dispData.Name = "dispData";
            this.dispData.Size = new System.Drawing.Size(99, 23);
            this.dispData.TabIndex = 10;
            this.dispData.Text = "显示采集数据";
            this.dispData.UseVisualStyleBackColor = true;
            this.dispData.Click += new System.EventHandler(this.dispData_Click);
            // 
            // dispRoute
            // 
            this.dispRoute.Location = new System.Drawing.Point(483, 323);
            this.dispRoute.Name = "dispRoute";
            this.dispRoute.Size = new System.Drawing.Size(99, 23);
            this.dispRoute.TabIndex = 9;
            this.dispRoute.Text = "显示路由列表";
            this.dispRoute.UseVisualStyleBackColor = true;
            this.dispRoute.Click += new System.EventHandler(this.dispRoute_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(454, 41);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(55, 13);
            this.label1.TabIndex = 11;
            this.label1.Text = "节点选择";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(454, 82);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(55, 13);
            this.label2.TabIndex = 12;
            this.label2.Text = "命令选择";
            // 
            // cmdList
            // 
            this.cmdList.FormattingEnabled = true;
            this.cmdList.Items.AddRange(new object[] {
            "获取Sink节点",
            "路由请求",
            "震动查询",
            "发送数据"});
            this.cmdList.Location = new System.Drawing.Point(511, 79);
            this.cmdList.Name = "cmdList";
            this.cmdList.Size = new System.Drawing.Size(75, 21);
            this.cmdList.TabIndex = 13;
            // 
            // stopRev
            // 
            this.stopRev.Location = new System.Drawing.Point(483, 299);
            this.stopRev.Name = "stopRev";
            this.stopRev.Size = new System.Drawing.Size(99, 23);
            this.stopRev.TabIndex = 14;
            this.stopRev.Text = " 停止接收";
            this.stopRev.UseVisualStyleBackColor = true;
            this.stopRev.Click += new System.EventHandler(this.stopRev_Click);
            // 
            // timer_cmd_send_period
            // 
            this.timer_cmd_send_period.Interval = 2000;
            this.timer_cmd_send_period.Tick += new System.EventHandler(this.timer_trace_Tick);
            // 
            // check_periodical_cmd_sent
            // 
            this.check_periodical_cmd_sent.AutoSize = true;
            this.check_periodical_cmd_sent.Checked = global::WorldView.Properties.Settings.Default.check1;
            this.check_periodical_cmd_sent.DataBindings.Add(new System.Windows.Forms.Binding("Checked", global::WorldView.Properties.Settings.Default, "check1", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.check_periodical_cmd_sent.Location = new System.Drawing.Point(466, 116);
            this.check_periodical_cmd_sent.Name = "check_periodical_cmd_sent";
            this.check_periodical_cmd_sent.Size = new System.Drawing.Size(110, 17);
            this.check_periodical_cmd_sent.TabIndex = 16;
            this.check_periodical_cmd_sent.Text = "命令周期性发送";
            this.check_periodical_cmd_sent.UseVisualStyleBackColor = true;
            this.check_periodical_cmd_sent.CheckedChanged += new System.EventHandler(this.check_periodical_cmd_sent_CheckedChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(456, 141);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(74, 13);
            this.label3.TabIndex = 17;
            this.label3.Text = "发送周期(ms)";
            // 
            // maskedTextBox_send_cmd_period
            // 
            this.maskedTextBox_send_cmd_period.Location = new System.Drawing.Point(535, 138);
            this.maskedTextBox_send_cmd_period.Mask = "#####";
            this.maskedTextBox_send_cmd_period.Name = "maskedTextBox_send_cmd_period";
            this.maskedTextBox_send_cmd_period.Size = new System.Drawing.Size(52, 20);
            this.maskedTextBox_send_cmd_period.TabIndex = 18;
            // 
            // maskedTextBox_route_retrieve_period
            // 
            this.maskedTextBox_route_retrieve_period.Location = new System.Drawing.Point(536, 189);
            this.maskedTextBox_route_retrieve_period.Mask = "#####";
            this.maskedTextBox_route_retrieve_period.Name = "maskedTextBox_route_retrieve_period";
            this.maskedTextBox_route_retrieve_period.Size = new System.Drawing.Size(52, 20);
            this.maskedTextBox_route_retrieve_period.TabIndex = 21;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(457, 192);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(74, 13);
            this.label4.TabIndex = 20;
            this.label4.Text = "刷新周期(ms)";
            // 
            // checkBox_periodical_route_retrieve
            // 
            this.checkBox_periodical_route_retrieve.AutoSize = true;
            this.checkBox_periodical_route_retrieve.Location = new System.Drawing.Point(467, 167);
            this.checkBox_periodical_route_retrieve.Name = "checkBox_periodical_route_retrieve";
            this.checkBox_periodical_route_retrieve.Size = new System.Drawing.Size(122, 17);
            this.checkBox_periodical_route_retrieve.TabIndex = 19;
            this.checkBox_periodical_route_retrieve.Text = "路由表周期性刷新";
            this.checkBox_periodical_route_retrieve.UseVisualStyleBackColor = true;
            this.checkBox_periodical_route_retrieve.CheckedChanged += new System.EventHandler(this.checkBox_periodical_route_retrieve_CheckedChanged);
            // 
            // maskedTextBox_data_retrieve_period
            // 
            this.maskedTextBox_data_retrieve_period.Location = new System.Drawing.Point(536, 240);
            this.maskedTextBox_data_retrieve_period.Mask = "#####";
            this.maskedTextBox_data_retrieve_period.Name = "maskedTextBox_data_retrieve_period";
            this.maskedTextBox_data_retrieve_period.Size = new System.Drawing.Size(52, 20);
            this.maskedTextBox_data_retrieve_period.TabIndex = 24;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(457, 243);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(74, 13);
            this.label5.TabIndex = 23;
            this.label5.Text = "刷新周期(ms)";
            // 
            // checkBox_periodical_data_retrieve
            // 
            this.checkBox_periodical_data_retrieve.AutoSize = true;
            this.checkBox_periodical_data_retrieve.Location = new System.Drawing.Point(467, 218);
            this.checkBox_periodical_data_retrieve.Name = "checkBox_periodical_data_retrieve";
            this.checkBox_periodical_data_retrieve.Size = new System.Drawing.Size(158, 17);
            this.checkBox_periodical_data_retrieve.TabIndex = 22;
            this.checkBox_periodical_data_retrieve.Text = "数据采集信息周期性刷新";
            this.checkBox_periodical_data_retrieve.UseVisualStyleBackColor = true;
            // 
            // timer_route_retrieve_period
            // 
            this.timer_route_retrieve_period.Interval = 2000;
            this.timer_route_retrieve_period.Tick += new System.EventHandler(this.timer_route_retrieve_period_Tick);
            // 
            // timer_data_retrieve_period
            // 
            this.timer_data_retrieve_period.Interval = 2000;
            this.timer_data_retrieve_period.Tick += new System.EventHandler(this.timer_data_retrieve_period_Tick);
            // 
            // vibrationMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(631, 404);
            this.Controls.Add(this.maskedTextBox_data_retrieve_period);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.checkBox_periodical_data_retrieve);
            this.Controls.Add(this.maskedTextBox_route_retrieve_period);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.checkBox_periodical_route_retrieve);
            this.Controls.Add(this.maskedTextBox_send_cmd_period);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.check_periodical_cmd_sent);
            this.Controls.Add(this.stopRev);
            this.Controls.Add(this.cmdList);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.dispRoute);
            this.Controls.Add(this.sendCmd);
            this.Controls.Add(this.dispData);
            this.Controls.Add(this.nodeList);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.groupBox2);
            this.Name = "vibrationMain";
            this.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.Text = "vibrationMain";
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Timer queryTimer;
        private System.Windows.Forms.Timer checkRouteTimer;
        private System.Windows.Forms.ComboBox nodeList;
        private System.Windows.Forms.ListView listViewDataRev;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ListView listViewRouteInfo;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button sendCmd;
        private System.Windows.Forms.Button dispData;
        private System.Windows.Forms.Button dispRoute;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox cmdList;
        public System.Windows.Forms.ColumnHeader srcnodeid;
        public System.Windows.Forms.ColumnHeader route;
        private System.Windows.Forms.Button stopRev;
        public System.Windows.Forms.ColumnHeader columnNodeid;
        public System.Windows.Forms.ColumnHeader columnContent;
        private System.Windows.Forms.ColumnHeader dstnodeid;
        public System.Windows.Forms.ColumnHeader datatype;
        private System.Windows.Forms.Timer timer_cmd_send_period;
        private System.Windows.Forms.CheckBox check_periodical_cmd_sent;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.MaskedTextBox maskedTextBox_send_cmd_period;
        private System.Windows.Forms.MaskedTextBox maskedTextBox_route_retrieve_period;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.CheckBox checkBox_periodical_route_retrieve;
        private System.Windows.Forms.MaskedTextBox maskedTextBox_data_retrieve_period;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.CheckBox checkBox_periodical_data_retrieve;
        private System.Windows.Forms.Timer timer_route_retrieve_period;
        private System.Windows.Forms.Timer timer_data_retrieve_period;
    }
}