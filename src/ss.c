// NOTE(griffin): This is only included for snprintf(), not a fan.
#include <stdio.h>

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include "ss_types.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define BACKGROUND_COLOR (Color){15, 15, 15, 255}

global v2 Stars[128];

global Texture2D EnemyTexture;
global Texture2D PlayerTexture;
global Sound ExplosionSound;
global Sound ThrusterSound;
global Sound ButtonClickSound;

global bool32 IsRunning = true;

INLINE f32 RandomFloat(f32 Min, f32 Max);
INLINE v2 GetWindowCenter(void);

#include "ss_particle.c"
#include "ss_player.c"
#include "ss_projectile.c"
#include "ss_enemy.c"
#include "ss.h"
#include "ss_ui.c"

internal void InitGame(game_state* State)
{
    *State = (game_state){0};
    State->Enemies.CurrentWave = 1;
    State->Enemies.EnemiesToSpawn = 10;
    State->Enemies.EnemiesRemaining = 10;
    State->Enemies.Speed = 50.0f;
    State->Enemies.Damage = 1.0f;
    
    State->Player.Position = GetWindowCenter();
    State->Player.Acceleration = (v2){300.0f, 300.0f};
    State->Player.Health = 100.0f;
    State->Player.MaxHealth = 100.0f;
    State->Player.Level = 1;
    State->Player.ProjectileCount = 1;
    
    State->Camera.target = GetWindowCenter(); 
    State->Camera.offset = GetWindowCenter();
    State->Camera.zoom = 1.0f;
    State->CameraShakeStrength = 0.0f;
    
    SpawnEnemies(&State->Enemies, State->Player.Position, State->Enemies.EnemiesToSpawn);
}

INLINE void ShakeCamera(game_state* State)
{
    State->CameraShakeStrength = 10.0f;
}

internal void UpdateGameCamera(game_state* State, f32 dt)
{
    State->CameraShakeStrength = Lerp(State->CameraShakeStrength, 0.0f, 3.0f * dt);
    State->Camera.offset.x = (WINDOW_WIDTH / 2.0f) + (RandomFloat(-1.0f, 1.0f) * State->CameraShakeStrength);
    State->Camera.offset.y = (WINDOW_HEIGHT / 2.0f) + (RandomFloat(-1.0f, 1.0f) * State->CameraShakeStrength);
}

internal void ResolveCollisions(game_state* State)
{
    for(int32 i = 0; i < MAX_PROJECTILES; i++)
    {
        if(State->Projectiles.Active[i])
        {
            for(int32 j = 0; j < MAX_ENEMIES; j++)
            {
                if(State->Enemies.Active[j])
                {
                    if(Vector2Distance(State->Projectiles.Positions[i], State->Enemies.Positions[j]) < 32.0f)
                    {
                        State->Enemies.Active[j] = false;
                        State->Enemies.EnemiesRemaining--;
                        State->Projectiles.Active[i] = false;
                        
                        State->Player.Exp += ExpRequiredForLevelUp(State->Player.Level) / 20.0f;
                        
                        EmitParticles(&State->Particles, State->Enemies.Positions[j], RED, 32);
                        PlaySound(ExplosionSound);
                        ShakeCamera(State);
                        break;
                    }
                }
            }
        }
    }
    
    for(int32 i = 0; i < MAX_ENEMIES; i++)
    {
        if(State->Enemies.Active[i])
        {
            if(Vector2Distance(State->Player.Position, State->Enemies.Positions[i]) < 32.0f)
            {
                if(State->Player.DamageCooldown <= 0.0f)
                {
                    State->Player.Health -= State->Enemies.Damage;
                    State->Player.DamageCooldown = 0.25f;
                }
                if(State->Player.Health <= 0)
                {
                    State->Type = GameOver;
                }
                break;
            }
        }
    }
}

