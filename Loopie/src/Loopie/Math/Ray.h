#pragma once
#include "Loopie/Math/MathTypes.h"
#include <vector>

namespace Loopie
{
	class Ray
	{
	public:
		Ray(vec3 startPoint, vec3 endPoint);
		Ray(vec3 startPoint, vec3 direction, float length);
		vec3 StartPoint() { return startPoint; }
		vec3 EndPoint() { return endPoint; }
        bool Intersects(const std::vector<vec3>& vertex, bool bounded_by_vertex, vec3& intersection_point);
	private:
		bool IntersectsLine(const vec3 lineStart, const vec3 lineEnd, vec3& intersectionPoint, bool only_hits_segment = false);
		bool IntersectsPlane(const std::vector<vec3>& vertex_array, vec3& intersectionPoint, bool only_hits_polygon = false);
	private:
		vec3 startPoint = vec3(0);
		vec3 endPoint = vec3(0);
	};
}