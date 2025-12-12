#if !defined(SS_H)
#define SS_H

typedef enum state_type state_type;
enum state_type
{
	MainMenu,
	Controls,
	Playing,
	LevelUp,
	Paused,
	GameOver,
	Quit,
};

typedef struct game_state game_state;
struct game_state
{
    state_type Type;
    state_type LastType;
    player Player;
    enemy_pool Enemies;
    particle_system Particles;
    projectile_pool Projectiles;
    Camera2D Camera;
    f32 CameraShakeStrength;
};

INLINE v2 GetWindowCenter(void)
{
    return((v2){WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f});
}

INLINE f32 RandomFloat(f32 Min, f32 Max)
{
    return(Min + ((f32)GetRandomValue(0, INT32_MAX) / (f32)INT32_MAX) * (Max - Min));
}

#endif