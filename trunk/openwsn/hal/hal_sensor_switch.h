#ifndef HAL_SENSOR_SWI
#define HAL_SENSOR_SWI


#define AVCCS_SWITCH_PORT   1
#define AVCCS_SWITCH_PIN   17

void avccs_on(void);           //sensor voltage switch
void avccs_off(void);

#endif