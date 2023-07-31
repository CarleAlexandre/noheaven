#include "class.hpp"

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

int	main(void) {
	Game::Context		*ctx = new (Game::Context);
	Game::Player		*player = new (Game::Player);
	engine::FileMgr		*filemgr = new (engine::FileMgr);

	//ctx->itemsAtlas = filemgr->load_Items();
	//ctx->textAtlas = filemgr->load_Textures();
	ctx->loop(player);
	return (0);
}
