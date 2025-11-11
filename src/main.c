#include "raylib.h"
#include "raymath.h"

#include "utils.h"
#include "particle.c"
#include "player.c"
#include "projectile.c"
#include "enemy.c"
#include "camera.c"

global_const int32 WindowWidth = 1280;
global_const int32 WindowHeight = 720;

#define BACKGROUND_COLOR (Color){15, 15, 15, 255}
#define MAX_STARS 32
#define PARALLAX_STRENGTH 0.25f

global f32 ParticleSpawnTimer = 0.0f;
global_const f32 ParticleSpawnInterval = 0.05f;

internal void ResolveCollisions(projectile_pool* Projectiles, enemy_pool* Enemies, particle_system* Particles, game_camera* GameCamera, Sound ExplosionSound)
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
                        PlaySound(ExplosionSound);
                        ShakeCamera(GameCamera);
                        break;
                    }
                }
            }
        }
    }
}

internal void DrawUI(void)
{
    DrawFPS(10, 10);
}

int main(void)
{
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WindowWidth, WindowHeight, "Space Survivors");
    // SetTargetFPS(60);

    InitAudioDevice();
    SetMasterVolume(0.1f);

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
    Player.Position = (Vector2){WindowWidth / 2.0f, WindowHeight / 2.0f};
    Player.Acceleration = (Vector2){300.0f, 300.0f};

    game_camera GameCamera = {0};
    GameCamera.Camera.target = (Vector2){WindowWidth / 2.0f, WindowHeight / 2.0f};
    GameCamera.Camera.offset = (Vector2){WindowWidth / 2.0f, WindowHeight / 2.0f};
    GameCamera.Camera.zoom = 1.0f;
    GameCamera.TargetZoom = 1.0f;

    Texture2D EnemyTexture = LoadTexture("../assets/enemy.png");
    Texture2D PlayerTexture = LoadTexture("../assets/ship.png");

    Sound ExplosionSound = LoadSound("../assets/sfx/explosion.mp3");

    // NOTE(griffin): TEMP
    SpawnEnemies(&Enemies, Player.Position, 10);

    while(!WindowShouldClose())
    {
        f32 dt = GetFrameTime();

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
        ResolveCollisions(&Projectiles, &Enemies, &Particles, &GameCamera, ExplosionSound);

        for(int32 i = 0; i < MAX_STARS; i++)
        {
            Stars[i].x -= Player.Velocity.x * PARALLAX_STRENGTH * dt;
            Stars[i].y -= Player.Velocity.y * PARALLAX_STRENGTH * dt;

            Stars[i].x = Wrap(Stars[i].x, 0.0f, (f32)WindowWidth);
            Stars[i].y = Wrap(Stars[i].y, 0.0f, (f32)WindowHeight);
        }

        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);

        for(int32 i = 0; i < MAX_STARS; i++)
        {
            DrawCircleV(Stars[i], 1.0f, GRAY);
        }

        BeginMode2D(GameCamera.Camera);
        DrawPlayer(Player, PlayerTexture);
        DrawProjectiles(Projectiles);
        DrawEnemies(Enemies, EnemyTexture);
        DrawParticles(&Particles);
        EndMode2D();

        DrawUI();
        EndDrawing();
    }

	#if defined(_WIN32)
    UnloadImage(WindowIcon);
	#endif
    UnloadTexture(PlayerTexture);
    UnloadTexture(EnemyTexture);

    CloseAudioDevice();
    CloseWindow();
    return(0);
}
