using System;
using System.Collections.Generic;
using System.Text;

namespace WorldView
{
    class dataRevItem
    {
            public ushort nodeid;
            public DataType datatype;
            private byte[] contents;
            private ushort head;
            private ushort tail;
            private ushort len;
            private ushort MAX_CNT;
            //public  functions;
           
            public void construct(ushort node, ushort lenth)
            {
                head = 0;
                tail = 0;
                len = 0;
                MAX_CNT = lenth;
                nodeid = node;
                contents = new byte[lenth];
            }

            public ushort Length() { return (len);}
       
        //when opt equals 1,the data is cut to buf,otherwise,the data is copy to buf.
            public int Read(ref byte[] buf, ushort capacity, ushort opt)
            {
                ushort count =0;
                int index = 0;
                if (opt == 1)
                {
                    count = len < capacity ? len : capacity;
                    for (index = 0; index < count; index++)
                    {
                        buf[index] = contents[head++];
                        if (head > MAX_CNT - 1) head = 0;
                        len--;
                    }
                }
                else {
                    count = len;
                    ushort ptr = head;
                    for (index = 0; index < count; index++)
                    {
                        buf[index] = contents[ptr++];
                        if (ptr > MAX_CNT - 1) ptr = 0;                        
                    }                
                }
                return count;
            }

            public int Write(byte[] buf, ushort length, ushort opt)
            {
                int count = (len + length) > MAX_CNT ? (MAX_CNT - len) : length;
                for (int index = 0; index < count; index++)
                {
                    contents[tail++] = buf[index];
                    if (tail > MAX_CNT - 1) tail = 0;
                    len++;
                }
                return count;
            }
        }

    class dataRevCache
    {
        private dataRevItem[] dataRevItem;
        private byte cur_count;
        private byte max_count;

        //public functions:
        public void construct(byte count)
        {
            dataRevItem = new dataRevItem[count];
            cur_count = 0;
            max_count = count;
        }

        public bool isExist(ushort node, DataType type)
        {
            bool isFind = false;
            for (int index = 0; index < cur_count; index++)
            {
                if ((dataRevItem[index].datatype == type) &&
                    (dataRevItem[index].nodeid == node))
                {
                    isFind = true;
                    break;
                }
            }

            return isFind;
        }

        public bool isOverFlow() { return (cur_count > max_count); }

        public byte getcount() { return (cur_count);}

        public dataRevItem getDataItem(byte index)
        {
            return (dataRevItem[index]);
        }

        public byte getindex(dataRevItem dataItem)
        {
            byte index;
            bool isFind = false;
            for (index = 0; index < cur_count; index++)
            {
                if ((dataRevItem[index].nodeid == dataItem.nodeid) &&
                    (dataRevItem[index].datatype == dataItem.datatype))
                {
                    isFind = true;
                    break;
                }
            }

            if (!isFind) index = max_count;
            return index;
        }
     
       
        public bool deleteDataItem(dataRevItem dataItem)
        {
            byte[] tempdata = new byte[128];
            int count = 0;

            //if (pathItem == null) return false;
            byte index = getindex(dataItem);
            if (index == max_count) return true;//hasn't got the index;
            while (index < cur_count)
            {
                //dataRevItem[index] = dataRevItem[index +1];
                count = dataRevItem[index + 1].Read(ref tempdata, 128, 0);
                dataRevItem[index].Write(tempdata,(ushort) count, 0);
                index++;
            }

            cur_count--;
            return true;
        }

        public bool deleteItemByIndex(int index)
        {
            byte [] tempdata = new byte[128];
            int count = 0;
            if (index < 0 || index == max_count) return true;
            while (index < cur_count)
            {
                count = dataRevItem[index +1].Read(ref tempdata, 128, 0);
                dataRevItem[index].Write(tempdata,(ushort) count, 0);
                //dataRevItem[index] = dataRevItem[index + 1];
                index++;
            }
            cur_count--;
            return true;
        }

        public bool appendataItem(dataRevItem item)
        {
            byte [] tempdata = new byte[128];
            int count = 0;
            if (cur_count >= max_count) return false;
            count = item.Read(ref tempdata, 128, 0);
            byte index = getindex(item);
            if (index < cur_count)
            {
                dataRevItem[index].Write(tempdata, (ushort)count, 0);
            }
            else {
                dataRevItem[cur_count++].Write(tempdata, (ushort)count, 0);
            }
            return true;
        }
    }
  }

