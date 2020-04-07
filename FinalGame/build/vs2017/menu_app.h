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
#include "GameData.h"
#include "audio/audio_manager.h"


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
	MenuApp(gef::Platform& platform, GameData *data);
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

	bool exit_menu;

	//sound
	gef::AudioManager* audio_manager_;
	int id_music_menu_;

	//sound chnage difficult
	int id_change_difficult_;

	//sound start game
	int id_start_effect_;

	//input
	Int32 active_touch_id_;
	gef::Vector2 touch_position_;

	//difficulty
	GameData *data_;

	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::Renderer3D* renderer_3d_;

	PrimitiveBuilder* primitive_builder_;

	//input
	gef::InputManager* input_;

};

#endif // _MENU_APP_H
