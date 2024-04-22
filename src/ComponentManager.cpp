#include "ComponentManager.h"
#include "Helper.h"
#include <iostream>
#include <filesystem>
#include <thread>

void ComponentManager::Initialize() {
	InitializeState();
	InitializeFunctions();
	InitialiazeComponents();
}

void ComponentManager::InitializeState() {
	lua_state = luaL_newstate();
	luaL_openlibs(lua_state);
}

void ComponentManager::Print(const std::string& message) {
	std::cout << message << std::endl;
}

void ComponentManager::PrintError(const std::string& message) {
	std::cerr << message << std::endl;
}

void ComponentManager::Quit() {
	exit(0);
}

void ComponentManager::Sleep(int milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

int ComponentManager::GetFrame() {
	return Helper::GetFrameNumber();
}

void ComponentManager::OpenURL(std::string url) {
	#ifdef _WIN32
		std::string command = "start " + url;
	#elif __APPLE__
		std::string command = "open " + url;
	#else // Linux
		std::string command = "xdg-open " + url;
	#endif

	std::system(command.c_str());
}

void ComponentManager::InitializeFunctions() {
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Debug")
		.addFunction("Log", Print)
		.addFunction("LogError", PrintError)
		.endNamespace();

	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Scene")
		.addFunction("Load", Load)
		.addFunction("GetCurrent", GetCurrent)
		.addFunction("DontDestroy", &Actor::DontDestroy)
		.endNamespace();

	luabridge::getGlobalNamespace(lua_state)
		.beginClass<Actor>("Actor")
		.addFunction("GetName", &Actor::GetName)
		.addFunction("GetID", &Actor::GetID)
		.addFunction("GetComponentByKey", &Actor::GetComponentByKey)
		.addFunction("GetComponent", &Actor::GetComponent)
		.addFunction("GetComponents", &Actor::GetComponents)
		.addFunction("AddComponent", &Actor::AddComponent)
		.addFunction("RemoveComponent", &Actor::RemoveComponent)
		.endClass();

	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Actor")
		.addFunction("Find", &Actor::Find)
		.addFunction("FindAll", &Actor::FindAll)
		.addFunction("Instantiate", &Actor::Instantiate)
		.addFunction("Destroy", &Actor::Destroy)
		.endNamespace();

	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Text")
		.addFunction("Draw", &Renderer::TextDraw)
		.endNamespace();

	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Image")
		.addFunction("DrawUI", &Renderer::UIDraw)
		.addFunction("DrawUIEx", &Renderer::UIDrawEx)
		.addFunction("Draw", &Renderer::ImageDraw)
		.addFunction("DrawEx", &Renderer::ImageDrawEX)
		.addFunction("DrawPixel", &Renderer::PixelDraw)
		.endNamespace();

	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Camera")
		.addFunction("SetPosition", &Renderer::SetPosition)
		.addFunction("GetPositionX", &Renderer::GetPositionX)
		.addFunction("GetPositionY", &Renderer::GetPositionY)
		.addFunction("SetZoom", &Renderer::SetZoom)
		.addFunction("GetZoom", &Renderer::GetZoom)
		.endNamespace();

	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Audio")
		.addFunction("Play", &AudioDB::RenderAudio)
		.addFunction("Halt", &AudioDB::Halt)
		.addFunction("SetVolume", &AudioDB::SetVolume)
		.endNamespace();

	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Application")
		.addFunction("Quit", Quit)
		.addFunction("Sleep", Sleep)
		.addFunction("GetFrame", GetFrame)
		.addFunction("OpenURL", OpenURL)
		.endNamespace();

	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Input")
		.addFunction("GetKey", &Input::GetKey)
		.addFunction("GetKeyDown", &Input::GetKeyDown)
		.addFunction("GetKeyUp", &Input::GetKeyUp)
		.addFunction("GetMousePosition", &Input::GetMousePosition)
		.addFunction("GetMouseButton", &Input::GetMouseButton)
		.addFunction("GetMouseButtonDown", &Input::GetMouseButtonDown)
		.addFunction("GetMouseButtonUp", &Input::GetMouseButtonUp)
		.addFunction("GetMouseScrollDelta", &Input::GetMouseScrollDelta)
		.addFunction("GetControllerButton", &Input::GetControllerButton)
		.addFunction("GetControllerButtonDown", &Input::GetControllerButtonDown)
		.addFunction("GetControllerButtonUp", &Input::GetControllerButtonUp)
		.addFunction("GetLeftXAxis", &Input::GetLeftXAxis)
		.addFunction("GetLeftYAxis", &Input::GetLeftYAxis)
		.addFunction("GetRightXAxis", &Input::GetRightXAxis)
		.addFunction("GetRightYAxis", &Input::GetRightYAxis)
		.addFunction("KeyPress", &Input::KeyPress)
		.addFunction("KeyPressEnd", &Input::KeyPressEnd)
		.addFunction("GetKeyName", &Input::GetKeyName)
		.endNamespace();

	luabridge::getGlobalNamespace(lua_state)
		.beginClass<glm::vec2>("vec2")
		.addProperty("x", &glm::vec2::x)
		.addProperty("y", &glm::vec2::y)
		.endClass();

	luabridge::getGlobalNamespace(lua_state)
		.beginClass<b2Vec2>("Vector2")
		.addConstructor<void(*) (float, float)>()
		.addProperty("x", &b2Vec2::x)
		.addProperty("y", &b2Vec2::y)
		.addFunction("Normalize", &b2Vec2::Normalize)
		.addFunction("Length", &b2Vec2::Length)
		.addFunction("__add", &b2Vec2::operator_add)
		.addFunction("__sub", &b2Vec2::operator_sub)
		.addFunction("__mul", &b2Vec2::operator_mul)
		.addStaticFunction("Distance", &b2Distance)
		.addStaticFunction("Dot", static_cast<float (*)(const b2Vec2&, const b2Vec2&)>(&b2Dot))
		.endClass();

	luabridge::getGlobalNamespace(lua_state)
		.beginClass<Rigidbody>("Rigidbody")
		.addData("enabled", &Rigidbody::enabled)
		.addData("key", &Rigidbody::key)
		.addData("type", &Rigidbody::type)
		.addData("actor", &Rigidbody::actor)
		.addProperty("x", &Rigidbody::x)
		.addProperty("y", &Rigidbody::y)
		.addProperty("width", &Rigidbody::width)
		.addProperty("height", &Rigidbody::height)
		.addProperty("trigger_type", &Rigidbody::trigger_type)
		.addProperty("trigger_width", &Rigidbody::trigger_width)
		.addProperty("trigger_height", &Rigidbody::trigger_height)
		.addProperty("trigger_radius", &Rigidbody::trigger_radius)
		.addProperty("collider_type", &Rigidbody::collider_type)
		.addProperty("radius", &Rigidbody::radius)
		.addProperty("friction", &Rigidbody::friction)
		.addProperty("bounciness", &Rigidbody::bounciness)
		.addProperty("body_type", &Rigidbody::body_type)
		.addProperty("precise", &Rigidbody::precise)
		.addProperty("gravity_scale", &Rigidbody::gravity_scale)
		.addProperty("density", &Rigidbody::density)
		.addProperty("angular_friction", &Rigidbody::angular_friction)
		.addProperty("rotation", &Rigidbody::rotation)
		.addProperty("has_collider", &Rigidbody::has_collider)
		.addProperty("has_trigger", &Rigidbody::has_trigger)
		.addFunction("Ready", &Rigidbody::Ready)
		.addFunction("OnDestroy", &Rigidbody::OnDestroy)
		.addFunction("GetPosition", &Rigidbody::GetPosition)
		.addFunction("GetRotation", &Rigidbody::GetRotation)
		.addFunction("GetWidth", &Rigidbody::GetWidth)
		.addFunction("GetHeight", &Rigidbody::GetHeight)
		.addFunction("AddForce", &Rigidbody::AddForce)
		.addFunction("SetVelocity", &Rigidbody::SetVelocity)
		.addFunction("SetPosition", &Rigidbody::SetPosition)
		.addFunction("SetRotation", &Rigidbody::SetRotation)
		.addFunction("SetAngularVelocity", &Rigidbody::SetAngularVelocity)
		.addFunction("SetGravityScale", &Rigidbody::SetGravityScale)
		.addFunction("SetUpDirection", &Rigidbody::SetUpDirection)
		.addFunction("SetRightDirection", &Rigidbody::SetRightDirection)
		.addFunction("GetVelocity", &Rigidbody::GetVelocity)
		.addFunction("GetAngularVelocity", &Rigidbody::GetAngularVelocity)
		.addFunction("GetGravityScale", &Rigidbody::GetGravityScale)
		.addFunction("GetUpDirection", &Rigidbody::GetUpDirection)
		.addFunction("GetRightDirection", &Rigidbody::GetRightDirection)
		.endClass();

	luabridge::getGlobalNamespace(lua_state)
		.beginClass<Rigidbody::Collision>("collision")
		.addProperty("other", &Rigidbody::Collision::other)
		.addProperty("point", &Rigidbody::Collision::point)
		.addProperty("relative_velocity", &Rigidbody::Collision::relative_velocity)
		.addProperty("normal", &Rigidbody::Collision::normal)
		.endClass();

	luabridge::getGlobalNamespace(lua_state)
		.beginClass<Raycast::HitResult>("HitResult")
		.addProperty("actor", &Raycast::HitResult::actor)
		.addProperty("point", &Raycast::HitResult::point)
		.addProperty("is_trigger", &Raycast::HitResult::is_trigger)
		.addProperty("normal", &Raycast::HitResult::normal)
		.endClass();

	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Physics")
		.addFunction("Raycast", &Raycast::PhsicsRaycast)
		.addFunction("RaycastAll", &Raycast::PhysicsRaycastAll)
		.endNamespace();

	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Event")
		.addFunction("Publish", &Event::Publish)
		.addFunction("Subscribe", &Event::Subscribe)
		.addFunction("Unsubscribe", &Event::Unsubscribe)
		.endNamespace();
}

void ComponentManager::InitialiazeComponents() {
	std::filesystem::path directoryPath("resources/component_types/");

	if (std::filesystem::exists(directoryPath) && std::filesystem::is_directory(directoryPath)) {
		for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
			if (luaL_dofile(lua_state, entry.path().string().c_str()) != LUA_OK) {
				std::cout << "problem with lua file " << entry.path().stem().string();
				exit(0);
			}
			
			std::string componentName = entry.path().stem().string();
			component_tables.insert({ componentName, std::make_shared<luabridge::LuaRef>(luabridge::getGlobal(lua_state, componentName.c_str())) });
		}
	}

}

std::shared_ptr<luabridge::LuaRef> ComponentManager::GetComponent(const std::string& componentName) {
	auto it = component_tables.find(componentName);
	if (it != component_tables.end()) {
		return it->second;
	}
	else {
		std::cout << "error: failed to locate component " << componentName;
		exit(0);
	}
	return nullptr;
}

void ComponentManager::EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table) {
	luabridge::LuaRef new_metatable = luabridge::newTable(lua_state);
	new_metatable["__index"] = parent_table;
	instance_table.push(lua_state);
	new_metatable.push(lua_state);
	lua_setmetatable(lua_state, -2);
	lua_pop(lua_state, 1);
}

luabridge::LuaRef ComponentManager::GetLuaState() {
	return luaL_newstate();
}

void ComponentManager::Load(std::string scene_name) {
	nextSceneName = scene_name;
	changeScene = true;
}

std::string ComponentManager::GetCurrent() {
	return currentScene;
}

luabridge::LuaRef ComponentManager::CreateRigidbody() {
	Rigidbody* rigidbody = new Rigidbody();
	luabridge::LuaRef luaRef(ComponentManager::lua_state, rigidbody);
	return luaRef;
}