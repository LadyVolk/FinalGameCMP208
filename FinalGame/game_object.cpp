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
		// setup object rotation
		gef::Matrix44 object_rotation;
		object_rotation.RotationZ(body->GetAngle());

		// setup the object translation
		gef::Vector4 object_translation(body->GetPosition().x, body->GetPosition().y, 0.0f);

		// build object transformation matrix
		gef::Matrix44 object_transform = object_rotation;
		object_transform.SetTranslation(object_translation);
		set_transform(object_transform);
	}
}

void GameObject::SetType(ObjectType type) {
	type_member = type;
}
GameObject::ObjectType GameObject::GetType() {
	return type_member;
}

void GameObject::SetSpeed(float speed_) {
	speed = speed_;
}

float GameObject::GetSpeed() {
	return speed;
}