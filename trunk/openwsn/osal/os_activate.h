
/* boot up the "osal" functionality. usually, the "os_activate" function is called by target booting code. 
 * these code may some ASM codes.  and then, os_activate will load the os functions and finally call
 * application layers "appmain()" function.
void os_activate()
{
	os_init();
	os_run();
}

void os_init();
void os_run();
void os_wakeup();
void os_sleep();
