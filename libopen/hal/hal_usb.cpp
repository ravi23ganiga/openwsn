#include "hal_usb.h"

//----------------------------------------------------------------------------
// refer to: C++ Builder 访问 USB 口的方法 (Victor Chen)
// 编译环境: Borland C++ Builder 6.0, Windows Server 2003 DDK
// 运行环境: Win98/2000/XP/2003
//----------------------------------------------------------------------------

#define CONFIG_USE_WINDDK
#undef CONFIG_USE_WINDDK

#ifdef CONFIG_USE_WINDDK

#include <vcl.h>
#include <dir.h>
#include <setupapi.h>
#include "C:/WINDDK/3790/inc/ddk/w2k/usbdi.h"
#include "C:/WINDDK/3790/inc/ddk/w2k/devioctl.h"
#include <initguid.h>


//----------------------------------------------------------------------------
// USB 设备、USB 驱动、USB 应用程序
//
// 1.USB 设备硬件部分
//  a.这个硬件的标识是用的 Vender ID 和 Product ID, 即“厂家标识”和“产品标识”
//  b.这个硬件规定了各个 End Point (端点) 的性质, 读/写 及 类型 (Control/Interrupt/Bulk/Isochronous)
//  c.这个硬件的固件里面有 DeviceIoControl 的实现部分, 规定了这个函数的具体参数和动作
//
// 2.USB 设备驱动
// ①硬件接口
//  a.需要识别 Vender ID 和 Product ID
//  b.对每个 EndPoint 的每个 I/O 分配一个 Pipe, 并且起一个名字作为软件接口
//  c.做 DeviceIoControl 的接口
// ②软件接口
//  a.GUID, 驱动程序的标识, 每个驱动程序使用不同的 GUID, GUID 是识别驱动的, 与硬件无关 (驱动程序升级版本 GUID 不能修改)
//  b.硬件接口里面的 b: Pipe 名字是软件接口, 这个 Pipe 名字纯粹由驱动定义的, 和硬件无关, 升级驱动不能改 Pipe 的名字
//  c.硬件接口里面的 c 的各个参数也是软件的接口, 这些参数是由硬件带来的, 不是驱动规定的, 当然也可以在驱动里面转义, 隐藏设备的真实情况
// ③这个驱动程序是用 WinDDK 编译的, 可以用文本编辑器或其他开发工具的编辑器编程序代码, 然后调用 WinDDK 编译
//
// 3.读写 USB 口的程序
// ①与驱动的接口
//  a.利用驱动程序里面的 GUID 找出设备的文件名, 用 CreateFile 函数打开设备。我前面的程序里面的 OpenUsbDevice 就是这个作用
//  b.通过 a.得到的设备文件名和驱动程序里面的 Pipe 名打开 Pipe, 访问这个 Pipe 对应的 USB 端点 (读写数据)
//  c.使用 a.的 CreateFile 得到的句柄, 通过 DeviceIoControl 实现设备规定的动作
// ②有关需要的资料
//  a.Vender ID, Product ID 和 GUID 一般在驱动程序的 .inf 文件里面能看到, 如果找不到就需要和厂家联系
//  b.Pipe 的名字是驱动程序规定的, 需要有驱动程序的资料才能知道
//  c.DeviceIoControl 的参数需要有驱动程序的资料或者硬件资料才能知道
// ③这个程序一般用 C/C++ 直接编写, 如果使用其他语言(VB/PB等)需要调用 C/C++ 编的 DLL
//
//
// 其他相关内容:
//
// USB 驱动程序可以到注册表里面找到：
// "HKEY_LOCAL_MACHINE\\SYSTEM\\ControlSet001\\Enum\\USB\\Vid_厂家标识&Pid_产品标识\\驱动程序"
//
// 里面的 ClassGUID 就是驱动程序的 GUID 标识, 例如 {36FC9E60-C465-11CF-8056-444553540000}
// 相当于程序的: DEFINE_GUID(USB_DRIVER_GUID, 0x36FC9E60,0xC465,0x11CF,0x80,0x56,0x44,0x45,0x53,0x54,0x00,0x00);
// 另外在这个注册表键里面还可找到有关设备的其他描述, 例如 DeviceDesc = "USB Mass Storage Device" 等
//
// 请参考本站: 有关 BCB 访问 USB 口的讨论, 里面还有不少相关的内容。
// 	网址: http://www.cppfans.com/
//
//----------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 下面必须为驱动程序的 GUID 值, 这里我乱写的数
DEFINE_GUID(USB_DRIVER_GUID, 0x12345678,0xabcd,0x1122,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0x00);
//---------------------------------------------------------------------------
HANDLE OpenOneDevice(HDEVINFO hDvcInfo, PSP_INTERFACE_DEVICE_DATA DvcInfoData, char *sDevNameBuf)
{
  HANDLE hOut = INVALID_HANDLE_VALUE;

  ULONG  iReqLen = 0;
  SetupDiGetInterfaceDeviceDetail(hDvcInfo, DvcInfoData, NULL, 0, &iReqLen, NULL);

  ULONG iDevDataLen = iReqLen; //sizeof(SP_FNCLASS_DEVICE_DATA) + 512;
  PSP_INTERFACE_DEVICE_DETAIL_DATA pDevData = (PSP_INTERFACE_DEVICE_DETAIL_DATA)malloc(iDevDataLen);

  pDevData->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
  if(SetupDiGetInterfaceDeviceDetail(hDvcInfo, DvcInfoData, pDevData, iDevDataLen, &iReqLen, NULL))
   {
     strcpy(sDevNameBuf, pDevData->DevicePath);
     hOut = CreateFile(pDevData->DevicePath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
   }

  free(pDevData);
  return hOut;
}
//---------------------------------------------------------------------------
HANDLE OpenUsbDevice(const GUID *pGuid, char *sDevNameBuf)
{
  HANDLE hOut = INVALID_HANDLE_VALUE;

  HDEVINFO hDevInfo = SetupDiGetClassDevs(pGuid, NULL, NULL, DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);

  SP_INTERFACE_DEVICE_DATA deviceInfoData;
  deviceInfoData.cbSize = sizeof (SP_INTERFACE_DEVICE_DATA);

  ULONG nGuessCount = MAXLONG;
  for(ULONG iDevIndex=0; iDevIndex<nGuessCount; iDevIndex++)
   {
     if(SetupDiEnumDeviceInterfaces(hDevInfo, 0, pGuid, iDevIndex, &deviceInfoData))
      {
        if((hOut=OpenOneDevice(hDevInfo, &deviceInfoData, sDevNameBuf)) != INVALID_HANDLE_VALUE)
          break;
      }
     else if(GetLastError() == ERROR_NO_MORE_ITEMS) //No more items
      {
        break;
      }
   }
  SetupDiDestroyDeviceInfoList(hDevInfo);
  return hOut;
}
//---------------------------------------------------------------------------
bool GetUsbDeviceFileName(const GUID *pGuid, char *sDevNameBuf)
{
  HANDLE hDev = OpenUsbDevice(pGuid, sDevNameBuf);
  if(hDev != INVALID_HANDLE_VALUE)
   {
     CloseHandle(hDev);
     return true;
   }
  return false;
}
//---------------------------------------------------------------------------
HANDLE OpenMyDevice()
{
  char DeviceName[MAXPATH] = "";
  return OpenUsbDevice(&USB_DRIVER_GUID, DeviceName);
}
//---------------------------------------------------------------------------
HANDLE OpenMyDevPipe(const char *PipeName)
{
  char DeviceName[MAXPATH] = "";
  if(GetUsbDeviceFileName(&USB_DRIVER_GUID, DeviceName))
   {
     strcat(DeviceName,"\\");
     strcat(DeviceName,PipeName);
     return CreateFile(DeviceName, GENERIC_WRITE|GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
   }
  return INVALID_HANDLE_VALUE;
}
//---------------------------------------------------------------------------

有了上面的函数就可以访问 USB 口了:
//打开 USB 口读写, 由驱动程序的 Pipe 名确定

HANDLE hPipe = OpenMyDevPipe("MyPipe1"); //驱动程序里面的 Pipe 名, 对应访问某个端点的 I/O, 这里我乱写的, 需要与驱动一致
if(hPipe != INVALID_HANDLE_VALUE) //打开 Pipe 成功
 {
   ReadFile(hPipe, Buffer, BufSize, &nBytesRead, NULL); //从 hPipe 里读取数据到 Buffer 里
   //WriteFile(hPipe, Buffer, BytesToWrite, &nBytesWritten, NULL); //把 Buffer 里面的 BytesToWrite 字节写入 hPipe
   CloseHandle(hPipe);
 }

//使用 DeviceIoControl 访问 USB 设备

HANDLE hDevice = OpenMyDevice();
if(hDevice != INVALID_HANDLE_VALUE) //打开设备成功
 {
   //这些 DeviceIoControl 功能都是由设备定义的, 具体看设备和驱动的资料
   if(DeviceIoControl(hDevice, IOCTL_READ_xxxx, &IOBlock, sizeof(IOBLOCK), &c, 1, &nBytes, NULL))
    {
      //成功
    }
   CloseHandle(hDevice);
}

#endif // CONFIG_USE_WINDDK