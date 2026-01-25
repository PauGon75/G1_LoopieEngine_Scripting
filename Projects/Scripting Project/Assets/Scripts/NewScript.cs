using System;
using Loopie;

public class NewScript : LoopieScript
{
    public override void Start()
    {
        InternalCalls.Log("Script NewScript ready. ID: " + EntityID);
    }

    public override void Update(float dt)
    {
    }
}

