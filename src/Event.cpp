#include "Event.h"

Event::SubscriptionMap Event::subscriptions;
std::queue<std::tuple<std::string, luabridge::LuaRef, luabridge::LuaRef>> Event::unsubscriptionQueue;
std::queue<std::tuple<std::string, luabridge::LuaRef, luabridge::LuaRef, bool>> Event::subscriptionQueue;

void Event::Publish(const std::string& event_type, const luabridge::LuaRef& event_object) {
    if (subscriptions.find(event_type) != subscriptions.end()) {
        for (const auto& pair : subscriptions[event_type]) {
            pair.second(pair.first, event_object);
        }
    }
}

void Event::Subscribe(const std::string& event_type, const luabridge::LuaRef& component, const luabridge::LuaRef& function) {
    subscriptionQueue.push(std::make_tuple(event_type, component, function, true)); // Enqueue subscription request
}

void Event::Unsubscribe(const std::string& event_type, const luabridge::LuaRef& component, const luabridge::LuaRef& function) {
    unsubscriptionQueue.push(std::make_tuple(event_type, component, function)); // Enqueue unsubscription request
}

void Event::ProcessQueuedRequests() {
    while (!subscriptionQueue.empty()) {
        auto request = subscriptionQueue.front();
        subscriptionQueue.pop();
        if (std::get<3>(request)) {
            subscriptions[std::get<0>(request)].push_back({ std::get<1>(request), std::get<2>(request) });
        }
    }

    while (!unsubscriptionQueue.empty()) {
        auto request = unsubscriptionQueue.front();
        unsubscriptionQueue.pop();
        auto& subscribers = subscriptions[std::get<0>(request)];
        for (auto it = subscribers.begin(); it != subscribers.end();) {
            if (it->first == std::get<1>(request) && it->second == std::get<2>(request)) {
                it = subscribers.erase(it);
            }
            else {
                ++it;
            }
        }
    }
}
