#include <math.h>
#include <position.h>
#include <global.h>
#include <printf.h>

int prev_encoder_L;
int prev_encoder_R;

extern position_t robot_position;
extern position_t prev_robot_position;
extern int direct_speed;
extern int angular_speed;
extern int direct_integrate;
extern int angular_integrate;

extern uint32_t encoder_resolution ;
extern float_t perimeter_L         ;
extern float_t perimeter_R         ;
extern float_t wheel_distance      ;
extern coef_t direct_coef          ;
extern coef_t angular_coef         ;

extern float_t direct_range        ;
extern float_t angular_range       ;

extern int sign_codeur_L;
extern int sign_codeur_R;

int safe_encoder_substraction(int encoder, int prev_encoder){
	int diff= encoder - prev_encoder;
	if (diff> 1<<15)
		return diff - (1<<16);
	if (diff < -1<<15)
		return diff + (1<<16);
	else
		return diff;
}

void update_position(int encoder_L, int encoder_R)
{
	//get increase of encoder in tick
	int delta_encoder_L;
	delta_encoder_L = sign_codeur_L*safe_encoder_substraction(encoder_L, prev_encoder_L);
	int delta_encoder_R;
	delta_encoder_R = sign_codeur_R*safe_encoder_substraction(encoder_R, prev_encoder_R);
	prev_encoder_L =  encoder_L;
	prev_encoder_R =  encoder_R;

	//change this ticks into mm
	float_t distance_L = delta_encoder_L * perimeter_L / encoder_resolution;
	float_t distance_R = delta_encoder_R * perimeter_R / encoder_resolution;

	//get instance done and angle changed
	float_t distance = (distance_L+distance_R) /2;
	float_t alpha =  (distance_L - distance_R)/ wheel_distance;

	double sin_thetha_alpha,cos_thetha_alpha;
	sin_thetha_alpha = sin(robot_position.theta+(alpha/2));
	cos_thetha_alpha = cos(robot_position.theta+(alpha/2));
	prev_robot_position.x=robot_position.x;
	prev_robot_position.y=robot_position.y;
	prev_robot_position.theta=robot_position.theta;
	robot_position.x += distance * cos_thetha_alpha;
	robot_position.y += distance * sin_thetha_alpha;
	robot_position.theta += alpha;

	if (robot_position.theta > M_PI)
		robot_position.theta -= M_TWOPI;
	else if (robot_position.theta <= - M_PI)
		robot_position.theta += M_TWOPI;

	direct_speed = distance;
	angular_speed = alpha;
	int integrate_L = encoder_L + prev_encoder_L;
	int integrate_R = encoder_R + prev_encoder_R;
	direct_integrate = integrate_L + integrate_R;
	angular_integrate = integrate_L - integrate_R;


}
