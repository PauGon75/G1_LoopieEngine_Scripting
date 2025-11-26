#include "Octree.h"

#include "Loopie/Components/MeshRenderer.h"
#include "Loopie/Components/Transform.h"
#include "Loopie/Scene/Entity.h"

namespace Loopie
{
	Octree::Octree(const AABB& rootBounds)
	{
		m_rootNode = std::make_unique<OctreeNode>(rootBounds);
		m_rootNode->m_aabb.SetNegativeInfinity();
	}

	void Octree::Insert(std::shared_ptr<Entity> entity)
	{
		// a. If it is within the bounds of the Octree, add it to the root node
		// b. If you run out of Bucket Space and you are a leaf, subdivide node
		//     into 8 child nodes and add them in the parent node's list
		// c. Redistribute all GameObjects to proper childs based on their
		//     position in space
		// d. If it reaches maximum depth, stop subdividing and add the object in
		//     the list of nodes even we are over bucket capacity

		InsertRecursively(m_rootNode.get(), entity, 0);
	}

	void Octree::Clear()
	{
		m_rootNode.reset();
	}

	// This debug draws the whole Octree. We might consider doing optimizations, 
	// like frustrum, and expand it to debug from a certain Octree downwards
	void Octree::DebugDraw(vec3 color)
	{
		DebugDrawRecursively(m_rootNode.get(), color);
	}

	AABB Octree::GetEntityAABB(const std::shared_ptr<Entity>& entity) const
	{
		auto meshRenderer = entity->GetComponent<MeshRenderer>();
		if (meshRenderer)
		{
			return meshRenderer->GetWorldAABB();
		}
		else
		{
			AABB aabb;
			aabb.Enclose(entity->GetTransform()->GetPosition());
			return aabb;
		}
	}
	
	void Octree::InsertRecursively(OctreeNode* node, std::shared_ptr<Entity> entity, int depth)
	{
		if (!node)
		{
			return;
		}

		AABB entityAABB = GetEntityAABB(entity);

		// If the node is a leaf, add entity and check for entities and depth
		// If max capacity has reached and hasn't reached max depth, it will subdivide the node
		// and redistribute all entities
		if (node->m_isLeaf)
		{
			node->m_entities.push_back(entity);

			if (node->m_entities.size() > MAX_ENTITIES_PER_NODE && depth < MAXIMUM_DEPTH)
			{
				Subdivide(node);
				RedsitributeEntities(node);
			}
			return;
		}


		// If not leaf, find child/ren that interesect with the entity's AABB
		bool inserted = false;
		for (int i = 0; i < MAX_ENTITIES_PER_NODE; ++i)
		{
			if (node->m_children[i] && node->m_children[i]->m_aabb.Intersects(entityAABB))
			{
				InsertRecursively(node->m_children[i].get(), entity, depth + 1);
				inserted = true;
			}
		}

		// If no child intersected, add to this node
		node->m_entities.push_back(entity);
	}

	void Octree::Subdivide(OctreeNode* node)
	{
		// Subdivide node into 8 different nodes
		std::array<AABB, MAX_ENTITIES_PER_NODE> childAABBs = ComputeChildAABBs(node->m_aabb);

		for (int i = 0; i < MAX_ENTITIES_PER_NODE; ++i)
		{
			node->m_children[i] = std::make_unique<OctreeNode>(childAABBs[i]);
			node->m_children[i]->m_parent = node;
			node->m_children[i]->m_isLeaf = true;
		}

		node->m_isLeaf = false;

	}

	void Octree::RedsitributeEntities(OctreeNode* node)
	{
		// Redistribute entities into those 8 different nodes
		if (node->m_isLeaf)
		{
			return;
		}

		std::vector<std::shared_ptr<Entity>> entitiesToRedistribute = std::move(node->m_entities);

		for (auto& entity : entitiesToRedistribute)
		{
			AABB entityAABB = GetEntityAABB(entity);
			bool redistributed = false;

			for (int i = 0; i < MAX_ENTITIES_PER_NODE; ++i)
			{
				if (node->m_children[i] && node->m_children[i]->m_aabb.Intersects(entityAABB))
				{
					InsertRecursively(node->m_children[i].get(), entity, 0);
					redistributed = true;
					break;
				}
			}

			// If entity doesn't fit it any child, keep it at the parent
			if (!redistributed)
			{
				node->m_entities.push_back(entity);
			}
		}

	}

	std::array<AABB, MAX_ENTITIES_PER_NODE> Octree::ComputeChildAABBs(const AABB& parentAABB) const
	{
		std::array<AABB, MAX_ENTITIES_PER_NODE> children;
		vec3 center = parentAABB.GetCenter();
		vec3 min = parentAABB.MinPoint;
		vec3 max = parentAABB.MaxPoint;

		for (int i = 0; i < MAX_ENTITIES_PER_NODE; ++i)
		{
			// This generates all octants by using bit-wise operations
			vec3 childMin, childMax;
			childMin.x = (i & 1) ? center.x : min.x;
			childMin.y = (i & 2) ? center.y : min.y;
			childMin.z = (i & 4) ? center.z : min.z;
			childMax.x = (i & 1) ? max.x : center.x;
			childMax.y = (i & 2) ? max.y : center.y;
			childMax.z = (i & 4) ? max.z : center.z;

			AABB aabb;
			aabb.MinPoint = childMin;
			aabb.MaxPoint = childMax;
			children[i] = aabb;
		}

		return children;
	}

	void Octree::DebugDrawRecursively(OctreeNode* node, vec3 color)
	{
		if (!node)
		{
			return;
		}

		// TODO (PSS - 27/11/25): Draw node->m_children[i]->m_aabb

		for (int i = 0; i < MAX_ENTITIES_PER_NODE; ++i)
		{
			if (node->m_children[i])
			{
				DebugDrawRecursively(node->m_children[i].get(), color);
			}
		}
	}
}