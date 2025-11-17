#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include "common.h"
#include "particle.c"
#include "player.c"
#include "projectile.c"
#include "enemy.c"
#include "camera.c"

typedef enum game_state game_state;
enum game_state
{
	MainMenu,
	Controls,
	Playing,
	Paused,
	Exit,
};

global bool32 IsRunning = true;

global_const int32 WindowWidth = 1280;
global_const int32 WindowHeight = 720;
global_const Vector2 WindowCenter = (Vector2){WindowWidth / 2.0f, WindowHeight / 2.0f};

#define BACKGROUND_COLOR (Color){15, 15, 15, 255}
#define MAX_STARS 128
#define PARALLAX_STRENGTH 0.25f

global f32 ParticleSpawnTimer = 0.0f;
global_const f32 ParticleSpawnInterval = 0.05f;

internal bool32 ResolveCollisions(projectile_pool* Projectiles, enemy_pool* Enemies, particle_system* Particles)
{
    for(int32 i = 0; i < MAX_PROJECTILES; i++)
    {
        if(Projectiles->Active[i])
        {
            for(int32 j = 0; j < MAX_ENEMIES; j++)
            {
                if(Enemies->Active[j])
                {
                    // FIXME(griffin): Guesstimate for the collision radius, probably should fix later.
                    if(Vector2Distance(Projectiles->Positions[i], Enemies->Positions[j]) < 16.0f)
                    {
                        Enemies->Active[j] = false;
                        Projectiles->Active[i] = false;
                        EmitParticles(Particles, Enemies->Positions[j], RED, 32);
						return(true);
                    }
                }
            }
        }
    }
	return(false);
}

internal void DrawBackground(Vector2* Stars)
{
	ClearBackground(BACKGROUND_COLOR);

    rlSetTexture(0);
    rlBegin(RL_QUADS);

	Color C = GRAY;
    rlColor4ub(C.r, C.g, C.b, C.a);

    for(int32 i = 0; i < MAX_STARS; i++)
    {
        f32 X = Stars[i].x;
        f32 Y = Stars[i].y;

        rlVertex2f(X - 1.0f, Y - 1.0f);
        rlVertex2f(X + 1.0f, Y - 1.0f);
        rlVertex2f(X + 1.0f, Y + 1.0f);
        rlVertex2f(X - 1.0f, Y + 1.0f);
    }

    rlEnd();
}

internal void DrawHUD(void)
{
    DrawFPS(10, 10);
}

// NOTE(griffin): TEMP
internal void DrawMainMenu(Vector2* Stars)
{
	BeginDrawing();
	DrawBackground(Stars);
	int FirstOffset = MeasureText("Space Survivors", 48);
	int SecondOffset = MeasureText("Play", 48);
	int ThirdOffset = MeasureText("Controls", 48);
	int FourthOffset = MeasureText("Exit", 48);
	DrawText("Space Survivors", (WindowWidth / 2.0f) - (FirstOffset / 2.0f), (WindowHeight / 2.0f) - 200.0f, 48, WHITE);
	DrawText("Play", (WindowWidth / 2.0f) - (SecondOffset / 2.0f), (WindowHeight / 2.0f), 48, WHITE);
	DrawText("Controls", (WindowWidth / 2.0f) - (ThirdOffset / 2.0f), (WindowHeight / 2.0f) + 50.0f, 48, WHITE);
	DrawText("Exit", (WindowWidth / 2.0f) - (FourthOffset / 2.0f), (WindowHeight / 2.0f) + 100.0f, 48, WHITE);
	EndDrawing();
}

// NOTE(griffin): TEMP
internal void DrawPausedMenu(void)
{
	BeginDrawing();
	int Offset = MeasureText("Paused", 48);
	DrawText("Paused", (WindowWidth / 2.0f) - (Offset / 2.0f), (WindowHeight / 2.0f) - 200.0f, 48, WHITE);
	EndDrawing();
}

// NOTE(griffin): TEMP
internal void DrawControlsMenu(Vector2* Stars)
{
	BeginDrawing();
	DrawBackground(Stars);
	int Offset = MeasureText("Controls", 48);
	DrawText("Controls", (WindowWidth / 2.0f) - (Offset / 2.0f), (WindowHeight / 2.0f) - 200.0f, 48, WHITE);
	EndDrawing();
}

