#define PARTICLE_COUNT 1024

typedef struct particle_system particle_system;
struct particle_system
{
    v2 Positions[PARTICLE_COUNT];
    v2 Velocities[PARTICLE_COUNT];
    f32 Sizes[PARTICLE_COUNT];
    f32 Lifetimes[PARTICLE_COUNT];
    Color Colors[PARTICLE_COUNT];

    f32 Accumulator;
};

void UpdateParticles(particle_system* Particles, f32 dt)
{
    for(int32 i = 0; i < PARTICLE_COUNT; i++)
    {
        if(Particles->Lifetimes[i] > 0.0f)
        {
            Particles->Positions[i] = Vector2Add(Particles->Positions[i], Vector2Scale(Particles->Velocities[i], dt));
            Particles->Lifetimes[i] -= dt;
        }
    }
}

void EmitParticles(particle_system* Particles, v2 Position, Color ParticleColor, int32 Count)
{
    for(int32 i = 0; i < Count; i++)
    {
        for(int32 j = 0; j < PARTICLE_COUNT; j++)
        {
            if(Particles->Lifetimes[j] <= 0.0f)
            {
                f32 Angle = RandomFloat(0.0f, 2.0f * PI);
                f32 Speed = RandomFloat(50.0f, 100.0f);
                Particles->Positions[j] = (v2){Position.x + RandomFloat(-5.0f, 5.0f), Position.y + RandomFloat(-5.0f, 5.0f)};
                Particles->Velocities[j] = (v2){cosf(Angle) * Speed, sinf(Angle) * Speed};
                Particles->Colors[j] = ParticleColor;
                Particles->Sizes[j] = RandomFloat(0.5f, 1.5f);
                Particles->Lifetimes[j] = 1.0f;
                break;
            }
        }
    }
}

void DrawParticles(particle_system Particles)
{
    for(int32 i = 0; i < PARTICLE_COUNT; i++)
    {
        if(Particles.Lifetimes[i] > 0.0f)
        {
            DrawCircleV(Particles.Positions[i], Particles.Sizes[i], Particles.Colors[i]);
        }
    }
}
