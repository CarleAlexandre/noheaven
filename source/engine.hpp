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
# include <stdio.h>

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

enum proffesion {
	holy_warrior = 0,
	mujahid = 1,
	monk = 2,
	Feldra = 3,
};

#define BG (Color){ 40, 40, 40, 255 }
#define FG (Color){ 235, 219, 178, 255 }

#define N_BUTTON_STARTUI	3
#define N_BUTTON_SETTINGUI	4

#define DEBUG fprintf(stderr, "error is after line: %i, %s\n", __LINE__, __FUNCTION__);

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
	u32			id;
	u32			text_index;
	std::string	name;
	int			properties;
}	Item;

typedef struct s_InventoryPanel {
	u32			id;
	u32			stack_size;
}	InvPanel;

typedef struct s_LootTable {
	std::vector<u32>	item_id;
	std::vector<float>	drop_rate;
	u32	size;
}	LootTable;

typedef struct s_entity {
	u32				id;
	std::string		*name;
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

int		gcd(int a, int b);
float	smoothStep(float t);
float	flDistance(struct Vector2 a, struct Vector2 b);
float	Q_rsqrt( float number);
Vector2	toTravel(struct Vector2 current, struct Vector2 target, float velocity, float delta_time);
Vector2	screenPosToWorldPos(Vector2 screen_pos, Vector2 camera_target, int s_width, int s_height, float zoom);
Vector2	worldPosToScreenPos(Vector2 world_pos, Vector2 camera_target, int s_width, int s_height, float zoom);

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
	char	*toData(const char *filepath, int *size) {
		std::string line;
		std::string	str;
		std::ifstream file(filepath);
		assert(file.is_open());
		while (std::getline(file, line)) {
			str.append(line);
		}
		file.close();
		file.clear();
		line.clear();
		*size = str.size();
		char *data = static_cast<char *>(malloc(*size));
		memcpy(data, str.c_str(), *size);
		return (data);
	}
	void	write(const char *filepath, const char *data, const size_t size) {
		std::ofstream file(filepath);
		assert(file.is_open());
		file.write(data, size);
		file.close();
		file.clear();
	}
	std::vector<Item>	*load_Items() {
		std::vector<Item> *items;
		int	size;
		char *data = toData("items.nhc", &size);
		items = static_cast<std::vector<Item> *>(malloc(size));
		memcpy(items, data, size);
		return (items);
	}
	std::vector<Texture2D>	*load_Textures() {
		std::vector<Texture2D> *text = new (std::vector<Texture2D>);
		int	size;
		char *data = toData("items.nhc", &size);
		text = static_cast<std::vector<Texture2D> *>(malloc(size));
		memcpy(text, data, size);
		return (text);
	}
	FileMgr() {
	}
	~FileMgr() {
	}
};

}

void	addFadingTxt(std::string text, double delay, Color color, int font_size, Vector2 pos, std::vector<s_FadeTxt> *Fadetxt_list);
void	renderFadingTxt(double delta_time, std::vector<s_FadeTxt> *Fadetxt_list);

#endif
