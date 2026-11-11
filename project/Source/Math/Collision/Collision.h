#pragma once

#include "AABB.h"
#include "Sphere.h"
#include "OBB.h"
#include "Plane.h"
#include "Triangle.h"
#include "Line.h"
#include "Ray.h"
#include "Segment.h"
#include "Capsule.h"

namespace Tuna {
    class Collision {
    public:
        Collision() = delete;
        ~Collision() = delete;

        //==============================
        // 球の当たり判定
        //==============================

        // 球同士
        static bool IsCollision(const Sphere &sphere1, const Sphere &sphere2);

        // 球と平面
        static bool IsCollision(const Sphere &sphere, const Plane &plane);

        //==============================
        // 直線・半線・線分の当たり判定
        //==============================

        // 直線と平面
        static bool IsCollision(const Line &line, const Plane &plane);

        // 半線と平面
        static bool IsCollision(const Ray &ray, const Plane &plane);

        // 線分と平面
        static bool IsCollision(const Segment &segment, const Plane &plane);

        //==============================
        // 三角形の当たり判定
        //==============================

        static bool IsCollision(const Triangle &triangle, const Line &line);
        static bool IsCollision(const Triangle &triangle, const Ray &ray);
        static bool IsCollision(const Triangle &triangle, const Segment &segment);

        //==============================
        // AABBの当たり判定
        //==============================

        static bool IsCollision(const AABB &aabb1, const AABB &aabb2);
        static bool IsCollision(const AABB &aabb, const Sphere &sphere);
        static bool IsCollision(const AABB &aabb, const Line &line);
        static bool IsCollision(const AABB &aabb, const Ray &ray);
        static bool IsCollision(const AABB &aabb, const Segment &segment);

        //==============================
        // OBBの当たり判定
        //==============================

        static bool IsCollision(const OBB &obb, const Sphere &sphere);
        static bool IsCollision(const Line &line, const OBB &obb);
        static bool IsCollision(const Ray &ray, const OBB &obb);
        static bool IsCollision(const Segment &segment, const OBB &obb);
        static bool IsCollision(const OBB &obb1, const OBB &obb2);

        //==============================
        // カプセルの当たり判定
        //==============================

        static bool IsCollision(const Capsule &capsule, const Plane &plane);

    private:
        // OBB同士の判定に使用する分離軸判定
        static bool OverlapOnAxis(
            const Vector3 &axis,
            const OBB &obb1,
            const OBB &obb2
        );
    };
}