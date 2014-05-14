#include "client.h"

void control(const InputPacket* inpack, OutputPacket* outpack)
{
	// Put code here
    SensorType MD = inpack -> sen_mid_type;
    SensorType CLD = inpack -> sen_close_left_type;
    SensorType CRD = inpack -> sen_close_right_type;
    SensorType LD = inpack -> sen_left_type;
    SensorType RD = inpack -> sen_right_type;
    double sen_mid_dist;
	double sen_close_left_dist;
	double sen_close_right_dist;
	double sen_left_dist;
	double sen_right_dist;

    double acc;
	double steer;

    if ( MD != SEN_WALL ) {
        acc = 100; 
        steer = 0.2;
    } 

    if ( LD == SEN_WALL ) {
        steer = 2.0; 
        acc = 100.0;
        if ( CLD == SEN_WALL ) {
            steer = 5.0; 
            acc = 100.0;
        }
    } else if ( RD == SEN_WALL) {
        steer = -2.0;
        acc = 100.0;
        if ( CRD == SEN_WALL) {
            steer = -5.0;
            acc = 100.0;
        } 
    } 
    printf("acc: %f, steer: %f \n", acc, steer);

    outpack -> accelaration = acc;
    outpack -> steering = steer;

}
