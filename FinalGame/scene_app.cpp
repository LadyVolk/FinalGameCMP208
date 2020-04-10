#include "scene_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <system/debug_log.h>
#include <graphics/renderer_3d.h>
#include <graphics/mesh.h>
#include <maths/math_utils.h>
#include <iostream>
#include "system/debug_log.h"
#include "input/keyboard.h"
#include "box2d/b2_math.h"
#include "box2d/b2_shape.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "audio/audio_manager.h"


using namespace std;

SceneApp::SceneApp(gef::Platform& platform, GameData *data) :
	Application(platform),
	sprite_renderer_(NULL),
	renderer_3d_(NULL),
	primitive_builder_(NULL),
	font_(NULL),
	world_(NULL),
	player_body_(NULL),
	data_(data)
{
}

void SceneApp::Init()
{
	gef::DebugOut("\n");
	gef::DebugOut("------------ START NOW INIT ------------");
	gef::DebugOut("\n");

	//sound stuff
	audio_manager_ = gef::AudioManager::Create();
	gef::VolumeInfo volume_;
	volume_.volume = 5.0;

	id_music_menu_ = audio_manager_->LoadMusic("game_music.wav", platform_);
	int id_sample_new = audio_manager_->PlayMusic();
	audio_manager_->SetMusicVolumeInfo(volume_);

	id_start_sound_ = audio_manager_->LoadSample("start.wav", platform_);
	gef::VolumeInfo volume_2;
	volume_2.volume = 50.0f;
	audio_manager_->SetSampleVoiceVolumeInfo(id_start_sound_, volume_2);
	int id_sample_ = audio_manager_->PlaySample(id_start_sound_);

	id_damage_sound_ = audio_manager_->LoadSample("damage.wav", platform_);

	id_hit_enemy_sound_ = audio_manager_->LoadSample("hit_enemy.wav", platform_);

	id_death_sound_ = audio_manager_->LoadSample("death.wav", platform_);
	
	

	//setting arena variables
	arena_dimensions_.set_value(30.f, 1.f, 5.0f);
	wall_dimensions_.set_value(1.0f, 15.0f, 10.0f);
	enemy_dimensions_.set_value(0.5f, 0.5f, 0.5f);

	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);

	// create the renderer for draw 3D geometry
	renderer_3d_ = gef::Renderer3D::Create(platform_);

	// initialise primitive builder to make create some 3D geometry easier
	primitive_builder_ = new PrimitiveBuilder(platform_);

	InitFont();
	SetupLights();

	// initialise the physics world
	b2Vec2 gravity(0.0f, -9.81f);
	world_ = new b2World(gravity);

	//rand
	srand(time(NULL));
	rand();

	score = 0;

	// load the assets in from the .scn
	const char* scene_asset_filename = "final_moon.scn";

	scene_assets_ = LoadSceneAssets(platform_, scene_asset_filename);

	if (scene_assets_)
	{
		moon_mesh_.set_mesh(GetMeshFromSceneAssets(scene_assets_));
	}
	else
	{
		gef::DebugOut("Scene file %s failed to load\n", scene_asset_filename);
	}

	InitPlayer();
	player_is_dead = false;
	InitGround();
	InitWalls();
	
	switch (data_->difficulty_) {
		case GameData::hard:
			CreateEnemy(6);
			CreateEnemy(-6);
		case GameData::medium:
			CreateEnemy(4);
			CreateEnemy(-4);
		case GameData::easy:
			CreateEnemy(2);
			CreateEnemy(-2);
	}
	
	//create input object
	input_ = gef::InputManager::Create(platform_);

}

void SceneApp::CleanUp()
{
	// destroying the physics world also destroys all the objects within it
	delete world_;
	world_ = NULL;

	delete ground_mesh_;
	ground_mesh_ = NULL;

	CleanUpFont();

	delete primitive_builder_;
	primitive_builder_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	delete scene_assets_;
	scene_assets_ = NULL;

	audio_manager_->UnloadAllSamples();

	audio_manager_->UnloadMusic();

	delete audio_manager_;
	audio_manager_ = NULL;
	
}

