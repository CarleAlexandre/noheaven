#ifndef ENGINE_HPP
# define ENGINE_HPP

# ifdef _WIN64
#  include <C:/mingw64/include/raylib.h>
#  include <C:/mingw64/include/raymath.h>
#  include <C:/mingw64/include/rlgl.h>
#  define RLIGHTS_IMPLEMENTATION
#  include "rlights.h"
#  define GLSL_VERSION 330
# else
#  include <raylib.h>
#  include <raymath.h>
#  include <rlgl.h>
#  define RLIGHTS_IMPLEMENTATION
#  include "rlights.h"
#  define GLSL_VERSION 330
#  include <cstring>
# endif

# include <stdint.h>
# include <stddef.h>
# include <stdbool.h>
# include <math.h>
# include <iostream>
# include <fstream>
# include <vector>
# include <sstream>
# include <string>

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
	s_debug = 6,
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

typedef struct s_Attribut {
	float	speed;
	float	damage;
	float	life;
	float	armor;
	float	max_life;
	float	max_armor;
	float	stamina;
	float	max_speed;
	float	weapon_masteries;
	float	agility;
	float	classes;
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
	u32			model_index;
	std::string	name;
	int			properties;
}	Item;

typedef struct s_InventoryPanel {
	u32			id;
	u32			stack_size;
	u32			index;
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
	Rectangle		rec;
	Vector3			pos;
	Vector3			toPos;
	Attribut		attribut;
}	t_entity;

typedef struct s_spawn_entity {
	int			radius;
	Vector2		pos;
	u32			number;
	t_entity	entity;
}	EntitySpawn;

//to lower the render load, dungeon should be instanciated
typedef struct s_worldbit {
	Mesh	mesh;
	int64_t	matter;
	Vector3	pos;
	Vector3 rot_angle;
	Vector3	rot_axis;
}	t_worldbit;

typedef struct s_worldelement {
	Model	model;
	float	rot_angle;
	Vector3	pos;
	Vector3	rot_axis;
	Vector3 scale;
	Color	tint;
}	t_worldelement;

typedef struct s_Context {
	const char					*title = "noHeaven";
	int							width = 720, height = 480;
	int							state = s_menu;
	bool						inventoryOpen = false;
	Font						font;
	std::vector<Texture>		textAtlas;
	std::vector<i32>			input_buffer;
	std::vector<FadeTxt>		Fadetxt_list;
	std::vector<Item>			itemsAtlas;
	Shader						shader;
	Shader						filterShader;
	RenderTexture2D				fbo;
	std::vector<t_worldelement>	world;
}	Context;

//need to #define VOXEL_ENGINE to access this part

# ifdef VOXEL_ENGINE

#  define BLOCKSIZE 1
#  define CHUNK_SIZE 3375//number of block max in a chunk
#  define CHUNK_WIDTH 15
#  define CHUNK_HEIGHT 15
#  define CHUNK_LENGTH 15

typedef struct s_voxel {
	short	pos; //first two bit is an offsetting bits, then 4 for the x,an offsetting bits, 4 for the y, an offsetting bits and 4 for the z (between)
	short	element_idx;
}	t_voxel;

typedef struct s_chunk {
	t_voxel	*blocks;
	Vector2	pos;// this is the position of the chunk in the world grid
}	t_chunk;

#endif

bool	IsMouseInBound(Rectangle rec, Vector2 pos, Vector2 mouse_pos);
int		gcd(int a, int b);
float	smoothStep(float t);
float	flDistance(struct Vector2 a, struct Vector2 b);
float	Q_rsqrt( float number);
Vector2	toTravel(struct Vector2 current, struct Vector2 target, float velocity, float delta_time);
Vector2	screenPosToWorldPos(Vector2 screen_pos, Vector2 camera_target, int s_width, int s_height, float zoom);
Vector2	worldPosToScreenPos(Vector2 world_pos, Vector2 camera_target, int s_width, int s_height, float zoom);
Vector3	toTravel3d(struct Vector3 current, struct Vector3 target, float velocity, float delta_time);

void	addFadingTxt(std::string text, double delay, Color color, int font_size, Vector2 pos, std::vector<s_FadeTxt> *Fadetxt_list);
void	renderFadingTxt(double delta_time, std::vector<s_FadeTxt> *Fadetxt_list);

int		console(double delta_time);
void	initConsole(void);


class Loot {

private:
	s_LootTable loot_table;

public:

