using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Threading;

/*using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Windows.Forms;
*/




// @modified by zhangwei on 20070123
// for Zhou Songli
//
// - pls add a separate module as LocalService. this is a encapsulation of the DLL functions
// all the GUI should call the functions in LocalService rather call the DLL directly
//
// - namespace changed to WorldView
// - i also changed your local settings to adopt relative directory rather than absolute ones
// so others can easyliy use your project file.
//
namespace WorldView
{
    public struct RouteAddrItem
    {
        public ushort addr;
    } 

    public struct RouteAddr
    {
     public ushort srcAddr;
     public ushort dstAddr;//destionateion node address;   
     public  RouteAddrItem [] dstAddrItem ;
    }

    public struct TRoutePathCacheItem
    {
     public ushort nodeid;
     public ushort[] path; // path. we don't consider those exceed 15 hop 
     public byte length;   // path length
     public  bool  isOptimal;
     public  DateTime updateTime;
    }


    public  struct TRoutePathCache{
     public  TRoutePathCacheItem [] RoutePathItem;
     public  byte totalCnt;
    }


    public struct TSinkService
    {
     public ushort sinknode;
     public byte   maxhop;
     public UInt32 updateperiod;
     public TRoutePathCache pathCache;
    }


    public enum DataType
    {
        RouteRequest = 0x01,
        RouteFeedback,
        QueryData,
        QueryFeekback 
    } 

    public  struct PacketFrame{
  /* The FCF(frame control field) occupies one byte.It is an unsigned integer.The structure of the FCF  is defined as follow:

D3~D0 bits indicate the data type in the application. the range of the data value is between 0 and 15.

D7~D4 bits show the number of the hop in the route trace in this frame.The maximum value is 15 and the minimum value is 1.*/

     public   byte FrameControl;
     public   byte seqNumber;
     public   RouteAddr routeAddr;
     public   byte [] pData;//[100-sizeof(RouteAddr)];//里面的常数不能修改；
    }


    class LocalService
    {
        [DllImport("libsink", EntryPoint = "svc_read", CharSet = CharSet.Auto)]
        public unsafe static extern byte svc_read(void* svc, [In, Out]/* out*/ byte* buf, byte capacity, ushort opt);

        [DllImport("libsink", EntryPoint = "svc_write")]
        public unsafe static extern byte svc_write(void* svc, byte[] buf, byte len, ushort opt);

        [DllImport("libsink", EntryPoint = "svc_create")]
        public unsafe static extern void* svc_create(ushort id, ushort opt);

        [DllImport("libsink", EntryPoint = "svc_start")]
        public unsafe static extern void* svc_start(void* svc);

        [DllImport("libsink", EntryPoint = "svc_stop")]
        public unsafe static extern void* svc_stop(void* svc);
               
        [DllImport("libsink", EntryPoint = "svc_uart_open")]
         public unsafe static extern sbyte  svc_uart_open( void * svc, byte[] name);
        
        [DllImport("libsink", EntryPoint = "svc_uart_close")]
         public unsafe static extern sbyte svc_uart_close(void *svc);
        
        [DllImport("libsink", EntryPoint = "svc_uart_configure")]
        public unsafe static extern sbyte svc_uart_configure(void* svc, uint baudrate, byte databits, byte stopbits, byte parity, byte optflag);

        public static unsafe void* svc = null;
        public static UInt16 id, opt;

        public const byte MAX_DATA_REV_NUMBER = 0x0f;

        public const byte MAX_ROUTE_PATH_NUMBER = 200;

        public const byte MAX_ROUTE_PATH_OPTIMAL_NUMBER = 50;

        public const byte DATA_TYPE_BM = 0x0F;
        public const byte ADDRLIST_LENTH = 0x0F;

        public const byte ADDRLIST_BM = 0xF0;

        public const byte MAX_LEAP_NUMBER = 0x0a;
        public const byte MAX_ROUTE_LIST_NUMBER = 0x04;

        public static TSinkService Service = new TSinkService();

        public void Start()
        {
            if (id == 0)
            {
                id = 100;
                opt = 0;
            }

            unsafe
            {
                svc = null;
                svc = svc_create(id, opt);
                if (svc != null)
                    svc_start(svc);

            }         
         
            Service.maxhop = MAX_ROUTE_PATH_NUMBER;
            Service.pathCache.RoutePathItem = new TRoutePathCacheItem[MAX_ROUTE_PATH_NUMBER];
            for (byte i = 0; i < MAX_ROUTE_PATH_NUMBER; i++)
            {
                Service.pathCache.RoutePathItem[i].length = 0;
                Service.pathCache.RoutePathItem[i].nodeid = 0;
                Service.pathCache.RoutePathItem[i].isOptimal = false;
                Service.pathCache.RoutePathItem[i].path = new ushort[MAX_LEAP_NUMBER]; 
            }    
            
            return; 
        }

        
        public void Stop()
        {
            unsafe
            {
                svc_stop(svc);
            }
        
        }

 /* The command at the upper layer in this application occupies two bytes.It is 
an unsigned short in c shape. The structure of the command is defined as follow:

D3~D0 bits denote the data type in the application. the range of the data value is between 0 and 15.

D7~D4 bits denote the number of the hop in the route trace in this frame. The maximum value is 15 and 
the minimum value is 1.

D8 bit denotes whether optimum route trace is selected.It means the operations of read,write,update and
delete only work on the optimum route trace if the bit is set to 1. otherwise,these operations will work 
on all route trace.

D9 bit denotes whether the operations of read,write,update and delete only work on the appointed single node.

It means these operations only work on the appointed single node if the value of the bit is 1.otherwise, these
operations will work on all nodes.

D13~D10 bits are not used now and reserved for later version.
D15~D14 denotes the type of the command.
That the value of D15~D14 is 00 means the type of the command is read;
 01 means the type is add (insert);
 10 means the type is update;
 11 means the type is delete.
*/

        public int Read(byte[] buf, byte size, ushort opt)
        {
            byte cnt = 0;
            unsafe
            {

                lock (this)
                {

                    //cnt = svc_read(svc,out DataReved.Text.ToCharArray(),(byte)DataReved.Text.Length, 0);
                    fixed (byte* p = buf)
                    {
                        cnt = svc_read(svc, p, size, 0);
                    }

                }
            }

          /*  if (cnt > 0)
            {
              Encoding.UTF8.GetString(revData, 0, revData.Length);
              //  revNumber.Text = "已接收字符：" + cnt.ToString();
              //  revNumber.Visible = true;
            }
          */
            return cnt; 
        }
      
        public int Write(byte[] buf, byte size, ushort opt) 
        { 
            byte cnt;
            lock (this)
            {
                unsafe
                {
                    cnt = svc_write(svc, buf, size, opt);
                }                
            }
            
            return cnt; 
        }

        public int ReadDataPacket(byte[] packet, ushort size,ushort opt) { return 0; }
        public int WriteDataPacket(byte[] packet, ushort size, ushort opt) { return 0; }
        public int ReadRoutingPacket(byte[] packet, ushort size, ushort opt) { return 0; }
        public int GetSinkState() { return 0; }
        public int GetNodeData() { return 0; }
    }  
}
