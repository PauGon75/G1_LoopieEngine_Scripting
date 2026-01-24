using System;
using Loopie;

public class TestScript
{
    // Se ejecuta la primera vez que ScriptComponent::Update detecta la instancia
    void Start()
    {
        // Enviamos un mensaje a la consola de C++ a través de nuestro Glue
        InternalCalls.Log("¡TestScript iniciado desde C#!");

        // Creamos una entidad en la escena de C++
        Entity.Create("Entidad_Desde_CSharp");
    }

    // Se ejecuta cada frame desde ScriptComponent::Update
    void Update(float dt)
    {
        // Aquí puedes añadir lógica de movimiento o respuesta a input
        //TEST EDITOR WORKS!!!!!YEY =D
    }
}



