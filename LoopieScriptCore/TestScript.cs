using System;
using Loopie;

public class TestScript
{
    public void Start()
    {
        InternalCalls.Log("Hello World from C#!");
        InternalCalls.LogWarning("This is a warning!");
        InternalCalls.LogError("This is an error!");
    }

    public void Update(float dt)
    {
        // Puedes descomentar esto para ver logs cada frame (cuidado, spam!)
        // InternalCalls.Log($"Update: {dt}");
    }
}