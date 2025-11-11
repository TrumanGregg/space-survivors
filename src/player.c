#include "raylib.h"
#include "raymath.h"
#include "utils.h"

#define ROTATION_SPEED 300.0f
#define FRICTION_COEFFICIENT 0.98f

typedef struct player player;
struct player
{
    Vector2 Position;
    Vector2 Velocity;
    Vector2 Acceleration;
    f32 Angle;
};

void AcceleratePlayer(player* Player, f32 dt)
{
    f32 Radians = Player->Angle * DEG2RAD;
    Vector2 Direction = (Vector2){sinf(Radians), -cosf(Radians)};
    Player->Velocity.x += Direction.x * Player->Acceleration.x * dt;
    Player->Velocity.y += Direction.y * Player->Acceleration.y * dt;
}

void UpdatePlayer(player* Player, int32 WindowWidth, int32 WindowHeight, f32 dt)
{
    Player->Velocity = Vector2Lerp(Player->Velocity, (Vector2){0.0f, 0.0f}, FRICTION_COEFFICIENT * dt);
    Player->Position.x += Player->Velocity.x * dt;
    Player->Position.y += Player->Velocity.y * dt;

    Player->Position.x = Wrap(Player->Position.x, -15.0f, WindowWidth + 15.0f);
    Player->Position.y = Wrap(Player->Position.y, -15.0f, WindowHeight + 15.0f);
}

void DrawPlayer(player Player, Texture2D PlayerTexture)
{
    DrawTexturePro(PlayerTexture,
        (Rectangle){0.0f, 0.0f, (f32)PlayerTexture.width, (f32)PlayerTexture.height},
        (Rectangle){Player.Position.x, Player.Position.y, (f32)PlayerTexture.width, (f32)PlayerTexture.height},
        (Vector2){PlayerTexture.width / 2.0f, PlayerTexture.height / 2.0f},
        Player.Angle,
        WHITE);
}
