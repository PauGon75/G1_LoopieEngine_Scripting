using System;

namespace Loopie
{
    public class LoopieScript
    {
        public string EntityID { get; internal set; }

        private Entity _entity;
        public Entity Entity
        {
            get
            {
                if (_entity == null)
                    _entity = new Entity(EntityID);
                return _entity;
            }
        }

        private Transform _transform;
        public Transform Transform
        {
            get
            {
                if (_transform == null)
                    _transform = new Transform(EntityID);
                return _transform;
            }
        }

        public virtual void Start() { }
        public virtual void Update(float dt) { }
    }
}