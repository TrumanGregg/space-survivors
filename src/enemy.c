#include "raylib.h"
#include "raymath.h"
#include "utils.h"

#define MAX_ENEMIES 256
#define ENEMY_SPEED 50.0f
#define SPAWN_RADIUS 500.0f

typedef struct
{
    Texture2D Texture;
    Vector2 Position[MAX_ENEMIES];
    bool32 Active[MAX_ENEMIES];
} enemy_pool;

void SpawnEnemies(enemy_pool* Enemies, Vector2 Position, int32 Amount)
{
    for(int32 i = 0; i < Amount; i++)
    {
        for(int32 j = 0; j < MAX_ENEMIES; j++)
        {
            if(!Enemies->Active[j])
            {
                f32 Angle = RandomFloat(0.0f, 1.0f) * 2 * PI;
                Enemies->Position[j] = (Vector2){Position.x + cosf(Angle) * SPAWN_RADIUS, Position.y + sinf(Angle) * SPAWN_RADIUS};
                Enemies->Active[j] = true;
                break;
            }
        }
    }
}

void UpdateEnemies(enemy_pool* Enemies, Vector2 PlayerPosition, f32 dt)
{
    for(int32 i = 0; i < MAX_ENEMIES; i++)
    {
        if(Enemies->Active[i])
        {
            Vector2 Direction = Vector2Normalize(Vector2Subtract(PlayerPosition, Enemies->Position[i]));
            Enemies->Position[i] = Vector2Add(Enemies->Position[i], Vector2Scale(Direction, ENEMY_SPEED * dt));
        }
    }
}

void DrawEnemies(const enemy_pool Enemies)
{
    for(int32 i = 0; i < MAX_ENEMIES; i++)
    {
        if(Enemies.Active[i])
        {
            DrawTexturePro(Enemies.Texture,
                (Rectangle){0.0f, 0.0f, (f32)Enemies.Texture.width, (f32)Enemies.Texture.height},
                (Rectangle){Enemies.Position[i].x, Enemies.Position[i].y, (f32)Enemies.Texture.width, (f32)Enemies.Texture.height},
                (Vector2){Enemies.Texture.width / 2.0f, Enemies.Texture.height / 2.0f},
                0.0f,
                WHITE);
        }
    }
}
