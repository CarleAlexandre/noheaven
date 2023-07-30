#ifndef ENGINE_HPP
# define ENGINE_HPP

# ifdef _WIN64
#  include <C:/mingw64/include/raylib.h>
#  include <C:/mingw64/include/raymath.h>
#  include <C:/mingw64/include/rlgl.h>
# else
#  include <raylib.h>
#  include <raymath.h>
#  include <rlgl.h>
# endif

# include <stdint.h>
# include <stddef.h>
# include <stdbool.h>

# include <stdlib.h>
# include <time.h>
# include <math.h>

# include <assert.h>

# include <iostream>
# include <fstream>
# include <vector>
# include <string>
# include <map>

typedef int16_t		i16;
typedef uint16_t	u16;
typedef int32_t		i32;
typedef uint32_t	u32;
typedef void		*ptr;

enum itemProperties {
	PLACABLE = 1 << 0,
	BREAKABLE = 1 << 1,
	TRANSPARENT = 1 << 2,
	LOOTABLE = 1 << 3,
};

enum state {
	s_menu = 0,
	s_game = 1,
	s_close = 2,
	s_setting = 3,
	s_pause = 4,
	s_gameover = 5,
};

#define BG (Color){ 40, 40, 40, 255 }
#define FG (Color){ 235, 219, 178, 255 }

#define N_BUTTON_STARTUI	3
#define N_BUTTON_SETTINGUI	4

typedef struct s_Attribut {
	float	speed;
	int		damage;
	int		life;
	int		armor;
	int		max_life;
	int		max_armor;
	int		stamina;
	int		max_speed;
	int		weapon_masteries;
	int		agility;
	int		classes;
	float	life_regen;
}	Attribut;

typedef struct s_Button {
	Rectangle	bound;
	Texture2D	text;
	Vector2		pos;
	std::string	name;
}	Button;

typedef struct s_FadeTxt {
	std::string	fmt;
	Color		color;
	Vector2		pos;
	double		delay;
	double		time;
	float		alpha;
	int			font_size;
}	FadeTxt;

typedef struct s_Item {
	int			id;
	int			texture;
	std::string	name;
	int			properties;
}	Item;

typedef struct s_LootTable {
	std::vector<u32>	item_id;
	std::vector<float>	drop_rate;
	u32	size;
}	LootTable;

typedef struct s_entity {
	u32				id;
	char			*name;
	u32				loot_index;
	u32				text_index;
	Vector2			pos;
	Vector2			toPos;
	Attribut		attribut;
}	t_entity;

typedef struct s_spawn_entity{
	int			radius;
	Vector2		pos;
	u32			loot_index;
	u32			text_index;
	u32			number_left;
	Attribut	attribut;
}	EntitySpawn;

/*
	find the greatest common denominator between to signed integer
*/
static
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
static
float smoothStep(float t) {
    float v1 = t * t;
    float v2 = 1.0f - (1.0f - t) * (1.0f - t);
    return (Lerp(v1, v2, t));
}

/*
	find the distance between two Vector2
*/
static
float flDistance(struct Vector2 a, struct Vector2 b) {
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
static
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
static
Vector2 toTravel(struct Vector2 current, struct Vector2 target, float velocity, float delta_time) {
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
static
Vector2	screenPosToWorldPos(Vector2 screen_pos, Vector2 camera_target, int s_width, int s_height, float zoom) {
	Vector2	world_pos;

	world_pos.x = (screen_pos.x - (float)s_width / 2) / zoom + camera_target.x;
	world_pos.y = (screen_pos.y - (float)s_height / 2) / zoom + camera_target.y;
	return (world_pos);
}

/*
	convert in game world coordinate to screen coordinate 
*/
static
Vector2	worldPosToScreenPos(Vector2 world_pos, Vector2 camera_target, int s_width, int s_height, float zoom) {
	Vector2	screen_pos;

	screen_pos.x = (world_pos.x - camera_target.x) * zoom + (float)s_width / 2;
	screen_pos.y = (world_pos.y - camera_target.y) * zoom + (float)s_height / 2;
	return (screen_pos);
}

namespace engine {

class FileMgr {
	private:
	public:
	// read from a file and put them in a vector, seperated by : every data are in order of the setting struct
	//std::map<std::string, std::string> parse_setting() {
	//	std::string line;
	//	std::map<std::string, std::string> ret_map;
	//	std::ifstream file("./setting.nhc");
	//	assert(file.is_open());
	//	while (std::getline(file, line)) {
	//	}
	//}
	std::string	toString(const char *filepath) {
		std::string line;
		std::string ret_val;
		std::ifstream file(filepath);
		assert(file.is_open());
		while (std::getline(file, line)) {
			ret_val.append(line);
		}
		file.close();
		file.clear();
		line.clear();
		return (ret_val);
	}
	void	write(const char *filepath, const char *data, const size_t size) {
		std::ofstream file(filepath);
		assert(file.is_open());
		file.write(data, size);
		file.close();
		file.clear();
	}
	FileMgr() {
	}
	~FileMgr() {
	}
};

class Rendering {

private:

	std::vector<s_FadeTxt>	Fadetxt_list;

public:

	void	addFadingTxt(std::string text, double delay, Color color, int font_size, Vector2 pos) {
		Fadetxt_list.push_back({
			.color = color,
			.pos = pos,
			.delay = delay,
			.time = 0.0f,
			.alpha = 1.0f,
			.font_size = font_size,
		});
	}
	void	renderFadingTxt(double delta_time) {
		for (i32 i = 0; i < Fadetxt_list.size(); i++) {
			if (Fadetxt_list.at(i).time >= Fadetxt_list.at(i).delay || Fadetxt_list.at(i).fmt.empty()) {
				Fadetxt_list.erase(Fadetxt_list.begin() + i);
			}
			Fadetxt_list.at(i).alpha = ((Fadetxt_list.at(i).delay - Fadetxt_list.at(i).time) / Fadetxt_list.at(i).delay);
			DrawText(Fadetxt_list.at(i).fmt.c_str(), Fadetxt_list.at(i).pos.x, Fadetxt_list.at(i).pos.y, Fadetxt_list.at(i).font_size, Fade(Fadetxt_list.at(i).color, Fadetxt_list.at(i).alpha));
			Fadetxt_list.at(i).time += delta_time;
		}
	}
	Rendering(void) {
	}
	~Rendering(void) {
	}
};

}

#endif