	void	addLoot(u32 id, float drop_rate) {
		loot_table.drop_rate.push_back(drop_rate);
		loot_table.item_id.push_back(id);
		loot_table.size++;
	}

	std::vector<u32>	generateLoot(int loot_size, time_t time_arg) {
		std::vector<u32> loot;
		float	roll = 0.0f;
		float	cumulative_prob = 0.05f;
		time_t	seed = time_arg * 1000;
		// Initialize random number generator
		srand((unsigned int) seed);
		// Generate loot items
		for (int i = 0; i < loot_size; i++) {
			for (u32 i = 0; i < loot_table.size; i++) {
				roll = (float) rand() / (float)RAND_MAX;
				for (u32 j = 0; j < loot_table.size; j++) {
					cumulative_prob += loot_table.drop_rate[j];
					if (roll <= cumulative_prob) {
						loot.push_back(loot_table.item_id[j]);
						break;
					}
				}
			}
		}
		return (loot);
	}
	Loot(void) {
	}
	~Loot(void) {

	}
};

class Inventory {
	//item doesn't need to be dragged, i can make it so that it only move when needed
	private:
	public:
	std::vector<s_InventoryPanel>	store;
	Rectangle						bound;
	int								capacity = 30;
	int								max_stack = 999;
	int								cursorPos = 0;
	int								scrollOffset = 0;
	int								row = 0;
	int								column = 0;
	int								scrollBarSize = 0;

	i32	add(u32 item_id, u32 number, std::vector<s_Item> item_list) {
		(void)item_list;
		if (capacity <= static_cast<int>(store.size())) {
			//cannot add the items;
			return (-1);
		}
		//for (i32 i = 0; i < inventory.size(); i++) {
		//	if (inventory.at(i).id == item_id) {
		//		inventory.at(i).stack_size += number;
		//		return (0);
		//	}
		//}
		store.push_back({item_id, number, (unsigned int)store.size()});
		return (0);
	}

	void	del(u32 index) {
		store.erase(store.begin() + index);
	}

	void	init(int height, int width, int x, int y) {
		bound.height = height * 0.5;
		bound.width = width * 0.5;
		bound.x = x;
		bound.y = y;
		row = floor((bound.height - 20) / 45);
		column = floor((bound.width - 10) / 45);
		scrollBarSize = std::min(static_cast<float>((bound.height - 50) * (static_cast<float>(row * column) / store.size())), bound.height - 30);
	}

	void	resetRender(int height, int width, int x, int y) {
		bound.height = height * 0.5;
		bound.height -= static_cast<int>(bound.height) % 45 + 20;
		bound.width = width * 0.5;
		bound.width -= static_cast<int>(bound.width) % 45 + 20;
		bound.x = x;
		bound.y = y;
		row = floor((bound.height - 20) / 45);
		column = floor((bound.width - 10) / 45);
		cursorPos = 0;
		scrollOffset = 0;
		scrollBarSize = std::min(static_cast<float>((bound.height - 50) * (static_cast<float>(row * column) / store.size())), bound.height - 30);
	}

