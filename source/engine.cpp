#include "engine.hpp"

#define FRAME_SEC 120

Context ctx;

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

void	addFadingTxt(std::string text, double delay, Color color, int font_size, Vector2 pos, std::vector<s_FadeTxt> *Fadetxt_list) {
	Fadetxt_list->push_back(s_FadeTxt{
		.fmt = text,
		.color = color,
		.pos = pos,
		.delay = delay,
		.time = 0.0f,
		.alpha = 1.0f,
		.font_size = font_size,
	});
}

void	renderFadingTxt(double delta_time, std::vector<s_FadeTxt> *Fadetxt_list) {
	if (Fadetxt_list->size() == 0)
		return;
	for (i32 i = 0; i < Fadetxt_list->size(); i++) {
		if (Fadetxt_list->at(i).time >= Fadetxt_list->at(i).delay) {
			Fadetxt_list->erase(Fadetxt_list->begin() + i);
			return;
		}
		Fadetxt_list->at(i).alpha = ((Fadetxt_list->at(i).delay - Fadetxt_list->at(i).time) / Fadetxt_list->at(i).delay);
		DrawText(Fadetxt_list->at(i).fmt.c_str(), Fadetxt_list->at(i).pos.x, Fadetxt_list->at(i).pos.y, Fadetxt_list->at(i).font_size, Fade(Fadetxt_list->at(i).color, Fadetxt_list->at(i).alpha));
		Fadetxt_list->at(i).time += delta_time;
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
			ctx.state = s_close;
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
				ctx.state = s_game;
				break;
			case (1):
				ctx.state = s_setting;
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
		DrawTextureRec(ctx.textAtlas.at(0), button[k].bound, button[k].pos, WHITE);
		DrawTextEx(ctx.font, button[k].name.c_str(), (Vector2){ button[k].pos.x + 10, static_cast<float>(button[k].pos.y + button[k].bound.height * 0.5 - 6)}, 22, 0, FG);
	}
	DrawText("Menu", 0, 0, 25, BG);
	if (window_close == true) {
		DrawRectangle(0, ctx.height * 0.5 - 100, ctx.width, 200, BG);
		DrawTextEx(ctx.font, "Are you sure you want to exit program? [Y/N]", (Vector2){40, static_cast<float>(ctx.height * 0.5 - 20)}, 24, 0, FG);
	}
	for (int k = 0; k < N_BUTTON_STARTUI; k++) {
		if (IsMouseInBound(button[k].bound, button[k].pos, mouse_pos)) {		
			DrawTextureRec(ctx.textAtlas.at(1), button[k].bound, button[k].pos, WHITE);
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
		ctx.state = s_menu; 
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
				ctx.state = s_menu;
				break;
			default:
				break;
		}
	}
	BeginDrawing();
	ClearBackground(FG);	
	DrawText("Setting", 20, 20, 30, BG);
	for (int k = 0; k < N_BUTTON_SETTINGUI; k++) {
		DrawTextureRec(ctx.textAtlas.at(0), button[k].bound, button[k].pos, WHITE);
		DrawTextEx(ctx.font, button[k].name.c_str(), (Vector2){ button[k].pos.x + 10, static_cast<float>(button[k].pos.y + button[k].bound.height * 0.5 - 6)}, 22, 0, FG);
	}
	for (int k = 0; k < N_BUTTON_SETTINGUI; k++) {
		if (IsMouseInBound(button[k].bound, button[k].pos, mouse_pos)) {		
			DrawTextureRec(ctx.textAtlas.at(1), button[k].bound, button[k].pos, WHITE);
		}
	}
	EndDrawing();
}

void	LoadTextureAtlas() {
	ctx.textAtlas.push_back(LoadTexture("asset/texture/button.png"));
	ctx.textAtlas.push_back(LoadTexture("asset/texture/highlight.png"));
	ctx.textAtlas.push_back(LoadTexture("asset/texture/character.png"));
}

