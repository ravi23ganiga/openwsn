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
   /* 
    public struct RouteAddr
    {
        private ushort srcAddr;
        private ushort dstAddr;//destionateion node address;   
        private ushort [] leapStep;        
        private byte steptotal;
        
        //public interface function;
        public void setSrcAddr(ushort source) { srcAddr = source;}
        public void setDestAddr(ushort destination) { dstAddr = destination; }
        public ushort getSrcAddr() {return (srcAddr);}
        public ushort getDestAddr() {return (dstAddr);}
        
        public ushort getIndexAddr(byte index) 
        {
            ushort result = 0;
            if (index > totaleap) return result;
            return (leapStep[index]);
        }
        public void addleap(ushort leap)
        {
            leapStep[steptotal++] = leap;
        }
    }
    */

    public struct TRoutePathCacheItem
    {
        private ushort srcNode;
        private ushort dstNode;
        private ushort[] leapStep; // path. we don't consider those exceed 15 hop 
        private int leaptotal;   // path length
        private bool isOptimal;
        private DateTime updateTime;

        public void setSrcNode(ushort source) { srcNode = source; }
        public void setDestNode(ushort destination) { dstNode = destination; }

        //public interface function;
        public void construct(ushort source,ushort destination,int steptotal,bool opt)
        {
            setSrcNode(source);
            setDestNode(destination);                      
            leaptotal = steptotal;
            isOptimal = opt;
            updateTime = System.DateTime.Now;
            leapStep = new ushort[steptotal];
        }
        public ushort getSrcNode() { return (srcNode); }
        public ushort getDestNode() { return (dstNode); }
        public DateTime getRevTime() { return (updateTime);}
        public void setRevTime(DateTime time) { updateTime = time; }    

        public ushort getLeapStep(int index)
        {
            ushort result = 0;
            if (index > leapStep.Length || index <0) return result;
            return (leapStep[index]);
        }
      
        public void addleap(ushort leap)
        {
            leapStep[leapStep.Length] = leap;
        }

        public int getleaptotal() { return(leaptotal); }
        public void setleaptotal(int total) { leaptotal = total; }
        public bool getOptimal() { return(isOptimal);}
        public void setOptimal(bool optimal) { isOptimal = optimal;}
         
    }


    public  struct TRoutePathCache{
        private TRoutePathCacheItem [] RoutePathItem;
        private byte cur_count;
        private  byte max_count;
        

        //private functions:
        private bool copyPathItem(ref TRoutePathCacheItem dest, TRoutePathCacheItem source)
        {
            //if (source == null || dest == null) return false;
            dest.setSrcNode(source.getSrcNode());
            dest.setDestNode(source.getDestNode());
            dest.setOptimal(source.getOptimal());
            dest.setleaptotal(source.getleaptotal());

            for (int index = 0; index < source.getleaptotal(); index++)
            {
                dest.addleap(source.getLeapStep(index));
            }

            return true;
        }

        private bool camparePathItem(TRoutePathCacheItem item1, TRoutePathCacheItem item2)
        {
            bool result = true;
            if (item1.getleaptotal() != item2.getleaptotal())
            {
                result = false;
            }
            else if (item1.getSrcNode() != item2.getSrcNode()) {
                result = false;
            }
            else if (item1.getDestNode() != item2.getDestNode()){
                result = false;
            }
            else if (item1.getOptimal() != item2.getOptimal()) {
                result = false;
            }
            else if (item1.getRevTime() != item2.getRevTime()) {

                result = false;
            }

            if (result)
            {
                for (int index = 0; index < item1.getleaptotal(); index++)
                {
                    if (item1.getLeapStep(index) == item2.getLeapStep(index))
                        continue;
                    result = false;
                    break;
                }
            }
            return result;
        }

        //public functions:
        public void construct(byte count) 
        { 
          RoutePathItem = new TRoutePathCacheItem[count];
          cur_count = 0;
          max_count = count;
        }
        public bool isOverFlow() { return (cur_count >max_count); }

        public TRoutePathCacheItem getPathItem(byte index)
        {            
           return (RoutePathItem[index]);                 
        }
       
        public byte getindex(TRoutePathCacheItem pathItem) 
        {
            byte index;
            bool is_find = false;
            for (index = 0; index < cur_count; index++)
            {
                if (!camparePathItem(RoutePathItem[index], pathItem))
                    continue;
                is_find = true;
                break;
            }

            if (!is_find) index = max_count;
            return index;
        }

        public bool deletePathItem(TRoutePathCacheItem pathItem) 
        {
            //if (pathItem == null) return false;
            byte index = getindex(pathItem);
            if (index == max_count) return true;//hasn't got the index;
            while (index < cur_count)
            {
                copyPathItem(ref RoutePathItem[index], RoutePathItem[index + 1]);
                index++;
            }

            cur_count--;
            return true;
        }

        public bool deleteItemByIndex(int index)
        {
            if (index < 0 || index == max_count) return true;
            while (index < cur_count)
            {
                copyPathItem( ref RoutePathItem[index], RoutePathItem[index + 1]);
                index++;
            }
            cur_count--;
            return true;        
        }

        public bool appendRoutePath(TRoutePathCacheItem pathItem)
        {
            if (cur_count >= max_count) return false;
           
            byte index = getindex(pathItem);
            if (index < cur_count) return true;//has found the index;

            RoutePathItem[cur_count].construct(pathItem.getSrcNode(),
                pathItem.getDestNode(),
                pathItem.getleaptotal(),
                false);

            for (int i = 0; i < pathItem.getleaptotal(); i++)
                RoutePathItem[cur_count].addleap(pathItem.getLeapStep(i));
            cur_count++;
            return true;
        }        
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
       public   ushort srcNodeid;
       public   ushort destNodeid;
       public   ushort[] leapStep;   
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

        public const byte DATA_TYPE_MASK = 0x0F;
        public const byte DATA_TYPE_BM = 0x0;

        public const byte ROUTE_ADDRLIST_MASK = 0x0F;
        public const byte ROUTE_ADDRLIST_BM = 0x04;

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
            Service.pathCache.construct(MAX_ROUTE_PATH_NUMBER);
      
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


        public int ReadDataPacket(byte[] packet, ushort size,ushort opt) 
        {
            byte[] tempdata = new byte[128];
            int len = Read(tempdata,128,0);            
            ushort  nextleap = 0;
            int RouteleapNumber = 0;
            int i = 0,j = 0;
            if (len < 1) return 0;
            //check the datatype.
        
            DataType datatype = (DataType) (tempdata[i] & DATA_TYPE_MASK >> DATA_TYPE_BM);
            RouteleapNumber = tempdata[i++] & ROUTE_ADDRLIST_MASK >> ROUTE_ADDRLIST_BM;

            PacketFrame pframe = new PacketFrame();
            pframe.seqNumber = tempdata[i++];
            pframe.srcNodeid = tempdata[i++];
            pframe.srcNodeid += (ushort)(tempdata[i++] << 8);            
            pframe.destNodeid = tempdata[i++];
            pframe.destNodeid += (ushort)(tempdata[i++] << 8);
            
            for (j = 0; j < RouteleapNumber; j++)
            { 
                nextleap = tempdata[i++];
                nextleap += (ushort)(tempdata[i++] << 8);
                pframe.leapStep[j] = nextleap;
            }

            len = tempdata[i++];

            for (j = 0; j < len; j++)
            { 
            
                pframe.pData[j] = tempdata[i++];
            }
               
         
                switch (datatype)
                {
                    /*   case DataType.RouteRequest:
                           break;
                     */  

                    case DataType.RouteFeedback://路由反馈包                    
                        TRoutePathCacheItem pathitem = new TRoutePathCacheItem();
                        pathitem.construct(pframe.srcNodeid, pframe.destNodeid, RouteleapNumber, false);       
                      
                        for (i = 0; i < RouteleapNumber; i++)
                        {
                             nextleap = pframe.leapStep[i];
                             pathitem.addleap(nextleap);
                        }
                       
                        Service.pathCache.appendRoutePath(pathitem);
                        return 0;
                    case DataType.QueryData:
                        pframe.pData.CopyTo(packet,0);

                        break;
                    /*
                       case DataType.QueryFeekback:
                           break;
                    */
                }          
           
            return len; 
        }
        public int WriteDataPacket(byte[] packet, ushort size, ushort opt) { return 0; }
        public int ReadRoutingPacket(byte[] packet, ushort size, ushort opt) 
        { 
            return 0;
        }
        public int GetSinkState() { return 0; }
        public int GetNodeData() { return 0; }
    }  
}
