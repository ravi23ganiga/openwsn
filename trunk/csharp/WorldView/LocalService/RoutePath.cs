using System;
using System.Collections.Generic;
using System.Text;

namespace WorldView
{
    class TRoutePathItem
    {
        private ushort srcNode;
        private ushort dstNode;
        private ushort[] leapStep; // path. we don't consider those exceed 15 hop 
        private int leaptotal;   // path length
        private bool isOptimal;
        private DateTime updateTime;

        private bool setleap(int index, ushort nodeid)
        {
            if (index < 0 || index > leaptotal - 1)
                return false;
            else
                leapStep[index] = nodeid;
            return true;
        }

        public void setSrcNode(ushort source) { srcNode = source; }
        public void setDestNode(ushort destination) { dstNode = destination; }

        //public interface function;
        public void construct(ushort source, ushort destination, int maxstep, bool opt)
        {
            setSrcNode(source);
            setDestNode(destination);
            leaptotal = 0;
            isOptimal = opt;
            updateTime = System.DateTime.Now;
            leapStep = new ushort[maxstep];
        }
        public ushort getSrcNode() { return (srcNode); }
        public ushort getDestNode() { return (dstNode); }
        public DateTime getRevTime() { return (updateTime); }
        public void setRevTime(DateTime time) { updateTime = time; }
       
        public int getleaptotal() { return (leaptotal); }
        public void setleaptotal(int total) { leaptotal = total; }
        public bool getOptimal() { return (isOptimal); }
        public void setOptimal(bool optimal) { isOptimal = optimal; }
     
        public ushort getLeapStep(int index)
        {
            ushort result = 0;
            if (index> getleaptotal() || index < 0) return result;
            return (leapStep[index]);
        }

        public void addleap(ushort leap)
        {
            leapStep[leapStep.Length] = leap;
        }



        public void reverse()
        {
            ushort nexthop = 0;
            //reverse the source nodeid and destinate nodeid;
            nexthop = srcNode;
            srcNode = dstNode;
            dstNode = nexthop;
            //get the total leap count;
            int totaleapcnt = getleaptotal();

            for (int index = 0; index < totaleapcnt / 2; index++)
            {
                nexthop = getLeapStep(index);
                setleap(totaleapcnt - index - 1, nexthop);
            }
        }       
    }


    class TRoutePathCache
    {
        private TRoutePathItem[] RoutePathItem;
        private byte cur_count;
        private byte max_count;


        //private functions:
        private bool copyPathItem(ref TRoutePathItem dest, TRoutePathItem source)
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

        private bool camparePathItem(TRoutePathItem item1, TRoutePathItem item2)
        {
            bool result = true;
            if (item1.getleaptotal() != item2.getleaptotal())
            {
                result = false;
            }
            else if (item1.getSrcNode() != item2.getSrcNode())
            {
                result = false;
            }
            else if (item1.getDestNode() != item2.getDestNode())
            {
                result = false;
            }
            else if (item1.getOptimal() != item2.getOptimal())
            {
                result = false;
            }
            else if (item1.getRevTime() != item2.getRevTime())
            {

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
            RoutePathItem = new TRoutePathItem[count];
            cur_count = 0;
            max_count = count;
        }
        public bool isOverFlow() { return (cur_count > max_count); }

        public byte itemCount() { return (cur_count);}

        public TRoutePathItem getPathItem(byte index)
        {
            return (RoutePathItem[index]);
        }

        public byte getindex(TRoutePathItem pathItem)
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

        public bool deletePathItem(TRoutePathItem pathItem)
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
                copyPathItem(ref RoutePathItem[index], RoutePathItem[index + 1]);
                index++;
            }
            cur_count--;
            return true;
        }

        public bool appendRoutePath(TRoutePathItem pathItem)
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

}
