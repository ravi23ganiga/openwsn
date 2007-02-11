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
   
        public vibrationMain()
        {
            InitializeComponent();           
            LocalService.seqNum = 0;
            service.Start();
        }

        private void vibrationMain_Load(object sender, EventArgs e)
        {

        }

        private void queryTimer_Tick(object sender, EventArgs e)
        {
           DataType datatype =  service.phaseDataRev();
           switch(datatype)
           {
               case DataType.GetSink:
                   statusStrip1.Items.Add();
                   break;
               case DataType.QueryFeekback:
                   break;
               case DataType.RouteFeedback:
                   break;
           }
        }

        private void dispData_Click(object sender, EventArgs e)
        {
           byte count = LocalService.revCache.getcount();
           //ListView view = new ListView();
           ListViewItem nodeid = listViewDataRev.Items.Add("nodeid", 0);
           ListViewItem datatype = listViewDataRev.Items.Add("datatype", 1);
           ListViewItem contents = listViewDataRev.Items.Add("contents", 2);
           byte [] tempdata = new byte[128];
           int len = 0;
           dataRevItem dataitem = new dataRevItem();
           
           for (byte index = 0; index < count; index++)
           {
              dataitem = LocalService.revCache.getDataItem(index);
              len = dataitem.Read(ref tempdata,128,0);
              if (len > 0)
              {
                  nodeid.SubItems.Add(dataitem.nodeid.ToString());
                  datatype.SubItems.Add(dataitem.datatype.ToString());
                  string text = Encoding.UTF8.GetString(tempdata,0,len);
                  contents.SubItems.Add(text);
              }
           }
        }

        private void dispRoute_Click(object sender, EventArgs e)
        {
            byte count = LocalService.pathCache.itemCount();
            
            //ListView view = new ListView();
            ListViewItem nodeid = listViewRouteInfo.Items.Add("nodeid", 0);
            ListViewItem contents = listViewRouteInfo.Items.Add("contents", 1);
          
            int len = 0;
            TRoutePathItem routeitem = new TRoutePathItem();

            for (byte index = 0; index < count; index++)
            {
                routeitem = LocalService.pathCache.getPathItem(index);
                int j =0;
                byte[] tempdata = new byte[128];
                
                for (int i = 0; i < routeitem.getleaptotal();i++)
                {
                    tempdata[j++] = (byte) routeitem.getLeapStep(i);
                    tempdata[j++] = (byte)(routeitem.getLeapStep(i) >> 8);
                }

                if (len > 0)
                {
                    nodeid.SubItems.Add(routeitem.getSrcNode().ToString());
                    string text = Encoding.UTF8.GetString(tempdata, 0, j);
                    contents.SubItems.Add(text);
                }
            }
        }

        
        private void sendCmd_Click(object sender, EventArgs e)
        {
            /*命令类型：获取Sink节点、路由请求、数据请求*/
            /*  RouteRequest = 0x01,
                RouteFeedback,
                QueryData,
                QueryFeekback，
                GetSink        
            */
            DataType cmdtype = new DataType();
            switch(cmdList.Text.Trim())
            {
                case "获取Sink节点":
                    cmdtype = DataType.GetSink;
                    break;
                case "路由请求":
                    cmdtype = DataType.RouteRequest;
                    break;
                case "数据请求":
                    cmdtype = DataType.QueryData;
                    break;
                case "发送数据":
                    cmdtype = DataType.DataStream;
                    break;
            }
            byte [] packet = new byte[60];
            
            byte len =  service.generatePacketFrame(ref packet,LocalService.payload,LocalService.routpath,cmdtype);
            service.Write(packet,len,0);

            if (!queryTimer.Enabled) queryTimer.Enabled = true;
        }

        private void listViewRouteInfo_SelectedIndexChanged(object sender, EventArgs e)
        {
            //LocalService.routpath = listViewRouteInfo.Items[listViewRouteInfo.Items.IndexOf(this.)];
        }
    }
}