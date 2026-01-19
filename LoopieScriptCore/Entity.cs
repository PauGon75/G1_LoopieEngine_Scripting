using System;
using System.Runtime.CompilerServices;

namespace Loopie
{
    public class Entity
    {
        public readonly ulong ID;
        protected Entity() { ID = 0; }
        internal Entity(ulong id) { ID = id; }

        public virtual void Init() { }
        public virtual void Update(float dt) { }
    }
}