#include "Component.h"


#include "Loopie/Scene/Entity.h"
#include "Loopie/Components/Transform.h"

namespace Loopie {
	Component::~Component(){}
	Transform* Component::GetTransform() const
	{
		return GetOwner()->GetTransform();
	}
}