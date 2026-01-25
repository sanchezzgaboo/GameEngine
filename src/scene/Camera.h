#pragma once
#include <SFML/System/Vector3.hpp>


class Camera
{
    private:
    float x,y,z;
    public:
    sf::Vector3f vCamera;
    sf::Vector3f vLookDir;
    sf::Vector3f vUp;
    sf::Vector3f vTarget;


    Camera();

};