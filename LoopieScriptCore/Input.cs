using System;

namespace Loopie
{
    public enum KeyCode
    {
        A = 4,
        B = 5,
        C = 6,
        D = 7,
        E = 8,
        F = 9,
        G = 10,
        H = 11,
        I = 12,
        J = 13,
        K = 14,
        L = 15,
        M = 16,
        N = 17,
        O = 18,
        P = 19,
        Q = 20,
        R = 21,
        S = 22,
        T = 23,
        U = 24,
        V = 25,
        W = 26,
        X = 27,
        Y = 28,
        Z = 29,

        Num1 = 30,
        Num2 = 31,
        Num3 = 32,
        Num4 = 33,
        Num5 = 34,
        Num6 = 35,
        Num7 = 36,
        Num8 = 37,
        Num9 = 38,
        Num0 = 39,

        Return = 40,
        Escape = 41,
        Backspace = 42,
        Tab = 43,
        Space = 44,

        Up = 82,
        Down = 81,
        Left = 80,
        Right = 79,

        LeftShift = 225,
        RightShift = 229,
        LeftCtrl = 224,
        RightCtrl = 228,
        LeftAlt = 226,
        RightAlt = 230
    }

    public enum MouseButton
    {
        Left = 1,
        Middle = 2,
        Right = 3
    }

    public static class Input
    {
        public static bool GetKey(KeyCode key)
        {
            return InternalCalls.Input_IsKeyPressed((int)key);
        }

        public static bool GetKeyDown(KeyCode key)
        {
            return InternalCalls.Input_IsKeyDown((int)key);
        }

        public static bool GetKeyUp(KeyCode key)
        {
            return InternalCalls.Input_IsKeyReleased((int)key);
        }

        public static Vector2 MousePosition
        {
            get
            {
                InternalCalls.Input_GetMousePosition(out float x, out float y);
                return new Vector2(x, y);
            }
        }

        public static Vector2 MouseDelta
        {
            get
            {
                InternalCalls.Input_GetMouseDelta(out float x, out float y);
                return new Vector2(x, y);
            }
        }

        public static bool GetMouseButton(MouseButton button)
        {
            return InternalCalls.Input_IsMouseButtonPressed((int)button);
        }
    }

    public struct Vector2
    {
        public float X, Y;

        public Vector2(float x, float y)
        {
            X = x;
            Y = y;
        }

        public static Vector2 Zero => new Vector2(0, 0);
        public static Vector2 One => new Vector2(1, 1);

        public float Magnitude => (float)Math.Sqrt(X * X + Y * Y);

        public Vector2 Normalized
        {
            get
            {
                float mag = Magnitude;
                if (mag > 0.00001f)
                    return new Vector2(X / mag, Y / mag);
                return Zero;
            }
        }

        public static Vector2 operator +(Vector2 a, Vector2 b)
        {
            return new Vector2(a.X + b.X, a.Y + b.Y);
        }

        public static Vector2 operator -(Vector2 a, Vector2 b)
        {
            return new Vector2(a.X - b.X, a.Y - b.Y);
        }

        public static Vector2 operator *(Vector2 a, float scalar)
        {
            return new Vector2(a.X * scalar, a.Y * scalar);
        }

        public override string ToString()
        {
            return $"({X:F2}, {Y:F2})";
        }
    }
}