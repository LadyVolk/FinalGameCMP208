#include "game_object.h"
#include "system/debug_log.h"
#include <string>

//
// UpdateFromSimulation
// 
// Update the transform of this object from a physics rigid body
//
void GameObject::UpdateFromSimulation(const b2Body* body)
{
	if (body)
	{
		//set up object scale
		gef::Matrix44 object_scale;
		object_scale.Scale(scale);

		// setup object rotation
		gef::Matrix44 object_rotation;
		gef::Quaternion quart(rotation_x, rotation_y, body->GetAngle(), 1.0f);
		object_rotation.Rotation(quart);

		// setup the object translation
		gef::Vector4 object_translation(body->GetPosition().x, body->GetPosition().y, 0.0f);

		// build object transformation matrix
		gef::Matrix44 object_transform = object_rotation*object_scale;
		object_transform.SetTranslation(object_translation);
		set_transform(object_transform);
	}
}

void GameObject::SetObjectType(ObjectType type) {
	type_member = type;
}
GameObject::ObjectType GameObject::GetObjectType() {
	return type_member;
}

void GameObject::SetSpeed(float speed_) {
	speed = speed_;
}

float GameObject::GetSpeed() {
	return speed;
}

void GameObject::SetHealth(int health_) {
	health = health_;
}

int GameObject::GetHealth() {
	return health;
}

void GameObject::DealDamage() {
	health -= 1;
}