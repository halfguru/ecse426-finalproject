#ifndef KalmanFilter_h
#define KalmanFilter_h

typedef struct
{
	float q;
	float r;
	float x;
	float p;
	float k;
}kalman_state;

void Kalmanfilter_C(float measurement, kalman_state* kstate);

#endif