internal void UpdateBackground(game_state* State, f32 dt)
{
    for(int32 i = 0; i < ArrayCount(Stars); i++)
	{
		Stars[i] = Vector2Subtract(Stars[i], Vector2Scale(State->Player.Velocity, 0.25f * dt));
        
		Stars[i].x = Wrap(Stars[i].x, 0.0f, (f32)WINDOW_WIDTH);
		Stars[i].y = Wrap(Stars[i].y, 0.0f, (f32)WINDOW_HEIGHT);
	}
}

internal void DrawBackground(void)
{
	ClearBackground(BACKGROUND_COLOR);
    
    rlSetTexture(0);
    rlBegin(RL_QUADS);
    rlColor4ub(130, 130, 130, 255);
    
    for(int32 i = 0; i < ArrayCount(Stars); i++)
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

internal void DrawMainMenu(game_state* State)
{
	DrawText("Space Survivors", (WINDOW_WIDTH / 2) - (MeasureText("Space Survivors", 64) / 2), (WINDOW_HEIGHT / 2) - 200, 64, WHITE);
	if(DoButton("Play", (WINDOW_WIDTH / 2) - (MeasureText("Play", 48) / 2), (WINDOW_HEIGHT / 2), 48))
	{
		InitGame(State);
        State->Type = Playing;
	}
	if(DoButton("Controls", (WINDOW_WIDTH / 2) - (MeasureText("Controls", 48) / 2), (WINDOW_HEIGHT / 2) + 50, 48))
	{
	    State->LastType = MainMenu;
	    State->Type = Controls;
	}
	if(DoButton("Quit", (WINDOW_WIDTH / 2) - (MeasureText("Quit", 48) / 2), (WINDOW_HEIGHT / 2) + 100, 48))
	{
	    State->Type = Quit;
	}
}

internal void DrawPausedMenu(game_state* State)
{
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){0, 0, 0, 150});
    
	DrawText("Paused", (WINDOW_WIDTH / 2) - (MeasureText("Paused", 48) / 2), (WINDOW_HEIGHT / 2) - 200, 48, WHITE);
	if(DoButton("Resume", (WINDOW_WIDTH / 2) - (MeasureText("Resume", 48) / 2), (WINDOW_HEIGHT / 2), 48))
	{
	    State->Type = Playing;
	}
	if(DoButton("Controls", (WINDOW_WIDTH / 2) - (MeasureText("Controls", 48) / 2), (WINDOW_HEIGHT / 2) + 50, 48))
	{
	    State->LastType = Paused;
	    State->Type = Controls;
	}
	if(DoButton("Main Menu", (WINDOW_WIDTH / 2) - (MeasureText("Main Menu", 48) / 2), (WINDOW_HEIGHT / 2) + 100, 48))
	{
	    State->Type = MainMenu;
	}
}

internal void DrawControlsMenu(game_state* State)
{
    DrawText("Controls", (WINDOW_WIDTH / 2) - (MeasureText("Controls", 48) / 2), (WINDOW_HEIGHT / 2) - 220, 48, WHITE);
    DrawText("W - Move Forward", (WINDOW_WIDTH / 2) - (MeasureText("W - Move Forward", 48) / 2), (WINDOW_HEIGHT / 2) - 100, 48, WHITE);
    DrawText("A - Rotate Left", (WINDOW_WIDTH / 2) - (MeasureText("A - Rotate Left", 48) / 2), (WINDOW_HEIGHT / 2) - 50, 48, WHITE);
    DrawText("D - Rotate Right", (WINDOW_WIDTH / 2) - (MeasureText("D - Rotate Right", 48) / 2), (WINDOW_HEIGHT / 2), 48, WHITE);
    DrawText("SPACE - Shoot", (WINDOW_WIDTH / 2) - (MeasureText("SPACE - Shoot", 48) / 2), (WINDOW_HEIGHT / 2) + 50, 48, WHITE);
    DrawText("ESC - Pause / Back", (WINDOW_WIDTH / 2) - (MeasureText("ESC - Pause / Back", 48) / 2), (WINDOW_HEIGHT / 2) + 100, 48, WHITE);
    if(DoButton("Back", (WINDOW_WIDTH / 2) - (MeasureText("Back", 48) / 2), WINDOW_HEIGHT - 100, 48))
    {
        State->Type = State->LastType;
    }
}

