#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include "lua.hpp"
#include "LuaBridge.h"
#include "Actor.h"
#include "Input.h"
#include "Renderer.h"
#include "AudioDB.h"
#include "../Third Party/glm/glm/glm.hpp"
#include "box2d/box2d.h"
#include "Rigidbody.h"
#include "Raycast.h"
#include "Event.h"

class ComponentManager {
public:
    static inline lua_State* lua_state;

    static inline int nAdd = 0;

    static inline int nAddStart = 0;

    void Initialize();

    void InitializeState();

    void InitializeFunctions();

    static void Print(const std::string & message);

    static void PrintError(const std::string & message);

    static void Quit();

    static void Sleep(int milliseconds);

    static int GetFrame();

    static void OpenURL(std::string url);

    void InitialiazeComponents();

    static luabridge::LuaRef GetLuaState();

    static std::shared_ptr<luabridge::LuaRef> GetComponent(const std::string & componentName);

    static void EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table);

    static inline std::string nextSceneName;

    static inline bool changeScene;

    static void Load(std::string scene_name);

    static inline std::string currentScene;

    static std::string GetCurrent();

    static luabridge::LuaRef CreateRigidbody();

private:
    static inline std::unordered_map<std::string, std::shared_ptr<luabridge::LuaRef>> component_tables;
};

#endif
