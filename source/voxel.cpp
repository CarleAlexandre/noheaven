#include "engine.hpp"

//should have coordinate of point that draw the terrain (can be destroyed)
// and i should use the world vector to store every structure model and render them if they are in the player vision

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
