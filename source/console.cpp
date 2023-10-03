#include "engine.hpp"
# ifdef WIN_32
#  include <C:/mingw64/include/raylib.h>
# endif
#include <string.h>

# define MAX_CMD 6

static const char *cmd_list[MAX_CMD] = {
	"move",
	"delete",
	"add",
	"give",
	"rotate",
	"clear"
};

typedef enum dbc_stats{
	error = 1,
	none = 0,
}	stats_enum;

typedef enum dbc_token {
	cmd = 0,
	arg = 1,
	input = 2,
	element = 3,
	coord = 4,
}	token_enum;

/*
	dbc_move = 1,
	dbc_delete = 2,
	dbc_add = 3,
	dbc_give = 4,
	dbc_rotate = 5,
 * */

typedef struct lexer_s {
	std::string	str;
	int			idx;
}	lexer_t;

typedef struct cmd_s {
	std::string					cmd;
	std::vector<std::string>	arg;
	std::vector<token_enum>		token;
} cmd_t;

typedef struct debug_console {
	std::string					current_buffer;
	bool						is_enter;
	int							stats;
	std::vector<std::string>	error;
	std::vector<std::string>	lines;
}	t_debug_console;

t_debug_console	dbc;
extern Context	ctx;

static const char *struct_type_list[5] = {
	"world_element",
	"world_bit",
	"entity_spawn",
	"entity",
	"item"
};

int	get_arg_dbc_add(const std::string &str) {
	size_t index = 0;

	while (strncmp(str.c_str(), struct_type_list[index], str.size()) != 0 && index < 5) {
		index++;
	}
	return (index);
}

void	dbc_add(std::vector<std::string> arg) {
	//arg 1 should be what i add, arg 2 to n should be what each element of the struct is	
	switch(get_arg_dbc_add(arg[0].c_str())) {
		case (1):
			break;
		case (2):
			break;
		default:
			break;
	}
}

int	character_is_ok(int c) {
	return ((isalnum(c) || c == '_' || c == ' '));
}

int	lexer(std::vector<lexer_t> &lexer_info) {
	size_t	i = 0;
	lexer_t	tmp_token;
	
	tmp_token.idx = 0;
	while (i < dbc.current_buffer.length()) {
		if (!character_is_ok(dbc.current_buffer[i])) {
			dbc.stats = error;
			dbc.error.push_back(TextFormat("Error, couldn't handle char %c, at %i", dbc.current_buffer[i], i));
		} else if (dbc.current_buffer[i] == ' ') {
			lexer_info.push_back(tmp_token);
			tmp_token.str.clear();
			tmp_token.idx++;
		} else {
			tmp_token.str.push_back(dbc.current_buffer[i]);
		}
		i++;
	}
	lexer_info.push_back(tmp_token);
	return (i);
}

int	parser(std::vector<lexer_t> lexer_info, cmd_t *cmd) {
	if (!lexer_info.empty()) {
		cmd->cmd = lexer_info[0].str.c_str();
		for (size_t i = 1; i < lexer_info.size(); i++) {
			cmd->arg.push_back(lexer_info[i].str.c_str());
		}
	}
	return (0);
}

int	get_cmd_number(std::string &cmd) {
	for (int i = 1; i <= MAX_CMD; i++) {
		if (strncmp(cmd_list[i - 1], cmd.c_str(), cmd.length()) == 0 && !cmd.empty())
			return (i);
	}
	return (0);
}

void	initConsole(void) {
	dbc.is_enter = false;
	dbc.stats = none;
}

int	execute(cmd_t cmd) {
	int cmd_number = get_cmd_number(cmd.cmd);
	switch (cmd_number) {
		case (1):
//			dbc_move();
			break;
		case (2):
//			dbc_delete();
			break;
		case (3):
//			dbc_add();
			break;
		case (4):
//			dbc_give();
			break;
		case (5):
//			dbc_rotate();
			break;
		case (6):
			if (!dbc.lines.empty()) {
				dbc.lines.clear();
				printf("cleared terminal lines buffer\n");
			}
			break;
		default:
			dbc.error.push_back("Error! unknown Cmd pls enter a valid cmd!");
			break;
	}
	return (0);
}

void	renderConsole(double delta_time) {
	static int		current_index = 0;
	static char		blinker = '_';
	static double	time_accu = 0;
	
	time_accu += delta_time;
	if (time_accu >= 0.5) {
		if (blinker == '_') {
			blinker = 0;
		} else {
			blinker = '_';
		}
		time_accu = 0;
	}
	if (dbc.lines.size() > 10) {
		current_index = dbc.lines.size() - 10;
	}
	BeginDrawing();
		DrawRectangle(0, 0, ctx.width, 100, BG);
		for (size_t i = 0; i < 10 && i + current_index < dbc.lines.size(); i++) {
			DrawText(dbc.lines[i + current_index].c_str(), 0, i * 10, 10, FG);
		}
		DrawRectangle(0, 100, ctx.width, 12, BG);
		DrawText(TextFormat(">%s%c", dbc.current_buffer.c_str(), blinker), 2, 100, 10, FG);
	EndDrawing();
}

int	console(double delta_time) {
	std::vector<lexer_t>	lexer_info;
	cmd_t					cmd;

	if (dbc.is_enter == false) {
		int key = GetCharPressed();
		while (key > 0) {
			if (key >= 32 && key <= 125) {
				dbc.current_buffer.push_back((char)key);
			}
			key = GetCharPressed();
		}
		switch (GetKeyPressed()) {
			case (KEY_ENTER): {
				dbc.is_enter = true;
				break;
			}
			case (KEY_ESCAPE): {
				ctx.state = s_game;
				break;
			}
			case (KEY_BACKSPACE): {
				if (!dbc.current_buffer.empty())
					dbc.current_buffer.erase(dbc.current_buffer.size() - 1);
				break;
			}
		}
	} else {
		if (!dbc.current_buffer.empty()) {
			dbc.lines.push_back(dbc.current_buffer);
			lexer(lexer_info);
			if (dbc.stats != error) {
				parser(lexer_info, &cmd);
			}
			if (dbc.stats != error) {
				execute(cmd);
			}
			for (size_t i = 0; i < dbc.error.size(); i++) {
				dbc.lines.push_back(dbc.error[i]);
			}
		}
		//clear all cmd element here
//		for (size_t i = 0; i < lexer_info.size(); i++) {
//			lexer_info[i].str.clear();
//		}
//		lexer_info.clear();
//		cmd.cmd.clear();
		dbc.error.clear();
		dbc.current_buffer.clear();
		dbc.is_enter = false;
		dbc.stats = none;
	}
	renderConsole(delta_time);
	return (0);
}
