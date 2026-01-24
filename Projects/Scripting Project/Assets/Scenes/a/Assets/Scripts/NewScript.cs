using System;
using Loopie;

public class NewScript : LoopieScript
{
    public override void Start()
    {
        InternalCalls.Log("Script NewScript iniciado en la entidad: " + Entity.Name);
    }

    public override void Update(float dt)
    {
    }
}
