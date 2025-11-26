#pragma once
#include "Loopie/Math/OctreeNode.h"

#include <memory>
#include <array>


namespace Loopie {
	constexpr int MAXIMUM_DEPTH = 3; // Can be modified as necessary

	class Entity;

	//template<typename T>
	class Octree
	{
	public:
		Octree(const AABB& rootBounds);
		~Octree() = default;

		void Insert(std::shared_ptr<Entity> entity);
		void Remove(std::shared_ptr<Entity> entity);
		void Clear();
		void DebugDraw(vec3 color);
		void CollectIntersectingObjectsWithRay(vec3 rayOrigin, vec3 rayDirection,
											   std::vector<std::shared_ptr<Entity>>& entities);

	private:
		AABB GetEntityAABB(const std::shared_ptr<Entity>& entity) const;
		void InsertRecursively(OctreeNode* node, std::shared_ptr<Entity> entity, int depth);
		void Subdivide(OctreeNode* node);
		void RedsitributeEntities(OctreeNode* node);
		std::array<AABB, MAX_ENTITIES_PER_NODE> ComputeChildAABBs(const AABB& parentAABB) const;
		void DebugDrawRecursively(OctreeNode* node, vec3 color);

	private:
		std::unique_ptr<OctreeNode> m_rootNode;
	};
}