internal void DrawGameOver(game_state* State)
{
	DrawText("Game Over!", (WINDOW_WIDTH / 2) - (MeasureText("Game Over!", 64) / 2), (WINDOW_HEIGHT / 2) - 200, 64, WHITE);
	if(DoButton("Restart", (WINDOW_WIDTH / 2) - (MeasureText("Restart", 48) / 2), (WINDOW_HEIGHT / 2), 48))
	{
		InitGame(State);
        State->Type = Playing;
	}
	if(DoButton("Back to Main Menu", (WINDOW_WIDTH / 2) - (MeasureText("Back to Main Menu", 48) / 2), (WINDOW_HEIGHT / 2) + 50, 48))
	{
	    State->Type = MainMenu;
	}
}

internal void DrawHUD(game_state* State)
{
    f32 Health = Clamp(State->Player.Health, 0.0f, State->Player.MaxHealth);
    
    f32 MaxExp = ExpRequiredForLevelUp(State->Player.Level);
    f32 Exp = Clamp(State->Player.Exp, 0.0f, MaxExp);
    
    f32 BarWidth = 200.0f;
    f32 BarHeight = 20.0f;
    
    f32 X = 20.0f;
    f32 Y = WINDOW_HEIGHT - BarHeight - 20.0f;
    f32 ExpBarY = Y - BarHeight - 10.0f;
    
    f32 ExpPercent = Exp / MaxExp;
    f32 FillWidth = BarWidth * ExpPercent;
    
    DrawRectangle((int32)X, (int32)ExpBarY, (int32)BarWidth, (int32)BarHeight, BLACK);
    DrawRectangle((int32)X, (int32)ExpBarY, (int32)FillWidth, (int32)BarHeight, DARKGREEN);
    DrawRectangleLines((int32)X, (int32)ExpBarY, (int32)BarWidth, (int32)BarHeight, WHITE);
    
    char ExpBarLabel[64];
    snprintf(ExpBarLabel, sizeof(ExpBarLabel), "EXP: %.0f", Exp);
    DrawText(ExpBarLabel, (int32)(X + (BarWidth / 2) - (MeasureText(ExpBarLabel, 18) / 2)), (int32)(ExpBarY + (BarHeight / 2) - 9), 18, WHITE);
    
    f32 HealthPercent = Health / State->Player.MaxHealth;
    FillWidth = BarWidth * HealthPercent;
    
    DrawRectangle((int32)X, (int32)Y, (int32)BarWidth, (int32)BarHeight, BLACK);
    DrawRectangle((int32)X, (int32)Y, (int32)FillWidth, (int32)BarHeight, RED);
    DrawRectangleLines((int32)X, (int32)Y, (int32)BarWidth, (int32)BarHeight, WHITE);
    
    char HealthBarLabel[64];
    snprintf(HealthBarLabel, sizeof(HealthBarLabel), "Health: %.0f", Health);
    DrawText(HealthBarLabel, (int32)(X + (BarWidth / 2) - (MeasureText(HealthBarLabel, 18) / 2)), (int32)(Y + (BarHeight / 2) - 9), 18, WHITE);
    
    char WaveText[64];
    snprintf(WaveText, sizeof(WaveText), "Wave: %d", State->Enemies.CurrentWave);
    DrawText(WaveText, WINDOW_WIDTH - MeasureText(WaveText, 32) - 20, 20, 32, WHITE);
    
    char LevelText[64];
    snprintf(LevelText, sizeof(LevelText), "Level: %d", State->Player.Level);
    DrawText(LevelText, WINDOW_WIDTH - MeasureText(LevelText, 32) - 20, 60, 32, WHITE);
}

