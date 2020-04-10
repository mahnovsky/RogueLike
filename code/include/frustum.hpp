#pragma once

#include "glm/glm.hpp"

#include "octree.h"

struct Plane3D
{
	glm::vec3 normal;
	//Distance from the coordinate system origin to the plane along the normal direction
	float  distance = 0; 
};

struct ViewFrustum
{
	Plane3D left_plane; 
	Plane3D right_plane;
	Plane3D bottom_plane;
	Plane3D top_plane; 
	Plane3D near_plane;
	Plane3D far_plane;
};

// For OpenGL, matrix is still considered row-major. The only difference is that
// near clip plane is at -1, not 0.
inline void extract_view_frustum_planes_from_matrix(const glm::mat4& matrix, ViewFrustum& frustum, bool bIsOpenGL)
{
	// For more details, see Gribb G., Hartmann K., "Fast Extraction of Viewing frustum Planes from the
	// World-View-Projection matrix" (the paper is available at
	// http://gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf)

	// Left clipping plane


	//glm::mat4 matrix = glm::transpose(matrix1);
	frustum.left_plane.normal.x = matrix[0][3] + matrix[0][0];
	frustum.left_plane.normal.y = matrix[1][3] + matrix[1][0];
	frustum.left_plane.normal.z = matrix[2][3] + matrix[2][0];
	frustum.left_plane.distance = matrix[3][3] + matrix[3][0];

	// Right clipping plane
	frustum.right_plane.normal.x = matrix[0][3] - matrix[0][0];
	frustum.right_plane.normal.y = matrix[1][3] - matrix[1][0];
	frustum.right_plane.normal.z = matrix[2][3] - matrix[2][0];
	frustum.right_plane.distance = matrix[3][3] - matrix[3][0];

	// Top clipping plane
	frustum.top_plane.normal.x = matrix[0][3] - matrix[0][1];
	frustum.top_plane.normal.y = matrix[1][3] - matrix[1][1];
	frustum.top_plane.normal.z = matrix[2][3] - matrix[2][1];
	frustum.top_plane.distance = matrix[3][3] - matrix[3][1];

	// Bottom clipping plane
	frustum.bottom_plane.normal.x = matrix[0][3] + matrix[0][1];
	frustum.bottom_plane.normal.y = matrix[1][3] + matrix[1][1];
	frustum.bottom_plane.normal.z = matrix[2][3] + matrix[2][1];
	frustum.bottom_plane.distance = matrix[3][3] + matrix[3][1];

	// Near clipping plane
	if (bIsOpenGL)
	{
		// -w <= z <= w
		frustum.near_plane.normal.x = matrix[0][3] + matrix[0][2];
		frustum.near_plane.normal.y = matrix[1][3] + matrix[1][2];
		frustum.near_plane.normal.z = matrix[2][3] + matrix[2][2];
		frustum.near_plane.distance = matrix[3][3] + matrix[3][2];
	}
	else
	{
		// 0 <= z <= w
		frustum.near_plane.normal.x = matrix[0][2];
		frustum.near_plane.normal.y = matrix[1][2];
		frustum.near_plane.normal.z = matrix[2][2];
		frustum.near_plane.distance = matrix[3][2];
	}

	// Far clipping plane
	frustum.far_plane.normal.x = matrix[0][3] - matrix[0][2];
	frustum.far_plane.normal.y = matrix[1][3] - matrix[1][2];
	frustum.far_plane.normal.z = matrix[2][3] - matrix[2][2];
	frustum.far_plane.distance = matrix[3][3] - matrix[3][2];
	/*
	frustum.far_plane.normal = glm::normalize(frustum.far_plane.normal);
	frustum.near_plane.normal = glm::normalize(frustum.near_plane.normal);
	frustum.top_plane.normal = glm::normalize(frustum.top_plane.normal);
	frustum.bottom_plane.normal = glm::normalize(frustum.bottom_plane.normal);
	frustum.left_plane.normal = glm::normalize(frustum.left_plane.normal);
	frustum.right_plane.normal = glm::normalize(frustum.right_plane.normal);
	*/
}

enum class BoxVisibility
{
	//  Bounding box is guaranteed to be outside of the view frustum
	//                 .
	//             . ' |
	//         . '     |
	//       |         |
	//         .       |
	//       ___ ' .   |
	//      |   |    ' .
	//      |___|
	//
	Invisible,

	//  Bounding box intersects the frustum
	//                 .
	//             . ' |
	//         . '     |
	//       |         |
	//        _.__     |
	//       |   '|.   |
	//       |____|  ' .
	//
	Intersecting,

