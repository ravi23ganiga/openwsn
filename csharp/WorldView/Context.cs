using System;
using System.Collections.Generic;
using System.Text;

// @author zhangwei on 20070211
// this class will hold all the necessary instances in the system.
//
namespace WorldView
{
    class Context
    {
        private LocalService m_localservice;

        public Context()
		{
            //m_localservice = new LocalService();
		}

        LocalService getLocalService()
        {
            return m_localservice;
        }
    }
}
