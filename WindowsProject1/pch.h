// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#define NOMINMAX
#define GLM_ENABLE_EXPERIMENTAL
#include <Windows.h>
#include <windowsx.h>
#include <glew.h>
#include <wglew.h>
#include <glm.hpp>
#include <gtx/rotate_vector.hpp>
#include <gtc/matrix_transform.hpp> // translate, rotate, scale, perspective, ortho
#include <gtc/type_ptr.hpp>     // glm::value_ptr (OpenGL에 포인터 넘길 때)
#include <gtx/transform.hpp>    // (선택) 번들 행렬 생성 유틸 (rotate, scale 등 함수 버전)
#include <gtc/constants.hpp>    // pi, epsilon 등
#include <inc/fmod.hpp>

//Data Structure & Algorithm
#include <list>
#include <vector>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <set>
#include <string>
#include <queue>
#include <algorithm>
#include <array>

//C++ std
#include <iostream>
#include <memory>
#include <typeindex>
#include <functional>
#include <fstream>
#include <sstream>
#include <cmath>
#include <random>
#include <numeric>
#include <limits>
#include <chrono>
#include <type_traits>

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//#ifndef DBG_NEW 
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
//#define new DBG_NEW 

//#endif
#endif


#endif //PCH_H

enum class ObjectType : int
{
	Background,
	Terrain,
	Door,
	HitEffect,
	BackgroundEffect,
	Item,
	Enemy,
	EnemyArm,
	Boss,
	BossArm,
	BossFire,
	Player,
	Laser,
	Neutral2,
	Neutral3,
	Effect,
	Blood2,
	Blood3,
	EBullet,
	PBullet,
	Light,
	UI,
	EffectOverUI,
	Count
};

enum class ItemType
{
	None,
	Knife,
	Smoke,
	Explosive,
	Count
};

enum class ItemStatus
{
	Unpicked,
	Picked,
	Thrown,
	Count
};

inline GLuint quadVAO = 0, quadVBO = 0;
inline GLuint lineVAO =0, lineVBO = 0;

constexpr double PI = 3.141592;
constexpr double SHAKE_DUR = 0.2;
constexpr double SHAKE_MAG = 6.;
constexpr int HITSTOP_DUR = 23;

struct ObjectSnapshot {
	glm::vec2 position;        
	glm::vec2 scale;           
	float rotate;              
	int spriteFrame;           

	std::string spriteName;    

	bool isDead;               
	bool dummy = true;         
};

inline float Dist(glm::vec2 l, glm::vec2 r)
{
	return l.x * l.x + l.y + l.y;
}

constexpr glm::vec2 HermiteCurve(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& t0, const glm::vec2& t1, double t)
{
	float h00 = 2 * t * t * t - 3 * t * t + 1;   // basis for p0
	float h10 = t * t * t - 2 * t * t + t;       // basis for t0
	float h01 = -2 * t * t * t + 3 * t * t;      // basis for p1
	float h11 = t * t * t - t * t;               // basis for t1

	return p0 * h00 + t0 * h10 + p1 * h01 + t1 * h11;
}

constexpr glm::vec2 HermiteDerivative(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& t0, const glm::vec2& t1, double t)
{
	float h00 = 6 * t * t - 6 * t;
	float h10 = 3 * t * t - 4 * t + 1;
	float h01 = -6 * t * t + 6 * t;
	float h11 = 3 * t * t - 2 * t;
	float dx = h00 * p0[0] + h10 * t0[0] + h01 * p1[0] + h11 * t1[0];
	float dy = h00 * p0[1] + h10 * t0[1] + h01 * p1[1] + h11 * t1[1];
	return glm::vec2(dx, dy);
}