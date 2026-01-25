using System;
using Loopie;

public class TankController : LoopieScript
{
    // --- Configuración de Movimiento ---
    public float MoveSpeed = 10.0f;       // Velocidad de avance
    public float RotationSpeed = 2.0f;    // Velocidad de giro del chasis
    public float TurretRotationSpeed = 1.0f; // Sensibilidad del ratón

    // --- Configuración de Disparo ---
    public float ShootCooldown = 0.5f;    // Tiempo entre disparos
    private float _timer = 0.0f;

    // --- Referencias ---
    private Entity _turret;
    private Entity _bulletTemplate;       // La bala original que usaremos de molde

    // --- Estado Interno ---
    private float _tankYaw = 0.0f;        // Angulo del chasis
    private float _turretYaw = 0.0f;      // Angulo de la torreta

    public override void Start()
    {
        InternalCalls.Log("Iniciando TankController...");

        // 1. Vincular Torreta
        _turret = Entity.Find("TankTurret");
        if (_turret == null)
            InternalCalls.LogError("ERROR: No encuentro la entidad 'Turret'.");

        // 2. Vincular Plantilla de Bala
        // Busca un objeto llamado 'Bullet' (que deberías tener oculto o dentro del tanque)
        _bulletTemplate = Entity.Find("Bullet");

        if (_bulletTemplate != null)
        {
            // Apagamos la plantilla para que no moleste en la escena
            _bulletTemplate.IsActive = false;
            InternalCalls.Log("Plantilla 'Bullet' encontrada y desactivada.");
        }
        else
        {
            InternalCalls.LogError("ERROR: No encuentro la entidad 'Bullet' para usar de plantilla.");
        }

        // 3. Sincronizar rotación inicial
        // Evita que el tanque salte a 0 grados al empezar si ya estaba rotado en el editor
        Quaternion startRot = Transform.Rotation;
        // Formula inversa aproximada para sacar el ángulo Y del Quaternion
        _tankYaw = 2.0f * (float)Math.Atan2(startRot.Y, startRot.W);
    }

    public override void Update(float dt)
    {
        if (string.IsNullOrEmpty(EntityID)) return;

        // =========================================================
        // 1. MOVIMIENTO DEL TANQUE (Estilo Tanque Real)
        // =========================================================

        // GIRO (A/D)
        if (Input.GetKey(KeyCode.A)) _tankYaw -= RotationSpeed * dt;
        if (Input.GetKey(KeyCode.D)) _tankYaw += RotationSpeed * dt;

        // Aplicamos la rotación al chasis
        float tankHalf = _tankYaw * 0.5f;
        float tankSin = (float)Math.Sin(tankHalf);
        float tankCos = (float)Math.Cos(tankHalf);
        Transform.Rotation = new Quaternion(0, tankSin, 0, tankCos);

        // AVANCE (W/S)
        // Calculamos el vector "Hacia Adelante" basado en el ángulo actual
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

        // =========================================================
        // 2. MOVIMIENTO DE LA TORRETA (Independiente)
        // =========================================================
        if (_turret != null)
        {
            Vector2 mouseDelta = Input.MouseDelta;
            _turretYaw += mouseDelta.X * TurretRotationSpeed * dt;

            float turretHalf = _turretYaw * 0.5f;
            float turretSin = (float)Math.Sin(turretHalf);
            float turretCos = (float)Math.Cos(turretHalf);

            // Solo aplicamos rotación local. La posición la hereda del tanque.
            _turret.Transform.Rotation = new Quaternion(0, turretSin, 0, turretCos);
        }

        // =========================================================
        // 3. DISPARO
        // =========================================================
        _timer -= dt;
        if (Input.GetKey(KeyCode.Space) && _timer <= 0)
        {
            Shoot();
            _timer = ShootCooldown;
        }
    }

    void Shoot()
    {
        // Validaciones de seguridad
        if (_turret == null) return;
        if (_bulletTemplate == null) return;

        // 1. INSTANCIAR (CLONAR)
        // Crea una copia exacta de la plantilla (mesh, material, escala...)
        Entity newBullet = Entity.Instantiate(_bulletTemplate);
        if (newBullet == null) return;

        // 2. ACTIVAR
        // Como la plantilla estaba apagada, la copia nace apagada. La encendemos.
        newBullet.IsActive = true;

        // 3. AÑADIR LOGICA
        // Le pegamos el script que hace que se mueva hacia adelante
        newBullet.AddScript("Bullet");

        // 4. POSICIONAR EN LA PUNTA DEL CAÑÓN
        // Calculamos el vector Forward de la torreta para saber dónde es "adelante"
        Quaternion rot = _turret.Transform.Rotation;

        // Formula matemática para vector Forward desde Quaternion
        float fx = 2 * (rot.X * rot.Z + rot.W * rot.Y);
        float fy = 2 * (rot.Y * rot.Z - rot.W * rot.X);
        float fz = 1 - 2 * (rot.X * rot.X + rot.Y * rot.Y);
        Vector3 turretForward = new Vector3(fx, fy, fz);

        // Posicion: Centro de torreta + 3 metros hacia adelante
        Vector3 spawnPos = _turret.Transform.Position + (turretForward * 3.0f);

        // Ajuste de altura (opcional, por si el cañón está alto)
        spawnPos.Y += 0.5f;

        newBullet.Transform.Position = spawnPos;
        newBullet.Transform.Rotation = rot; // Sale con la misma rotación que la torreta
    }
}