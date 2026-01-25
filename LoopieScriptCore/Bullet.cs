using System;
using Loopie;

public class Bullet : LoopieScript
{
    public float Speed = 30.0f;
    public float LifeTime = 1.0f; // <--- 1 SEGUNDO EXACTO

    public override void Update(float dt)
    {
        // DEBUG: Comentar la destrucción para ver si la bala existe
        /*
        LifeTime -= dt;
        if (LifeTime <= 0)
        {
            Entity.Destroy();
            return;
        }
        */

        // Movimiento
        Quaternion rot = Transform.Rotation;
        float x = 2 * (rot.X * rot.Z + rot.W * rot.Y);
        float y = 2 * (rot.Y * rot.Z - rot.W * rot.X);
        float z = 1 - 2 * (rot.X * rot.X + rot.Y * rot.Y);
        Vector3 forward = new Vector3(x, y, z);

        Transform.Position = Transform.Position + (forward * Speed * dt);
    }
}