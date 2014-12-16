#ifndef GLOBAL_H
#define GLOBAL_H
#include <math.h>
#include <stdint.h>
/* map is in meter. angles are in radian */

typedef struct {
	 char * buf;
	 int head;
	 int tail;
	 int size;
} fifo_t;

void fifo_init(fifo_t * f, char * buf, int size);

void init_global();

typedef struct POSITION {
	float_t x;
	float_t y;
	float_t theta;
} position_t;

typedef struct {
	float_t P;
	float_t I;
	float_t D;
} coef_t;

typedef enum{
	angle,
	distance,
	position,
	angle_then_position,
	angle_and_position,
}asserv_type_t;

typedef enum{
	straight,
	backward,
	don_t_care,
}direction_t;

	// INIT
	asserv_type_t asserv_type   ;
	direction_t direction_asserv  ;
	position_t robot_position   ;
	position_t willing_position ;
	uint32_t encoder_resolution ;
	float_t perimeter_L         ;
	float_t perimeter_R         ;
	float_t wheel_distance      ;
	coef_t direct_coef          ;
	coef_t angular_coef         ;
	int v_max                   ;


	int sign_codeur_L;
	int sign_codeur_R;

	float_t direct_range        ;
	float_t angular_range       ;

	// BUFFER
	int direct_speed;
	int angular_speed;
	int direct_integrate;
	int angular_integrate;
	float_t direct_range;
	float_t angular_range;
	position_t prev_robot_position;




	#define RXSIZE 30

	uint8_t RxBuffer[RXSIZE];
	fifo_t RxFifo[1];

	#define TXSIZE 50

	uint8_t TxBuffer[TXSIZE];
	fifo_t TxFifo[1];

#endif
