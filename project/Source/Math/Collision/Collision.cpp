#include "Collision.h"
#include "Math/VectorFunctions.h"
#include "Math/MatrixFunctions.h"
#include <algorithm>
#include <cmath>

namespace Tuna {

    namespace {

        // 三角形の内側にあるか(点pは三角形と同一平面上)
        bool IsInsideTriangle(const Triangle &triangle, const Vector3 &normal, const Vector3 &p) {
            Vector3 v01 = triangle.vertices[1] - triangle.vertices[0];
            Vector3 v12 = triangle.vertices[2] - triangle.vertices[1];
            Vector3 v20 = triangle.vertices[0] - triangle.vertices[2];

            Vector3 cross01 = Math::Cross(v01, p - triangle.vertices[0]);
            Vector3 cross12 = Math::Cross(v12, p - triangle.vertices[1]);
            Vector3 cross20 = Math::Cross(v20, p - triangle.vertices[2]);

            return Math::Dot(cross01, normal) >= 0.0f &&
                Math::Dot(cross12, normal) >= 0.0f &&
                Math::Dot(cross20, normal) >= 0.0f;
        }

        // 三角形と「origin + t*diff」の交点tを求める(平行ならfalse)
        bool CalcTriangleHitT(const Triangle &triangle, const Vector3 &origin, const Vector3 &diff,
            Vector3 &normal, float &t) {
            Vector3 v01 = triangle.vertices[1] - triangle.vertices[0];
            Vector3 v02 = triangle.vertices[2] - triangle.vertices[0];
            normal = Math::Cross(v01, v02);

            float denom = Math::Dot(normal, diff);
            if (std::fabs(denom) < Math::kEpsilon) {
                return false;
            }

            t = Math::Dot(normal, triangle.vertices[0] - origin) / denom;
            return true;
        }

        // AABBと「origin + t*diff」のスラブ判定。tmin/tmaxを返す
        // diffの成分が0の軸はスラブ内かどうかで判定する
        bool SlabTest(const AABB &aabb, const Vector3 &origin, const Vector3 &diff,
            float &tmin, float &tmax) {
            tmin = -std::numeric_limits<float>::infinity();
            tmax = std::numeric_limits<float>::infinity();

            const float o[3] = { origin.x, origin.y, origin.z };
            const float d[3] = { diff.x, diff.y, diff.z };
            const float mn[3] = { aabb.min.x, aabb.min.y, aabb.min.z };
            const float mx[3] = { aabb.max.x, aabb.max.y, aabb.max.z };

            for (int i = 0; i < 3; ++i) {
                if (std::fabs(d[i]) < Math::kEpsilon) {
                    // この軸に平行: スラブの外なら交差しない
                    if (o[i] < mn[i] || o[i] > mx[i]) {
                        return false;
                    }
                    continue;
                }
                float tNear = (mn[i] - o[i]) / d[i];
                float tFar = (mx[i] - o[i]) / d[i];
                if (tNear > tFar) std::swap(tNear, tFar);

                tmin = (std::max)(tmin, tNear);
                tmax = (std::min)(tmax, tFar);
            }
            return true;
        }

        // OBBのワールド行列の逆行列を作る
        Matrix4x4 MakeOBBInverse(const OBB &obb) {
            Matrix4x4 worldMatrix = Math::MakeIdentity<Matrix4x4>();
            for (int i = 0; i < 3; ++i) {
                worldMatrix.m[i][0] = obb.orientations[i].x;
                worldMatrix.m[i][1] = obb.orientations[i].y;
                worldMatrix.m[i][2] = obb.orientations[i].z;
            }
            worldMatrix.m[3][0] = obb.center.x;
            worldMatrix.m[3][1] = obb.center.y;
            worldMatrix.m[3][2] = obb.center.z;
            return Math::Inverse(worldMatrix);
        }

        // OBBに対応するローカル空間のAABB
        AABB MakeLocalAABB(const OBB &obb) {
            return AABB{
                .min = { -obb.size.x, -obb.size.y, -obb.size.z },
                .max = {  obb.size.x,  obb.size.y,  obb.size.z },
            };
        }
    }

    //==============================
    // 球の当たり判定
    //==============================

    bool Collision::IsCollision(const Sphere &sphere1, const Sphere &sphere2) {
        float distance = Math::Length(sphere2.center - sphere1.center);
        return distance <= sphere1.radius + sphere2.radius;
    }

