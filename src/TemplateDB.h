#ifndef TEMPLATE_DB_H
#define TEMPLATE_DB_H

#include <string>
#include <unordered_map>
#include "Actor.h" // Include the header file for Actor
#include "../SDL2/SDL.h"
#include "ImageDB.h"
#include "ComponentManager.h"

class TemplateDB {
private:
    static inline std::unordered_map<std::string, Actor*> templates;

public:
    TemplateDB(); // Constructor

    // Function to load actor templates from files
    static Actor* loadTemplates(const std::string& directory);

    // Function to apply a template to an actor
    static void applyTemplate(Actor*& actor, const std::string& templateName);


};

#endif // TEMPLATE_DB_H