	//  Bounding box is fully inside the view frustum
	//                 .
	//             . ' |
	//         . '___  |
	//       |   |   | |
	//         . |___| |
	//           ' .   |
	//               ' .
	//
	FullyVisible
};

inline BoxVisibility get_box_visibility_against_plane(const Plane3D& plane, const Box& box)
{
	const glm::vec3 normal = plane.normal;

	glm::vec3 max_point //
	{
		(normal.x > 0) ? box.max.x : box.min.x,
		(normal.y > 0) ? box.max.y : box.min.y,
		(normal.z > 0) ? box.max.z : box.min.z
	};
	float DMax = glm::dot(max_point, normal) + plane.distance;
	if (DMax < 0)
		return BoxVisibility::Invisible;

	glm::vec3 min_point //
	{
		(normal.x > 0) ? box.min.x : box.max.x,
		(normal.y > 0) ? box.min.y : box.max.y,
		(normal.z > 0) ? box.min.z : box.max.z //
	};
	float DMin = glm::dot(min_point, normal) + plane.distance;
	if (DMin > 0)
		return BoxVisibility::FullyVisible;

	return BoxVisibility::Intersecting;
}

inline bool is_sphere_visible_against_plane(const Plane3D& plane, const Sphere& sphere)
{
	const glm::vec3 normal = plane.normal;

	glm::vec3 max_point //
	{
		(normal.x > 0) ? sphere.radius : -sphere.radius,
		(normal.y > 0) ? sphere.radius : -sphere.radius,
		(normal.z > 0) ? sphere.radius : -sphere.radius,
	};
	
	float DMax = glm::dot(sphere.pos + max_point, normal) + plane.distance;
	if (DMax < 0)
		return false;

	return true;
}

inline bool is_sphere_visible(const ViewFrustum& view_frustum, const Sphere& sphere)
{
	const Plane3D* pPlanes = reinterpret_cast<const Plane3D*>(&view_frustum);

	bool is_visible = true;
	for (int view_frustum_plane = 0; view_frustum_plane < 6; view_frustum_plane++)
	{
		const Plane3D& curr_plane = pPlanes[view_frustum_plane];

		auto visibility_against_plane = is_sphere_visible_against_plane(curr_plane, sphere);

		is_visible = is_visible && visibility_against_plane;
		if (!is_visible)
			return false;
	}

	return is_visible;
}

// Tests if bounding box is visible by the camera
inline BoxVisibility get_box_visibility(const ViewFrustum& view_frustum, const Box& box)
{
	const Plane3D* pPlanes = reinterpret_cast<const Plane3D*>(&view_frustum);

	int num_planes_inside = 0;
	int total_planes = 0;

	for (int view_frustum_plane = 0; view_frustum_plane < 6; view_frustum_plane++)
	{
		const Plane3D& curr_plane = pPlanes[view_frustum_plane];

		auto visibility_against_plane = get_box_visibility_against_plane(curr_plane, box);

		// If bounding box is "behind" one of the planes, it is definitely invisible
		if (visibility_against_plane == BoxVisibility::Invisible)
			return BoxVisibility::Invisible;

		// Count total number of planes the bound box is inside
		if (visibility_against_plane == BoxVisibility::FullyVisible)
			++num_planes_inside;

		++total_planes;
	}

	return (num_planes_inside == total_planes) ? BoxVisibility::FullyVisible : BoxVisibility::Intersecting;
}

// Retuns the minimum bounding sphere of a view frustum
inline void get_frustum_minimum_bounding_sphere(
	float   proj_00,   // cot(HorzFOV / 2)
	float   proj_11,   // cot(VertFOV / 2) == proj_00 / AspectRatio
	float   near_plane, // Near clip plane
	float   far_plane,  // Far clip plane
	glm::vec3& center, // Sphere center == (0, 0, c)
	float& radius  // Sphere radius
)
{
	// https://lxjk.github.io/2017/04/15/Calculate-Minimal-Bounding-Sphere-of-Frustum.html
	assert(far_plane >= near_plane);
	auto k2 = 1.f / (proj_00 * proj_00) + 1.f / (proj_11 * proj_11);
	if (k2 > (far_plane - near_plane) / (far_plane + near_plane))
	{
		center = glm::vec3(0, 0, far_plane);
		radius = far_plane * std::sqrt(k2);
	}
	else
	{
		center = glm::vec3(0, 0, 0.5f * (far_plane + near_plane) * (1 + k2));
		radius = 0.5f * std::sqrt((far_plane - near_plane) * (far_plane - near_plane) + 2 * (far_plane * far_plane + near_plane * near_plane) * k2 + (far_plane + near_plane) * (far_plane + near_plane) * k2 * k2);
	}
}