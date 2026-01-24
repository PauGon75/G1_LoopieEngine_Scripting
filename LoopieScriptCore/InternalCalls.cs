using System;
using System.Runtime.CompilerServices;

namespace Loopie
{
    public static class InternalCalls
    {
        // === LOGGING ===
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Log(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LogWarning(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LogError(string message);

        // === ENTITY MANAGEMENT === (CAMBIO: string en lugar de ulong)
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string Entity_Create(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Entity_Destroy(string entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string Entity_GetName(string entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Entity_SetName(string entityID, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool Entity_GetActive(string entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Entity_SetActive(string entityID, bool active);

        // === COMPONENT MANAGEMENT ===
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool Entity_HasComponent(string entityID, string componentType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool Entity_AddComponent(string entityID, string componentType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Entity_RemoveComponent(string entityID, string componentType);

        // === TRANSFORM COMPONENT ===
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Transform_GetPosition(string entityID, out float x, out float y, out float z);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Transform_SetPosition(string entityID, float x, float y, float z);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Transform_GetRotation(string entityID, out float x, out float y, out float z, out float w);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Transform_SetRotation(string entityID, float x, float y, float z, float w);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Transform_GetScale(string entityID, out float x, out float y, out float z);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Transform_SetScale(string entityID, float x, float y, float z);

        // === INPUT ===
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool Input_IsKeyPressed(int keyCode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool Input_IsKeyDown(int keyCode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool Input_IsKeyReleased(int keyCode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Input_GetMousePosition(out float x, out float y);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Input_GetMouseDelta(out float x, out float y);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool Input_IsMouseButtonPressed(int button);
    }
}