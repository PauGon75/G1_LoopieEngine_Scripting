using System.Runtime.CompilerServices;

namespace Loopie
{
    internal static class InternalCalls
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string CreateEntity_Internal(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void DestroyEntity_Internal(string uuid);
    }

    public class Entity
    {
        public string UUID; // Almacenamos el ID generado en C++

        public static Entity Create(string name)
        {
            string id = InternalCalls.CreateEntity_Internal(name);
            return new Entity { UUID = id };
        }

        public void Destroy()
        {
            InternalCalls.DestroyEntity_Internal(this.UUID);
        }
    }
}