	void updateInventory(double delta_time, int win_width, int win_height) {
		(void)delta_time;	
		static bool		draggingScrollBar = false;
		static bool		dragWindow = false;
		static Vector2	old_pos;
		Vector2			mouse_pos = GetMousePosition();
		
		if (draggingScrollBar == true) {
			cursorPos = mouse_pos.y - (bound.y + 10 + scrollBarSize * 0.5);
			int maxOffset = bound.height - 10 - scrollBarSize;
			if (cursorPos < 0) {
				cursorPos = 0;
			}
			if (cursorPos > maxOffset) {
				cursorPos = maxOffset;
			}
			scrollOffset = static_cast<int>(((static_cast<float>(cursorPos) / maxOffset) * (static_cast<float>(store.size()) / column)));
		}
		if (dragWindow == true) {
			Vector2 new_pos;
			new_pos.x = mouse_pos.x - old_pos.x;
			new_pos.y = mouse_pos.y - old_pos.y;
			bound.x += new_pos.x;
			bound.y += new_pos.y;
			if (bound.x >= win_width - bound.width - 10) {
				bound.x = win_width - bound.width - 10;
			} else if (10 >= bound.x) {
				bound.x = 10;
			}
			if (bound.y >= win_height - bound.height - 10) {
				bound.y = win_height - bound.height - 10;
			} else if (10 >= bound.y) {
				bound.y = 10;
			}
		}
		if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
			Rectangle scrollBar = {0, 0, 10, bound.height};
        	if (IsMouseInBound(scrollBar, {bound.x + bound.width - 10, bound.y + 10}, mouse_pos)) {
        	    draggingScrollBar = true;
        	} else if (IsMouseInBound({0, 0, bound.width, 10}, {bound.x, bound.y}, mouse_pos)) {
				dragWindow = true;
				old_pos = mouse_pos;
			} else {
				dragWindow = false;
			}
		}
		if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
			draggingScrollBar = false;
			dragWindow = false;
		}
	}

	void renderInventory(double delta_time, const std::vector<s_Item>& item_list, const std::vector<Texture2D>& textAtlas) {
        Item		item;
		(void)delta_time;
		int startIdx = scrollOffset * column;
		int endIdx = std::min(startIdx + (row * column), static_cast<int>(store.size()));

		DrawRectangle(bound.x, bound.y, bound.width, bound.height, ColorAlpha(BG, 0.1));
		if (startIdx < static_cast<int>(store.size())) {
			for (int y = 0; y < row; y++) {
				DrawText(TextFormat("%i", (startIdx / 8) + y + 1), bound.x, bound.y + 20 + y * 45, 10, WHITE);
				for (int x = 0; x < column; x++) {
					int index = startIdx + x + (y * column);
					if (index >= endIdx) {
						y = row + 1;
						break;
					}
					DrawRectangle(bound.x + 10 + x * 45, bound.y + 20 + y * 45, 40, 40, ColorAlpha(WHITE, 0.2));
					u32 item_id = store[index].id;
					u32 stack_size = store[index].stack_size;
					Item item;
					for (uint32_t j = 0; j < item_list.size(); j++) {
						if (item_list[item_id].id == item_id) {
							item = item_list[item_id];
							DrawTextureRec(textAtlas[item.text_index], (Rectangle){0, 0, 32, 32}, {static_cast<float>(bound.x + 10 + x * 45 + 4), static_cast<float>(bound.y + 20 + y * 45 + 4)}, WHITE);
							DrawText(TextFormat("%i", stack_size), bound.x + 30 + x * 45, bound.y + 50 + y * 45, 10, WHITE);
							break;
						}
					}
				}
			}
		}
		DrawRectangle(bound.x, bound.y, bound.width, 10, ColorAlpha(FG, 0.5));
		Rectangle scrollBar = {bound.x + bound.width - 10, cursorPos + bound.y + 10, 10, static_cast<float>(scrollBarSize)};
    	DrawRectangleRec(scrollBar, DARKGRAY);
		DrawText("Inventory", bound.x + 1, bound.y + 1, 8, BG);
	}

	Inventory(){}
	~Inventory(){}
};

//equipement should have multiplicator and not falt value, it should be multiplicative and not additiv to the player stats, so a lvl 50 common armor is still les op than a lvl 1 legendarie armor

class Player {

private:

public:
	Vector3		pos;
	Vector3		vel;
	Vector3		topos;
	Camera3D	cam;
	Attribut	attribut;
	Vector3		orientation;
	Inventory	*inventory;
	Model		model;
	bool		isAlive;

	void	die() {

	}

	void	live() {

	}
	void	update(double delta_time, std::vector<i32> input_buffer, int *state, std::vector<s_FadeTxt> *Fadetxt_list) {
		static double	acc_time = 0;
	
		(void)input_buffer;
		if (Vector3Distance(pos, topos) > 0.1f) {
			pos = toTravel3d(pos, topos, attribut.speed * delta_time, delta_time);
		}
		if (attribut.life < 0) {
			attribut.life = 0;
			*state = s_gameover;
		}
		if (attribut.life > attribut.max_life) {
			attribut.life = attribut.max_life; 
		}
		if (acc_time >= 1) {
			if (attribut.life < attribut.max_life) {
				attribut.life += attribut.life_regen;
				addFadingTxt(TextFormat("+%.0f", attribut.life_regen), 0.8f, GREEN, 12, {.x = pos.x + 1, .y = pos.y + 1}, Fadetxt_list);
			}
			acc_time = 0;
		}
		acc_time += delta_time;
	}

