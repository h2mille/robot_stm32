#include <math.h>
#include <position.h>
#include <global.h>
#include <driver.h>

#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
#define max(X, Y) (((X) > (Y)) ? (X) : (Y))
#define abs(X) (((X) > 0) ? (X) : (-X))


extern asserv_type_t asserv_type;
extern int v_max;

void control(Motor motor_L, Motor motor_R)
{
	int power_L=0;
	int power_R=0;
	int temp_power;
	int direction_move = 1;
	float_t angle_diff = willing_position.theta - robot_position.theta;
	float_t x_diff = willing_position.x - robot_position.x;
	float_t y_diff = willing_position.y - robot_position.y;
	float_t angle_alpha;
	if ( x_diff > 0)
		angle_alpha = atan(y_diff/x_diff) - robot_position.theta;
	else if ( y_diff > 0)
		angle_alpha = atan(y_diff/x_diff) - robot_position.theta + M_PI;
	else if ( y_diff < 0)
		angle_alpha = atan(y_diff/x_diff) - robot_position.theta - M_PI;
	else
		//else you are on the right position
		angle_alpha = 0;
	if (angle_alpha<-M_PI)
		angle_alpha+=2*M_PI;
	else if (angle_alpha>M_PI)
		angle_alpha-=2*M_PI;
	float_t diff_distance = sqrt(pow(x_diff,2)+pow(y_diff,2));
	switch (direction_asserv){
		case straight:
			break;
		case backward:
			direction_move=-1;
			angle_alpha+=M_PI;
			if (angle_alpha>M_PI)
				angle_alpha -=2*M_PI;
			break;
		case don_t_care:
			if (angle_alpha>M_PI_2)
			{
				angle_alpha-=M_PI;
				direction_move=-1;
			}
			else if (angle_alpha<-M_PI_2)
			{
				angle_alpha+=M_PI;
				direction_move=-1;
			}
			break;
		default:
			break;
	}

	switch (asserv_type){
	case angle:
		temp_power = angular_coef.P* angle_diff + angular_coef.I* angular_integrate + angular_coef.D* angular_speed;
		temp_power = min(max((int)(temp_power),-v_max),v_max);
		power_L = - temp_power;
		power_R = temp_power;
		break;
	case distance:

		break;
	case position:
		break;
	case angle_then_position:
		if (abs(angle_alpha)> angular_range){
			temp_power = angular_coef.P* angle_alpha + angular_coef.I* angular_integrate + angular_coef.D* angular_speed;
			temp_power = min(max((int)(temp_power),-v_max),v_max);
			power_L = - temp_power;
			power_R = temp_power;
		}else if(abs(diff_distance) > direct_range){
			temp_power = direct_coef.P* diff_distance + direct_coef.I* direct_integrate + direct_coef.D* direct_speed;
			temp_power = min(max((int)(temp_power),-v_max),v_max);
			power_L = temp_power*direction_move;
			power_R = temp_power*direction_move;
		}
		break;

	case angle_and_position:
		temp_power = angular_coef.P* angle_alpha + angular_coef.I* angular_integrate + angular_coef.D* angular_speed;
		temp_power = min(max((int)(temp_power),-v_max),v_max);
		power_L = - temp_power;
		power_R =  temp_power;
		temp_power = direct_coef.P* diff_distance + direct_coef.I* direct_integrate + direct_coef.D* direct_speed;
		temp_power = min(max((int)(temp_power),-v_max),v_max);
		power_L += temp_power*direction_move;
		power_R += temp_power*direction_move;
		break;

	default:
		break;
	}

		if (abs(angle_diff) < direct_range && abs(diff_distance) < angular_range )
			{
				power_L = 0;
				power_R = 0;

		}
//	printf("control: %d %d\n\r", (int)(power_L), (int)(power_R));
	Motor_power(motor_L, power_L);
	Motor_power(motor_R, power_R);

}
