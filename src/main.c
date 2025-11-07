#include "raylib.h"
#include "raymath.h"

#include "utils.h"
#include "particle.c"
#include "player.c"
#include "projectile.c"
#include "enemy.c"


global int32 WindowWidth = 1280;
global int32 WindowHeight = 720;

#define BACKGROUND_COLOR (Color){15, 15, 15, 255}
#define MAX_STARS 128
#define PARALLAX_STRENGTH 0.25f

global_const f32 ShakeDecayRate = 3.0f;
global f32 CameraShakeStrength = 0.0f;

global f32 ParticleTimer = 0.0f;
global_const f32 ParticleSpawnInterval = 0.05f;

internal void UpdateCameraState(Camera2D* Camera, Vector2 PlayerPosition, f32 dt)
{
    CameraShakeStrength = Lerp(CameraShakeStrength, 0, ShakeDecayRate * dt);
	if(CameraShakeStrength < 0.01f)
	{
		CameraShakeStrength = 0.0f;
	}
    Camera->offset.x = WindowWidth / 2.0f + RandomFloat(-1.0f, 1.0f) * CameraShakeStrength;
    Camera->offset.y = WindowHeight / 2.0f + RandomFloat(-1.0f, 1.0f) * CameraShakeStrength;

    local_persist f32 TargetZoom = 1.0f;
    f32 WheelMove = GetMouseWheelMove();
    if(WheelMove != 0.0f)
    {
        TargetZoom = Clamp(TargetZoom + WheelMove, 1.0f, 10.0f);
    }
    Camera->zoom = Lerp(Camera->zoom, TargetZoom, 1.0f - expf(-10.0f * dt));

    Vector2 CameraTarget;
    if(TargetZoom > 1.0f)
    {
        CameraTarget = PlayerPosition;
    }
    else
    {
        CameraTarget = (Vector2){WindowWidth / 2.0f, WindowHeight / 2.0f};
    }
    Camera->target = Vector2Lerp(Camera->target, CameraTarget, 1.0f - expf(-5.0f * dt));
}

internal void ResolveCollisions(projectile_pool* Projectiles, enemy_pool* Enemies, particle_system* Particles)
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
                    if(Vector2Distance(Projectiles->Position[i], Enemies->Position[j]) < 16.0f)
                    {
                        Enemies->Active[j] = false;
                        Projectiles->Active[i] = false;
                        EmitParticles(Particles, Enemies->Position[j], RED, 32);
                        CameraShakeStrength = 10.0f;
                        break;
                    }
                }
            }
        }
    }
}

int main(void)
{
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);
    InitWindow(WindowWidth, WindowHeight, "Space Survivors");
    // SetTargetFPS(60);

	#if defined(_WIN32)
    Image WindowIcon = LoadImage("../assets/enemy.png");
    SetWindowIcon(WindowIcon);
	#endif

    Vector2 Stars[MAX_STARS];
    for(int32 i = 0; i < MAX_STARS; i++)
    {
        Stars[i] = (Vector2){RandomFloat(0.0f, (f32)WindowWidth), RandomFloat(0.0f, (f32)WindowHeight)};
    }

    particle_system Particles = {0};
    projectile_pool Projectiles = {0};
    enemy_pool Enemies = {0};
    Enemies.Texture = LoadTexture("../assets/enemy.png");

    player Player;
    Player.Texture = LoadTexture("../assets/ship.png");
    Player.Position = (Vector2){WindowWidth / 2.0f, WindowHeight / 2.0f};
    Player.Velocity = (Vector2){0.0f, 0.0f};
    Player.Acceleration = (Vector2){300.0f, 300.0f};
    Player.Angle = 0.0f;

    Camera2D Camera = {0};
    Camera.target = (Vector2){WindowWidth / 2.0f, WindowHeight / 2.0f};
    Camera.offset = (Vector2){WindowWidth / 2.0f, WindowHeight / 2.0f};
    Camera.rotation = 0.0f;
    Camera.zoom = 1.0f;

    // NOTE(griffin): TEMP
    SpawnEnemies(&Enemies, Player.Position, 10);

    while(!WindowShouldClose())
    {
        f32 dt = GetFrameTime();
        int32 NewWindowWidth = GetScreenWidth();
        int32 NewWindowHeight = GetScreenHeight();

        if(NewWindowWidth != WindowWidth || NewWindowHeight != WindowHeight)
        {
            WindowWidth = NewWindowWidth;
            WindowHeight = NewWindowHeight;
            Camera.offset = (Vector2){WindowWidth / 2.0f, WindowHeight / 2.0f};

            for(int32 i = 0; i < MAX_STARS; i++)
            {
                Stars[i] = (Vector2){RandomFloat(0.0f, (f32)WindowWidth), RandomFloat(0.0f, (f32)WindowHeight)};
            }
        }

        for(int32 i = 0; i < MAX_STARS; i++)
        {
            Stars[i].x -= Player.Velocity.x * PARALLAX_STRENGTH * dt;
            Stars[i].y -= Player.Velocity.y * PARALLAX_STRENGTH * dt;

            if(Stars[i].x < 0.0f)
            {
                Stars[i].x += WindowWidth;
            }
            if(Stars[i].x > WindowWidth)
            {
                Stars[i].x -= WindowWidth;
            }
            if(Stars[i].y < 0.0f)
            {
                Stars[i].y += WindowHeight;
            }
            if(Stars[i].y > WindowHeight)
            {
                Stars[i].y -= WindowHeight;
            }
        }

        if(IsKeyDown(KEY_W))
        {
            AcceleratePlayer(&Player, dt);

            ParticleTimer += dt;
            if(ParticleTimer >= ParticleSpawnInterval)
            {
                f32 Radians = Player.Angle * DEG2RAD;
                Vector2 Direction = (Vector2){sinf(Radians), -cosf(Radians)};
                Vector2 FirePosition = Vector2Subtract(Player.Position, Vector2Scale(Direction, Player.Texture.height * 0.5f));
                EmitParticles(&Particles, FirePosition, ORANGE, 2);
                ParticleTimer = 0.0f;
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
        UpdateCameraState(&Camera, Player.Position, dt);
        ResolveCollisions(&Projectiles, &Enemies, &Particles);

        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);

        for(int32 i = 0; i < MAX_STARS; i++)
        {
            DrawCircleV(Stars[i], 1.0f, GRAY);
        }

        BeginMode2D(Camera);
        DrawPlayer(Player);
        DrawProjectiles(Projectiles);
        DrawEnemies(Enemies);
        DrawParticles(&Particles);
        EndMode2D();

        DrawFPS(10, 10);
        EndDrawing();
    }

	#if defined(_WIN32)
    UnloadImage(WindowIcon);
	#endif
    UnloadTexture(Player.Texture);
    UnloadTexture(Enemies.Texture);

    CloseWindow();
    return(0);
}
