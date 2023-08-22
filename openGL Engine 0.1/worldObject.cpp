#include "worldObject.h"
unsigned int worldObject::nextID = 1;
worldObject::worldObject()
{

    ID = nextID++;

    // You can use bit manipulation to extract components from the cube ID
 // and map them to the RGB range (0-255).
    objectColour.r =  (ID >> 16) & 0xFF; // Extract bits 16-23 for the red component
    objectColour.g = (ID >> 8) & 0xFF;  // Extract bits 8-15 for the green component
    objectColour.b = ID & 0xFF;         // Extract bits 0-7 for the blue component

    // Normalize the values to the range [0.0, 1.0]
    objectColour.r /= 255.0f;
    objectColour.g/= 255.0f;
    objectColour.b /=  255.0f;

    //create unique color for drawing to actual world.
    objectRandomColour = glm::linearRand(glm::vec3(0.0f), glm::vec3(1.0f));


}

unsigned int worldObject::returnID()
{
    return ID;
}
