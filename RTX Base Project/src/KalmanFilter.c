/*
Group 8
*/

#include "KalmanFilter.h"

// Kalman Filter
void Kalmanfilter_C(float measurement, kalman_state* kstate)
{
		kstate->p = kstate->p + kstate->q;
		kstate->k = kstate->p / (kstate->p + kstate->r);
		kstate->x = kstate->x + kstate->k * (measurement-kstate->x);
		kstate->p = (1-kstate->k) * kstate->p;
}

