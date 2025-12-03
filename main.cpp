#include "FEHLCD.h"
#include "FEHUtility.h"
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <bits/stdc++.h>
#include <random>
#include <string.h>
#include <fstream>
#include <iostream>
#include <FEHImages.h>
using namespace std;
class target{
    private:
    //init variables for money, pwd, and bitstrength
    int money;
    char pwd[16];
    float bitStrength;
    
    public:
    //set up basic functions to get private values along with constructor
    target();
    int getMoney();
    // int function to get string
    float getBitStrength();  

};
class player{
    private:
    //init variables for money, hashpower, and hardware
    int money;
    float hashpower;
    int hardware[5];
    public:
    //set up basic functions to get private values along with constructor
    player();
    int getMoney();
    float getHashpower();
    float* getHardware();
};
int main(int argc, char const *argv[])
{
    //credits top left 9, 212 bottom 9, 230 77,212 77,230
    //play button 211 - 238 y 128 - 193 x
    //help 254 - 294 x 212 - 230 y
    //stats 275 - 306 x 5 - 18 y
    while(1){
        float x, y;
        FEHImage homescreen, returnsample;
        homescreen.Open("homescreen.png");
        returnsample.Open("returnsample.png");
        homescreen.Draw(0,0);
        while(!LCD.Touch(&x, &y)){}
        while (LCD.Touch(&x, &y)){}
        if ((9 < x) && (x < 77) && (212 < y) && (y < 230))
        {
            //credits
            returnsample.Draw(0,0);
        }else if ((128 < x) && (x < 193) && (211 < y) && (y < 238)){
            //play
            returnsample.Draw(0,0);
        }else if ((254 < x) && (x < 294) && (212 < y) && (y < 230)){
            //help
            returnsample.Draw(0,0);
        }else if ((275 < x) && (x < 306) && (5 < y) && (y < 18)){
            //stats
            returnsample.Draw(0,0);
        }
        while(!LCD.Touch(&x, &y)){}
        while(LCD.Touch(&x, &y)){}
        
        LCD.Update();
    }
}


//function to generate a random number given a range of nums
int rand(int min, int max){
    std::random_device randseed;
    std::mt19937 randengine(randseed());
    std::uniform_int_distribution<int> distribution(min, max);
    int randomNum = distribution(randengine);
    return randomNum;

}

target::target(){
    //randomized password - bit strength will be based off of it
    /*  first randomize type str
    1 - common word
    2 - random lowercase letters
    3 - letters & nums
    4 - uppercase & nums
    5 - uppcase nums & symbols
    randomize length - 4 or longer for 2-5
    */
    
    
}