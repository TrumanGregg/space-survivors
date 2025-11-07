#include "raylib.h"
#include "raymath.h"
#include "utils.h"

#define MAX_PROJECTILES 32
#define PROJECTILE_SPEED 400.0f

typedef struct
{
    Vector2 Position[MAX_PROJECTILES];
    Vector2 Direction[MAX_PROJECTILES];
    bool32 Active[MAX_PROJECTILES];
} projectile_pool;

void SpawnProjectile(projectile_pool* Projectiles, Vector2 Position, f32 Angle)
{
    for(int32 i = 0; i < MAX_PROJECTILES; i++)
    {
        if(!Projectiles->Active[i])
        {
            Projectiles->Position[i] = Position;
            Projectiles->Direction[i] = (Vector2){sinf(Angle), -cosf(Angle)};
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
            if(Projectiles->Position[i].x < 0 || Projectiles->Position[i].x > WindowWidth ||
               Projectiles->Position[i].y < 0 || Projectiles->Position[i].y > WindowHeight)
            {
                Projectiles->Active[i] = false;
            }
            else
            {
                Projectiles->Position[i] = Vector2Add(Projectiles->Position[i], Vector2Scale(Projectiles->Direction[i], PROJECTILE_SPEED * dt));
            }
        }
    }
}

void DrawProjectiles(const projectile_pool Projectiles)
{
    for(int32 i = 0; i < MAX_PROJECTILES; i++)
    {
        if(Projectiles.Active[i])
        {
            DrawLineEx(Projectiles.Position[i], Vector2Add(Projectiles.Position[i], Vector2Scale(Projectiles.Direction[i], 15.0f)), 3.0f, SKYBLUE);
        }
    }
}
