#pragma once
#include <iostream>
#include <cstring>
#include <glimac/glm.hpp>
#include <string>

using namespace std;
using namespace glm;
class Monster {
    private:
        unsigned int id;   
        vec2 position;  
        string name;   
        unsigned int type;
        unsigned int life;
        unsigned int attack;
        string model3D;

    public:
        Monster(unsigned int monsterId, vec2 initialPosition, string monsterName, unsigned int monsterType, unsigned int initialLife,
                    unsigned int attackCoeff, string objectModelFile) {
            id = monsterId;
            position = initialPosition;
            name = monsterName;
            type = monsterType;
            life = initialLife;
            attack = attackCoeff;
            model3D = objectModelFile;
        }

	Monster(){};

        unsigned int getID() {
            return id;
        }

        vec2 getPosition() {
            return position;
        }

	float getPositionX()
	{
		return position[0];
	}

	float getPositionY()
	{
		return position[1];
	}

	void setPosition(vec2 p)
	{
		position = p;
	}


        string getName() {
            return name;
        }

        unsigned int getType() {
            return type;
        }

        unsigned int getLife() { 
            return life;
        }

        void decreaseLife(unsigned int attack) {
            life -= attack;                    
        }

        unsigned int getAttack() {
            return attack;
        }

        void checkAround() { 

        }

        void moveTowards(vec2 aimCoord) {
            
        }

        void move(vec2 relativeMovement) {
            position += relativeMovement;
        }

        string getModel() {
            return model3D;
        }

        void printAttr() {
            cout << "id: " << id << endl;
            cout << "position: (" << position.x << "," << position.y << ")" << endl;
            cout << "name: " << name << endl;
            cout << "type: " << type << endl;
            cout << "life: " << life << endl;
            cout << "attack: " << attack << endl;
            cout << "model3D: " << model3D << endl;
        }
};
