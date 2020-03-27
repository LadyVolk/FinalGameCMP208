#ifndef _SCENE_APP_H
#define _SCENE_APP_H

#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>
#include "game_object.h"
#include <input/input_manager.h>
#include <vector>
#include <string>
#include <platform/d3d11/system/platform_d3d11.h>
#include "GameData.h"

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
}

class SceneApp : public gef::Application
{
public:
	SceneApp(gef::Platform& platform, GameData *data);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
private:
	void InitPlayer();
	void CreateEnemy(float x);
	void InitGround();
	void InitWalls();
	void InitFont();
	void CleanUpFont();
	void DrawHUD();
	void SetupLights();
	void HandleInput(float timeStep);
    
	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::Renderer3D* renderer_3d_;

	PrimitiveBuilder* primitive_builder_;

	//difficulty
	GameData* data_;

	// create the physics world
	b2World* world_;

	// player variables
	GameObject player_;
	b2Body* player_body_;
	bool player_is_dead;

	//enemy variables
	gef::Vector4 enemy_dimensions_;
	vector <GameObject*> enemies_;
	vector <b2Body*> enemy_bodies_;

	// ground variables
	gef::Mesh* ground_mesh_;
	GameObject ground_;
	b2Body* ground_body_;
	gef::Vector4 arena_dimensions_;

	//wall variables
	gef::Vector4 wall_dimensions_;

	gef::Mesh* wall_mesh_;
	GameObject wall_1_;
	b2Body* wall_body_1_;

	GameObject wall_2_;
	b2Body* wall_body_2_;

	//input
	gef::InputManager* input_;

	float fps_;

	float score;
};

#endif // _SCENE_APP_H
