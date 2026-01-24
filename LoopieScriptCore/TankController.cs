using System;
using Loopie;

public class TankController : LoopieScript
{
    public float MoveSpeed = 15.0f;
    public float TurretRotationSpeed = 0.5f;

    private Entity _turret;
    private float _turretYaw = 0.0f;

    public override void Start()
    {
        InternalCalls.Log("Tank iniciado. Buscando torreta...");

        // Buscamos la entidad por su nombre en la escena
        _turret = Entity.Find("Turret");

        if (_turret == null)
            InternalCalls.LogError("No se encontro la entidad llamada 'Turret'!");
        else
            InternalCalls.Log("Torreta encontrada con ID: " + _turret.ID);
    }

    public override void Update(float dt)
    {
        if (string.IsNullOrEmpty(EntityID)) return;

        // 1. MOVIMIENTO DEL TANQUE (WASD)
        Vector3 tankPos = Transform.Position;
        bool moved = false;

        if (Input.GetKey(KeyCode.W)) { tankPos.Z += MoveSpeed * dt; moved = true; }
        if (Input.GetKey(KeyCode.S)) { tankPos.Z -= MoveSpeed * dt; moved = true; }
        if (Input.GetKey(KeyCode.A)) { tankPos.X -= MoveSpeed * dt; moved = true; }
        if (Input.GetKey(KeyCode.D)) { tankPos.X += MoveSpeed * dt; moved = true; }

        if (moved) Transform.Position = tankPos;

        // 2. ROTACION DE LA TORRETA (Raton)
        if (_turret != null)
        {
            // Sincronizar posición: Poner la torreta encima del tanque
            // Ajusta el '+ 2.0f' segun la altura de tu modelo
            _turret.Transform.Position = new Vector3(tankPos.X, tankPos.Y + 2.0f, tankPos.Z);

            // Rotar segun el movimiento horizontal del raton
            Vector2 mouseDelta = Input.MouseDelta;
            _turretYaw += mouseDelta.X * TurretRotationSpeed * dt;

            // Aplicar rotacion (Eje Y)
            float halfAngle = _turretYaw * 0.5f;
            float sin = (float)Math.Sin(halfAngle);
            float cos = (float)Math.Cos(halfAngle);

            // Quaternion (X, Y, Z, W) -> Rotacion en Y
            _turret.Transform.Rotation = new Quaternion(0, sin, 0, cos);
        }
    }
}