#ifndef CLASS_HPP
# define CLASS_HPP

# include "engine.hpp"

namespace Game {

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
	u32						max_size = 32;
	u32						max_stack = 5000;//useless for now
	InvPanel				ItemUp;
	bool					item_is_up;

public:
	u32						size;

	i32	add_item(u32 item_id, u32 number, std::vector<s_Item>	item_list) {
		if (inventory.size() >= max_size) {
			//cannot add the items;
			return (-1);
		}
		for (i32 i = 0; i < inventory.size(); i++) {
			if (inventory.at(i).id == item_id) {
				inventory.at(i).stack_size += number;
				return (0);
			}
		}
		inventory.push_back(InvPanel{item_id, number});
		size ++;
		return (0);
	}

	void	delete_item(u32 index) {
		inventory.erase(inventory.begin() + index);
		size--;
	}

	void updateInventory(double delta_time, int height, int width) {
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			Vector2 mouse_pos = GetMousePosition();
			Rectangle rec = {0, 0, 40, 40};
			int i = 0;
			int y = 0;

			if (item_is_up == false) {

				while (i * 5 + y * 8 < inventory.size()) {
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
				inventory.erase(inventory.begin() + ItemUp.index);
				ItemUp.index = inventory.size();
				inventory.push_back(ItemUp);
			}
		}
	}

	void renderInventory(double delta_time, int height, int width, const std::vector<s_Item>& item_list, const std::vector<Texture2D>& textAtlas) {
        static int	cursorPos = 0;
        Item		item;
		int			index;

        DrawRectangle(100, 100, width - 200, height - 200, ColorAlpha(WHITE, 0.8));
        for (int i = 0; i < 5; i++) {
            for (int y = 0; y < 8; y++) {
                DrawRectangle(110 + y * 45, 130 + i * 45, 40, 40, ColorAlpha(GRAY, 0.5));
                index = y + (i * 8);
                if (index < inventory.size()) {
                    for (int j = 0; j < item_list.size(); j++) {
                        if (item_list.at(j).id == inventory.at(index).id) {
                            item = item_list[j];
							DrawTextureRec(textAtlas.at(item.text_index), (Rectangle){0, 0, 32, 32}, (Vector2){static_cast<float>(110 + y * 45 + 4), static_cast<float>(130 + i * 45 + 4)}, WHITE);
							DrawText(TextFormat("%i", inventory.at(index).stack_size), 130 + y * 45, 160 + i * 45, 10, BLACK);
                            break;
                        }
                    }
                }
            }
        }
        DrawText("Inventory", 110, 110, 10, BLACK);
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
		item_is_up = false;
	}

	~Inventory() {

	}
};

class GameMap {

	private:
	std::vector<int> background;

	public:
	//list of element
	u32	width;
	u32	height;

	void	renderMap(Texture2D background) {
		Rectangle src;
		DrawTextureRec(background, src, {0, 0}, WHITE);
	}

	GameMap() {
	}

