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
        private float m_Timer = 0.0f;

        public void Start()
        {
            InternalCalls.NativeLog("Script Player incializado con ID: " + ID);
        }

        public void Update(float dt)
        {
            m_Timer += dt;

            if (m_Timer >= 2.0f)
            {
                InternalCalls.NativeLog("Player vivo. Tiempo transcurrido: " + m_Timer);
                m_Timer = 0.0f;
            }
        }
    }
}