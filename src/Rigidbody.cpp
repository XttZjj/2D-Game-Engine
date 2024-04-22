#include "Rigidbody.h"
#include "Actor.h"

Rigidbody::Rigidbody() : type("Rigidbody"), key("???"), actor(nullptr), enabled(true) {}

void Rigidbody::Ready() {
    if (!world_initialized) {
        world = new b2World(b2Vec2(0.0f, 9.8f));
        world->SetContactListener(&detector);

        world_initialized = true;
    }
    b2BodyDef body_def;
    if (body_type == "dynamic") {
        body_def.type = b2_dynamicBody;
    }
    else if (body_type == "kinematic")
    {
        body_def.type = b2_kinematicBody;
    }
    else if (body_type == "static") {
        body_def.type = b2_staticBody;
    }

    body_def.position = b2Vec2(x, y);
    body_def.bullet = precise;
    body_def.angularDamping = angular_friction;
    body_def.gravityScale = gravity_scale;
    body_def.angle = rotation * (b2_pi / 180.0f);

    body = world->CreateBody(&body_def);
    if (!has_collider && !has_trigger) {
        b2PolygonShape my_shape;
        my_shape.SetAsBox(width * 0.5f, height * 0.5f);

        b2FixtureDef fixture;
        fixture.shape = &my_shape;
        fixture.density = density;
        fixture.isSensor = true;
        body->CreateFixture(&fixture);
    }
    if (has_collider) {
        b2Shape* my_shape = nullptr;
        if (collider_type == "box") {
            b2PolygonShape* shape = new b2PolygonShape();
            shape->SetAsBox(width * 0.5f, height * 0.5f);
            my_shape = shape;
        }
        else if (collider_type == "circle") {
            b2CircleShape* shape = new b2CircleShape();
            shape->m_radius = radius;
            my_shape = shape;
        }
        b2FixtureDef fixture;
        fixture.shape = my_shape;
        fixture.density = density;
        fixture.isSensor = false;
        fixture.restitution = bounciness;
        fixture.friction = friction;
        fixture.userData.pointer = reinterpret_cast<uintptr_t> (actor);
        body->CreateFixture(&fixture);
    }
    if (has_trigger) {
        b2Shape* my_shape = nullptr;
        if (trigger_type == "box") {
            b2PolygonShape* shape = new b2PolygonShape();
            shape->SetAsBox(trigger_width * 0.5f, trigger_height * 0.5f);
            my_shape = shape;
        }
        else if (trigger_type == "circle") {
            b2CircleShape* shape = new b2CircleShape();
            shape->m_radius = trigger_radius;
            my_shape = shape;
        }

        b2FixtureDef fixture;
        fixture.shape = my_shape;
        fixture.density = density;
        fixture.isSensor = true;
        fixture.restitution = bounciness;
        fixture.friction = friction;
        fixture.userData.pointer = reinterpret_cast<uintptr_t> (actor);
        body->CreateFixture(&fixture);
    }
}

void Rigidbody::WorldUpdate() {
    if (world_initialized) {
        world->Step(1.0f / 60.0f, 8, 3);
    }
}

b2Vec2 Rigidbody::GetPosition() {
    if (body == nullptr) {
        return b2Vec2(x, y);
    }
    return body->GetPosition();
}

float Rigidbody::GetRotation() {
    float angleRadians = body->GetAngle();
    float angleDegrees = angleRadians * (180.0f / b2_pi);
    return angleDegrees;
}

void Rigidbody::AddForce(b2Vec2 dir) {
    body->ApplyForceToCenter(dir, true);
}

void Rigidbody::SetVelocity(b2Vec2 vel) {
    body->SetLinearVelocity(vel);
}

void Rigidbody::SetPosition(b2Vec2 loc) {
    if (body == nullptr) {
        x = loc.x;
        y = loc.y;
    }
    else {
        float rotation = body->GetAngle();
        body->SetTransform(loc, rotation);
    }
}

void Rigidbody::SetRotation(float degrees_clockwise) {
    body->SetTransform(body->GetPosition(), degrees_clockwise * (b2_pi / 180.0f));
}

void Rigidbody::SetAngularVelocity(float degrees_clockwise) {
    body->SetAngularVelocity(degrees_clockwise * (b2_pi / 180.0f));
}

void Rigidbody::SetGravityScale(float gravityScale) {
    body->SetGravityScale(gravityScale);
}

