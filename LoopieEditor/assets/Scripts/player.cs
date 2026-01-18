using System;
using System.Runtime.CompilerServices;

namespace Loopie
{
    public static class InternalCalls
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void NativeLog(string message);
    }

    public class Player
    {
        public uint ID;

        public void Start()
        {
            InternalCalls.NativeLog("¡Script Player iniciado correctamente!");
        }

        public void Update(float dt)
        {
            InternalCalls.NativeLog("Actualizando Player con dt: " + dt);
        }
    }
}