int main(void)
{
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_HIGHDPI /*| FLAG_VSYNC_HINT*/);
    InitWindow(WindowWidth, WindowHeight, "Space Survivors");
	//SetTargetFPS(60);
	
    InitAudioDevice();
    SetMasterVolume(0.1f);

	SetExitKey(KEY_NULL);

    rlDisableBackfaceCulling();
    rlDisableDepthTest();

	#if defined(_WIN32)
    Image WindowIcon = LoadImage("../assets/enemy.png");
    SetWindowIcon(WindowIcon);
	#endif

    Vector2 Stars[MAX_STARS];
    for(int32 i = 0; i < MAX_STARS; i++)
    {
        Stars[i] = (Vector2){RandomFloat(-250.0f, (f32)WindowWidth + 250.0f), RandomFloat(-250.0f, (f32)WindowHeight) + 250.0f};
    }

    particle_system Particles = {0};
    projectile_pool Projectiles = {0};
    enemy_pool Enemies = {0};

    player Player = {0};
    Player.Position = WindowCenter;
    Player.Acceleration = (Vector2){300.0f, 300.0f};

    game_camera GameCamera = {0};
    GameCamera.Camera.target = WindowCenter;
    GameCamera.Camera.offset = WindowCenter;
    GameCamera.Camera.zoom = 1.0f;
    GameCamera.TargetZoom = 1.0f;

    Texture2D EnemyTexture = LoadTexture("../assets/enemy.png");
    Texture2D PlayerTexture = LoadTexture("../assets/ship.png");

    Sound ExplosionSound = LoadSound("../assets/sfx/explosion.mp3");

    // NOTE(griffin): TEMP
    SpawnEnemies(&Enemies, Player.Position, 10);

	game_state GameState = MainMenu;

    while(!WindowShouldClose() && IsRunning)
    {
        f32 dt = GetFrameTime();

		if(GameState == MainMenu)
		{
			if(IsKeyPressed(KEY_ONE))
			{
				GameState = Playing;
			}
			else if(IsKeyPressed(KEY_TWO))
			{
				GameState = Controls;
			}
			else if(IsKeyPressed(KEY_THREE))
			{
				GameState = Exit;
			}
		}
		if(IsKeyPressed(KEY_ESCAPE))
		{
			switch(GameState)
			{
				case MainMenu:
				{
					GameState = Exit;
					break;
				}
				case Controls:
				{
					GameState = MainMenu;
					break;
				}
				case Playing:
				{
					GameState = Paused;
					break;
				}
				case Paused:
				{
					GameState = Playing;
					break;
				}
				default:
				{
					break;
				}
			}
		}

		switch(GameState)
		{
			case MainMenu:
			{
				DrawMainMenu(Stars);
				break;
			}

			case Controls:
			{
				DrawControlsMenu(Stars);
				break;
			}

			case Playing:
			{
					if(IsKeyDown(KEY_W))
					{
						AcceleratePlayer(&Player, dt);

						ParticleSpawnTimer += dt;
						if(ParticleSpawnTimer >= ParticleSpawnInterval)
						{
							f32 Radians = Player.Angle * DEG2RAD;
							Vector2 Direction = (Vector2){sinf(Radians), -cosf(Radians)};
							Vector2 FirePosition = Vector2Subtract(Player.Position, Vector2Scale(Direction, PlayerTexture.height * 0.5f));
							EmitParticles(&Particles, FirePosition, ORANGE, 1);
							ParticleSpawnTimer = 0.0f;
						}
					}
					if(IsKeyDown(KEY_A))
					{
						Player.Angle -= ROTATION_SPEED * dt;
					}
					if(IsKeyDown(KEY_D))
					{
						Player.Angle += ROTATION_SPEED * dt;
					}
					if(IsKeyPressed(KEY_SPACE))
					{
						SpawnProjectile(&Projectiles, Player.Position, Player.Angle * DEG2RAD);
					}

					UpdateParticles(&Particles, dt);
					UpdateProjectiles(&Projectiles, WindowWidth, WindowHeight, dt);
					UpdateEnemies(&Enemies, Player.Position, dt);
					UpdatePlayer(&Player, WindowWidth, WindowHeight, dt);
					UpdateGameCamera(&GameCamera, WindowWidth, WindowHeight, Player.Position, dt);

					bool32 Result = ResolveCollisions(&Projectiles, &Enemies, &Particles);
					if(Result)
					{
                        PlaySound(ExplosionSound);
                        ShakeCamera(&GameCamera);
					}

					for(int32 i = 0; i < MAX_STARS; i++)
					{
						Stars[i].x -= Player.Velocity.x * PARALLAX_STRENGTH * dt;
						Stars[i].y -= Player.Velocity.y * PARALLAX_STRENGTH * dt;

						Stars[i].x = Wrap(Stars[i].x, 0.0f, (f32)WindowWidth);
						Stars[i].y = Wrap(Stars[i].y, 0.0f, (f32)WindowHeight);
					}

					BeginDrawing();
					DrawBackground(Stars);

					BeginMode2D(GameCamera.Camera);
					DrawPlayer(Player, PlayerTexture);
					DrawProjectiles(Projectiles);
					DrawEnemies(Enemies, EnemyTexture);
					DrawParticles(&Particles);
					EndMode2D();

					DrawHUD();
					EndDrawing();
					break;
				}

				case Paused:
				{
					DrawBackground(Stars);

					BeginMode2D(GameCamera.Camera);
					DrawPlayer(Player, PlayerTexture);
					DrawProjectiles(Projectiles);
					DrawEnemies(Enemies, EnemyTexture);
					DrawParticles(&Particles);
					EndMode2D();

					DrawHUD();
					DrawPausedMenu();
					break;
				}

				case Exit:
				{
					IsRunning = false;
					break;
				}
			}
		}

    CloseWindow();
    return(0);
}
