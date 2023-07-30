#ifndef CLASS_HPP
# define CLASS_HPP

# include "engine.hpp"

namespace engine {

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

class GameMap {

};

class Player {

private:

	Camera3D	cam;
	Vector3		pos;

public:

	void	update(double delta_time, std::vector<i32> input_buffer) {
		(void)delta_time;
		(void)input_buffer;
	}
	Player(void) {
		cam = (Camera3D){
			.position =  {0, 0, 0},
			.target = {},
			.up = {},
			.fovy = 70,
			.projection = {},
		};
		pos = {0, 0, 0};
	}
	~Player(void) {

	}
};


typedef struct s_entity {
	u32				id;
	char			*name;
	u32				loot_index;
	u32				text_index;
	Vector2			pos;
	Vector2			toPos;
	float			speed;
	int				damage;
	int				life;
	int				armor;
}	t_entity;

typedef struct s_spawn_entity{
	int			radius;
	Vector2		pos;
	u32			loot_index;
	u32			text_index;
	u32			number_left;
	float		speed;
	int			damage;
	int			life;
	int			armor;
}	EntitySpawn;

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

class Render {

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

	void	render(void *scene, double delta_time) {
		BeginDrawing();
		renderFadingTxt(delta_time);
		EndDrawing();
	}
	Render(void) {
	}
	~Render(void) {
	}
};

class Context {

private:

	const char		*title = "noHeaven";
	int				width = 0, height = 0;

	void	LoadTextureAtlas() {
		textAtlas.push_back(LoadTexture("asset/texture/button.png"));
		textAtlas.push_back(LoadTexture("asset/texture/highlight.png"));
		textAtlas.push_back(LoadTexture("asset/texture/character.png"));
	}

	void	UnloadTextureAtlas() {
		for (int i = 0; i < textAtlas.size(); i++) {
			UnloadTexture(textAtlas[i]);
		}
	}

public:

	Font font;
	std::vector<Texture>	textAtlas;
	std::vector<i32>		input_buffer;

	bool IsMouseInBound(Rectangle rec, Vector2 pos, Vector2 mouse_pos) {
		return (mouse_pos.x >= pos.x && mouse_pos.x <= pos.x + rec.width
			&& mouse_pos.y >= pos.y && mouse_pos.y <= pos.y + rec.height);
	}
	void	input(void) {

	}
	Context() {
		width = 600, height = 400;
		InitWindow(width, height, title);
		font = LoadFont("asset/font/SF_Atarian_System.ttf");
		SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);
		LoadTextureAtlas();
	}
	~Context(void) {
		UnloadFont(font);
		UnloadTextureAtlas();
		CloseWindow();
	}
};

}

#endif