bool SceneApp::Update(float frame_time)
{
	int i;

	if (!player_is_dead) {
		score += frame_time;
	}

	fps_ = 1.0f / frame_time;

	// update physics world
	float timeStep = 1.0f / 60.0f;

	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	world_->Step(timeStep, velocityIterations, positionIterations);

	//handle input
	HandleInput(timeStep);

	// update object visuals from simulation data
	if (!player_is_dead) {
		player_.UpdateFromSimulation(player_body_);
	}
	
	for (i = 0; i < enemies_.size(); i++) {
		enemies_[i]->UpdateFromSimulation(enemy_bodies_[i]);
	}

	// don't have to update the ground visuals as it is static


	// collision detection
	// get the head of the contact list
	b2Contact* contact = world_->GetContactList();
	// get contact count
	int contact_count = world_->GetContactCount();
	
	bool destroy_player = false;
	for (int contact_num = 0; contact_num<contact_count; ++contact_num)
	{
		if (contact->IsTouching())
		{
			// get the colliding bodies
			b2Body* bodyA = contact->GetFixtureA()->GetBody();
			b2Body* bodyB = contact->GetFixtureB()->GetBody();
			GameObject* dataA = ((GameObject*)bodyA->GetUserData());
			GameObject* dataB = ((GameObject*)bodyB->GetUserData());
			
			// DO COLLISION RESPONSE HERE
			if (bodyA->GetUserData() != nullptr && bodyB->GetUserData() != nullptr) {
				GameObject::ObjectType type_a = dataA->GetObjectType();
				GameObject::ObjectType type_b = dataB->GetObjectType();
				
				if ((type_a == GameObject::player && type_b == GameObject::enemy) ||
					(type_a == GameObject::enemy && type_b == GameObject::player)) {
					if (type_a == GameObject::player) {
						dataA->DealDamage();
					
						if (dataA->GetHealth() <= 0) {
							audio_manager_->StopPlayingSampleVoice(id_start_sound_);
							gef::VolumeInfo* volume = new gef::VolumeInfo();
							volume->volume = 60.0f;
							audio_manager_->SetSampleVoiceVolumeInfo(id_death_sound_, *volume);
							int id_sample_death_ = audio_manager_->PlaySample(id_death_sound_);
							destroy_player = true;
						}
						else {
							gef::VolumeInfo* volume = new gef::VolumeInfo();
							volume->volume = 60.0f;
							audio_manager_->SetSampleVoiceVolumeInfo(id_damage_sound_, *volume);
							int id_sample_damage_ = audio_manager_->PlaySample(id_damage_sound_);
						}
					}
					else {
						dataB->DealDamage();
						if (dataB->GetHealth() <= 0) {
							destroy_player = true;
						}
					}
					
				}
				else if (type_a == GameObject::enemy && type_b == GameObject::enemy) {
					 gef::VolumeInfo volume;
					 volume.volume = 10.0f;
					 audio_manager_->SetSampleVoiceVolumeInfo(id_hit_enemy_sound_, volume);
					 int id_sample_ = audio_manager_->PlaySample(id_hit_enemy_sound_);
				}
				
			}
		}

		// Get next contact point
		contact = contact->GetNext();
	}
	if (destroy_player) {
		player_is_dead = true;
		world_->DestroyBody(player_body_);
	}


	return true;
}

void SceneApp::Render()
{
	int i;
	// setup camera

	// projection
	float fov = gef::DegToRad(45.0f);
	float aspect_ratio = (float)platform_.width() / (float)platform_.height();
	gef::Matrix44 projection_matrix;
	projection_matrix = platform_.PerspectiveProjectionFov(fov, aspect_ratio, 0.1f, 100.0f);
	renderer_3d_->set_projection_matrix(projection_matrix);

	// view
	gef::Vector4 camera_eye(0.0f, 8.0f, 30.0f);
	gef::Vector4 camera_lookat(0.0f, 8.0f, 0.0f);
	gef::Vector4 camera_up(0.0f, 1.0f, 0.0f);
	gef::Matrix44 view_matrix;
	view_matrix.LookAt(camera_eye, camera_lookat, camera_up);
	renderer_3d_->set_view_matrix(view_matrix);


	// draw 3d geometry
	renderer_3d_->Begin();

	// draw ground
	renderer_3d_->DrawMesh(ground_);

	//draw walls
	renderer_3d_->DrawMesh(wall_1_);
	renderer_3d_->DrawMesh(wall_2_);

	// draw player
	if (!player_is_dead) {
		//renderer_3d_->set_override_material(&primitive_builder_->blue_material());
		renderer_3d_->DrawMesh(player_);
		//renderer_3d_->set_override_material(NULL);
	}
	

	//draw enemies
	renderer_3d_->set_override_material(&primitive_builder_->red_material());
	for (i = 0; i < enemies_.size(); i++) {
		renderer_3d_->DrawMesh(*enemies_[i]);
	}
	renderer_3d_->set_override_material(NULL);

	renderer_3d_->End();

	// start drawing sprites, but don't clear the frame buffer
	sprite_renderer_->Begin(false);
	DrawHUD();
	sprite_renderer_->End();
}