	~GameMap() {
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

class Context {

private:

	const char				*title = "noHeaven";
	int						width = 720, height = 480;
	int						state = s_menu;
	Font					font;
	bool					inventoryOpen = false;

	void	LoadTextureAtlas() {
		textAtlas.push_back(LoadTexture("asset/texture/button.png"));
		textAtlas.push_back(LoadTexture("asset/texture/highlight.png"));
		textAtlas.push_back(LoadTexture("asset/texture/character.png"));
	}

	void	UnloadTextureAtlas() {
		for (int i = 0; i < textAtlas.size(); i++) {
			UnloadTexture(textAtlas.at(i));
		}
	}

public:

	std::vector<Texture>	textAtlas;
	std::vector<i32>		input_buffer;
	std::vector<FadeTxt>	Fadetxt_list;
	std::vector<Item>		itemsAtlas;

	void	gameInput(Player *player) {
		if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) || IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
			player->topos = screenPosToWorldPos(GetMousePosition(), player->cam.target, width, height, player->cam.zoom);
			//should move this line after pathfinding find next point and not here
			player->angle = atan2(player->topos.y - player->pos.y, player->topos.x - player->pos.x) * 180 * Q_rsqrt(PI * PI);
		}
		player->cam.zoom += ((float)GetMouseWheelMove() * 0.05f);
		Vector2 mouse_pos = GetMousePosition();
		if (IsMouseInBound((Rectangle){0, 0, static_cast<float>(width), static_cast<float>(height)}, (Vector2){ 0, 0}, mouse_pos)) {
			if (mouse_pos.x <= 0) {
				mouse_pos.x = 1;
			} else if (mouse_pos.x >= width) {
				mouse_pos.x = width - 1;
			}
			if (mouse_pos.y <= 0) {
				mouse_pos.y = 1;
			} else if (mouse_pos.y >= height) {
				mouse_pos.y = height - 1;
			}
			SetMousePosition(mouse_pos.x, mouse_pos.y);
		}
		if (IsMouseInBound((Rectangle){0, 0, static_cast<float>(width), 20}, (Vector2){ 0, 0}, mouse_pos)) {
			player->cam.target.y--;
		}
		if (IsMouseInBound((Rectangle){0, 0, 20, static_cast<float>(height)}, (Vector2){ 0, 0}, mouse_pos)) {
			player->cam.target.x--;
		}
		if (IsMouseInBound((Rectangle){0, 0, static_cast<float>(width), 20}, (Vector2){ 0, static_cast<float>(height - 20)}, mouse_pos)) {
			player->cam.target.y++;
		}
		if (IsMouseInBound((Rectangle){0, 0, 20, static_cast<float>(height)}, (Vector2){ static_cast<float>(width - 20), 0}, mouse_pos)) {
			player->cam.target.x++;
		}
		if (player->cam.zoom > 2.0f) {
			player->cam.zoom = 2.0f;
		} else if (player->cam.zoom < 0.7f) {
			player->cam.zoom = 0.7f;
		}
		switch (GetKeyPressed()) {
			//spellone
			case (KEY_Q):
				player->attribut.life -= 100;
				break;
			case (KEY_W):
			//spelltwo
				break;
			//spellthree
			case (KEY_E):
				break;
			//spellfour
			case (KEY_R):
				break;
			//interact
			case (KEY_T):
				break;
			//warp or dash or leap
			case (KEY_F):
				break;
			//center camera or jump (jump if iso or 3d game)
			case (KEY_SPACE):
				player->cam.target = player->pos;
				break;
			//open inventory or close
			case (KEY_TAB):
				inventoryOpen = !inventoryOpen;
				break;
			//openmenu
			case (KEY_ESCAPE):
				state = s_menu;
				break;
			default:
				break;
		}
		if (IsKeyDown(KEY_SPACE)) {
			player->cam.target = player->pos;
		}
	}

