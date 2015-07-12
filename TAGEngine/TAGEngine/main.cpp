
//
// Disclamer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// Note that the "Run Script" build phase will copy the required frameworks
// or dylibs to your application bundle so you can execute it on any OS X
// computer.
//
// Your resource files (images, sounds, fonts, ...) are also copied to your
// application bundle. To get the path to these resource, use the helper
// method resourcePath() from ResourcePath.hpp
//

#include "MyApplication.hpp"
#include <stdexcept>
#include <iostream>
#include <SFML/Config.hpp>

int main(int, char const**)
{
    std::cout << "SFML Verision - " << SFML_VERSION_MAJOR << "." << SFML_VERSION_MINOR << "-" << SFML_VERSION_PATCH << "\n";
    
    try {
        MyApplication app(640, 480);
        app.run();
    } catch (std::exception& e) {
        std::cout << "\nEXCEPTION: " << e.what() << std::endl;
    }
}
