using System;
using Loopie;

public class TankController : LoopieScript
{
    public float MoveSpeed = 10.0f;
    public float RotationSpeed = 2.0f;
    public float TurretRotationSpeed = 0.5f;
    public float ShootCooldown = 0.5f;
    private float _timer = 0.0f;
    public float BulletHeight = 2.0f;
    public float BulletForward = 3.5f;
    public Vector3 SpawnScale = new Vector3(0.2f, 0.2f, 0.2f);

    public float RotationOffset = 0.0f;

    private Entity _turret;
    private Entity _bulletTemplate;

    private float _tankYaw = 0.0f;
    private float _turretYaw = 0.0f;

    public override void Start()
    {
        _turret = Entity.Find("TankTurret");
        _bulletTemplate = Entity.Find("Bullet");

        if (_bulletTemplate != null) _bulletTemplate.IsActive = false;

        Quaternion startRot = Transform.Rotation;
        _tankYaw = 2.0f * (float)Math.Atan2(startRot.Y, startRot.W);

        _turretYaw = _tankYaw;
    }

    public override void Update(float dt)
    {
        if (string.IsNullOrEmpty(EntityID)) return;

        if (Input.GetKey(KeyCode.A)) _tankYaw -= RotationSpeed * dt;
        if (Input.GetKey(KeyCode.D)) _tankYaw += RotationSpeed * dt;

        Transform.Rotation = EulerToQuaternion(_tankYaw);

        Vector3 forward = new Vector3((float)Math.Sin(_tankYaw), 0, (float)Math.Cos(_tankYaw));
        Vector3 currentPos = Transform.Position;

        if (Input.GetKey(KeyCode.W)) Transform.Position += forward * MoveSpeed * dt;
        if (Input.GetKey(KeyCode.S)) Transform.Position -= forward * MoveSpeed * dt;


        if (_turret != null)
        {
            _turretYaw -= Input.MouseDelta.X * TurretRotationSpeed * dt;

            _turret.Transform.Rotation = EulerToQuaternion(_turretYaw);
        }


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

        float offsetRad = RotationOffset * (float)(Math.PI / 180.0f);
        float shootAngle = _turretYaw + offsetRad;

        Quaternion bulletRot = EulerToQuaternion(shootAngle);

        Vector3 aimDir = new Vector3((float)Math.Sin(shootAngle), 0, (float)Math.Cos(shootAngle));

        Vector3 tankPos = Transform.Position;
        Vector3 finalPos = tankPos + (aimDir * BulletForward);
        finalPos.Y = BulletHeight;

        newBullet.Transform.Position = finalPos;
        newBullet.Transform.Rotation = bulletRot;
    }

    Quaternion EulerToQuaternion(float angleRadians)
    {
        float halfAngle = angleRadians * 0.5f;
        float sin = (float)Math.Sin(halfAngle);
        float cos = (float)Math.Cos(halfAngle);
        return new Quaternion(0, sin, 0, cos);
    }
}