void SceneApp::InitPlayer()
{
	// setup the mesh for the player
	player_.scale = gef::Vector4(0.5, 0.5, 0.5, 1);

	player_.set_mesh(moon_mesh_.mesh());

	// create a physics body for the player
	b2BodyDef player_body_def;
	player_body_def.type = b2_dynamicBody;
	player_body_def.position = b2Vec2(0.0f, 4.0f);
	//fixed rotation
	player_body_def.fixedRotation = true;

	player_body_ = world_->CreateBody(&player_body_def);

	// create the shape for the player
	b2PolygonShape player_shape;
	player_shape.SetAsBox(0.7f, 0.7f);

	// create the fixture
	b2FixtureDef player_fixture_def;
	player_fixture_def.shape = &player_shape;
	player_fixture_def.density = 1.0f;
	player_fixture_def.friction = 3.0f;

	// create the fixture on the rigid body
	player_body_->CreateFixture(&player_fixture_def);

	// update visuals from simulation data
	player_.UpdateFromSimulation(player_body_);

	//set user data
	player_body_->SetUserData(&player_);

	player_.SetObjectType(GameObject::player);

	//speed
	player_.SetSpeed(200);

	//set health
	player_.SetHealth(3);

}

void SceneApp::CreateEnemy(float x) {

	GameObject* enemy_ = new GameObject();

	b2Body* enemy_body_ (NULL);

	// ground dimensions
	gef::Vector4 enemy_half_dimensions( enemy_dimensions_.x() / 2, enemy_dimensions_.y() / 2, enemy_dimensions_.z() / 2);

	// setup the mesh for the enemy
	enemy_->set_mesh(primitive_builder_->CreateBoxMesh(enemy_half_dimensions));

	// create a physics body for the enemy
	float max_pos = 12;
	float min_pos = 8;
	float y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	y = y * (max_pos - min_pos);
	y += min_pos;

	b2BodyDef enemy_body_def;
	enemy_body_def.type = b2_dynamicBody;
	enemy_body_def.position = b2Vec2(x, y);
	enemy_body_def.fixedRotation = true;
	enemy_body_def.linearDamping = 0.0f;
	enemy_body_def.angularDamping = 0.0f;
	enemy_body_def.gravityScale = 1.5f;

	enemy_body_ = world_->CreateBody(&enemy_body_def);

	// create the shape for the enemy
	b2PolygonShape enemy_shape;
	enemy_shape.SetAsBox(enemy_half_dimensions.x(), enemy_half_dimensions.y());

	// create the fixture
	b2FixtureDef enemy_fixture_def;
	enemy_fixture_def.shape = &enemy_shape;
	enemy_fixture_def.density = 1.0f;
	enemy_fixture_def.restitution = 1.0f;
	enemy_fixture_def.friction = 0.0f;

	// create the fixture on the rigid body
	enemy_body_->CreateFixture(&enemy_fixture_def);

	// update visuals from simulation data
	enemy_->UpdateFromSimulation(enemy_body_);

	//set user data
	enemy_body_->SetUserData(enemy_);

	enemy_->SetObjectType(GameObject::enemy);

	//gef::DebugOut("%s", enemy_->GetObjectType());

	//speed
	enemy_->SetSpeed(20);

	//apply force for direction
	float max_force = 200;
	float min_force = 100;
	float random_force = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float force = (random_force * 2 * max_force) - max_force;
	if (force < min_force && force >= 0) {
		force = min_force;
	}
	else if (force > -min_force && force < 0) {
		force = -min_force;
	}
	enemy_body_->ApplyForceToCenter(b2Vec2(force, 0.0), true);

	//put enemy on vector
	enemies_.push_back(enemy_);
	enemy_bodies_.push_back(enemy_body_);

}

gef::Scene* SceneApp::LoadSceneAssets(gef::Platform& platform, const char* filename)
{
	gef::Scene* scene = new gef::Scene();

	if (scene->ReadSceneFromFile(platform, filename))
	{
		// if scene file loads successful
		// create material and mesh resources from the scene data
		scene->CreateMaterials(platform);
		scene->CreateMeshes(platform);
	}
	else
	{
		delete scene;
		scene = NULL;
	}

	return scene;
}

