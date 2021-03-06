#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	brrake = App->audio->LoadFx("Audio/Fx/Brake.wav");
	forward = App->audio->LoadFx("Audio/Fx/Forward.wav");
	respawn = App->audio->LoadFx("Audio/Fx/Respawn.wav");

	VehicleInfo car;

	// Car properties ----------------------------------------
	car.chassis_size.Set(1, 1, 2);
	car.chassis_offset.Set(0, 0.8, 0);
	car.mass = 500.0f;
	car.suspensionStiffness = 15.88f;
	car.suspensionCompression = 0.83f;
	car.suspensionDamping = 0.88f;
	car.maxSuspensionTravelCm = 1000.0f;
	car.frictionSlip = 50.5;
	car.maxSuspensionForce = 6000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.3f;
	float wheel_width = 0.3f;
	float suspensionRestLength = 0.9f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;
	
	vec3 direction(0,-1,0);
	vec3 axis(-1,0,0);
	
	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;

	vehicle = App->physics->AddVehicle(car);
	vehicle->SetPos(0,0,0);
	vehicle->GetTransform(&matrix);
	App->camera->Move(0, 0, -10);
	App->camera->Follow(vehicle, 10, 12.5, 1.f);

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	if (!game_paused) {
		turn = acceleration = brake = 0.0f;

		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		{
			if (bababooey == false) {
				if (vehicle->GetKmh() <= 100)
					acceleration = ACCELERATION;
				else
					acceleration = 0;
			}
			else {
				if (vehicle->GetKmh() >= -100)
					acceleration = -ACCELERATION;
				else
					acceleration = 0;
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
			App->audio->PlayFx(forward);

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			if (turn < TURN_DEGREES)
				turn += TURN_DEGREES;
		}

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			if (turn > -TURN_DEGREES)
				turn -= TURN_DEGREES;
		}

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
		{
			App->audio->PlayFx(brrake);
			brake = BRAKE_POWER;
		}

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			if (bababooey == false) {
				if (vehicle->GetKmh() >= -100)
					acceleration = -ACCELERATION / 2;
				else
					acceleration = 0;
			}
			else {
				if (vehicle->GetKmh() <= 100)
					acceleration = ACCELERATION / 2;
				else
					acceleration = 0;
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)
		{
			brake = BRAKE_POWER * 10000;
			vehicle->SetTransform(&matrix);

			switch (App->scene_intro->checkpoint)
			{
			case 0:
				vehicle->SetPos(0, 0, 0);
				App->camera->Move(0, 0, -10);
				vehicle->Bababooey2();
				bababooey = false;
				break;
			case 1:
				vehicle->SetPos(-73, 3.2, 249.9);
				App->camera->Move(-73, 3.2, 237.9);
				vehicle->Bababooey2();
				bababooey = false;
				break;
			case 2:
				vehicle->SetPos(30, 10, 413.2);
				App->camera->Move(30, 10, 425.2);
				vehicle->Bababooey();
				bababooey = true;
				break;
			case 3:
				vehicle->SetPos(127, 10, 251);
				App->camera->Move(129, 10, 263);
				vehicle->Bababooey();
				bababooey = true;
				break;
			case 4:
				vehicle->SetPos(-122.25, 10, 71.3);
				App->camera->Move(-122.25, 10, 83.3);
				vehicle->Bababooey();
				bababooey = true;
				break;
			case 5:
				vehicle->SetPos(-76.5, 3, 75);
				App->camera->Move(-76.5, 3, 87);
				vehicle->Bababooey();
				bababooey = true;
				break;
			case 6:
				vehicle->SetPos(-186, 0, -49);
				App->camera->Move(-186, 0, -37);
				vehicle->Bababooey();
				bababooey = true;
				break;
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_DOWN)
			App->audio->PlayFx(respawn);

		vehicle->ApplyEngineForce(acceleration);
		vehicle->Turn(turn);
		vehicle->Brake(brake);

		vehicle->Render();

		char title[80];
		sprintf_s(title, "Time: %i Lap: %i/3 Last Time: %i Best Time: %i", App->scene_intro->lap_timer.Read() / 1000, App->scene_intro->laps, App->last_time, App->best_time);
		App->window->SetTitle(title);
	}
	
	if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN ) {
		if (game_paused) {
			game_paused = false;
			App->scene_intro->lap_timer.Continue();
		}
		else {
			game_paused = true;
			App->scene_intro->lap_timer.Stop();
		}
	}

	return UPDATE_CONTINUE;
}