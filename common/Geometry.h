#pragma once

#include <vector>

#include <glm/glm.hpp>

/// <summary>
/// Returns an axis aligned bounding box around a set of vertices.
/// The previous server mesh parser initialised min/max to (0, 0, 0), which
/// produced wrong dimensions for meshes that do not straddle the origin.
/// </summary>
glm::vec3 calculateMeshBoundingBox(const std::vector<glm::vec3>& vertices);

/// <summary>
/// Builds the conservative bounding box used by ObjectCore: the real mesh
/// extent plus the rule previously used by the old model loader.
/// </summary>
glm::vec3 calculateModelBoundingBox(const std::vector<glm::vec3>& vertices);