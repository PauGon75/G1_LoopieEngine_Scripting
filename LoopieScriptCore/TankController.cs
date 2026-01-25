using System;
using Loopie;

public class TankController : LoopieScript
{
    public float MoveSpeed = 10.0f;       // Velocidad de avance
    public float RotationSpeed = 2.5f;    // Velocidad de giro del chasis
    public float TurretRotationSpeed = 1.0f;

    private Entity _turret;
    private float _turretYaw = 0.0f;      // Ángulo de la torreta (Local)
    private float _tankYaw = 0.0f;        // Ángulo del tanque (World/Local)

    public override void Start()
    {
        InternalCalls.Log("Tank Controls Activados: W/S (Motor) - A/D (Giro)");

        // 1. Buscamos la torreta
        _turret = Entity.Find("Turret");
        if (_turret != null) InternalCalls.Log("Torreta vinculada.");

        // 2. IMPORTANTE: Leer la rotación inicial para no pegar un "salto" al empezar
        // Asumimos que el tanque solo rota en Y. La fórmula es yaw = 2 * atan2(y, w)
        Quaternion startRot = Transform.Rotation;
        _tankYaw = 2.0f * (float)Math.Atan2(startRot.Y, startRot.W);
    }

    public override void Update(float dt)
    {
        if (string.IsNullOrEmpty(EntityID)) return;

        // --- 1. GIRO DEL TANQUE (A/D) ---
        // A -> Izquierda (-), D -> Derecha (+)
        if (Input.GetKey(KeyCode.A)) _tankYaw -= RotationSpeed * dt;
        if (Input.GetKey(KeyCode.D)) _tankYaw += RotationSpeed * dt;

        // Aplicamos la rotación al chasis
        float tankHalf = _tankYaw * 0.5f;
        float tankSin = (float)Math.Sin(tankHalf);
        float tankCos = (float)Math.Cos(tankHalf);
        Transform.Rotation = new Quaternion(0, tankSin, 0, tankCos);

        // --- 2. AVANCE DEL TANQUE (W/S) ---
        // Calculamos el vector "Forward" (Hacia adelante) basado en el ángulo actual
        // En matemáticas estándar (y Unity/OpenGL), si 0º es Z+, entonces:
        // X = sin(angle), Z = cos(angle)
        Vector3 forward = new Vector3((float)Math.Sin(_tankYaw), 0, (float)Math.Cos(_tankYaw));

        Vector3 currentPos = Transform.Position;
        bool moved = false;

        if (Input.GetKey(KeyCode.W))
        {
            currentPos = currentPos + (forward * MoveSpeed * dt);
            moved = true;
        }
        if (Input.GetKey(KeyCode.S))
        {
            currentPos = currentPos - (forward * MoveSpeed * dt);
            moved = true;
        }

        if (moved) Transform.Position = currentPos;

        // --- 3. GIRO DE LA TORRETA (Ratón) ---
        // La torreta gira independientemente sobre el chasis
        if (_turret != null)
        {
            Vector2 mouseDelta = Input.MouseDelta;
            _turretYaw += mouseDelta.X * TurretRotationSpeed * dt;

            float turretHalf = _turretYaw * 0.5f;
            float turretSin = (float)Math.Sin(turretHalf);
            float turretCos = (float)Math.Cos(turretHalf);

            _turret.Transform.Rotation = new Quaternion(0, turretSin, 0, turretCos);
        }
    }
}