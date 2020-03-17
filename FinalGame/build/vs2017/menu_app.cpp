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


MenuApp::MenuApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	primitive_builder_(NULL),
	font_(NULL)
{
}

void MenuApp::Init()
{	
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);

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
}

bool MenuApp::Update(float frame_time) {
	
	float timeStep = 1.0f / 60.0f;

	//handle input
	HandleInput(timeStep);

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

	// start drawing sprites, but don't clear the frame buffer
	sprite_renderer_->Begin(false);
	DrawHUD();
	sprite_renderer_->End();
}

void MenuApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void MenuApp::DrawHUD()
{
	if (font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "MENU");
		

	}
}

void MenuApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}


void MenuApp::HandleInput(float timeStep) {
	input_->Update();

	gef::Keyboard* keyboard = input_->keyboard();

	if (keyboard) {
		
	}
}