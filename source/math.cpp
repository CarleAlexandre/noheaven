#include "engine.hpp"

bool IsMouseInBound(Rectangle rec, Vector2 pos, Vector2 mouse_pos) {
	return (mouse_pos.x >= pos.x && mouse_pos.x <= pos.x + rec.width
		&& mouse_pos.y >= pos.y && mouse_pos.y <= pos.y + rec.height);
}

/*
	find the greatest common denominator between to signed integer
*/
int	gcd(int a, int b) {
	if (b == 0)
		return (a);
	return (gcd(b, (a % b)));
}

/*
	Smoothstep lerp usefull for nice walking speed curve
	take a float should be percentage of distance between two point
	you can travel and return the new distance you will travel
*/
float	smoothStep(float t) {
    float v1 = t * t;
    float v2 = 1.0f - (1.0f - t) * (1.0f - t);
    return (Lerp(v1, v2, t));
}

/*
	find the distance between two Vector2
*/
float	flDistance(struct Vector2 a, struct Vector2 b) {
    return (pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

/*
By wikipedia:

"The algorithm was often misattributed to John Carmack,
but in fact the code is based on an unpublished paper by William Kahan
and K.C. Ng circulated in May 1986.
The original constant was produced from a collaboration between Cleve Moler and Gregory Walsh,
while they worked for Ardent Computing in the late 1980s."

I love Id
*/
float	Q_rsqrt( float number) {
	long		i = 0;
	float		x2 = 0, y = 0;
	const float	threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = * ( long * ) &y; // evil floating point bit level hacking
	i = 0x5f3759df - ( i >> 1 ); // what the fuck?
	y = * ( float * ) &i;
	y = y * ( threehalfs - ( x2 * y * y ) ); // 1st iteration
//	y = y * ( threehalfs - ( x2 * y * y ) ); // 2nd iteration, this can be removed

	return (y);
}

/*
	Travel a distance to your objectiv coordinate depending of the delta time of last frame and player velocity
*/
Vector2	toTravel(struct Vector2 current, struct Vector2 target, float velocity, float delta_time) {
    struct Vector2 direction = { target.x - current.x, target.y - current.y };
    float distance_to_target = flDistance(current, target);
    
    if (sqrt(distance_to_target) > velocity * delta_time) {
        float move_distance = velocity * delta_time;
        direction.x *= Q_rsqrt(distance_to_target);
        direction.y *= Q_rsqrt(distance_to_target);
        direction.x *= move_distance;
        direction.y *= move_distance;
        current.x += direction.x;
        current.y += direction.y;
    } else {
        current = target;
    }
    return (current);
}

/*
	convert screen coordinate to in game world coordinate
*/
Vector2	screenPosToWorldPos(Vector2 screen_pos, Vector2 camera_target, int s_width, int s_height, float zoom) {
	Vector2	world_pos;

	world_pos.x = (screen_pos.x - (float)s_width / 2) / zoom + camera_target.x;
	world_pos.y = (screen_pos.y - (float)s_height / 2) / zoom + camera_target.y;
	return (world_pos);
}

/*
	convert in game world coordinate to screen coordinate 
*/
Vector2	worldPosToScreenPos(Vector2 world_pos, Vector2 camera_target, int s_width, int s_height, float zoom) {
	Vector2	screen_pos;

	screen_pos.x = (world_pos.x - camera_target.x) * zoom + (float)s_width / 2;
	screen_pos.y = (world_pos.y - camera_target.y) * zoom + (float)s_height / 2;
	return (screen_pos);
}