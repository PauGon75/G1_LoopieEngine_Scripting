using System;
using System.Runtime.CompilerServices;

namespace Loopie
{
    public static class InternalCalls
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void NativeLog(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Native_SetTranslation(uint entityID, float x, float y, float z);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Native_CreateEntity(string name);
    }

    public class Entity
    {
        public uint ID;
        private float _timer = 0;

        public void OnUpdate()
        {
            _timer += 0.01f;
            if (_timer > 5.0f)
            {
                InternalCalls.NativeLog("HelloWorld desde C#");
                _timer = 0;
            }
            InternalCalls.Native_SetTranslation(ID, 2.0f, 0.0f, 0.0f);
        }
    }
}