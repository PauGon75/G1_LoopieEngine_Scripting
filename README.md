# LoopieEngine - G1 Scripting
<img src="https://i.imgur.com/BjLTpoM.png" style="width: 200px; height: 200px;">

LoopieEngine is a video game engine created by Adrià García, Ana Alcazar and Pol Sierra, video game development and design students from CITM, university affiliated with the UPC in Barcelona. This engine was created as a project for the Game Engines course. For the third assignment, Loopie Engine was chosen to serve as the base to build the different systems on top of it. Our group, consisting of Pau González, Noa Pont and Judit Parras, was responsible of implementing the Scripting System.

## Members
<div align="center">
  <img src="./images/Pau.jpg" style="width: 190px;">
  <img src="./images/Noa.jpg" style="width: 200px;">
  <img src="./images/Judit.jpg" style="width: 201px;">
  
**Pau González:** [PauGon75](https://github.com/PauGon75)&emsp; |&emsp; **Noa Pont:** [noapont](https://github.com/noapont)&emsp; |&emsp; **Judit Parras:** [JulaChan](https://github.com/JulaChan)&emsp;

**Cute Engine Team (Or simply Group 1)**

<img src="./images/CuteTeamGameEngines.jpg" style="width: 500px;">
</div>

**Individual work of each member:** [TaskList](https://docs.google.com/spreadsheets/d/1SdDNdTcWUb7oJugyaaYaRzsubqppoDt9VPHvI7UfRxA/edit?usp=drive_link) [TODO: Hay que poner el link del pdf cuando acabemos el TaskList]

## Main Core Systems

**1. Scripting:** Provides gameplay logic through C# scripts executed via the Mono runtime. It allows entities to define behavior, access engine functionality, and respond to lifecycle events such as creation and updates.

**2. Physics:** Simulates physical behavior of entities, including collisions, rigid body dynamics, and constraints. It ensures consistent and realistic movement and interaction within the game world.

**3. Audio:** Manages sound playback, spatial audio, and audio resource loading. It allows the engine to reproduce sound effects and music synchronized with gameplay events.

**4. Particles:** Generates and controls visual effects composed of large numbers of small sprites or meshes. It is used for effects such as smoke, fire, sparks, or explosions.

**5. Animation:** Controls time-based transformations of objects and characters. It handles skeletal animation, keyframe interpolation, and state transitions.

**6. Shaders:** Manages GPU programs responsible for rendering visuals. It loads, compiles, and binds shaders that define how objects are drawn on screen.

**7. User Interface:** Provides tools for creating and managing user interface elements. It handles layout, input interaction, and rendering of menus, buttons, and HUD elements.

## Scripting System (3rd Assignment)

As our group was in charge of implementing the Scripting System, we decided to integrate Mono and use C# for scripting as Unity does.

### Scripting Subsystems

**1. Mono Integration Subsystem:** Manages the initialization of the Mono runtime and the communication between C++ and C#.

**2. Script Resource and Importing Subsystem:** Treat scripts as engine resources.

**3. Script Component Subsystem:** Allow engine entities to have C# logic attached.

**4. C++ & C# Binding Subsystem:** Expose engine functionality to scripts.

**5. Script Serialization Subsystem:** Allow public script variables to be saved within scenes or prefabs.

**6. Script Editor Subsystem:** Provide in-engine script editing tools.

**7. Script Compilation Subsystem:** Compile *.cs* files into *.dll* assemblies.

**8. Hot Reload Subsystem:** Reload scripts without restarting the engine.

Our system uses all of this subsystems, including Hot Reload and the Text Editor. With our Scripting System, users are able to create and manage scripts as resources, add them to the different entities and edit them within the editor. You can see an example of the functionalities in the tank script. With this script you can move a tank with the keyboard and rotate the turret with the mouse around the scene. The tank shoots bullets (which are new entities).

![ezgif-88e91b2a242759c9](https://github.com/user-attachments/assets/f4ef01d9-f8b4-4271-b112-69ef70558196)

![ezgif-86896126e2ea963f](https://github.com/user-attachments/assets/058eaf0b-646e-464d-90e7-dbbecd2a3874)

https://github.com/user-attachments/assets/8f0363ed-962b-4e40-b658-5105081a3a50


## GitHub Repository

[G1_LoopieEngine_Scripting](https://github.com/PauGon75/G1_LoopieEngine_Scripting)

## Final Release

[GitHub Release](https://github.com/PauGon75/G1_LoopieEngine_Scripting/releases) [TODO: Cambiar link cuando tengamos release]

## License

MIT License

Copyright (c) 2025 CITM - UPC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
