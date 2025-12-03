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
#include <cmath>
using namespace std;


class target{
    private:
    //init variables for money, pwd, and bitstrength
    int money;
    string pwd;
    float bitStrength;
    
    public:
    //set up basic functions to get private values along with constructor
    target();
    float getMoney();
    string getPass();
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
    int* getHardware();
};
//function declarations
string generatePass();
int generateRandInt();
float getBitsEntropy(string password);
float generateRandFloat();
//main
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


//function to generate a random int given a range of ints
int generateRandInt(int min, int max){
    std::random_device randseed;
    std::mt19937 randengine(randseed());
    std::uniform_int_distribution<int> distribution(min, max);
    int randomNum = distribution(randengine);
    return randomNum;

}
//function to generate a random float given a range of floats
float generateRandFloat(float min, float max){
    std::random_device randseed;
    std::mt19937 randengine(randseed());
    std::uniform_int_distribution<float> distribution(min, max);
    int randomNum = distribution(randengine);
    return randomNum;

}
string generatePassword(){
    //randomized password - bit strength will be based off of it
    /*  first randomize type str
    1 - common word
    2 - random lowercase letters
    3 - letters & nums
    4 - uppercase & nums
    5 - uppcase nums & symbols
    randomize length - 4 or longer for 2-5
    */
   string pass, charArr = "abcdefghijklmnopqrstuvwxyz";
   int randCase = generateRandInt(1, 5);
   int pwdLength = generateRandInt(4, 20);
   ifstream ifFile;
   switch (randCase)
   {
    case 1:
        //case 1 - common word from database
        ifFile.open("commonwords.txt");
        int tempindex = generateRandInt(1,9999);
        for (int i = 0; i < tempindex; i++)
        {
            getline(ifFile, pass);
        }
        ifFile.close();
        return pass;
        break;
    case 2:
        //case 2 - just lowercase
        pass.reserve(pwdLength);
        for (int i = 0; i < pwdLength; i++)
        {
            pass += charArr[generateRandInt(0, charArr.size())];
        }
        return pass;
        break;
    case 3:
        //case 3 - lowercase and ints
        charArr += "1234567890";
        pass.reserve(pwdLength);
        for (int i = 0; i < pwdLength; i++)
        {
            pass += charArr[generateRandInt(0, charArr.size())];
        }
        return pass;
        break;
    case 4:
        //case 4 - lowercase, uppercase, and ints
        charArr += "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        pass.reserve(pwdLength);
        for (int i = 0; i < pwdLength; i++)
        {
            pass += charArr[generateRandInt(0, charArr.size())];
        }
        return pass;
        break;
    case 5:
        //lowercase, uppercase, and ints
        charArr += "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*";
        pass.reserve(pwdLength);
        for (int i = 0; i < pwdLength; i++)
        {
            pass += charArr[generateRandInt(0, charArr.size())];
        }
        return pass;
        break;
    default:
        break;
   }
}
/*function to get bit strength (will be able to calculate time based off this)
    formula for measuring bits of entropy - E = L * log(base2)R
    source and more information: https://proton.me/blog/what-is-password-entropy
*/
float getBitsEntropy(string password){
    //store password length and initialize var for possible num of characters
    int length = password.length();
    int possibleChars = 26;
    //add to characters if any of these are found
    if(password.find_first_of("1234567890") != string::npos){
        possibleChars += 10;
    }
    if(password.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ") != string::npos){
        possibleChars += 26;
    }
    if(password.find_first_of("!@#$%^&*") != string::npos){
        possibleChars += 8;
    }
    //compute bits
    return length * log2(possibleChars);
}
target::target(){
    //construct each target objects random values
    string temp = generatePassword();
    pwd = temp;
    bitStrength = getBitsEntropy(temp);
    money = 1000 * (pow(bitStrength, generateRandFloat(1.0, 2.0)));
    
}
//getter functions for player class variables
string target::getPass(){
    return pwd;
}
float target::getBitStrength(){
    return bitStrength;
}
float target::getMoney(){
    return money;
}
//player constructor function
player::player(){
    
}
//getter functions for player class 
int player::getMoney(){
    return money;
}
float player::getHashpower(){
    return hashpower;
}
int* player::getHardware(){
    return hardware;
}
// hi