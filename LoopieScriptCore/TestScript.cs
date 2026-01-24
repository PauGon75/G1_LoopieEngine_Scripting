using System;
using Loopie;

public class TestScript : LoopieScript
{
    private Entity spawnedEntity;
    private float timer = 0;
    private bool hasSpawned = false;

    public override void Start()
    {
        InternalCalls.Log("=== TestScript Started ===");
        InternalCalls.Log($"My entity: {Entity.Name}");
        InternalCalls.Log($"My position: {Transform.Position}");
        InternalCalls.Log($"Is active: {Entity.IsActive}");
    }

    public override void Update(float dt)
    {
        timer += dt;

        // Crear entidad después de 2 segundos
        if (timer > 2.0f && !hasSpawned)
        {
            InternalCalls.Log("Creating new entity...");
            spawnedEntity = Entity.Create("DynamicCube");

            Transform spawnedTransform = new Transform(spawnedEntity.ID);
            spawnedTransform.Position = new Vector3(5, 0, 0);
            spawnedTransform.Scale = new Vector3(2, 2, 2);

            if (spawnedEntity.AddComponent("MeshRenderer"))
            {
                InternalCalls.Log("MeshRenderer added!");
            }

            hasSpawned = true;
        }

        // Rotar esta entidad continuamente
        Vector3 currentRot = Transform.Position;
        Transform.Position = new Vector3(
            currentRot.X,
            currentRot.Y + (float)Math.Sin(timer) * 0.01f,
            currentRot.Z
        );

        // Destruir después de 10 segundos
        if (timer > 10.0f && spawnedEntity != null)
        {
            InternalCalls.Log($"Destroying {spawnedEntity.Name}");
            spawnedEntity.Destroy();
            spawnedEntity = null;
        }
    }
}