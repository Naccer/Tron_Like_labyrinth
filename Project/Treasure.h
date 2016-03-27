#pragma once
#include <iostream>
#include <cstring>
#include <glimac/glm.hpp>
#include <string>

using namespace std;
using namespace glm;
class Treasure {
    private:
        unsigned int id;   
        vec2 position;  
        string name;   
        unsigned int type;
        unsigned int characteristics;
        string model3D;
    public: 
        Treasure(unsigned int treasureId, vec2 mapPosition, string treasureName, unsigned int treasureType, unsigned int treasureCharacteristics, string objectModelFile) {
            id = treasureId;
            position = mapPosition;
            name = treasureName;
            type = treasureType;
            characteristics = treasureCharacteristics; // value if coins, life restore if aid
            model3D = objectModelFile;
        }

        unsigned int getID() {
            return id;
        }

        vec2 getPosition() {
            return position;
        }

        string getName() {
            return name;
        }

        unsigned int getType() {
            return type;
        }

        unsigned int getCharacteristics() {
            return characteristics;
        }

        string getModel() {
            return model3D;
        }

        void printAttr() {
            cout << "id: " << id << endl;
            cout << "position: (" << position.x << "," << position.y << ")" << endl;
            //cout << "name: " << name << endl;
            cout << "type: " << type << endl;
            cout << "characteristics: " << characteristics << endl;
            cout << "model3D: " << model3D << endl;
        }
};
