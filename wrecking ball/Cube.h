#ifndef _CUBE_H_
#define _CUBE_H_

#include "Geode.h"
#include "Vector3d.h"
#include "Constant.h"
#include "Material.h"


class Cube : public Geode{

public:
	Cube(double s, Vector3d c, draw::mode m, Material * ma = NULL);
	Bs update();
private:
	void render();
	Material * material;
	double size;
	Vector3d color; 
	draw::mode mode;
};

#endif