using System;
using Loopie;

public class NewScript_1 : LoopieScript
{
    public override void Start()
    {
        InternalCalls.Log("Script NewScript_1 iniciado en la entidad: " + Entity.Name);
    }

    public override void Update(float dt)
    {
    }
}