    bool Collision::IsCollision(const Sphere &sphere, const Plane &plane) {
        float distance = Math::Dot(sphere.center, plane.normal) - plane.distance;
        return -sphere.radius <= distance && distance <= sphere.radius;
    }

    //==============================
    // 直線・半線・線分と平面
    //==============================

    bool Collision::IsCollision(const Line &line, const Plane &plane) {
        float dot = Math::Dot(plane.normal, line.diff);
        // 平行でなければ必ず交わる
        return std::fabs(dot) >= Math::kEpsilon;
    }

    bool Collision::IsCollision(const Ray &ray, const Plane &plane) {
        float dot = Math::Dot(plane.normal, ray.diff);
        if (std::fabs(dot) < Math::kEpsilon) {
            return false;
        }
        float t = (plane.distance - Math::Dot(ray.origin, plane.normal)) / dot;
        return t >= 0.0f;
    }

    bool Collision::IsCollision(const Segment &segment, const Plane &plane) {
        float dot = Math::Dot(plane.normal, segment.diff);
        if (std::fabs(dot) < Math::kEpsilon) {
            return false;
        }
        float t = (plane.distance - Math::Dot(segment.origin, plane.normal)) / dot;
        return 0.0f <= t && t <= 1.0f;
    }

    //==============================
    // 三角形の当たり判定
    //==============================

    bool Collision::IsCollision(const Triangle &triangle, const Line &line) {
        Vector3 normal;
        float t;
        if (!CalcTriangleHitT(triangle, line.origin, line.diff, normal, t)) {
            return false;
        }
        Vector3 p = line.origin + line.diff * t;
        return IsInsideTriangle(triangle, normal, p);
    }

    bool Collision::IsCollision(const Triangle &triangle, const Ray &ray) {
        Vector3 normal;
        float t;
        if (!CalcTriangleHitT(triangle, ray.origin, ray.diff, normal, t)) {
            return false;
        }
        if (t < 0.0f) {
            return false;
        }
        Vector3 p = ray.origin + ray.diff * t;
        return IsInsideTriangle(triangle, normal, p);
    }

    bool Collision::IsCollision(const Triangle &triangle, const Segment &segment) {
        Vector3 normal;
        float t;
        if (!CalcTriangleHitT(triangle, segment.origin, segment.diff, normal, t)) {
            return false;
        }
        if (t < 0.0f || t > 1.0f) {
            return false;
        }
        Vector3 p = segment.origin + segment.diff * t;
        return IsInsideTriangle(triangle, normal, p);
    }

    //==============================
    // AABBの当たり判定
    //==============================

    bool Collision::IsCollision(const AABB &aabb1, const AABB &aabb2) {
        return
            (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) &&
            (aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) &&
            (aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z);
    }

    bool Collision::IsCollision(const AABB &aabb, const Sphere &sphere) {
        Vector3 closestPoint{
            std::clamp(sphere.center.x, aabb.min.x, aabb.max.x),
            std::clamp(sphere.center.y, aabb.min.y, aabb.max.y),
            std::clamp(sphere.center.z, aabb.min.z, aabb.max.z),
        };
        float distance = Math::Length(closestPoint - sphere.center);
        return distance <= sphere.radius;
    }

    bool Collision::IsCollision(const AABB &aabb, const Line &line) {
        float tmin, tmax;
        if (!SlabTest(aabb, line.origin, line.diff, tmin, tmax)) {
            return false;
        }
        return tmin <= tmax;
    }

    bool Collision::IsCollision(const AABB &aabb, const Ray &ray) {
        float tmin, tmax;
        if (!SlabTest(aabb, ray.origin, ray.diff, tmin, tmax)) {
            return false;
        }
        if (tmax < 0.0f) {
            return false;
        }
        return tmin <= tmax;
    }

    bool Collision::IsCollision(const AABB &aabb, const Segment &segment) {
        float tmin, tmax;
        if (!SlabTest(aabb, segment.origin, segment.diff, tmin, tmax)) {
            return false;
        }
        if (tmax < 0.0f || tmin > 1.0f) {
            return false;
        }
        return tmin <= tmax;
    }

    //==============================
    // OBBの当たり判定
    //==============================

    bool Collision::IsCollision(const OBB &obb, const Sphere &sphere) {
        // 球の中心をOBBのローカル空間へ
        Vector3 diff = sphere.center - obb.center;
        Vector3 localCenter = {
            Math::Dot(diff, obb.orientations[0]),
            Math::Dot(diff, obb.orientations[1]),
            Math::Dot(diff, obb.orientations[2]),
        };

        Sphere localSphere{
            .center = localCenter,
            .radius = sphere.radius
        };
        return IsCollision(MakeLocalAABB(obb), localSphere);
    }