	Player(void) {
		pos = {
			.x = 0,
			.y = 0,
			.z = 5,
		};
		vel = {
			.x = 0,
			.y = 0,
			.z = 5,
		};
		topos = {
			.x = 0,
			.y = 0,
			.z = 0,
		};
		cam = (Camera3D){
			.position = {
				.x = 2,
				.y = -3,
				.z = 10,
			},
			.target = {
				.x = 0,
				.y = 0,
				.z = 0
			},
			.up = {
				.x = 0,
				.y = 1.0f,
				.z = 0,
			},
			.fovy = 45.0f,
			.projection = CAMERA_PERSPECTIVE,
		};
		attribut = {
			.speed = 100.,
			.damage = 0,
			.life = 1500,
			.armor = 0,
			.max_life = 1500,
			.max_armor = 0,
			.stamina = 0,
			.max_speed = 200.,
			.weapon_masteries = 0,
			.agility = 1.4f,
			.classes = 0,
			.life_regen = 12.,
		};
		inventory = new(Inventory);
	}

	~Player(void) {

	}
};

class Entity {

private:

	std::vector<s_entity> elements;
	std::vector<s_spawn_entity>	spawns;

public:

	void	updateSpawn(double delta_time) {
		(void)delta_time;
		for (size_t i = 0; i < spawns.size(); i++) {
			static uint32_t	spwn_idx = 0;
			if (spawns.at(i).number >= spwn_idx) {
				elements.push_back(spawns.at(i).entity);
			}
			if (spawns.at(i).number == spwn_idx) {
				spawns.erase(spawns.begin() + i);
			}
			spwn_idx++;
		}
	}

	void	updateEntity(double delta_time) {
		for (uint32_t i = 0; i < elements.size(); i++) {
			if (Vector3Distance(elements[i].pos, elements[i].toPos)) {
				toTravel3d(elements[i].pos, elements[i].toPos, elements[i].attribut.speed, delta_time);
			}
		}
	}

	void	add_spawn(EntitySpawn new_spawn) {
		spawns.push_back(new_spawn);
	}

	void	add_elements(t_entity new_entity) {
		elements.push_back(new_entity);
	}
	
	void	del_element(int pos, int size) {
		elements.erase(elements.begin() + pos, elements.begin() + pos + size);
	}

	void	update(double delta_time, Vector2 player_pos) {
		(void)delta_time;
		for (size_t i = 0; i < elements.size(); i++) {
			elements.at(i).toPos.x = player_pos.x;
			elements.at(i).toPos.y = player_pos.y;
		}
	}

	void	render(double delta_time, std::vector<Texture2D> textAtlas) {
		(void)delta_time;
		(void)textAtlas;
		for (size_t i = 0; i < elements.size(); i++) {
			//DrawTextureRec(textAtlas.at(elements.at(i).text_index), elements.at(i).rec, elements.at(i).pos, WHITE);
		}
	}

	Entity(void) {}

	~Entity(void) {}
};

class FileMgr {
	private:

	public:

	std::string	toString(const char *filepath) {
		std::string line;
		std::string ret_val;
		std::ifstream file(filepath);

		if (!file.is_open()) {
			std::cout << "Failed to open the file: " << filepath << std::endl;
		}
		while (std::getline(file, line)) {
			ret_val.append(line);
		}
		file.close();
		file.clear();
		line.clear();
		return (ret_val);
	}
	char	*toData(const char *filepath) {
		std::string line;
		std::string	str;
		int	size = 0;
		std::ifstream file(filepath);

		if (!file.is_open()) {
			std::cout << "Failed to open the file: " << filepath << std::endl;
		}
		while (std::getline(file, line)) {
			str.append(line);
		}
		file.close();
		file.clear();
		line.clear();
		size = str.size();
		char *data = static_cast<char *>(malloc(size));
		memcpy(data, str.c_str(), size);
		return (data);
	}
	void	write(const char *filepath, void *data, const size_t size) {
		char	*span = static_cast<char *>(data);
		std::ofstream file(filepath);
		if (!file.is_open()) {
			std::cout << "Failed to open the file: " << filepath << std::endl;
		}
		for (size_t i = 0; i < size; i++) {
			file.put(span[i]);
		}
		file.close();
		file.clear();
	}
	std::vector<Item> loadItemsFromFile(const char *filename) {
		std::vector<Item>	items;
		std::ifstream		file(filename);

		if (!file.is_open()) {
			std::cout << "Failed to open the file: " << filename << std::endl;
			return items;
		}
		std::string line;
		while (std::getline(file, line)) {
			Item item;
			std::istringstream iss(line);
			iss >> item.id >> item.text_index >> item.name >> item.properties;
			if (item.id != items.size())
				std::cout << "not the correct id\n";
			items.push_back(item);
		}

		file.close();
		return items;
	}
	FileMgr() {
	}
	~FileMgr() {
	}
};

#endif