	void	MenuStart(double delta_time) {
		static bool window_close = false;
		static Button button[N_BUTTON_STARTUI] = {
			{
				.bound = {
					0, 0, 64, 32
				},
				.text = {0},//LoadTexture("./asset/button.png"),
				.pos = { 20, 50},
				.name = "play",	
			},
			{
				.bound = {
					0, 0, 64, 32
				},
				.text = {0},//LoadTexture("./asset/button.png"),
				.pos = { 104, 50},
				.name = "setting",
			},
			{
				.bound = {
					0, 0, 64, 32
				},
				.text = {0},//LoadTexture("./asset/button.png"),
				.pos = { 188, 50},
				.name = "leave",
			}
		};

		Vector2 mouse_pos = GetMousePosition();

		if (window_close == true) {
			if (IsKeyPressed(KEY_Y)) {
				state = s_close;
			}
			else if (IsKeyPressed(KEY_N)) {
				window_close = false;
			}
		}
		if (IsKeyPressed(KEY_ESCAPE)) {
			window_close = true;
		}
		int i = 0;
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			while (i < N_BUTTON_STARTUI && !IsMouseInBound(button[i].bound, button[i].pos, mouse_pos)) {
				i++;
			}
			switch(i) {
				case (0):
					state = s_game;
					break;
				case (1):
					state = s_setting;
					break;
				case (2):
					window_close = true;
					break;
				default:
					break;
			}
		}
		BeginDrawing();
		ClearBackground(FG);
		//render ui element here
		for (int k = 0; k < N_BUTTON_STARTUI; k++) {
			DrawTextureRec(textAtlas.at(0), button[k].bound, button[k].pos, WHITE);
			DrawTextEx(font, button[k].name.c_str(), (Vector2){ button[k].pos.x + 10, static_cast<float>(button[k].pos.y + button[k].bound.height * 0.5 - 6)}, 22, 0, FG);
		}
		DrawText("Menu", 0, 0, 25, BG);
		if (window_close == true) {
			DrawRectangle(0, height * 0.5 - 100, width, 200, BG);
			DrawTextEx(font, "Are you sure you want to exit program? [Y/N]", (Vector2){40, static_cast<float>(height * 0.5 - 20)}, 24, 0, FG);
		}
		for (int k = 0; k < N_BUTTON_STARTUI; k++) {
			if (IsMouseInBound(button[k].bound, button[k].pos, mouse_pos)) {		
				DrawTextureRec(textAtlas.at(1), button[k].bound, button[k].pos, WHITE);
			}
		}
		EndDrawing();
	}

	void	MenuSetting(double delta_time) {
		static Button button[N_BUTTON_SETTINGUI] = {
		{
			.bound = {
				0, 0, 64, 32
			},
			.text = {0},//LoadTexture("./asset/button.png"),
			.pos = { 64, 50},
			.name = "Video",	
			},
		{
			.bound = {
				0, 0, 64, 32
			},
			.text = {0},//LoadTexture("./asset/button.png"),
			.pos = { 64, 100},
			.name = "Game",
			},
		{
			.bound = {
				0, 0, 64, 32
			},
			.text = {0},//LoadTexture("./asset/button.png"),
			.pos = { 64, 150},
			.name = "Input",
			},
		{
			.bound = {
				0, 0, 64, 32
			},
			.text = {0},//LoadTexture("./asset/button.png"),
			.pos = { 64, 200},
			.name = "back",
			}
		};

		Vector2 mouse_pos = GetMousePosition();

		if (IsKeyPressed(KEY_ESCAPE)) {
			state = s_menu; 
		}
		int i = 0;
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			while (i < N_BUTTON_SETTINGUI && !IsMouseInBound(button[i].bound, button[i].pos, mouse_pos)) {
				i++;
			}
			switch(i) {
				case (0):
					break;
				case (1):
					break;
				case (2):
					break;
				case (3):
					state = s_menu;
					break;
				default:
					break;
			}
		}
		BeginDrawing();
		ClearBackground(FG);	
		DrawText("Setting", 20, 20, 30, BG);
		for (int k = 0; k < N_BUTTON_SETTINGUI; k++) {
			DrawTextureRec(textAtlas.at(0), button[k].bound, button[k].pos, WHITE);
			DrawTextEx(font, button[k].name.c_str(), (Vector2){ button[k].pos.x + 10, static_cast<float>(button[k].pos.y + button[k].bound.height * 0.5 - 6)}, 22, 0, FG);
		}
		for (int k = 0; k < N_BUTTON_SETTINGUI; k++) {
			if (IsMouseInBound(button[k].bound, button[k].pos, mouse_pos)) {		
				DrawTextureRec(textAtlas.at(1), button[k].bound, button[k].pos, WHITE);
			}
		}
		EndDrawing();
	}

	void	Game(double delta_time, Player *player) {
		gameInput(player);
		player->update(delta_time, input_buffer, &state, &Fadetxt_list);
		player->inventory->updateInventory(delta_time, height, width);
		//updateEntity(ctx);
		BeginDrawing();
		ClearBackground(BLACK);
		BeginMode2D(player->cam);
		//for (int i = 0; i < STAGE_SIZE; i++) {
		//	DrawTextureRec(ctx->stage[i].text, ctx->stage[i].rec, ctx->stage[i].pos, ctx->stage[i].tint);
		//}
		DrawTexturePro(textAtlas.at(2), player->recsource, player->bound,
			player->origin, player->angle + 90,WHITE);
		DrawLine(player->pos.x, player->pos.y,
			player->topos.x, player->topos.y, RED);
		renderFadingTxt(delta_time, &Fadetxt_list);
		EndMode2D();
		DrawRectangle(width * 0.5 - 100, height - 60, 200, 20, RED);
		DrawRectangle(width * 0.5 - 100, height - 60, player->attribut.life * 200 / player->attribut.max_life, 20, GREEN);
		DrawText(TextFormat("%i / %i", player->attribut.life, player->attribut.max_life), width * 0.5 - 50, height - 60, 20, BLACK);
		if (inventoryOpen == true) {
			player->inventory->renderInventory(delta_time, height, width, itemsAtlas, textAtlas);
		}
		DrawFPS(10, 10);
		EndDrawing();
	}

	void	loop(Player *player) {
		while(state != s_close) {
			double delta_time = GetFrameTime();
			switch (state){
			case (s_menu):
				MenuStart(delta_time);
				break;
			case (s_game):
				Game(delta_time, player);
				break;
			case (s_setting):
				MenuSetting(delta_time);
				break;
			case (s_pause):
				break;
			default:
				state = s_close;
				break;
			}
		}
	}

	Context() {
		state = s_menu;
		InitWindow(width, height, title);
		font = LoadFont("asset/font/SF_Atarian_System.ttf");
		SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);
		LoadTextureAtlas();
		SetTargetFPS(120);
	}
	~Context(void) {
		UnloadFont(font);
		UnloadTextureAtlas();
		CloseWindow();
	}
};

}

#endif