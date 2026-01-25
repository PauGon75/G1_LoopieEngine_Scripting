using System;
using Loopie;

public class TankController : LoopieScript
{
    public float MoveSpeed = 10.0f;
    public float RotationSpeed = 2.0f;
    public float TurretRotationSpeed = 0.5f; // Bajé un poco la velocidad para apuntar mejor
    public float ShootCooldown = 0.5f;
    private float _timer = 0.0f;

    // --- AJUSTES DE POSICIONAMIENTO ---
    public float BulletHeight = 2.0f;
    public float BulletForward = 3.5f;
    public Vector3 SpawnScale = new Vector3(0.2f, 0.2f, 0.2f);

    // --- ¡NUEVO! CORRECCION DE ROTACION ---
    // Ajusta esto en el editor si dispara chueco: 0, 90, -90, 180...
    public float RotationOffset = 0.0f;

    private Entity _turret;
    private Entity _bulletTemplate;

    // Guardamos los ángulos acumulados aquí
    private float _tankYaw = 0.0f;
    private float _turretYaw = 0.0f;

    public override void Start()
    {
        _turret = Entity.Find("TankTurret");
        _bulletTemplate = Entity.Find("Bullet");

        if (_bulletTemplate != null) _bulletTemplate.IsActive = false;

        // Inicializamos el ángulo del tanque basado en su rotación actual
        Quaternion startRot = Transform.Rotation;
        _tankYaw = 2.0f * (float)Math.Atan2(startRot.Y, startRot.W);

        // Inicializamos la torreta (asumiendo que empieza recta con el tanque)
        _turretYaw = _tankYaw;
    }

    public override void Update(float dt)
    {
        if (string.IsNullOrEmpty(EntityID)) return;

        // --- 1. MOVIMIENTO TANQUE (W/S/A/D) ---
        if (Input.GetKey(KeyCode.A)) _tankYaw -= RotationSpeed * dt;
        if (Input.GetKey(KeyCode.D)) _tankYaw += RotationSpeed * dt;

        // Aplicar rotación al Tanque
        Transform.Rotation = EulerToQuaternion(_tankYaw);

        Vector3 forward = new Vector3((float)Math.Sin(_tankYaw), 0, (float)Math.Cos(_tankYaw));
        Vector3 currentPos = Transform.Position;

        if (Input.GetKey(KeyCode.W)) Transform.Position += forward * MoveSpeed * dt;
        if (Input.GetKey(KeyCode.S)) Transform.Position -= forward * MoveSpeed * dt;


        if (_turret != null)
        {
            // CAMBIO: He puesto un MENOS (-) delante de Input.MouseDelta.X
            // Esto invierte la dirección del giro.
            _turretYaw -= Input.MouseDelta.X * TurretRotationSpeed * dt;

            // Aplicamos rotación visual
            _turret.Transform.Rotation = EulerToQuaternion(_turretYaw);
        }


        // --- 3. DISPARO ---
        _timer -= dt;
        if (Input.GetKey(KeyCode.Space) && _timer <= 0)
        {
            Shoot();
            _timer = ShootCooldown;
        }
    }

    void Shoot()
    {
        if (_turret == null || _bulletTemplate == null) return;

        Entity newBullet = Entity.Instantiate(_bulletTemplate);
        if (newBullet == null) return;

        newBullet.Transform.Scale = SpawnScale;
        newBullet.IsActive = true;
        newBullet.AddScript("Bullet");

        // --- CALCULO MATEMATICO LIMPIO ---

        // 1. Calculamos el ángulo REAL de disparo sumando el Offset
        // Convertimos grados a radianes para la suma
        float offsetRad = RotationOffset * (float)(Math.PI / 180.0f);
        float shootAngle = _turretYaw + offsetRad;

        // 2. Creamos la rotación exacta para la bala
        Quaternion bulletRot = EulerToQuaternion(shootAngle);

        // 3. Calculamos el vector Dirección (Forward) usando Seno y Coseno del ángulo
        // Esto es mucho más fiable que intentar sacar vectores de Quaternions sucios
        Vector3 aimDir = new Vector3((float)Math.Sin(shootAngle), 0, (float)Math.Cos(shootAngle));

        // 4. Posicionamos
        Vector3 tankPos = Transform.Position;
        Vector3 finalPos = tankPos + (aimDir * BulletForward);
        finalPos.Y = BulletHeight; // Altura forzada

        newBullet.Transform.Position = finalPos;
        newBullet.Transform.Rotation = bulletRot;
    }

    // Función auxiliar para convertir ángulo Y (Yaw) a Quaternion sin dolor de cabeza
    Quaternion EulerToQuaternion(float angleRadians)
    {
        float halfAngle = angleRadians * 0.5f;
        float sin = (float)Math.Sin(halfAngle);
        float cos = (float)Math.Cos(halfAngle);
        return new Quaternion(0, sin, 0, cos);
    }
}