void	UnloadTextureAtlas() {
	for (int i = 0; i < ctx.textAtlas.size(); i++) {
		UnloadTexture(ctx.textAtlas.at(i));
	}
}

// Function to convert screen coordinates to a 3D ray
/*Ray ScreenToRay(Camera3D camera, Vector2 screenPoint) {
	Matrix view = GetCameraMatrix(camera);
	Matrix projection = CAMERA_PERSPECTIVE;
	// Calculate the screen space point in normalized device coordinates (NDC)
	Vector3 rayStartNDC = { 2.0f * (screenPoint.x / GetScreenWidth()) - 1.0f, 1.0f - 2.0f * (screenPoint.y / GetScreenHeight()), -1.0f };
	// Calculate the ray direction in world space
	Vector3 rayEndNDC = { rayStartNDC.x, rayStartNDC.y, 1.0f };
	Vector3 rayStartWorld = Vector3Unproject(rayStartNDC, camera.position,
	camera.target, camera.up);
	Vector3 rayEndWorld = Vector3Unproject(rayEndNDC, camera.position,
	camera.target, camera.up);
	Vector3 rayDirectionWorld = Vector3Normalize(Vector3Subtract(rayEndWorld,
	rayStartWorld));
	Ray ray = { rayStartWorld, rayDirectionWorld };
	return ray;
}

Vector3 getTerrainCollision(Camera3D camera, Vector3 terrainNormal, Vector3 terrainOrigin) {
	Vector3	terrainCollision;
	bool	result = false;

	Vector3 MulMat = camera.up;

	Vector3 ray = Vector3Subtract(camera.target, camera.position);
	ray = Vector3Normalize(ray);
	ray = Vector3Multiply(ray, camera.up);
	while (!result) {
		terrainCollision =
	}
	//need to really know what is the Camera up, target and pos exactly
	return (terrainCollision);
}*/

void	gameInput3d(Player *player, Light *light) {
	player->cam.position.z += GetMouseWheelMove();
	switch (GetKeyPressed()) {
		case (KEY_Q):
			player->attribut.life -= 100;
			break;
		case (KEY_W):
			player->topos.y += 1.0f;
			break;
		case (KEY_S):
			player->topos.y -= 1.0f;
			break;
		case (KEY_D):
			player->topos.x += 1.0f;
			break;
		case (KEY_A):
			player->topos.x -= 1.0f;
			break;
		case (KEY_E):
			light->enabled = !light->enabled;
			break;
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
			player->cam.target.x = player->pos.x;
			player->cam.target.y = player->pos.y;
			player->cam.position.x = player->pos.x + 2;
			player->cam.position.y = player->pos.y - 3;
			break;
		//open inventory or close
		case (KEY_TAB):
			ctx.inventoryOpen = true;
			break;
		//openmenu
		case (KEY_ESCAPE):
			ctx.state = s_menu;
			break;
		case (KEY_F1):
			ctx.state = s_debug;
			break;
		default:
			break;
	}
	if (IsKeyDown(KEY_SPACE)) {
		player->cam.target = player->pos;
	}
	if (IsKeyDown(KEY_W))
		player->topos.y += 1.0f;
	if (IsKeyDown(KEY_S))
		player->topos.y -= 1.0f;
	if (IsKeyDown(KEY_D))
		player->topos.x += 1.0f;
	if (IsKeyDown(KEY_A))
		player->topos.x -= 1.0f;
}

