#include "engine.hpp"
#include <cctype>
#include <raylib.h>

# define MAX_CMD 5

static const char *cmd_list[MAX_CMD] = {
	"move\0",
	"delete\0",
	"add\0",
	"give\0",
	"rotate\0"};

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

int	character_is_ok(int c) {
	return ((isalnum(c) || c == '_' || c == ' '));
}

int	lexer(std::vector<lexer_t> &lexer_info) {
	int		i = 0;
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
	return (i);
}

int	parser(std::vector<lexer_t> &lexer_info, cmd_t &cmd) {
	return (0);
}

int	get_cmd_number(std::string &cmd) {
	for (int i = 1; i < MAX_CMD; i++) {
		if (cmd_list[i] == cmd)
			return (i);
	}
	return (0);
}



int	execute(cmd_t &cmd) {
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
		default:
			dbc.error.push_back("Error! unknown Cmd pls enter a valid cmd!");
			break;
	}
	return (0);
}

void	renderConsole(double delta_time) {
	static int	current_index = 0;
	char		blinker = 0;
	double		time_accu;
	
	time_accu += delta_time;
	if (time_accu >= 1) {
		if (blinker & '_') {
			blinker = 0;
		} else {
			blinker = '_';
		}
		time_accu = 0;
	}
	BeginDrawing();
		DrawRectangle(0, 0, ctx.width, 100, BG);
		for (int i = 0; i < 5 && i + current_index < dbc.lines.size(); i++) {
			DrawText(dbc.lines[i + current_index].c_str(), 0, i * 20, 18, FG);
		}
		DrawRectangle(0, 101, ctx.width, 20, BG);
		DrawText(TextFormat("%s%c", dbc.current_buffer.c_str(), blinker), 0, 101, 18, FG);
	EndDrawing();
}

int	console(double delta_time) {
	std::vector<lexer_t>	lexer_info;
	cmd_t					cmd;

	if (dbc.is_enter == false) {
		dbc.current_buffer.push_back(GetCharPressed());
		if (IsKeyPressed(KEY_ENTER))
			dbc.is_enter = true;
	}
	else {
		if (!dbc.current_buffer.empty()) {
			dbc.lines.push_back(dbc.current_buffer);
			lexer(lexer_info);
			if (dbc.stats != error) {
				parser(lexer_info, cmd);
			}
			if (dbc.stats != error) {
				execute(cmd);
			}
			for (int i = 0; i < dbc.error.size(); i++) {
				dbc.lines.push_back(dbc.error[i]);
			}
		}
		for (int i = 0; i < lexer_info.size(); i++) {
			lexer_info[i].str.clear();
		}
		lexer_info.clear();
		cmd.cmd.clear();
		//clear all cmd element here
		dbc.error.clear();
		dbc.current_buffer.clear();
		dbc.is_enter = false;
	}
	renderConsole(delta_time);
	return (0);
}
