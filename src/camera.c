#include "raylib.h"
#include "raymath.h"
#include "utils.h"

#define SHAKE_DECAY_RATE 3.0f

typedef struct game_camera game_camera;
struct game_camera
{
    Camera2D Camera;
    f32 CameraShakeStrength;
    f32 TargetZoom;
};

inline void ShakeCamera(game_camera* GameCamera)
{
    GameCamera->CameraShakeStrength = 10.0f;
}

void UpdateGameCamera(game_camera* GameCamera, int32 WindowWidth, int32 WindowHeight, Vector2 PlayerPosition, f32 dt)
{
    GameCamera->CameraShakeStrength = Lerp(GameCamera->CameraShakeStrength, 0.0f, SHAKE_DECAY_RATE * dt);
    GameCamera->Camera.offset.x = WindowWidth / 2.0f + RandomFloat(-1.0f, 1.0f) * GameCamera->CameraShakeStrength;
    GameCamera->Camera.offset.y = WindowHeight / 2.0f + RandomFloat(-1.0f, 1.0f) * GameCamera->CameraShakeStrength;

    f32 MouseWheelMove = GetMouseWheelMove();
    if(MouseWheelMove)
    {
        GameCamera->TargetZoom = Clamp(GameCamera->TargetZoom + MouseWheelMove, 1.0f, 10.0f);
    }
    GameCamera->Camera.zoom = Lerp(GameCamera->Camera.zoom, GameCamera->TargetZoom, 1.0f - expf(-10.0f * dt));

    Vector2 CameraTarget = GameCamera->TargetZoom > 1.0f ? PlayerPosition : (Vector2){WindowWidth / 2.0f, WindowHeight / 2.0f};
    GameCamera->Camera.target = Vector2Lerp(GameCamera->Camera.target, CameraTarget, 1.0f - expf(-5.0f * dt));
}
