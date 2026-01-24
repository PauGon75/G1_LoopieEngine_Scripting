using System;

namespace Loopie
{
    public abstract class LoopieScript
    {
        // Reference to the Entity that contains the script
        //public Entity entity { get; internal set; }

        public virtual void Awake() { }

        public virtual void Start() { }

        public virtual void Update() { }

        public virtual void OnDestroy() { }
    }
}