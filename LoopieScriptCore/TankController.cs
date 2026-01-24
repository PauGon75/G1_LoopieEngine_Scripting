using System;
using Loopie;

public class TankController : LoopieScript
{
    public float MoveSpeed = 10.0f;
    public float RotationSpeed = 2.0f;
    public float TurretSensitivity = 0.5f;

    private Entity _turret;
    private float _tankRotation = 0.0f;
    private float _turretYaw = 0.0f;

    public override void Start()
    {
        InternalCalls.Log("TankController: Inicializado");
        // Buscamos una entidad llamada 'Turret' o la creamos
        _turret = Entity.Create("Tank_Turret");
        _turret.AddComponent("MeshRenderer");
    }

    public override void Update(float dt)
    {
        // 1. MOVIMIENTO DEL CHASIS (Keyboard W/S/A/D)
        float moveInput = 0.0f;
        if (Input.GetKey(KeyCode.W)) moveInput = 1.0f;
        if (Input.GetKey(KeyCode.S)) moveInput = -1.0f;

        float rotateInput = 0.0f;
        if (Input.GetKey(KeyCode.A)) rotateInput = 1.0f;
        if (Input.GetKey(KeyCode.D)) rotateInput = -1.0f;

        // Rotación del chasis (Eje Y)
        _tankRotation += rotateInput * RotationSpeed * dt;
        Transform.Rotation = new Quaternion(0, (float)Math.Sin(_tankRotation / 2), 0, (float)Math.Cos(_tankRotation / 2));

        // Mover hacia adelante basado en la rotación actual
        Vector3 pos = Transform.Position;
        pos.X += (float)Math.Sin(_tankRotation) * moveInput * MoveSpeed * dt;
        pos.Z += (float)Math.Cos(_tankRotation) * moveInput * MoveSpeed * dt;
        Transform.Position = pos;

        // 2. ROTACIÓN DE LA TORRETA (Mouse Delta)
        Vector2 mouseDelta = Input.MouseDelta;
        _turretYaw += mouseDelta.X * TurretSensitivity * dt;

        if (_turret != null)
        {
            Transform turretTransform = new Transform(_turret.ID);
            // La torreta sigue la posición del tanque (con un offset de altura)
            turretTransform.Position = Transform.Position + new Vector3(0, 1.5f, 0);

            // Rotación combinada: Rotación del Tanque + Rotación propia de la torreta
            float totalRotation = _tankRotation + _turretYaw;
            turretTransform.Rotation = new Quaternion(0, (float)Math.Sin(totalRotation / 2), 0, (float)Math.Cos(totalRotation / 2));
        }
    }
}