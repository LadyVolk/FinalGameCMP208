#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>
#include <iostream>

using namespace std;

class GameObject : public gef::MeshInstance
{
	
	
public:
	
	float speed;

	enum ObjectType { player, enemy };
	ObjectType type_member;
	void UpdateFromSimulation(const b2Body* body);
	ObjectType GetType();
	void SetType(ObjectType type);
	void SetSpeed(float speed);
	float GetSpeed();
};

#endif // _GAME_OBJECT_H