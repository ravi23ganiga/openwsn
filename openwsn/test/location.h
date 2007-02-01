#ifndef _LOCATION_H
#define _LOCATION_H

int location_test(void);
void rssi2distance(uint8 rssi, double * dis);
void reverse_t(double *f); //求逆矩阵
void compute_location(double *x,double *y,double *c2,double *d2);

//通过三点二维定位
void localize(double *x,double *y,double x1,double y1,double d1,double x2,double y2,double d2,double x3,double y3,double d3);

#endif