#include "raylib.h"
#include "raymath.h"
#include "utils.h"

#define MAX_ENEMIES 256
#define ENEMY_SPEED 50.0f
#define SPAWN_RADIUS 500.0f

typedef struct enemy_pool enemy_pool;
struct enemy_pool
{
    Vector2 Positions[MAX_ENEMIES];
    bool32 Active[MAX_ENEMIES];
};

void SpawnEnemies(enemy_pool* Enemies, Vector2 Position, int32 Amount)
{
    for(int32 i = 0; i < Amount; i++)
    {
        for(int32 j = 0; j < MAX_ENEMIES; j++)
        {
            if(!Enemies->Active[j])
            {
                f32 Angle = RandomFloat(0.0f, 1.0f) * 2 * PI;
                Enemies->Positions[j] = (Vector2){Position.x + cosf(Angle) * SPAWN_RADIUS, Position.y + sinf(Angle) * SPAWN_RADIUS};
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
            Vector2 Direction = Vector2Normalize(Vector2Subtract(PlayerPosition, Enemies->Positions[i]));
            Enemies->Positions[i] = Vector2Add(Enemies->Positions[i], Vector2Scale(Direction, ENEMY_SPEED * dt));
        }
    }
}

void DrawEnemies(enemy_pool Enemies, Texture2D EnemyTexture)
{
    for(int32 i = 0; i < MAX_ENEMIES; i++)
    {
        if(Enemies.Active[i])
        {
            DrawTexturePro(EnemyTexture,
                (Rectangle){0.0f, 0.0f, (f32)EnemyTexture.width, (f32)EnemyTexture.height},
                (Rectangle){Enemies.Positions[i].x, Enemies.Positions[i].y, (f32)EnemyTexture.width, (f32)EnemyTexture.height},
                (Vector2){EnemyTexture.width / 2.0f, EnemyTexture.height / 2.0f},
                0.0f,
                WHITE);
        }
    }
}
