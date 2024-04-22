#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <string>
#include "box2d/box2d.h"
#include "../Third Party/glm/glm/glm.hpp"

class Actor;

class Rigidbody {
public:

	class CollisionDetector : public b2ContactListener {
		void BeginContact(b2Contact* contact);
		void EndContact(b2Contact* contact);
	};

	class Collision {
	public:
		Actor* other;
		b2Vec2 point;
		b2Vec2 relative_velocity;
		b2Vec2 normal;

		void initialize(Actor* otherActor, const b2Vec2& collisionPoint, const b2Vec2& relVelocity, const b2Vec2& collisionNormal) {
			other = otherActor;
			point = collisionPoint;
			relative_velocity = relVelocity;
			normal = collisionNormal;
		}
	};

	static inline bool world_initialized = false;

	static inline b2World* world;

	Rigidbody();

	std::string type;
	
	std::string key;
	
	Actor* actor;
	
	bool enabled;

	float x = 0.0f;

	float y = 0.0f;

	float width = 1.0f;

	float height = 1.0f;

	float trigger_width = 1.0f;

	float trigger_height = 1.0f;

	float trigger_radius = 0.5f;

	std::string body_type = "dynamic";

	bool precise = true;

	float gravity_scale = 1.0f;

	float density = 1.0f;

	float angular_friction = 0.3f;

	float rotation = 0.0f;

	bool has_collider = true;

	bool has_trigger = true;

	std::string collider_type = "box";

	std::string trigger_type = "box";

	float radius = 0.5f;

	float friction = 0.3f;

	float bounciness = 0.3f;

	b2Vec2 GetPosition();

	float GetRotation();

	float GetWidth();

	float GetHeight();

	static void WorldUpdate();

	void Ready();

	void OnDestroy();

	void AddForce(b2Vec2 dir);

	void SetVelocity(b2Vec2 vel);

	void SetPosition(b2Vec2 loc);

	void SetRotation(float degrees_clockwise);

	void SetAngularVelocity(float degrees_clockwise);

	void SetGravityScale(float gravityScale);

	void SetUpDirection(b2Vec2 direction);

	void SetRightDirection(b2Vec2 direction);

	b2Vec2 GetVelocity();

	float GetAngularVelocity();

	float GetGravityScale();

	b2Vec2 GetUpDirection();

	b2Vec2 GetRightDirection();

	static inline CollisionDetector detector;

private:
	b2Body* body = NULL;
};


#endif // RIGIDBODY_H