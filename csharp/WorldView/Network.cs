using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace WorldView
{
    public partial class Network : Form
    {
        LocalService service = new LocalService();
        private unsafe void* svc = null;
        public Network()
        {
           InitializeComponent();
           unsafe
            {
                svc = service.Start(200, 0);
            }
         
        }

        private void Send_Click(object sender, EventArgs e)
        {
            byte cnt;

          if (dataToSend.Text != null)
            {
                
                string str = dataToSend.Text.Trim();
                //char[] p = str.ToCharArray(0, str.Length);
                //Encoding enc = Encoding.GetEncoding("Unicode");
                //System.Text.Encoding.GetEncoding("GB2312").GetBytes("字符串");

                byte[] p = System.Text.Encoding.GetEncoding("GB2312").GetBytes(str);
                //enc.GetBytes(str);
                lock (this)
                {
                    unsafe
                    {
                       
                        cnt = LocalService.svc_write(svc, p, (byte)(dataToSend.Text.Trim().Length), 0);
                    }

                }

                sent_number.Text = "已发送字符：" + cnt.ToString();
                sent_number.Enabled = true;
                dataToSend.Text = null;
            }
           
        }

        private void Network_Load(object sender, EventArgs e)
        {

        }

        private void revEnable_Click(object sender, EventArgs e)
        {
            if (!timer1.Enabled)
            {
                timer1.Start();
                timer1.Enabled = true;
                revEnable.Text = "停止接收";
            }
            else
            {
                timer1.Enabled = false;
                revEnable.Text = "启动接收";
            }

        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            byte cnt = 0;
            byte[] revData = new byte[100];
            unsafe
            {
                lock (this)
                {
                    //cnt = svc_read(svc,out DataReved.Text.ToCharArray(),(byte)DataReved.Text.Length, 0);
                    fixed (byte* p = revData)
                    {
                        cnt = LocalService.svc_read(svc, p, 64, 0);
                    }
                }
            }

            if (cnt > 0)
            {
                //DataReved.Text = Encoding.UTF8.GetString(revData, 0, revData.Length);
                DataReved.Text = Encoding.UTF8.GetString(revData, 0, revData.Length);
                revNumber.Text = "已接收字符：" + cnt.ToString();

                revNumber.Visible = true;
            }
            else
            {
                revNumber.Visible = false;
            }      
        }
    }
}