#pragma once
class Easing {};

inline float easeInQuad(float x) { return x * x; }

inline float easeOutQuad(float x) { return 1 - (1 - x) * (1 - x); }

inline float Lerp(float start, float end, float t) { return start + (end - start) * t; }
