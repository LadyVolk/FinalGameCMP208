#include "menu_app.h"
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
#include <input/touch_input_manager.h>
#include <platform/d3d11/system/platform_d3d11.h>
#include "assets/png_loader.h"


MenuApp::MenuApp(gef::Platform& platform, GameData *data) :
	Application(platform),
	sprite_renderer_(NULL),
	primitive_builder_(NULL),
	font_(NULL),
	active_touch_id_(-1),
	data_(data)
{
}

void MenuApp::Init()
{	
	gef::DebugOut("\n");
	gef::DebugOut("------------ START NOW MENU INIT ------------");
	gef::DebugOut("\n");

	//splash screen stuff
	splash_screen_ = new gef::ImageData();
	gef::PNGLoader loader_;
	
	loader_.Load("Abertay_University_Logo.png", platform_, *splash_screen_);

	splash_texture_ = gef::Texture::Create(platform_, *splash_screen_);
	splash_sprite_ = new gef::Sprite();
	splash_sprite_->set_texture(splash_texture_);
	
	splash_sprite_->set_position(gef::Vector4(480, 272, 0, 1));
	splash_sprite_->set_width(300.0f);
	splash_sprite_->set_height(350.0f);

	//splash screen lobo logo
	splash_screen_logo_ = new gef::ImageData();

	loader_.Load("lobo_logo.png", platform_, *splash_screen_logo_);

	splash_texture_logo_ = gef::Texture::Create(platform_, *splash_screen_logo_);
	splash_sprite_logo_ = new gef::Sprite();
	splash_sprite_logo_->set_texture(splash_texture_logo_);

	splash_sprite_logo_->set_position(gef::Vector4(480, 272, 0, 1));
	splash_sprite_logo_->set_width(960.0f);
	splash_sprite_logo_->set_height(544.0f);

	//sound stuff
	//sound music menu
	audio_manager_ = gef::AudioManager::Create();
	gef::VolumeInfo volume_;
	volume_.volume = 6.0;
	id_music_menu_ = audio_manager_->LoadMusic("cleric_path.wav", platform_);
	int id_sample_new = audio_manager_->PlayMusic();
	audio_manager_->SetMusicVolumeInfo(volume_);

	id_change_difficult_ = audio_manager_->LoadSample("change_difficult.wav", platform_);

	exit_menu = false;

	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);

	// create the renderer for draw 3D geometry
	renderer_3d_ = gef::Renderer3D::Create(platform_);

	// initialise primitive builder to make create some 3D geometry easier
	primitive_builder_ = new PrimitiveBuilder(platform_);

	InitFont();

	//create input object
	input_ = gef::InputManager::Create(platform_);
}

void MenuApp::CleanUp()
{
	// destroying the physics world also destroys all the objects within it

	CleanUpFont();

	delete primitive_builder_;
	primitive_builder_ = NULL;

	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	audio_manager_->UnloadAllSamples();

	audio_manager_->UnloadMusic();

	delete audio_manager_;
	audio_manager_ = NULL;

}

bool MenuApp::Update(float frame_time) {
	
	float timeStep = 1.0f / 60.0f;
	timerao += frame_time;
	

	//handle input
	HandleInput(timeStep);


	if (exit_menu) {
		return false;
	}

	return true;

}

void MenuApp::Render() {

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

	renderer_3d_->End();

	// start drawing sprites, but don't clear the frame buffer
	sprite_renderer_->Begin(false);
	
	DrawHUD();

	DrawSplashScreen();

	sprite_renderer_->End();
}

void MenuApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void MenuApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void MenuApp::DrawHUD()
{
	if (font_ && timerao > 8)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(400.0f, 20.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "DEATH BALLS");

		font_->RenderText(sprite_renderer_, gef::Vector4(300.0f, 100.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Use arrows to choose dificulty:");

		font_->RenderText(sprite_renderer_, gef::Vector4(300.0f, 200.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Press 'P' Play game");

		font_->RenderText(sprite_renderer_, gef::Vector4(300.0f, 300.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Press 'Q' to quit the game");

		switch (data_->difficulty_) {
			case GameData::easy:
				font_->RenderText(sprite_renderer_, gef::Vector4(400.0f, 150.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "EASY");
				break;
			case  GameData::medium:
				font_->RenderText(sprite_renderer_, gef::Vector4(400.0f, 150.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "MEDIUM");
				break;
			case GameData::hard:
				font_->RenderText(sprite_renderer_, gef::Vector4(400.0f, 150.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "HARD");
				break;
		}
	}
}

void MenuApp::HandleInput(float timeStep) {
	input_->Update();

	ProcessTouchInput();

	gef::VolumeInfo volume_difficult;
	volume_difficult.volume = 6.0f;

	gef::Keyboard* keyboard = input_->keyboard();

	if (keyboard) {
		int diff_int = static_cast <int> (data_->difficulty_);
		if (keyboard->IsKeyPressed(keyboard->KC_LEFT)) {

			//sound for difficult change
			int id_sample_new = audio_manager_->PlaySample(id_change_difficult_, false);
			audio_manager_->SetSampleVoiceVolumeInfo(id_sample_new, volume_difficult);

			diff_int--;
			if (diff_int == -1) {
				data_->difficulty_ = GameData::hard;
			}
			else {
				data_->difficulty_ = static_cast <GameData::Difficulty> (diff_int);
			}
		}
		if (keyboard->IsKeyPressed(keyboard->KC_RIGHT)) {

			int id_sample_new = audio_manager_->PlaySample(id_change_difficult_, false);
			audio_manager_->SetSampleVoiceVolumeInfo(id_sample_new, volume_difficult);

			diff_int++;
			if (diff_int == 3) {
				data_->difficulty_ = GameData::easy;
			}
			else {
				data_->difficulty_ = static_cast <GameData::Difficulty> (diff_int);;
			}
		}

		if (keyboard->IsKeyPressed(keyboard->KC_Q)) {
			exit(0);
		}
		if (keyboard->IsKeyPressed(keyboard->KC_P)) {

			exit_menu = true;
		}
	}
}

void MenuApp::ProcessTouchInput()
{
	const gef::TouchInputManager* touch_input = input_->touch_manager();
	if (touch_input && (touch_input->max_num_panels() > 0))
	{
		// get the active touches for this panel
		const gef::TouchContainer& panel_touches = touch_input->touches(0);

		// go through the touches
		for (gef::ConstTouchIterator touch = panel_touches.begin(); touch != panel_touches.end(); ++touch)
		{
			// if active touch id is -1, then we are not currently processing a touch
			if (active_touch_id_ == -1)
			{
				// check for the start of a new touch
				if (touch->type == gef::TT_NEW)
				{
					active_touch_id_ = touch->id;

					// do any processing for a new touch here
					// we're just going to record the position of the touch
					touch_position_ = touch->position;
				}
			}
			else if (active_touch_id_ == touch->id)
			{
				// we are processing touch data with a matching id to the one we are looking for
				if (touch->type == gef::TT_ACTIVE)
				{
					// update an active touch here
					// we're just going to record the position of the touch
					touch_position_ = touch->position;
				}
				else if (touch->type == gef::TT_RELEASED)
				{
					// the touch we are tracking has been released
					// perform any actions that need to happen when a touch is released here
					// we're not doing anything here apart from resetting the active touch id
					active_touch_id_ = -1;
				}
			}
		}
	}
}

void MenuApp::DrawSplashScreen() {

	if (timerao < 3.0) {
		sprite_renderer_->DrawSprite(*splash_sprite_);
	}
	else if (timerao > 3.0 && timerao < 6.0) {
		sprite_renderer_->DrawSprite(*splash_sprite_logo_);
	}
	
}