gef::Mesh* SceneApp::GetMeshFromSceneAssets(gef::Scene* scene)
{
	gef::Mesh* mesh = NULL;

	// if the scene data contains at least one mesh
	// return the first mesh
	if (scene && scene->meshes.size() > 0)
		mesh = scene->meshes.front();

	return mesh;
}

void SceneApp::InitGround()
{
	// ground dimensions
	gef::Vector4 ground_half_dimensions(arena_dimensions_.x()/2, arena_dimensions_.y()/2, arena_dimensions_.z()/2);

	// setup the mesh for the ground
	ground_mesh_ = primitive_builder_->CreateBoxMesh(ground_half_dimensions);
	ground_.set_mesh(ground_mesh_);

	// create a physics body
	b2BodyDef body_def;
	body_def.type = b2_staticBody;
	body_def.position = b2Vec2(0.0f, 0.0f);

	ground_body_ = world_->CreateBody(&body_def);

	// create the shape
	b2PolygonShape shape;
	shape.SetAsBox(ground_half_dimensions.x(), ground_half_dimensions.y());

	// create the fixture
	b2FixtureDef fixture_def;
	fixture_def.shape = &shape;

	// create the fixture on the rigid body
	ground_body_->CreateFixture(&fixture_def);

	// update visuals from simulation data
	ground_.UpdateFromSimulation(ground_body_);

}

void SceneApp::InitWalls() 
	{
		// ground dimensions
		gef::Vector4 wall_half_dimensions(wall_dimensions_.x() / 2, wall_dimensions_.y() / 2, wall_dimensions_.z() / 2);

		// setup the mesh for the ground
		wall_mesh_ = primitive_builder_->CreateBoxMesh(wall_half_dimensions);
		wall_1_.set_mesh(wall_mesh_);
		wall_2_.set_mesh(wall_mesh_);

		// create a physics body
		b2BodyDef body_def_1;
		b2BodyDef body_def_2;
		body_def_1.type = b2_staticBody;
		body_def_1.position = b2Vec2(-arena_dimensions_.x()/2 - wall_dimensions_.x()/2, wall_dimensions_.y()/ 2 - arena_dimensions_.y()/2);
		body_def_2.type = b2_staticBody;
		body_def_2.position = b2Vec2(arena_dimensions_.x() / 2 + wall_dimensions_.x() / 2, wall_dimensions_.y() / 2 - arena_dimensions_.y() / 2);

		wall_body_1_ = world_->CreateBody(&body_def_1);
		wall_body_2_ = world_->CreateBody(&body_def_2);

		// create the shape
		b2PolygonShape shape;
		shape.SetAsBox(wall_half_dimensions.x(), wall_half_dimensions.y());

		// create the fixture
		b2FixtureDef fixture_def;
		fixture_def.shape = &shape;

		// create the fixture on the rigid body
		wall_body_1_->CreateFixture(&fixture_def);
		wall_body_2_->CreateFixture(&fixture_def);

		// update visuals from simulation data
		wall_1_.UpdateFromSimulation(wall_body_1_);
		wall_2_.UpdateFromSimulation(wall_body_2_);
}

void SceneApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void SceneApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void SceneApp::DrawHUD()
{
	if(font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
		font_->RenderText(sprite_renderer_, gef::Vector4(10.0f, 10.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "LIFES: %d", player_.GetHealth());
		font_->RenderText(sprite_renderer_, gef::Vector4(400.0f, 10.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "SCORE: %.1f", score);
		
	}
}

void SceneApp::SetupLights()
{
	// grab the data for the default shader used for rendering 3D geometry
	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();

	// set the ambient light
	default_shader_data.set_ambient_light_colour(gef::Colour(0.25f, 0.25f, 0.25f, 1.0f));

	// add a point light that is almost white, but with a blue tinge
	// the position of the light is set far away so it acts light a directional light
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-500.0f, 400.0f, 700.0f));
	default_shader_data.AddPointLight(default_point_light);
}

void SceneApp::HandleInput(float timeStep) {
	input_->Update();

	gef::Keyboard* keyboard = input_->keyboard();

	if (keyboard) {
		if (keyboard->IsKeyDown(keyboard->KC_W)) {
			
		}
		if (keyboard->IsKeyDown(keyboard->KC_S)) {

		}
		if (keyboard->IsKeyDown(keyboard->KC_A)) {
			player_body_->ApplyForceToCenter(b2Vec2(-player_.GetSpeed(), 0.0), true);
		}
		if (keyboard->IsKeyDown(keyboard->KC_D)) {
			player_body_->ApplyForceToCenter(b2Vec2(player_.GetSpeed(), 0.0), true);
		}
	}

}
