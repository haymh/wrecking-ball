#include "Cube.h"

Cube::Cube(double s, Vector3d c, draw::mode m, Material * ma){
	size = s;
	color = c;
	mode = m;
	material = ma;
	bs.radius = size * cos(45 / 180 * M_PI);
	bs.center = Vector3d(0, 0, 0);
}

void Cube::render(){
	if (material != NULL){
		material->apply();
	}
	else
		glColor3d(color[0], color[1], color[2]);
	switch (mode){
	case draw::SOLID:
		glutSolidCube(size);
		break;
	case draw::WIRE:
		glutWireCube(size);
		break; 
	}
}

Bs Cube::update(){
	
	return bs;
}

