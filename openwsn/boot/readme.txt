this directory contains target-related booting source code. 
it responsible for booting the system only. 

the bootloader usually contains some hardware manipulation source code similar to "hal". but when the system in booting, the "hal" object may not constructed yet. so usually you should implement them in boot itself. for example, the bootloader will initialize UART for some purpose, while, after booting successfully, the system will create TUart object defined in "hal" layer. they two are usually not the same.