    bool Collision::IsCollision(const Line &line, const OBB &obb) {
        Matrix4x4 inverse = MakeOBBInverse(obb);

        Vector3 localOrigin = Math::TransformVector3(line.origin, inverse);
        Vector3 localEnd = Math::TransformVector3(line.origin + line.diff, inverse);

        Line localLine{
            .origin = localOrigin,
            .diff = localEnd - localOrigin,
        };
        return IsCollision(MakeLocalAABB(obb), localLine);
    }

    bool Collision::IsCollision(const Ray &ray, const OBB &obb) {
        Matrix4x4 inverse = MakeOBBInverse(obb);

        Vector3 localOrigin = Math::TransformVector3(ray.origin, inverse);
        Vector3 localEnd = Math::TransformVector3(ray.origin + ray.diff, inverse);

        Ray localRay{
            .origin = localOrigin,
            .diff = localEnd - localOrigin,
        };
        return IsCollision(MakeLocalAABB(obb), localRay);
    }

    bool Collision::IsCollision(const Segment &segment, const OBB &obb) {
        Matrix4x4 inverse = MakeOBBInverse(obb);

        Vector3 localOrigin = Math::TransformVector3(segment.origin, inverse);
        Vector3 localEnd = Math::TransformVector3(segment.origin + segment.diff, inverse);

        Segment localSegment{
            .origin = localOrigin,
            .diff = localEnd - localOrigin,
        };
        return IsCollision(MakeLocalAABB(obb), localSegment);
    }

    // 分離軸判定(private)
    bool Collision::OverlapOnAxis(const Vector3 &axis, const OBB &obb1, const OBB &obb2) {
        float r1 =
            obb1.size.x * std::fabs(Math::Dot(obb1.orientations[0], axis)) +
            obb1.size.y * std::fabs(Math::Dot(obb1.orientations[1], axis)) +
            obb1.size.z * std::fabs(Math::Dot(obb1.orientations[2], axis));

        float r2 =
            obb2.size.x * std::fabs(Math::Dot(obb2.orientations[0], axis)) +
            obb2.size.y * std::fabs(Math::Dot(obb2.orientations[1], axis)) +
            obb2.size.z * std::fabs(Math::Dot(obb2.orientations[2], axis));

        float c1 = Math::Dot(obb1.center, axis);
        float c2 = Math::Dot(obb2.center, axis);

        float min1 = c1 - r1, max1 = c1 + r1;
        float min2 = c2 - r2, max2 = c2 + r2;

        float sumSpan = (max1 - min1) + (max2 - min2);
        float longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

        return longSpan < sumSpan;
    }

    bool Collision::IsCollision(const OBB &obb1, const OBB &obb2) {
        Vector3 axes[15]{};

        // 面法線
        for (int i = 0; i < 3; ++i) {
            axes[i] = obb1.orientations[i];
            axes[3 + i] = obb2.orientations[i];
        }

        // 辺同士のクロス積
        int idx = 6;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                axes[idx++] = Math::Cross(obb1.orientations[i], obb2.orientations[j]);
            }
        }

        for (int i = 0; i < 15; ++i) {
            // 辺が平行だとクロス積がゼロになるのでスキップ
            if (Math::LengthSquare(axes[i]) < Math::kEpsilon) continue;

            Vector3 axis = Math::Normalize(axes[i]);
            if (!OverlapOnAxis(axis, obb1, obb2)) {
                return false;
            }
        }
        return true;
    }

    //==============================
    // カプセルの当たり判定
    //==============================

    bool Collision::IsCollision(const Capsule &capsule, const Plane &plane) {
        Vector3 start = capsule.segment.origin;
        Vector3 end = start + capsule.segment.diff;

        float startDistance = Math::Dot(start, plane.normal) - plane.distance;
        float endDistance = Math::Dot(end, plane.normal) - plane.distance;

        // 端点が半径以内なら衝突
        if (std::fabs(startDistance) <= capsule.radius ||
            std::fabs(endDistance) <= capsule.radius) {
            return true;
        }

        // 平面を跨いでいれば衝突
        return (startDistance > 0.0f && endDistance < 0.0f) ||
            (startDistance < 0.0f && endDistance > 0.0f);
    }
}