void Rigidbody::SetUpDirection(b2Vec2 direction) {
    direction.Normalize();
    float new_angle_radians = glm::atan(direction.x, -direction.y);
    body->SetTransform(body->GetPosition(), new_angle_radians);
}

void Rigidbody::SetRightDirection(b2Vec2 direction) {
    direction.Normalize();
    float new_angle_radians = glm::atan(direction.x, -direction.y) - (b2_pi / 2.0f);
    body->SetTransform(body->GetPosition(), new_angle_radians);
}

b2Vec2 Rigidbody::GetVelocity() {
    return body->GetLinearVelocity();
}

float Rigidbody::GetAngularVelocity() {
    return body->GetAngularVelocity() * (180.0f / b2_pi);
}

float Rigidbody::GetGravityScale() {
    return body->GetGravityScale();
}

b2Vec2 Rigidbody::GetUpDirection() {
    float angle = body->GetAngle();
    float upX = glm::sin(angle);
    float upY = -glm::cos(angle);
    b2Vec2 upVec = b2Vec2(upX, upY);
    upVec.Normalize();

    return upVec;
}

b2Vec2 Rigidbody::GetRightDirection() {
    float angle = body->GetAngle();
    float rightX = glm::cos(angle);
    float rightY = glm::sin(angle);
    b2Vec2 rightVec = b2Vec2(rightX, rightY);
    rightVec.Normalize();

    return rightVec;
}

void Rigidbody::CollisionDetector::BeginContact(b2Contact* contact) {
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();
    Actor* actorA = reinterpret_cast<Actor*>(fixtureA->GetUserData().pointer);
    Actor* actorB = reinterpret_cast<Actor*>(fixtureB->GetUserData().pointer);
    if (actorA == nullptr || actorB == nullptr) {
        return;
    }
    b2WorldManifold world_mainfold;
    contact->GetWorldManifold(&world_mainfold);
    b2Vec2 relative_velocity = fixtureA->GetBody()->GetLinearVelocity() - fixtureB->GetBody()->GetLinearVelocity();
    Collision collisionA;
    Collision collisionB;
    collisionA.initialize(actorB, world_mainfold.points[0], relative_velocity, world_mainfold.normal);
    collisionB.initialize(actorA, world_mainfold.points[0], relative_velocity, world_mainfold.normal);
    if (!fixtureA->IsSensor() && !fixtureB->IsSensor()) {
        actorA->OnCollisionEnter(collisionA);
        actorB->OnCollisionEnter(collisionB);
    }
    else if (fixtureA->IsSensor() && fixtureB->IsSensor()) {
        b2Vec2 point(-999.0f, -999.0f);
        collisionA.point = point;
        collisionA.normal = point;
        collisionB.point = point;
        collisionB.normal = point;
        actorA->OnTriggerEnter(collisionA);
        actorB->OnTriggerEnter(collisionB);
    }
}

void Rigidbody::CollisionDetector::EndContact(b2Contact* contact) {
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();
    Actor* actorA = reinterpret_cast<Actor*>(fixtureA->GetUserData().pointer);
    Actor* actorB = reinterpret_cast<Actor*>(fixtureB->GetUserData().pointer);
    if (actorA == nullptr || actorB == nullptr) {
        return;
    }
    b2WorldManifold world_mainfold;
    contact->GetWorldManifold(&world_mainfold);
    b2Vec2 relative_velocity = fixtureA->GetBody()->GetLinearVelocity() - fixtureB->GetBody()->GetLinearVelocity();
    Collision collisionA;
    Collision collisionB;
    b2Vec2 point(-999.0f, -999.0f);
    collisionA.initialize(actorB, point, relative_velocity, point);
    collisionB.initialize(actorA, point, relative_velocity, point);
    if (!fixtureA->IsSensor() && !fixtureB->IsSensor()) {
        actorA->OnCollisionExit(collisionA);
        actorB->OnCollisionExit(collisionB);
    }
    else if (fixtureA->IsSensor() && fixtureB->IsSensor()) {
        actorA->OnTriggerExit(collisionA);
        actorB->OnTriggerExit(collisionB);
    }
}

void Rigidbody::OnDestroy() {
    world->DestroyBody(body);
}

float Rigidbody::GetWidth() {
    return width;
}

float Rigidbody::GetHeight() {
    return height;
}
