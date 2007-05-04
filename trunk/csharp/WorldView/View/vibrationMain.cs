using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace WorldView
{
   
    public partial class vibrationMain : Form
    {
        LocalService service = new LocalService();
        private unsafe void* svc = null;
        private TRoutePathCache pathCache;
        private TRoutePathItem  pathitem;
        private byte pathItemCnt;
        
        private dataRevCache revDataCache;
        private byte dataRevItemCnt;
      
        public vibrationMain()
        {
            InitializeComponent();
            unsafe
            {
                svc = service.Start(100, 0);
            }
            pathCache = service.getPathCache();
            pathItemCnt = pathCache.itemCount();
            revDataCache = service.getRevDataCache();
            dataRevItemCnt = revDataCache.getcount();
            pathitem = new TRoutePathItem();
        }
       
        private void queryTimer_Tick(object sender, EventArgs e)
        {
           service.phaseDataRev();
           ushort node;
           TRoutePathItem item;
           for (byte index = 0; index < pathCache.itemCount(); index++)
           {
               item = pathCache.getPathItem(index);
               node = item.getSrcNode();
               if (!nodeList.Items.Contains(node.ToString())) nodeList.Items.Add(node.ToString());
           }    
        }

        private void dispData_Click(object sender, EventArgs e)
        {
           byte [] tempdata = new byte[128];
           int len = 0;
           string text = "";
           ushort result = 0;
           int temp = 0;           
           dataRevItem dataitem = new dataRevItem();
           dataRevItemCnt = revDataCache.getcount();
           for (byte index = 0; index < dataRevItemCnt; index++)
           {
              dataitem = revDataCache.getDataItem(index);               
              //len = dataitem.Read(ref tempdata,128,0);
              len = dataitem.Read(ref tempdata, 128, 1);
              if (len > 0)
              {
                  ListViewItem item = new ListViewItem();

                  if (listViewDataRev.Items.Count > 0)
                  {
                      item = listViewDataRev.FindItemWithText(dataitem.nodeid.ToString(), false, temp, true);
                      
                      if (item != null && item.Text != "")
                      {
                          if (item.Text != dataitem.nodeid.ToString())
                          {
                              item = listViewDataRev.Items.Add(dataitem.nodeid.ToString(), 0);
                          }                         
                      }
                      else
                      {
                         item = listViewDataRev.Items.Add(dataitem.nodeid.ToString(), 0);                          
                      }
                  }else {
                   
                      item = listViewDataRev.Items.Add(dataitem.nodeid.ToString(), 0);                                      
                  }                 

                  item.SubItems.Clear();
                  item.Text = dataitem.nodeid.ToString();
                  switch (dataitem.datatype)
                  {
                      case DataType.DATA_TYPE_LIGHTSENSOR_QUERY_ACK:
                          item.SubItems.Add("震动数据");
                          //text = "震动数据";
                          break;
                      case DataType.DATA_TYPE_STRAINSENSOR_QUERY_ACK:
                          break;
                  }

             
                  for (byte i = 0; i < len; i++)
                  {
                      result = tempdata[i];
                      result += tempdata[i++];
                      //text += Encoding.UTF8.GetString(tempdata, i, 2) + ",";
                      text += result.ToString() +",";
                  }

                 item.SubItems.Add(text);                
             }
           }
        }

        private void dispRoute_Click(object sender, EventArgs e)
        {
           
            int len = 0;
            TRoutePathItem routeitem = new TRoutePathItem();
            listViewRouteInfo.Items.Clear();
            pathItemCnt = pathCache.itemCount(); 
            for (byte index = 0; index < pathItemCnt; index++)
            {
                routeitem = pathCache.getPathItem(index);
                
                int j =0;
                byte[] tempdata = new byte[128];
                string text = "";
 
                ListViewItem item = listViewRouteInfo.Items.Add(routeitem.getSrcNode().ToString(),0);
                item.SubItems.Add(routeitem.getDestNode().ToString());
                len = routeitem.getleaptotal();

                for (int i = 0; i < len; i++)
                {
                   text += routeitem.getLeapStep(i).ToString() + ",";
                }

                if (text != "")
                {
                   byte [] leapBytes =  System.Text.Encoding.GetEncoding("GB2312").GetBytes(text);
                   text = Encoding.UTF8.GetString(leapBytes, 0, leapBytes.Length - 1); 
                }    
                item.SubItems.Add(text);

            }
            
        }

        
        private void sendCmd_Click(object sender, EventArgs e)
        {
            DataType cmdtype = new DataType();

                switch (cmdList.Text.Trim())
                {
                    case "获取Sink节点":
                        cmdtype = DataType.DATA_TYPE_GET_NODE_ID_REQUEST;
                        break;
                    case "路由请求":
                        cmdtype = DataType.DATA_TYPE_ROUTE_REQUEST;
                        break;
                    case "震动查询":
                        cmdtype = DataType.DATA_TYPE_LIGHTSENSOR_QUERY_REQUEST;
                        break;
                    case "发送数据":
                        cmdtype = DataType.DataStream;
                        break;
                }
                byte[] packet = new byte[60];

                if (pathitem.getSrcNode() == 0 && cmdtype == DataType.DATA_TYPE_LIGHTSENSOR_QUERY_REQUEST)
                    MessageBox.Show("请选择相应的路由路线。\n");

                byte len = service.generatePacketFrame(ref packet, service.payload, pathitem, cmdtype, 0);
                service.Write(packet, len, 0);

                if (!queryTimer.Enabled) queryTimer.Enabled = true;
          }
     

        private void listViewRouteInfo_SelectedIndexChanged(object sender, EventArgs e)
        {
            ushort srcnode, destnode;
            string routePathData,nextleap;
            int startindex,foundindex;
            ushort [] leaps = new ushort[15];
            byte[] leapBytes = new byte[100];
            byte totalleapCnt = 0;
            ListView.SelectedIndexCollection indexes = listViewRouteInfo.SelectedIndices;
            
            foreach ( int index in indexes )
		    {
                srcnode = System.Convert.ToUInt16(listViewRouteInfo.Items[index].SubItems[0].Text);
                destnode = System.Convert.ToUInt16(listViewRouteInfo.Items[index].SubItems[1].Text);
               
                routePathData = listViewRouteInfo.Items[index].SubItems[2].Text;
                if (routePathData != "")
                {
                    startindex = 0;
                    foundindex = 0;
                    while (startindex < routePathData.Length)
                    {
                        foundindex = routePathData.IndexOf(',', startindex, routePathData.Length - startindex);
                        if (foundindex < 0)//没有找到“，”；
                        {
                            break;
                        }

                        leapBytes = System.Text.Encoding.GetEncoding("GB2312").GetBytes(routePathData);
                        nextleap = Encoding.UTF8.GetString(leapBytes, startindex, foundindex - startindex);
                        leaps[totalleapCnt++] = System.Convert.ToUInt16(nextleap);
                        startindex = foundindex;
                        foundindex = 0;
                    }

                    leapBytes = System.Text.Encoding.GetEncoding("GB2312").GetBytes(routePathData.ToCharArray(), startindex, routePathData.Length - startindex);
                    nextleap = Encoding.UTF8.GetString(leapBytes, startindex, routePathData.Length - startindex);
                    if (nextleap != "")
                        leaps[totalleapCnt++] = System.Convert.ToUInt16(nextleap);

                    pathitem.construct(srcnode, destnode, totalleapCnt, true);
                    for (int i = 0; i < totalleapCnt; i++)
                    {
                        pathitem.addleap(leaps[i]);
                    }
                }
                else {

                    pathitem.construct(srcnode, destnode, totalleapCnt, true);
                }
		    }
        }

        private void stopRev_Click(object sender, EventArgs e)
        {
            if (queryTimer.Enabled) queryTimer.Enabled = false;
        }

        private void checkRouteTimer_Tick(object sender, EventArgs e)
        {
            this.sendCmd.Click += new System.EventHandler(this.sendCmd_Click);
        }


        private void timer_trace_Tick(object sender, EventArgs e)
        {

            DataType cmdtype = new DataType();
            switch (cmdList.Text.Trim())
            {
                case "获取Sink节点":
                    cmdtype = DataType.DATA_TYPE_GET_NODE_ID_REQUEST;
                    break;
                case "路由请求":
                    cmdtype = DataType.DATA_TYPE_ROUTE_REQUEST;
                    break;
                case "震动查询":
                    cmdtype = DataType.DATA_TYPE_LIGHTSENSOR_QUERY_REQUEST;
                    break;
                case "发送数据":
                    cmdtype = DataType.DataStream;
                    break;
            }
            byte[] packet = new byte[60];

            if (pathitem.getSrcNode() == 0 && cmdtype == DataType.DATA_TYPE_LIGHTSENSOR_QUERY_REQUEST)
                MessageBox.Show("请选择相应的路由路线。\n");

            byte len = service.generatePacketFrame(ref packet, service.payload, pathitem, cmdtype, 0);
            service.Write(packet, len, 0);
            if (!queryTimer.Enabled) queryTimer.Enabled = true;
         
        }

        private void check_periodical_cmd_sent_CheckedChanged(object sender, EventArgs e)
        {
            if (maskedTextBox_send_cmd_period.Text == "") return;
            Int32 timer_period = System.Convert.ToInt32(maskedTextBox_send_cmd_period.Text.Trim());
            if (check_periodical_cmd_sent.Checked)
            {

                if (timer_period < 50)
                {
                    MessageBox.Show("选择的命令发送周期太短，请输入50以上的值。");
                }

                if (timer_cmd_send_period.Enabled)
                {
                    timer_cmd_send_period.Enabled = false;
                }

                timer_cmd_send_period.Interval = timer_period;
                timer_cmd_send_period.Enabled = true;
            }
            else { 

                if (timer_cmd_send_period.Enabled)
                    timer_cmd_send_period.Enabled = false;
            
            }
        }

        private void checkBox_periodical_route_retrieve_CheckedChanged(object sender, EventArgs e)
        {
            if (maskedTextBox_route_retrieve_period.Text == "") return;
            Int32 timer_period  = System.Convert.ToInt32(maskedTextBox_route_retrieve_period.Text.Trim());
            if (checkBox_periodical_route_retrieve.Checked)
            {

                if (timer_period < 50)
                {
                    MessageBox.Show("选择的命令发送周期太短，请输入50以上的值。");
                }

                if (timer_route_retrieve_period.Enabled)
                {
                    timer_route_retrieve_period.Enabled = false;
                }

                timer_route_retrieve_period.Interval = timer_period;
                timer_route_retrieve_period.Enabled = true;
            }
            else {

                if (timer_route_retrieve_period.Enabled)
                    timer_route_retrieve_period.Enabled = false;
            
            }

        }

        private void checkBox_periodical_data_retrieve_CheckedChanged(object sender, EventArgs e)
        {
            if (maskedTextBox_data_retrieve_period.Text == "") return;
            Int32 timer_period = System.Convert.ToInt32(maskedTextBox_data_retrieve_period.Text.Trim());
            if (checkBox_periodical_data_retrieve.Checked)
            {

                if (timer_period < 50)
                {
                    MessageBox.Show("选择的命令发送周期太短，请输入50以上的值。");
                }

                if (timer_data_retrieve_period.Enabled)
                {
                    timer_data_retrieve_period.Enabled = false;
                }

                timer_data_retrieve_period.Interval = timer_period;
                timer_data_retrieve_period.Enabled = true;
            }
            else
            {

                if (timer_data_retrieve_period.Enabled)
                    timer_data_retrieve_period.Enabled = false;

            }

        }

        private void timer_route_retrieve_period_Tick(object sender, EventArgs e)
        {
            //调用“显示路由”按钮的clicked事件代码；

            int len = 0;
            TRoutePathItem routeitem = new TRoutePathItem();
            listViewRouteInfo.Items.Clear();
            pathItemCnt = pathCache.itemCount();
            for (byte index = 0; index < pathItemCnt; index++)
            {
                routeitem = pathCache.getPathItem(index);

                int j = 0;
                byte[] tempdata = new byte[128];
                string text = "";

                ListViewItem item = listViewRouteInfo.Items.Add(routeitem.getSrcNode().ToString(), 0);
                item.SubItems.Add(routeitem.getDestNode().ToString());
                len = routeitem.getleaptotal();

                for (int i = 0; i < len; i++)
                {
                    text += routeitem.getLeapStep(i).ToString() + ",";
                }

                if (text != "")
                {
                    byte[] leapBytes = System.Text.Encoding.GetEncoding("GB2312").GetBytes(text);
                    text = Encoding.UTF8.GetString(leapBytes, 0, leapBytes.Length - 1);
                }
                item.SubItems.Add(text);

            }
        }

        private void timer_data_retrieve_period_Tick(object sender, EventArgs e)
        {
            //调用“显示数据”按钮的clicked事件代码；
            byte[] tempdata = new byte[128];
            int len = 0;
            string text = "";
            ushort result = 0;
            int temp = 0;
            dataRevItem dataitem = new dataRevItem();
            dataRevItemCnt = revDataCache.getcount();
            for (byte index = 0; index < dataRevItemCnt; index++)
            {
                dataitem = revDataCache.getDataItem(index);
                //len = dataitem.Read(ref tempdata,128,0);
                len = dataitem.Read(ref tempdata, 128, 1);
                if (len > 0)
                {
                    ListViewItem item = new ListViewItem();

                    if (listViewDataRev.Items.Count > 0)
                    {
                        item = listViewDataRev.FindItemWithText(dataitem.nodeid.ToString(), false, temp, true);

                        if (item != null && item.Text != "")
                        {
                            if (item.Text != dataitem.nodeid.ToString())
                            {
                                item = listViewDataRev.Items.Add(dataitem.nodeid.ToString(), 0);
                            }
                        }
                        else
                        {
                            item = listViewDataRev.Items.Add(dataitem.nodeid.ToString(), 0);
                        }
                    }
                    else
                    {

                        item = listViewDataRev.Items.Add(dataitem.nodeid.ToString(), 0);
                    }

                    item.SubItems.Clear();
                    item.Text = dataitem.nodeid.ToString();
                    switch (dataitem.datatype)
                    {
                        case DataType.DATA_TYPE_LIGHTSENSOR_QUERY_ACK:
                            item.SubItems.Add("震动数据");
                            //text = "震动数据";
                            break;
                        case DataType.DATA_TYPE_STRAINSENSOR_QUERY_ACK:
                            break;
                    }


                    for (byte i = 0; i < len; i++)
                    {
                        result = tempdata[i];
                        result += tempdata[i++];
                        //text += Encoding.UTF8.GetString(tempdata, i, 2) + ",";
                        text += result.ToString() + ",";
                    }

                    item.SubItems.Add(text);
                }
            }

        }
    }
}