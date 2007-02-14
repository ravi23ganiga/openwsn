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
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.sinknode = new System.Windows.Forms.ToolStripStatusLabel();
            this.queryTimer = new System.Windows.Forms.Timer(this.components);
            this.checkRouteTimer = new System.Windows.Forms.Timer(this.components);
            this.nodeList = new System.Windows.Forms.ComboBox();
            this.listViewDataRev = new System.Windows.Forms.ListView();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.listViewRouteInfo = new System.Windows.Forms.ListView();
            this.srcnodeid = new System.Windows.Forms.ColumnHeader();
            this.route = new System.Windows.Forms.ColumnHeader();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.sendCmd = new System.Windows.Forms.Button();
            this.dispData = new System.Windows.Forms.Button();
            this.dispRoute = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.cmdList = new System.Windows.Forms.ComboBox();
            this.stopRev = new System.Windows.Forms.Button();
            this.columnNodeid = new System.Windows.Forms.ColumnHeader();
            this.columnContent = new System.Windows.Forms.ColumnHeader();
            this.dstnodeid = new System.Windows.Forms.ColumnHeader();
            this.statusStrip1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.sinknode});
            this.statusStrip1.Location = new System.Drawing.Point(0, 439);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(500, 22);
            this.statusStrip1.TabIndex = 0;
            // 
            // sinknode
            // 
            this.sinknode.Name = "sinknode";
            this.sinknode.Size = new System.Drawing.Size(0, 17);
            // 
            // queryTimer
            // 
            this.queryTimer.Interval = 1000;
            this.queryTimer.Tick += new System.EventHandler(this.queryTimer_Tick);
            // 
            // checkRouteTimer
            // 
            this.checkRouteTimer.Interval = 5000;
            // 
            // nodeList
            // 
            this.nodeList.FormattingEnabled = true;
            this.nodeList.Location = new System.Drawing.Point(98, 378);
            this.nodeList.Name = "nodeList";
            this.nodeList.Size = new System.Drawing.Size(75, 21);
            this.nodeList.TabIndex = 1;
            // 
            // listViewDataRev
            // 
            this.listViewDataRev.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnNodeid,
            this.columnContent});
            this.listViewDataRev.FullRowSelect = true;
            this.listViewDataRev.GridLines = true;
            this.listViewDataRev.Location = new System.Drawing.Point(42, 41);
            this.listViewDataRev.Name = "listViewDataRev";
            this.listViewDataRev.Size = new System.Drawing.Size(439, 146);
            this.listViewDataRev.Sorting = System.Windows.Forms.SortOrder.Ascending;
            this.listViewDataRev.TabIndex = 3;
            this.listViewDataRev.UseCompatibleStateImageBehavior = false;
            this.listViewDataRev.View = System.Windows.Forms.View.Details;
            // 
            // groupBox1
            // 
            this.groupBox1.Location = new System.Drawing.Point(42, 25);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(439, 162);
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
            this.listViewRouteInfo.Size = new System.Drawing.Size(439, 144);
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
            // route
            // 
            this.route.Text = "路由信息";
            this.route.Width = 358;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.listViewRouteInfo);
            this.groupBox2.Location = new System.Drawing.Point(42, 194);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(439, 158);
            this.groupBox2.TabIndex = 6;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "路由信息";
            // 
            // sendCmd
            // 
            this.sendCmd.Location = new System.Drawing.Point(190, 403);
            this.sendCmd.Name = "sendCmd";
            this.sendCmd.Size = new System.Drawing.Size(99, 23);
            this.sendCmd.TabIndex = 7;
            this.sendCmd.Text = "发布命令";
            this.sendCmd.UseVisualStyleBackColor = true;
            this.sendCmd.Click += new System.EventHandler(this.sendCmd_Click);
            // 
            // dispData
            // 
            this.dispData.Location = new System.Drawing.Point(191, 378);
            this.dispData.Name = "dispData";
            this.dispData.Size = new System.Drawing.Size(99, 23);
            this.dispData.TabIndex = 10;
            this.dispData.Text = "显示采集数据";
            this.dispData.UseVisualStyleBackColor = true;
            this.dispData.Click += new System.EventHandler(this.dispData_Click);
            // 
            // dispRoute
            // 
            this.dispRoute.Location = new System.Drawing.Point(289, 378);
            this.dispRoute.Name = "dispRoute";
            this.dispRoute.Size = new System.Drawing.Size(87, 23);
            this.dispRoute.TabIndex = 9;
            this.dispRoute.Text = "显示路由";
            this.dispRoute.UseVisualStyleBackColor = true;
            this.dispRoute.Click += new System.EventHandler(this.dispRoute_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(41, 381);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(55, 13);
            this.label1.TabIndex = 11;
            this.label1.Text = "节点选择";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(41, 408);
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
            this.cmdList.Location = new System.Drawing.Point(98, 405);
            this.cmdList.Name = "cmdList";
            this.cmdList.Size = new System.Drawing.Size(75, 21);
            this.cmdList.TabIndex = 13;
            // 
            // stopRev
            // 
            this.stopRev.Location = new System.Drawing.Point(289, 403);
            this.stopRev.Name = "stopRev";
            this.stopRev.Size = new System.Drawing.Size(85, 23);
            this.stopRev.TabIndex = 14;
            this.stopRev.Text = " 停止接收";
            this.stopRev.UseVisualStyleBackColor = true;
            this.stopRev.Click += new System.EventHandler(this.stopRev_Click);
            // 
            // columnNodeid
            // 
            this.columnNodeid.Text = "节点号";
            this.columnNodeid.Width = 92;
            // 
            // columnContent
            // 
            this.columnContent.Text = "内容";
            this.columnContent.Width = 333;
            // 
            // dstnodeid
            // 
            this.dstnodeid.Text = "目标节点号";
            this.dstnodeid.Width = 82;
            // 
            // vibrationMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(500, 461);
            this.Controls.Add(this.stopRev);
            this.Controls.Add(this.cmdList);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.dispRoute);
            this.Controls.Add(this.sendCmd);
            this.Controls.Add(this.dispData);
            this.Controls.Add(this.nodeList);
            this.Controls.Add(this.listViewDataRev);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.groupBox2);
            this.Name = "vibrationMain";
            this.Text = "vibrationMain";
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
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
        private System.Windows.Forms.ToolStripStatusLabel sinknode;
        private System.Windows.Forms.Button stopRev;
        public System.Windows.Forms.StatusStrip statusStrip1;
        public System.Windows.Forms.ColumnHeader columnNodeid;
        public System.Windows.Forms.ColumnHeader columnContent;
        private System.Windows.Forms.ColumnHeader dstnodeid;
    }
}