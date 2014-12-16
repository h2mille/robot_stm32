#include <global.h>

void fifo_init(fifo_t * f, char * buf, int size){
     f->head = 0;
     f->tail = 0;
     f->size = size;
     f->buf = buf;
}

void init_global()
{
	asserv_type            = angle_and_position;
	direction_asserv       = don_t_care;
	robot_position.x       = 0;
	robot_position.y       = 0;
	robot_position.theta   = 0;
	willing_position.x     = 0;
	willing_position.y     = 1;
	willing_position.theta = 0;
	encoder_resolution     = 200 * 14;
	perimeter_L            = 0.19;
	perimeter_R            = 0.19;
	wheel_distance         = 0.2;
	sign_codeur_L          = -1;
	sign_codeur_R          = 1;
	v_max                  = 500 ;


	direct_coef.P          = 10000;
    direct_coef.I	       = 00;
    direct_coef.D          = 500;
    angular_coef.P         = 10000;
	angular_coef.I	       = 00;
    angular_coef.D         = 500;

	angular_range       = 0.05;

	fifo_init(RxFifo, (void *)RxBuffer, sizeof(RxBuffer));
	fifo_init(TxFifo, (void *)TxBuffer, sizeof(TxBuffer));


}
