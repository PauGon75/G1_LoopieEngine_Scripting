using System;
using Loopie;

public class Bullet : LoopieScript
{
    public float Speed = 30.0f;
    public float LifeTime = 2.0f; // Segundos de vida

    public override void Update(float dt)
    {
        // 1. Cuenta regresiva de vida
        LifeTime -= dt;
        if (LifeTime <= 0)
        {
            Entity.Destroy();
            return;
        }

        // 2. Movimiento hacia adelante
        // Calculamos el vector Forward basándonos en la rotación actual
        Quaternion rot = Transform.Rotation;

        // Formula matemática para obtener vector Forward desde Quaternion (0,0,1 rotado)
        float x = 2 * (rot.X * rot.Z + rot.W * rot.Y);
        float y = 2 * (rot.Y * rot.Z - rot.W * rot.X);
        float z = 1 - 2 * (rot.X * rot.X + rot.Y * rot.Y);

        Vector3 forward = new Vector3(x, y, z);

        Transform.Position = Transform.Position + (forward * Speed * dt);
    }
}