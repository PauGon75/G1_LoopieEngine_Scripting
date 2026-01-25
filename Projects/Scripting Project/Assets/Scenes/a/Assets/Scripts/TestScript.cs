using System;
using Loopie;

public class TestScript
{

    void Start()
    {

        InternalCalls.Log("¡TestScript C#!");


        Entity.Create("Entidad_Desde_CSharp");
    }


    void Update(float dt)
    {


    }
}






