// Planes are in normal+distance form
class Plane
{
    Vector3 Normal;
    float Distance;

    float RayDistance(Vector3 origin, Vector3 direction); // Gives the distance along the ray (+direction) where intersection with the plane occurs
}

interface AABB
{
    Vector3 Center; // Center point of the bounding box
    Plane[] MidPlanes = new[] { // These are the planes which split a bounding box in half in each direction
            new Plane(Vector3.UnitZ, Center.Z),
            new Plane(Vector3.UnitY, Center.Y),
            new Plane(Vector3.UnitX, Center.X)
        };
    bool Contains(Vector3 point); // True if the point lies within the bounding box
}

class Octree
{
    bool Empty; // Whether this octant contains nothing
    Octree[] Nodes; // This is an array of 8 child octants, laid out where 0 bits correspond to the negative side of the plane, 1 bits are positive
                    // X == 4, Y == 2, Z == 1
    AABB BoundingBox; // Axis-aligned bounding box for this octant
    Mesh Leaf; // Triangles for a given leaf octant

    (Triangle, Vector3)? RayIntersection(Vector3 _origin, Vector3 direction)
    {
        if (Empty) return null;
        var origin = _origin;
        if (Leaf != null) return Leaf.RayIntersection(origin, direction);

        var planes = BoundingBox.MidPlanes;
        var side = (
            X: Vector3.Dot(origin, planes[2].Normal) - planes[2].Distance >= 0,
            Y: Vector3.Dot(origin, planes[1].Normal) - planes[1].Distance >= 0,
            Z: Vector3.Dot(origin, planes[0].Normal) - planes[0].Distance >= 0
        );
        var xDist = side.X == direction.X < 0
            ? planes[2].RayDistance(origin, direction)
            : float.PositiveInfinity;
        var yDist = side.Y == direction.Y < 0
            ? planes[1].RayDistance(origin, direction)
            : float.PositiveInfinity;
        var zDist = side.Z == direction.Z < 0
            ? planes[0].RayDistance(origin, direction)
            : float.PositiveInfinity;
        for (var i = 0; i < 3; ++i)
        {
            var idx = (side.Z ? 1 : 0) | (side.Y ? 2 : 0) | (side.X ? 4 : 0);
            var ret = Nodes[idx].RayIntersection(origin, direction);

            if (ret != null) return ret;

            var minDist = MathF.Min(MathF.Min(xDist, yDist), zDist);
            if (float.IsInfinity(minDist)) return null;

            origin = _origin + direction * minDist;

            if (!BoundingBox.Contains(origin))
                return null;

            if (minDist == xDist)
            {
                side.X = !side.X; xDist = float.PositiveInfinity;
            }
            else if (minDist == yDist)
            {
                side.Y = !side.Y; yDist = float.PositiveInfinity;
            }
            else if (minDist == zDist)
            {
                side.Z = !side.Z; zDist = float.PositiveInfinity;
            }
        }

        return null;
    }
}