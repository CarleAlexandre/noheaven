#include "class.hpp"

int	main(void) {
	double delta_time = 0;
	engine::Context ctx = engine::Context();
	engine::Render render = engine::Render();
	engine::Player player = engine::Player();
	engine::Entity entity = engine::Entity();
	engine::GameMap gamemap = engine::GameMap();
	engine::Loot loot = engine::Loot();
	while (!WindowShouldClose()) {
		delta_time = GetFrameTime();
		ctx.input();
		player.update(delta_time, ctx.input_buffer);
		entity.update(delta_time);
		render.render(0x00, delta_time);
	}
	return (0);
}
