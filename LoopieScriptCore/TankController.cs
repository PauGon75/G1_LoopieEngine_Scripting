using System;
using Loopie;

public class TankController : LoopieScript
{
    public float MoveSpeed = 15.0f;

    public override void Start()
    {
        // Si el paso 1 y 2 estan bien, esto imprimira el ID del Inspector
        InternalCalls.Log("TANK START - ID: [" + EntityID + "]");
    }

    public override void Update(float dt)
    {
        if (string.IsNullOrEmpty(EntityID)) return;

        // Movemos la entidad aplicada (this)
        Vector3 pos = Transform.Position;

        if (Input.GetKey(KeyCode.W)) pos.Z += MoveSpeed * dt;
        if (Input.GetKey(KeyCode.S)) pos.Z -= MoveSpeed * dt;
        if (Input.GetKey(KeyCode.A)) pos.X -= MoveSpeed * dt;
        if (Input.GetKey(KeyCode.D)) pos.X += MoveSpeed * dt;

        Transform.Position = pos;
    }
}