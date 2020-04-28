#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>
#include <iostream>
#include <string>
#include "maths/quaternion.h"

using namespace std;

class GameObject : public gef::MeshInstance
{
	int health;
	
public:
	
	float speed;

	enum ObjectType {player, enemy};

	ObjectType type_member;
	void UpdateFromSimulation(const b2Body* body);
	ObjectType GetObjectType();

	void SetObjectType(ObjectType type);
	void SetSpeed(float speed);

	float GetSpeed();

	void SetHealth(int health_);
	int GetHealth();

	gef::Vector4 scale = gef::Vector4(1, 1, 1, 1);

	void DealDamage();

	float rotation_x = 0.0;
	float rotation_y = 0.0;

	gef::Vector4* translation_offset = new gef::Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	
};

#endif // _GAME_OBJECT_H