internal void DrawLevelUpScreen(game_state* State)
{
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){0, 0, 0, 200});
    
    int32 ButtonWidth = 500;
    int32 ButtonHeight = 70;
    
    int32 CenterX = WINDOW_WIDTH / 2;
    int32 StartY = WINDOW_HEIGHT / 2 - 120;
    
    const char* Title = "Level-Up Upgrades";
    Rectangle TitleRect = {
        (f32)CenterX - ButtonWidth / 2,
        (f32)StartY - ButtonHeight,
        (f32)ButtonWidth,
        (f32)ButtonHeight
    };
    
    DrawRectangleRec(TitleRect, DARKGRAY);
    DrawRectangleLinesEx(TitleRect, 2, WHITE);
    
    int32 TextW = MeasureText(Title, 48);
    int32 TextX = (int32)(TitleRect.x + ButtonWidth / 2 - TextW / 2);
    int32 TextY = (int32)(TitleRect.y + ButtonHeight / 2 - 24);
    DrawText(Title, TextX, TextY, 48, WHITE);
    
    Rectangle ButtonRect1 = {
        (f32)CenterX - ButtonWidth / 2,
        (f32)StartY,
        (f32)ButtonWidth,
        (f32)ButtonHeight
    };
    
    Rectangle ButtonRect2 = {
        ButtonRect1.x,
        ButtonRect1.y + ButtonHeight,
        (f32)ButtonWidth,
        (f32)ButtonHeight
    };
    
    Rectangle ButtonRect3 = {
        ButtonRect1.x,
        ButtonRect1.y + ButtonHeight * 2,
        (f32)ButtonWidth,
        (f32)ButtonHeight
    };
    
    if(DoBoxedButton("Increase Max Health", ButtonRect1, 36, RED))
    {
        State->Player.MaxHealth += 20.0f;
        State->Player.Health = State->Player.MaxHealth;
        State->Player.HasLeveledUp = false;
        State->Type = Playing;
    }
    if(DoBoxedButton("Increase Projectile Count", ButtonRect2, 36, GREEN))
    {
        State->Player.ProjectileCount++;
        State->Player.HasLeveledUp = false;
        State->Type = Playing;
    }
    if(DoBoxedButton("Increase Move Speed", ButtonRect3, 36, BLUE))
    {
        State->Player.Acceleration = Vector2Add(State->Player.Acceleration, Vector2Scale(State->Player.Acceleration, 1.05f));
        State->Player.HasLeveledUp = false;
        State->Type = Playing;
    }
}