void	GameLogic(double delta_time, Player *player) {
	static Light light = CreateLight(LIGHT_POINT, {10, 10, 15}, Vector3Zero(), WHITE, ctx.shader);
	static double current_time = 0.0;

	if (ctx.inventoryOpen == false) {
		gameInput3d(player, &light);
		player->update(delta_time, ctx.input_buffer, &ctx.state, &ctx.Fadetxt_list);
		//updateEntity(ctx);
	} else {
		player->inventory->updateInventory(delta_time, ctx.width, ctx.height);
		if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_ESCAPE)) {
			ctx.inventoryOpen = false;
			player->inventory->resetRender(ctx.height, ctx.width, 100, ctx.height * 0.25);
		}
	}
	//update shader here
	//SetShaderValue(ctx.shader, ctx.shader.locs[SHADER_LOC_VECTOR_VIEW], &player->cam.position, SHADER_UNIFORM_VEC3);
	//SetShaderValue(ctx.filterShader, ctx.filterShader.locs[SHADER_LOC_VECTOR_VIEW], &player->cam.position, SHADER_UNIFORM_VEC3);
	//UpdateLightValues(ctx.shader, light);

	//BeginTextureMode(ctx.fbo);
	//	ClearBackground(BLACK);
	//	BeginMode3D(player->cam);
	//		DrawLine3D({-100, 0, 0}, {100, 0, 0}, RED);
	//		DrawLine3D({0, -100, 0}, {0, 100, 0}, GREEN);
	//		DrawLine3D({0, 0, -100}, {0, 0, 100}, BLUE);
	//		//DrawModelEx(player->model, player->pos, Vector3Zero(), 0, Vector3One(), WHITE);
	//		for (int i = 0; i < ctx.world.size(); i++) {
	//			DrawModelEx(ctx.world.at(i).model, ctx.world.at(i).pos, ctx.world.at(i).rot_axis, ctx.world.at(i).rot_angle,ctx.world.at(i).scale, ctx.world.at(i).tint);
	//		}
	//	EndMode3D();
	//EndTextureMode();
	BeginDrawing();
		ClearBackground(RAYWHITE);
		BeginMode3D(player->cam);
			DrawLine3D({-100, 0, 0}, {100, 0, 0}, RED);
			DrawLine3D({0, -100, 0}, {0, 100, 0}, GREEN);
			DrawLine3D({0, 0, -100}, {0, 0, 100}, BLUE);
			//DrawModelEx(player->model, player->pos, Vector3Zero(), 0, Vector3One(), WHITE);
			for (int i = 0; i < ctx.world.size(); i++) {
				DrawModelEx(ctx.world.at(i).model, ctx.world.at(i).pos, ctx.world.at(i).rot_axis, ctx.world.at(i).rot_angle,ctx.world.at(i).scale, ctx.world.at(i).tint);
			}
		EndMode3D();
		//BeginShaderMode(ctx.filterShader);
//        	DrawTextureRec(ctx.fbo.texture, {0, 0, static_cast<float>(ctx.width), -static_cast<float>(ctx.height)}, {0, 0}, WHITE);
		//EndShaderMode();

		renderFadingTxt(delta_time, &ctx.Fadetxt_list);
		DrawRectangle(ctx.width * 0.5 - 100, ctx.height - 60, 200, 20, RED);
		DrawRectangle(ctx.width * 0.5 - 100, ctx.height - 60, player->attribut.life * 200 / player->attribut.max_life, 20, GREEN);
		DrawText(TextFormat("%.0f / %.0f", player->attribut.life, player->attribut.max_life), ctx.width * 0.5 - 50, ctx.height - 60, 20, BLACK);
		if (ctx.inventoryOpen == true) {
			player->inventory->renderInventory(delta_time, ctx.itemsAtlas, ctx.textAtlas);
		}
		DrawText(TextFormat("%0.2f, %0.2f", player->pos.x, player->pos.y), 10, 200, 20, BLACK);
		DrawText(TextFormat("%0.2f %i", player->attribut.speed * delta_time, FRAME_SEC), 10, 50, 20, WHITE);
		DrawFPS(10, 10);
	EndDrawing();
}

/*Mesh	LoadTerrainMesh(int level_index, int level_size) {
	return(GenMeshHeightMap(TextFormat("asset/levels/heightmap%i.jpg", level_index), level_size));

}*/

