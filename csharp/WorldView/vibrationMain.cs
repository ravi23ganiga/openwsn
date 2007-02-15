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
              
           
           //ushort nodeid = service.getSink();
           //statusStrip1.Items.Add(nodeid.ToString());
           
        }

        private void dispData_Click(object sender, EventArgs e)
        {
                   
           //ListView view = new ListView();
           ListViewItem nodeid = listViewDataRev.Items.Add("nodeid", 0);
           ListViewItem datatype = listViewDataRev.Items.Add("datatype", 1);
           ListViewItem contents = listViewDataRev.Items.Add("contents", 2);
           byte [] tempdata = new byte[128];
           int len = 0;
           dataRevItem dataitem = new dataRevItem();
           
           for (byte index = 0; index < dataRevItemCnt; index++)
           {
              dataitem = revDataCache.getDataItem(index);               
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
           
            int len = 0;
            TRoutePathItem routeitem = new TRoutePathItem();
            listViewRouteInfo.Items.Clear();
            pathItemCnt = pathCache.itemCount(); 
            for (byte index = 0; index < pathItemCnt; index++)
            {
                routeitem = pathCache.getPathItem(index);
                
                int j =0;
                byte[] tempdata = new byte[128];
                
 
                    ListViewItem item = listViewRouteInfo.Items.Add(routeitem.getSrcNode().ToString(),0);
                    item.SubItems.Add(routeitem.getDestNode().ToString());
                    len = routeitem.getleaptotal();
                    for (int i = 0; i < len; i++)
                    {
                        tempdata[j++] = (byte) routeitem.getLeapStep(i);
                        tempdata[j++] = (byte)(routeitem.getLeapStep(i) >> 8);
                    }

                   //srcnodeid.SubItems.Add(routeitem.getSrcNode().ToString());
                   
                    if (len > 0)
                    {
                        string text = Encoding.UTF8.GetString(tempdata, 0, j);
                        item.SubItems.Add(text);
                    }
            }
            
        }

        
        private void sendCmd_Click(object sender, EventArgs e)
        {
     
            DataType cmdtype = new DataType();
            switch(cmdList.Text.Trim())
            {
              　case "获取Sink节点":
                    cmdtype = DataType.DATA_TYPE_GET_NODE_ID_REQUEST;
                    break;
                case "路由请求":
                    cmdtype = DataType.DATA_TYPE_ROUTE_REQUEST;
                    break;
                case "震动查询":
                    cmdtype = DataType.DATA_TYPE_VIBSENSOR_QUERY_REQUEST;
                    break;
                case "发送数据":
                    cmdtype = DataType.DataStream;
                    break;
            }
            byte [] packet = new byte[60];
            
            byte len =  service.generatePacketFrame(ref packet,service.payload,pathitem,cmdtype,0);
            service.Write(packet,len,0);

            if (!queryTimer.Enabled) queryTimer.Enabled = true;
        }

        private void listViewRouteInfo_SelectedIndexChanged(object sender, EventArgs e)
        {
            //LocalService.routpath = listViewRouteInfo.Items[listViewRouteInfo.Items.IndexOf(this.)];
            listViewRouteInfo.Items[0].GetSubItemAt(300,500);
        }

        private void stopRev_Click(object sender, EventArgs e)
        {
            if (queryTimer.Enabled) queryTimer.Enabled = false;
        }
    }
}