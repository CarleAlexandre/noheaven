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

typedef struct s_Context {
	const char				*title = "noHeaven";
	int						width = 720, height = 480;
	int						state = s_menu;
	bool					inventoryOpen = false;
	Font					font;
	std::vector<Texture>	textAtlas;
	std::vector<i32>		input_buffer;
	std::vector<FadeTxt>	Fadetxt_list;
	std::vector<Item>		itemsAtlas;
}	Context;

bool	IsMouseInBound(Rectangle rec, Vector2 pos, Vector2 mouse_pos);
int		gcd(int a, int b);
float	smoothStep(float t);
float	flDistance(struct Vector2 a, struct Vector2 b);
float	Q_rsqrt( float number);
Vector2	toTravel(struct Vector2 current, struct Vector2 target, float velocity, float delta_time);
Vector2	screenPosToWorldPos(Vector2 screen_pos, Vector2 camera_target, int s_width, int s_height, float zoom);
Vector2	worldPosToScreenPos(Vector2 world_pos, Vector2 camera_target, int s_width, int s_height, float zoom);

void	addFadingTxt(std::string text, double delay, Color color, int font_size, Vector2 pos, std::vector<s_FadeTxt> *Fadetxt_list);
void	renderFadingTxt(double delta_time, std::vector<s_FadeTxt> *Fadetxt_list);

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
		for (int i = 0; i < loot_table.size; i++) {
			roll = (float) rand() / (float)RAND_MAX;
			for (int j = 0; j < loot_table.size; j++) {
				cumulative_prob += loot_table.drop_rate[j];
				if (roll <= cumulative_prob) {
					loot.push_back(loot_table.item_id[j]);
					break;
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

private:

	std::vector<InvPanel>	inventory;
	u32						max_size = 200;
	u32						max_stack = 5000;//useless for now
	InvPanel				ItemUp;
	bool					item_is_up = false;
	bool					draggingScrollBar = false;
	int						cursorPos = 0;
	int						scrollOffset = 0;

public:
	u32						size;

	i32	add_item(u32 item_id, u32 number, std::vector<s_Item>	item_list) {
		if (inventory.size() >= max_size) {
			//cannot add the items;
			return (-1);
		}
		//for (i32 i = 0; i < inventory.size(); i++) {
		//	if (inventory.at(i).id == item_id) {
		//		inventory.at(i).stack_size += number;
		//		return (0);
		//	}
		//}
		inventory.push_back(InvPanel{item_id, number});
		size ++;
		return (0);
	}

	void	delete_item(u32 index) {
		inventory.erase(inventory.begin() + index);
		size--;
	}

	void updateInventory(double delta_time, int height, int width) {
		if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
			Vector2 mouse_pos = GetMousePosition();
			Rectangle scrollBar = {0, 0, 10, static_cast<float>(5 + height - 260)}; // Scrollbar dimensions
        	if (IsMouseInBound(scrollBar, {465, 130}, mouse_pos)) {
        	    draggingScrollBar = true;
        	} else {
				draggingScrollBar = false;
			}

			if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
				draggingScrollBar = false;
			}
			
			if (draggingScrollBar == true) {
				cursorPos = mouse_pos.y - 130;

				// Calculate the total height required for all rows
				int totalHeight = (inventory.size() + 7) / 8 * 45; // should use this for the crosse product in the render function
				// total inv size / diplayed inv size = total bar size / current bar pos
				// Calculate the maximum offset for scrolling
				int maxOffset = std::max(0, totalHeight - (5 * 45));

				// Update the cursor position based on scrolling
				if (cursorPos < 0) {
					cursorPos = 0;
				}
				if (cursorPos > height - maxOffset) {
					cursorPos = height - maxOffset;
				}
				scrollOffset = cursorPos;
			}
		}
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			Vector2 mouse_pos = GetMousePosition();
			Rectangle rec = {0, 0, 40, 40};

			if (item_is_up == false) {
				int i = 0;
				int y = 0;
				int startIdx = scrollOffset * 8;
				while (i < 5 && startIdx + y + (i * 8) < inventory.size()) {
					if (IsMouseInBound(rec, {static_cast<float>(110 + y * 45), static_cast<float>(130 + i * 45)}, mouse_pos)) {
						ItemUp.id = inventory.at(i * 5 + y * 8).id;
						ItemUp.stack_size = inventory.at(i * 5 + y * 8).stack_size;
						item_is_up = true;
						//inventory.erase(inventory.begin() + i + y);
						return;
					}
					y++;
					if (y >= 8) {
						i++;
						y = 0;
					}
				}
			} else {
				item_is_up = false;
				//inventory.erase(inventory.begin() + ItemUp.index);
				//ItemUp.index = inventory.size();
				//inventory.push_back(ItemUp);
			}
		}
	}

	void renderInventory(double delta_time, int height, int width, const std::vector<s_Item>& item_list, const std::vector<Texture2D>& textAtlas) {
        Item		item;
		int			index;

		DrawRectangle(100, 100, width - 200, height - 200, ColorAlpha(WHITE, 0.1));

		// Calculate the total height required for all rows
		int totalHeight = (inventory.size() + 7) / 8 * 45;
		// Calculate the maximum offset for scrolling
		int maxOffset = std::max(0, totalHeight - (5 * 45));
		// Calculate the starting index for the items in the current row
		int startIdx = scrollOffset * 8;
		// Calculate the ending index for the items in the current row
		int endIdx = std::min(startIdx + (8 * 5), static_cast<int>(inventory.size()));

		if (startIdx < inventory.size()) {
			for (int i = 0; i < 5; i++) {
				DrawText(TextFormat("%i", (startIdx / 8) + i + 1), 100, 130 + i * 45, 10, WHITE);
				for (int y = 0; y < 8; y++) {
					int index = startIdx + y + (i * 8);
					if (index >= endIdx) {
						i = 6;
						break;
					}
					DrawRectangle(110 + y * 45, 130 + i * 45, 40, 40, ColorAlpha(GRAY, 0.2));
					u32 item_id = inventory[index].id;
					u32 stack_size = inventory[index].stack_size;
					Item item;
					for (int j = 0; j < item_list.size(); j++) {
						if (item_list[item_id].id == item_id) {
							item = item_list[item_id];
							DrawTextureRec(textAtlas[item.text_index], (Rectangle){0, 0, 32, 32}, {static_cast<float>(110 + y * 45 + 4), static_cast<float>(130 + i * 45 + 4)}, WHITE);
							DrawText(TextFormat("%i", stack_size), 130 + y * 45, 160 + i * 45, 10, BLACK);
							break;
						}
					}
				}
			}
		}
		Rectangle scrollBar = {465, static_cast<float>(130 + cursorPos), 10, static_cast<float>(30)};
    	DrawRectangleRec(scrollBar, DARKGRAY);
        DrawText("Inventory", 110, 110, 10, WHITE);
		if (item_is_up == true) {
			for (int j = 0; j < item_list.size(); j++) {
        		if (item_list.at(j).id == ItemUp.id) {
					Vector2 mouse_pos = GetMousePosition();
        	        item = item_list[j];
					DrawTextureRec(textAtlas.at(item.text_index), (Rectangle){0, 0, 32, 32}, mouse_pos, WHITE);
					DrawText(TextFormat("%i", ItemUp.stack_size), mouse_pos.x + 15, mouse_pos.y + 20, 10, BLACK);
        	        break;
        	    }
        	}
		}
    }

	Inventory() {
	}

	~Inventory() {

	}
};