int main(void)
{
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Space Survivors");
    InitAudioDevice();
    
	SetExitKey(KEY_NULL);
    
    rlDisableBackfaceCulling();
    rlDisableDepthTest();
    
    EnemyTexture = LoadTexture("../assets/enemy.png");
    PlayerTexture = LoadTexture("../assets/ship.png");
    ExplosionSound = LoadSound("../assets/sfx/explosion.mp3");
    ThrusterSound = LoadSound("../assets/sfx/thruster.mp3");
    ButtonClickSound = LoadSound("../assets/sfx/button.mp3");
    
    for(int32 i = 0; i < ArrayCount(Stars); i++)
    {
        Stars[i] = (v2){RandomFloat(-250.0f, (f32)WINDOW_WIDTH + 250.0f), RandomFloat(-250.0f, (f32)WINDOW_HEIGHT) + 250.0f};
    }
    
    game_state State = {0};
    
    while(!WindowShouldClose() && IsRunning)
    {
        f32 dt = GetFrameTime();
        
		BeginDrawing();
		DrawBackground();
		switch(State.Type)
		{
			case MainMenu:
			{
				DrawMainMenu(&State);
				break;
			}
			case Controls:
			{
			    if(IsKeyPressed(KEY_ESCAPE))
				{
				    State.Type = State.LastType;
				}
                
				DrawControlsMenu(&State);
				break;
			}
			case Playing:
			{
			    if(IsKeyDown(KEY_W))
				{
					AcceleratePlayer(&State.Player, dt);
                    
					State.Particles.Accumulator += dt;
					if(State.Particles.Accumulator >= 0.05f)
					{
						f32 Radians = State.Player.Angle * DEG2RAD;
						v2 Direction = (v2){sinf(Radians), -cosf(Radians)};
						v2 FirePosition = Vector2Subtract(State.Player.Position, Vector2Scale(Direction, PlayerTexture.height * 0.5f));
						EmitParticles(&State.Particles, FirePosition, ORANGE, 1);
						State.Particles.Accumulator = 0.0f;
					}
					if(!IsSoundPlaying(ThrusterSound))
					{
					    PlaySound(ThrusterSound);
					}
				}
				else
				{
				    if(IsSoundPlaying(ThrusterSound))
					{
						StopSound(ThrusterSound);
					}
				}
				if(IsKeyDown(KEY_A))
				{
					State.Player.Angle -= ROTATION_SPEED * dt;
				}
				if(IsKeyDown(KEY_D))
				{
					State.Player.Angle += ROTATION_SPEED * dt;
				}
				if(IsKeyPressed(KEY_SPACE))
				{
				    for(int32 i = 0; i < State.Player.ProjectileCount; i++)
				    {
						f32 Offset = (2.0f * PI * i) / State.Player.ProjectileCount;
						f32 ProjectileAngle = (State.Player.Angle * DEG2RAD) + Offset;
						SpawnProjectile(&State.Projectiles, State.Player.Position, ProjectileAngle);
					}
				}
				if(IsKeyPressed(KEY_ESCAPE))
				{
				    State.Type = Paused;
				}
                
				UpdateParticles(&State.Particles, dt);
				UpdateProjectiles(&State.Projectiles, dt);
				UpdateEnemies(&State.Enemies, State.Player.Position, dt);
				UpdatePlayer(&State.Player, dt);
				UpdateGameCamera(&State, dt);
				UpdateBackground(&State, dt);
                
				ResolveCollisions(&State);
				if(State.Enemies.EnemiesRemaining <= 0)
				{
				    StartNextWave(&State.Enemies, State.Player.Position);
				}
                
				if(State.Player.HasLeveledUp)
				{
				    State.Type = LevelUp;
				}
                
				BeginMode2D(State.Camera);
				DrawPlayer(State.Player);
				DrawProjectiles(State.Projectiles);
				DrawEnemies(State.Enemies);
				DrawParticles(State.Particles);
				EndMode2D();
				DrawHUD(&State);
                
				break;
			}
			case LevelUp:
		    {
				if(IsSoundPlaying(ThrusterSound))
    			{
    			    StopSound(ThrusterSound);
    			}
                
			    BeginMode2D(State.Camera);
				DrawPlayer(State.Player);
				DrawProjectiles(State.Projectiles);
				DrawEnemies(State.Enemies);
				DrawParticles(State.Particles);
				EndMode2D();
				DrawHUD(&State);
                
				DrawLevelUpScreen(&State);
				break;
			}
			case Paused:
			{
    			if(IsSoundPlaying(ThrusterSound))
    			{
    			    StopSound(ThrusterSound);
    			}
                
			    if(IsKeyPressed(KEY_ESCAPE))
				{
				    State.Type = Playing;
				}
                
				BeginMode2D(State.Camera);
				DrawPlayer(State.Player);
				DrawProjectiles(State.Projectiles);
				DrawEnemies(State.Enemies);
				DrawParticles(State.Particles);
				EndMode2D();
				DrawHUD(&State);
                
				DrawPausedMenu(&State);
				break;
			}
			case GameOver:
	        {
				if(IsSoundPlaying(ThrusterSound))
				{
				    StopSound(ThrusterSound);
				}
                
				DrawGameOver(&State);
				break;
			}
			case Quit:
			{
				IsRunning = false;
				break;
			}
		}
        
		DrawFPS(10, 10);
		EndDrawing();
	}
    
    CloseWindow();
    return(0);
}
