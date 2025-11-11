#include "raylib.h"
#include "raymath.h"
#include "utils.h"

#define MAX_PROJECTILES 32
#define PROJECTILE_SPEED 400.0f

typedef struct projectile_pool projectile_pool;
struct projectile_pool
{
    Vector2 Positions[MAX_PROJECTILES];
    Vector2 Directions[MAX_PROJECTILES];
    bool32 Active[MAX_PROJECTILES];
};

void SpawnProjectile(projectile_pool* Projectiles, Vector2 Position, f32 Angle)
{
    for(int32 i = 0; i < MAX_PROJECTILES; i++)
    {
        if(!Projectiles->Active[i])
        {
            Projectiles->Positions[i] = Position;
            Projectiles->Directions[i] = (Vector2){sinf(Angle), -cosf(Angle)};
            Projectiles->Active[i] = true;
            break;
        }
    }
}

void UpdateProjectiles(projectile_pool* Projectiles, int32 WindowWidth, int32 WindowHeight, f32 dt)
{
    for(int32 i = 0; i < MAX_PROJECTILES; i++)
    {
        if(Projectiles->Active[i])
        {
            if(Projectiles->Positions[i].x < 0 || Projectiles->Positions[i].x > WindowWidth ||
               Projectiles->Positions[i].y < 0 || Projectiles->Positions[i].y > WindowHeight)
            {
                Projectiles->Active[i] = false;
            }
            else
            {
                Projectiles->Positions[i] = Vector2Add(Projectiles->Positions[i], Vector2Scale(Projectiles->Directions[i], PROJECTILE_SPEED * dt));
            }
        }
    }
}

void DrawProjectiles(projectile_pool Projectiles)
{
    for(int32 i = 0; i < MAX_PROJECTILES; i++)
    {
        if(Projectiles.Active[i])
        {
            DrawLineEx(Projectiles.Positions[i], Vector2Add(Projectiles.Positions[i], Vector2Scale(Projectiles.Directions[i], 15.0f)), 3.0f, SKYBLUE);
        }
    }
}
