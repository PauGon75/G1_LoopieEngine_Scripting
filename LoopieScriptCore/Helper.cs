using System;

namespace Loopie
{
    public struct Vector3
    {
        public float X, Y, Z;

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        public static Vector3 Zero => new Vector3(0, 0, 0);
        public static Vector3 One => new Vector3(1, 1, 1);
        public static Vector3 Up => new Vector3(0, 1, 0);
        public static Vector3 Forward => new Vector3(0, 0, 1);
        public static Vector3 Right => new Vector3(1, 0, 0);

        public override string ToString()
        {
            return $"({X:F2}, {Y:F2}, {Z:F2})";
        }

        public static Vector3 operator +(Vector3 a, Vector3 b)
        {
            return new Vector3(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
        }

        public static Vector3 operator -(Vector3 a, Vector3 b)
        {
            return new Vector3(a.X - b.X, a.Y - b.Y, a.Z - b.Z);
        }

        public static Vector3 operator *(Vector3 a, float scalar)
        {
            return new Vector3(a.X * scalar, a.Y * scalar, a.Z * scalar);
        }
    }

    public struct Quaternion
    {
        public float X, Y, Z, W;

        public Quaternion(float x, float y, float z, float w)
        {
            X = x;
            Y = y;
            Z = z;
            W = w;
        }

        public static Quaternion Identity => new Quaternion(0, 0, 0, 1);

        public override string ToString()
        {
            return $"({X:F2}, {Y:F2}, {Z:F2}, {W:F2})";
        }
    }

    public class Entity
    {
        public string ID { get; private set; }

        public Entity(string id)
        {
            ID = id;
        }

        public string Name
        {
            get => InternalCalls.Entity_GetName(ID);
            set => InternalCalls.Entity_SetName(ID, value);
        }

        public bool IsActive
        {
            get => InternalCalls.Entity_GetActive(ID);
            set => InternalCalls.Entity_SetActive(ID, value);
        }

        public void Destroy()
        {
            InternalCalls.Entity_Destroy(ID);
        }

        public bool HasComponent(string componentType)
        {
            return InternalCalls.Entity_HasComponent(ID, componentType);
        }

        public bool AddComponent(string componentType)
        {
            return InternalCalls.Entity_AddComponent(ID, componentType);
        }

        public void RemoveComponent(string componentType)
        {
            InternalCalls.Entity_RemoveComponent(ID, componentType);
        }

        public static Entity Create(string name = "Entity")
        {
            string id = InternalCalls.Entity_Create(name);
            return new Entity(id);
        }
    }

    public class Transform
    {
        private string entityID;

        public Transform(string id)
        {
            entityID = id;
        }

        public Vector3 Position
        {
            get
            {
                InternalCalls.Transform_GetPosition(entityID, out float x, out float y, out float z);
                return new Vector3(x, y, z);
            }
            set => InternalCalls.Transform_SetPosition(entityID, value.X, value.Y, value.Z);
        }

        public Quaternion Rotation
        {
            get
            {
                InternalCalls.Transform_GetRotation(entityID, out float x, out float y, out float z, out float w);
                return new Quaternion(x, y, z, w);
            }
            set => InternalCalls.Transform_SetRotation(entityID, value.X, value.Y, value.Z, value.W);
        }

        public Vector3 Scale
        {
            get
            {
                InternalCalls.Transform_GetScale(entityID, out float x, out float y, out float z);
                return new Vector3(x, y, z);
            }
            set => InternalCalls.Transform_SetScale(entityID, value.X, value.Y, value.Z);
        }
    }
}