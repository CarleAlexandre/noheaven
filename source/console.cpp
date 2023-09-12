#include "engine.hpp"

typedef struct debug_console {
	std::string current_buffer;
	bool		is_enter;
}	t_debug_console;

t_debug_console	dbc;

int	lexer() {
	while (1) {
		
	}
	return (0);
}

int	parser() {
	return (0);
}

int	execute() {
	return (0);
}

int	console() {
	if (dbc.is_enter == false) {
		dbc.current_buffer.push_back(GetCharPressed());
	}
	else {
		lexer();
		parser();
		execute();
		dbc.current_buffer.clear();
	}
	return (0);
}