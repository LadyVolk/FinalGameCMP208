#ifndef _MENU_APP_H
#define _MENU_APP_H

#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>
#include "game_object.h"
#include <input/input_manager.h>
#include <vector>
#include <string>


// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
}

class MenuApp : public gef::Application
{
public:
	MenuApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
private:
	
	void InitFont();
	void CleanUpFont();
	void DrawHUD();
	void HandleInput(float timeStep);
	void ProcessTouchInput();

	//input
	Int32 active_touch_id_;
	gef::Vector2 touch_position_;

	//difficulty
	enum Difficulty { easy, medium, hard };
	Difficulty difficulty_ = hard;

	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::Renderer3D* renderer_3d_;

	PrimitiveBuilder* primitive_builder_;

	//input
	gef::InputManager* input_;

};

#endif // _MENU_APP_H