static Mesh genMap(Image heightmap, int size) {
	Mesh mesh = { 0 };
	int mapx = heightmap.width;
	int mapz = heightmap.height;

	Color *pixels = LoadImageColors(heightmap);

	mesh.triangleCount = (mapx - 1) * (mapz - 1) * 2;
	mesh.vertexCount = mesh.triangleCount * 3;
	return (mesh);
}

int	main(void) {
	Player		*player = new (Player);
	FileMgr		*filemgr = new (FileMgr);

	ctx.title = "noHeaven";
	ctx.width = 720;
	ctx.height = 480;
	ctx.state = s_menu;
	ctx.inventoryOpen = false;
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(ctx.width, ctx.height, ctx.title);
	ctx.font = LoadFont("asset/font/SF_Atarian_System.ttf");
	SetTextureFilter(ctx.font.texture, TEXTURE_FILTER_TRILINEAR);
	LoadTextureAtlas();

	

	Image heightmap = LoadImage("asset/heightmap.png");

	Model custom = LoadModelFromMesh(genMap(heightmap, 1));
	//Model terrain = LoadModelFromMesh(GenMeshHeightmap(heightmap, {1, 1, 1}));
	//Model water = LoadModelFromMesh(GenMeshPlane(100, 100, 1, 1));

	UnloadImage(heightmap);

	//ctx.shader = LoadShader("shader/shader.vs", "shader/shader.fs");
	//ctx.shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(ctx.shader, "viewPos");
	//float shaderposlocaltmp[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    //SetShaderValue(ctx.shader, GetShaderLocation(ctx.shader, "ambient"), shaderposlocaltmp, SHADER_UNIFORM_VEC4);

	//sphere.materials[0].shader = ctx.shader;
	//terrain.materials[0].shader = ctx.shader;
	//water.materials[0].shader = ctx.shader;
	
	//player->model = sphere;
	//ctx.world.push_back({.model = terrain, .rot_angle = 90, .pos = { -50, 50, 0}, .rot_axis = {1.0f, 0, 0}, .scale = {100, 1, 100}, .tint = GREEN});
	ctx.world.push_back({.model = custom, .rot_angle = 90, .pos = {0, 0, 0.2}, .rot_axis = {1.0f, 0, 0}, .scale = {5, 5, 5}, .tint = BLACK});

	ctx.itemsAtlas = filemgr->loadItemsFromFile("asset/data/items.nhc");
	//for (int i = 0; i < 100; i++) {
	//	player->inventory->add(0, 999, ctx.itemsAtlas);
	//}
	player->inventory->init(ctx.height, ctx.width, 100, ctx.height * 0.25);
	ctx.height = GetScreenHeight();
	ctx.width = GetScreenWidth();
	SetTargetFPS(120);
	initConsole();

	//ctx.fbo = LoadRenderTexture(ctx.width, ctx.height);
    //SetTextureFilter(ctx.fbo.texture, TEXTURE_FILTER_BILINEAR);

    //ctx.filterShader = LoadShader(0, "shader/style_filter.fs");

	while(ctx.state != s_close) {
		if (IsWindowResized() == true) {
			ctx.height = GetScreenHeight();
			ctx.width = GetScreenWidth();
		}
		double delta_time = GetFrameTime();
		switch (ctx.state) {
			case (s_menu):
				MenuStart(delta_time);
				break;
			case (s_game):
				GameLogic(delta_time, player);
				break;
			case (s_setting):
				MenuSetting(delta_time);
				break;
			case (s_pause):
				break;
			case (s_debug):
				console(delta_time);
				break;
			default:
				ctx.state = s_close;
				break;
		}
	}
	UnloadFont(ctx.font);
	//UnloadModel(sphere);
	//UnloadModel(terrain);
	//UnloadModel(water);
	ctx.world.clear();
	//need to add this method : player->inventory.clear();
	UnloadShader(ctx.shader);
	UnloadShader(ctx.filterShader);
	UnloadTextureAtlas();
	CloseWindow();
	return (0);
}

