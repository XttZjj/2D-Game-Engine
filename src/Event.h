#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include "lua.hpp"
#include "LuaBridge.h"

class Event {
public:
    using FunctionPair = std::pair<luabridge::LuaRef, luabridge::LuaRef>;
    using SubscriptionMap = std::unordered_map<std::string, std::vector<FunctionPair>>;

    static void Publish(const std::string& event_type, const luabridge::LuaRef& event_object);
    static void Subscribe(const std::string& event_type, const luabridge::LuaRef& component, const luabridge::LuaRef& function);
    static void Unsubscribe(const std::string& event_type, const luabridge::LuaRef& component, const luabridge::LuaRef& function);

    static void ProcessQueuedRequests();

private:
    static SubscriptionMap subscriptions;
    static std::queue<std::tuple<std::string, luabridge::LuaRef, luabridge::LuaRef, bool>> subscriptionQueue; // Queue for subscription requests
    static std::queue<std::tuple<std::string, luabridge::LuaRef, luabridge::LuaRef>> unsubscriptionQueue; // Queue for unsubscription requests
};


#endif
