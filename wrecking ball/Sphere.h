#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "Geode.h"
#include "Constant.h"
#include "Vector3d.h"
#include "Material.h"

class Sphere : public Geode{
public:
	Sphere(double r, int slices, int stacks, Vector3d color, draw::mode m, Material *ma = NULL);
	Bs update();
private:
	void render();
	double radius;
	int slices;
	int stacks;
	Vector3d color; 
	draw::mode mode;
	Material *material;
};

#endif