using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Threading;

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
    class LocalService
    {
        [DllImport("libsink", EntryPoint = "svc_read")]
        public unsafe static extern byte svc_read(void* svc, char[] buf, byte capacity, UInt16 opt);

        [DllImport("libsink", EntryPoint = "svc_write")]
        public unsafe static extern byte svc_write(void* svc, char[] buf, byte len, UInt16 opt);

        [DllImport("libsink", EntryPoint = "svc_create")]
        public unsafe static extern void* svc_create(UInt16 id, UInt16 opt);

        [DllImport("libsink", EntryPoint = "svc_start")]
        public unsafe static extern void* svc_start(void* svc);

        [DllImport("libsink", EntryPoint = "svc_stop")]
        public unsafe static extern void* svc_stop(void* svc);

        public void Start()
        { 
            // svc_start
            return; 
        }
        public void Stop(){}
        public void Read(){}
        public void Write(){}
    }
}
