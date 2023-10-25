#define VOXEL_ENGINE
#include "engine.hpp"

short	set_voxel_pos(Vector3 pos) {
	short	position;

	for (int i = 0; i < 14; i++) {
		if (i < 4) {
			if (((int)pos.z >> i) & 1) {
				position += 1 << i;
			}
		} else if (i > 4 && i < 9) {
			if (((int)pos.y >> (i - 5)) & 1) {
				position += 1 << i;
			}
		} else if (i > 9 && i < 14) {
			if (((int)pos.x >> (i - 10)) & 1) {
				position += 1 << i;
			}
		}
	}
	return (position);
}

Vector3 get_voxel_pos(short pos) {
	Vector3	position = {0, 0, 0};

	for (int i = 0; i < 4; i++) {
		if ((pos >> i) & 1) {
			position.z += 1 << i;
		}
	}
	for (int i = 5; i < 9; i++) {
		if ((pos >> i) & 1) {
			position.y += 1 << (i - 5);
		}
	}
	for (int i = 10; i < 14; i++) {
		if ((pos >> i) & 1) {
			position.x += 1 << (i - 10);
		}
	}
	return (position);
}

t_chunk	*chunk_manager_load(Vector2 chunkpos, const char *chunkfile_path) {
	t_chunk		*chunk;
	FileMgr		fmg;
	static char	*ptr;

	if (chunkfile_path != NULL) {
		ptr = fmg.toData(chunkfile_path);
	}

	chunk = static_cast<t_chunk *>(malloc(sizeof(t_chunk)));
	if (!chunk) {
		return (NULL);
	}

	chunk->blocks = static_cast<t_voxel *>(malloc(sizeof(t_voxel)));
	if (!chunk->blocks) {
		free(chunk);
		return (NULL);
	}
	
	memcpy(chunk->blocks,(ptr + (int)chunkpos.x + (int)chunkpos.y), CHUNK_SIZE);
	return (chunk);
}

void	chunk_manager_unload(t_chunk *data) {
	free(data->blocks);
	free(data);
}

size_t	chunk_manager_query(Vector3 pos) {
	return (pos.x * CHUNK_WIDTH + pos.y * CHUNK_HEIGHT + pos.z * CHUNK_LENGTH);
}

//should have coordinate of point that draw the terrain (can be destroyed)
// and i should use the world vector to store every structure model and render them if they are in the player vision

t_voxel	*create_voxel(Vector3 coord) {
	t_voxel	*voxel;

	voxel = static_cast<t_voxel *>(malloc(sizeof(t_voxel)));
	return (voxel);
}

void	render_voxel(std::vector<t_chunk> &chunks) {
	for(int i = 0; i < chunks.size(); i++) {
		
	}
}

void	deleteGround(Vector2 coord, Mesh &terrain) {
	int maxx, minx;
	int maxz, minz;

	maxx = floorf(coord.x + 1);
	minx = floorf(coord.x);
	maxz = floorf(coord.y + 1);
	maxz = floorf(coord.y);

	//need to make a function to get pos at x, z in the current mesh vertices
}

Mesh	voxel(Image heightmap) {
	Mesh mesh = { 0 };
	int x = 0, z = 0;

	mesh.triangleCount = (heightmap.width - 1) * (heightmap.height - 1) * 2;
	mesh.vertexCount = mesh.triangleCount * 3;
	while (x * z > heightmap.width * heightmap.height) {
		
		x++;
		if (x > heightmap.width) {
			z++;
			x = 0;
		}
	}
	return (mesh);
}
