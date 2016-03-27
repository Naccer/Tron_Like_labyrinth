#pragma once
#include <iostream>
#include <cstring>
#include <glimac/glm.hpp>

using namespace glm;

/*class Character {
    private:
        vec2 position;
        string name;
        unsigned int life;
        unsigned int attack;
        unsigned int collectedCoins;
        unsigned int monstersKilled;
        string model3D;
    public:
        Character(vec2 initialPosition, string name, unsigned int initialLife, unsigned int attack, string objectModel);
        vec2 getPosition();
        string getName();
        unsigned int getLife();
        void rotateLeft(float degrees);
        void rotateUp(float degrees);
        glm::mat4 getViewMatrix() const;
};*/


class Character {
    private:
        //unsigned int id;   
        vec2 position;  
        char direction;
        string name;   
        unsigned int life;
        unsigned int attack;
        unsigned int collectedCoins;
        unsigned int monstersKilled;
        string model3D;

    public:
        Character(vec2 initialPosition, char initialDirection, string playerName, unsigned int initialLife, unsigned int attackCoeff, string objectModelFile) {
            position = initialPosition;
            direction = initialDirection;
            name = playerName;
            life = initialLife;
            attack = attackCoeff;
            collectedCoins = 0;
            monstersKilled = 0;
            model3D = objectModelFile;
        }

        /*unsigned int getID() {
            return id;
        }*/

		char getDirection()
		{
			return direction;
		}
		
		void setDirection(char d)
		{
			direction = d;
		}
        vec2 getPosition() {
            return position;
        }
        
        void setPosition(vec2 p)
        {
			position = p;
		}
		
		void translate(vec2 v)
        {
			position = position+v;
		}
		
		float getPositionX()
		{
			return position[0];
		}
		
		float getPositionY()
		{
			return position[1];
		}

        string getName() {
            return name;
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

        unsigned int getCoins() {
            return collectedCoins;
        }

        unsigned int getMonstersKilled() {
            return monstersKilled;
        }

        void increaseCoins(unsigned int numberOfCoins) {
            collectedCoins +=numberOfCoins;
        }

        void increaseMonstersKilled() {
            monstersKilled++; 
        }

        string getModel() {
            return model3D;
        }

        void printAttr() {
            //cout << "id: " << id << endl;
            cout << "position: (" << position.x << "," << position.y << ")" << endl;
            cout << "name: " << name << endl;
            cout << "life: " << life << endl;
            cout << "attack: " << attack << endl;
            cout << "collectedCoins: " << collectedCoins << endl;
            cout << "monstersKilled: " << monstersKilled << endl;
            cout << "model3D: " << model3D << endl;
        }
};
