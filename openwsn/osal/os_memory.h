
// provide static management of the whole memory
// doesn't support dynamic management

// @attention:
// though the sensor system is an deeply embedded system, we will still encounter the requirement
// to get some memory blocks in the program. you can call the functions in this module. however, 
// we should NOT introduce dynamic memory management in such simple light-weight systems.

void os_meminit();
void os_malloc();

 