class Player {

private:

public:
	Vector2		pos;
	Camera2D	cam;
	Attribut	attribut;
	Vector2		topos;
	Rectangle	bound;
	float		angle;
	Vector2		origin;
	Rectangle	recsource;
	Vector2		vel;
	Inventory	*inventory;


	void	update(double delta_time, std::vector<i32> input_buffer, int *state, std::vector<s_FadeTxt> *Fadetxt_list) {
		static double acc_time = 0;
		if (flDistance(pos, topos) > 0.01f) {
			pos = toTravel(pos, topos, attribut.speed * delta_time, delta_time);
		}
		bound.x = pos.x;
		bound.y = pos.y;
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
				addFadingTxt(TextFormat("+%.0f", attribut.life_regen), 0.8f, GREEN, 12, pos, Fadetxt_list);
			}
			acc_time = 0;
		}
		acc_time += delta_time;
	}

	Player(void) {
		pos = {
			.x = 60,
			.y = 60
		};
		vel = {
			.x = 0,
			.y = 0
		};
		topos = {
			.x = 60,
			.y = 60
		};
		cam = (Camera2D){
			.target = {
		 		.x = 60,
				.y = 60,
			},
			.rotation = 0,
			.zoom = 1.0f,
		};
		bound = { 0, 0, 64, 64};
		attribut = {
			.speed = 20000.0f,
			.life = 1500,
			.max_life = 1500,
			.life_regen = 12.0f,
		};
		origin = { 32, 32};
		recsource = { 0, 0, 64, 64};
		cam.offset.x = 360;
		cam.offset.y = 240;
		inventory = new(Inventory);
	}

	~Player(void) {

	}
};

class Entity {

private:

	std::vector<s_entity> element;
	std::vector<s_spawn_entity>	spawns;

public:

	void	update(double delta_time) {
		for (i32 i = 0; i < spawns.size(); i++) {\
			if (spawns.at(i).number_left == 0) {
				spawns.erase(spawns.begin() + i);
			}
		}
	}
	void	add_spawn(EntitySpawn new_spawn) {
		spawns.push_back(new_spawn);
		//element.push_back();
	}
	void	loadAllEntity(void) {

	};
	Entity(void) {
		loadAllEntity();
	}
	~Entity(void) {

	}
};

#endif
