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
#include <thread>
#include <windows.h>
#include <atomic>
#include <random>
#include <chrono>
using namespace std;
//create global stop flag to close thread whenever user presses next and target hackedflag to tell when target is hacked
atomic<bool> stopFlag(false);
atomic<bool> targethacked(false);

// global random number generator seeding engine - based off time for random seeding persisting through program
mt19937 randengine(static_cast<unsigned long>(
    chrono::high_resolution_clock::now().time_since_epoch().count()
));

//class declarations
class target{
    private:
    //init variables for money, pwd, and bitstrength
    double money;
    string pwd;
    float bitStrength;
    
    public:
    //set up basic functions to get private values along with constructor
    target();
    double getMoney();
    string getPass();
    float getBitStrength();  

};
class player{
    private:
    //init variables for money, hashpower, and hardware
    int targetsHacked;
    double money;
    double hashpower;
    /* h1 7000-8000
    h2 3.2m 
    h3 580 m
    h4 90 b m
    h5 1 t
    */
    int hardware[5];
    public:
    //set up basic functions to get private values along with constructor
    player();
    double getMoney();
    int getTargetsHacked();
    float getHashpower();
    int* getHardware();
    void saveDataToMemory();
};

//function declarations
string generatePassword();
int generateRandInt(int min, int max);
float getBitsEntropy(string password);
float generateRandFloat(float min, float max);
float computeTime(float bitStrength, double hashpower);
void updateProgressBar(float progress);
void createThread(float timeTillComp);
DWORD WINAPI clockThread(LPVOID lpparam);

//main
int main(int argc, char const *argv[])
{

    //credits top left 9, 212 bottom 9, 230 77,212 77,230
    //play button 205 - 237 y 117 - 201 x
    //help 254 - 294 x 212 - 230 y
    //stats 259 - 311 x 8 - 26 y
    
    while(1){
        float x, y;
        FEHImage homescreen, returnsample;
        homescreen.Open("homescreen.png");
        returnsample.Open("returnsample.png");
        homescreen.Draw(0,0);
        createThread(1.0);
        while(!LCD.Touch(&x, &y)){}
        while (LCD.Touch(&x, &y)){}
        if ((9 < x) && (x < 77) && (212 < y) && (y < 230))
        {
            //credits
            returnsample.Draw(0,0);
        }else if ((205 < x) && (x < 294) && (205 < y) && (y < 237)){
            //play
            returnsample.Draw(0,0);
        }else if ((254 < x) && (x < 294) && (212 < y) && (y < 230)){
            //help
            returnsample.Draw(0,0);
        }else if ((259 < x) && (x < 311) && (8 < y) && (y < 26)){
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
    uniform_int_distribution<int> distribution(min, max);
    int randomNum = distribution(randengine);
    return randomNum;

}
//function to generate a random float given a range of floats
float generateRandFloat(float min, float max){
    uniform_real_distribution<float> distribution(min, max);
    float randomNum = distribution(randengine);
    return randomNum;

}
string generatePassword() {
    string pass;
    string charArr = "abcdefghijklmnopqrstuvwxyz";
    int randCase = generateRandInt(1, 5);
    int pwdLength = generateRandInt(4, 20);
    cout << randCase << endl;
    cout << pwdLength << endl;
    switch (randCase) {
        case 1: { // common word
            vector<string> words;
            ifstream file("commonwords.txt");
            string line;
            while(getline(file, line)) words.push_back(line);
            file.close();
            if(words.empty()) return "default"; // fallback
            pass = words[ generateRandInt(0, words.size()-1) ];
            return pass;
        }
        case 2: { // lowercase only
            for(int i=0;i<pwdLength;i++)
                pass += charArr[generateRandInt(0,charArr.size()-1)];
            return pass;
        }
        case 3: { // lowercase + digits
            charArr += "0123456789";
            for(int i=0;i<pwdLength;i++)
                pass += charArr[generateRandInt(0,charArr.size()-1)];
            return pass;
        }
        case 4: { // lowercase + uppercase + digits
            charArr += "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            for(int i=0;i<pwdLength;i++)
                pass += charArr[generateRandInt(0,charArr.size()-1)];
            return pass;
        }
        case 5: { // lowercase + uppercase + digits + symbols
            charArr += "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*";
            for(int i=0;i<pwdLength;i++)
                pass += charArr[generateRandInt(0,charArr.size()-1)];
            return pass;
        }
        default:
            return "default";
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
/*
compute time to crack based on hashrate - formula found from 
https://auth0.com/blog/defending-against-password-cracking-understanding-the-math/
via formula T = S/(A*6.308*10^7) where t is time to crack in years, s is sample space, 
and a is hashes/s
*/
float computeTime(float bitStrength, double hashpower){
    double logTime = bitStrength * log10(2) - log10(hashpower) - log10(6.308 * pow(10.0, 7));
    float time = pow(10.0, logTime);
    return time;
}
//function to update the progress bar based on time left
void updateProgressBar(float progress){
    int totalWidth = 205;
    int progresspixel = static_cast<int>(round(totalWidth * progress));
    LCD.SetFontColor(GREEN);
    LCD.FillRectangle(103, 22, progresspixel, 6);
    LCD.Update();
}
//function running on thread
DWORD WINAPI clockThread(LPVOID lpparam){
//getting time value in yrs from ptr then deleting dynamic allocation
    float *p = static_cast<float*>(lpparam);
    float timeToCrack = *p;
    delete p;
    // in this game 1 real life year = 60 seconds hacking time
    float progress;
    timeToCrack = timeToCrack * 60.0;
    time_t timeStart = time(NULL);
    while((time(NULL) - timeStart) < timeToCrack){
        progress = (time(NULL) - timeStart)/timeToCrack;
        updateProgressBar(progress);
        if(stopFlag){
            //code to make bar go back to start
            LCD.SetFontColor(GRAY);
            LCD.FillRectangle(103, 22, 205, 6);
            LCD.Update();
            return 0;
        }
    }
    targethacked = true;
    return 0;
}
//function that creates thread - passing in ptr timeToCrack which is dynamically allocated
void createThread(float time){
    float *timeToCrack = new float(time);
    HANDLE thread = CreateThread(nullptr, 0, clockThread, timeToCrack, 0, nullptr);
    
}
//next person
/*
    Target class and functions


*/
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
double target::getMoney(){
    return money;
}
/*
    Player class and functions

*/
//player constructor function
player::player(){
    //open file to see if there is a previous save to take from
    ifstream playersavefile;
    playersavefile.open("BlackHatPlayerSaveFile.txt");
    if(playersavefile.is_open()){
        playersavefile >> targetsHacked;
        playersavefile >> money;
        playersavefile >> hashpower;
        for (int i = 0; i < 5; i++)
        {
            playersavefile >> hardware[i];
        }
        
    //else create starting stats
    }else{
        money = 0;
        hashpower = 0;
        targetsHacked = 0;
        for (int i = 0; i < 5; i++)
        {
            hardware[i] = 0;
        }
        
    }
}
//getter functions for player class 
double player::getMoney(){
    return money;
}
float player::getHashpower(){
    return hashpower;
}
int player::getTargetsHacked(){
    return targetsHacked;
}
int* player::getHardware(){
    return hardware;
}
//function to save player data to long-term memory
void player::saveDataToMemory(){
    ofstream saveFile;
    saveFile.open("BlackHatPlayerSaveFile.txt");
    saveFile << targetsHacked << "\n";
    saveFile << money << "\n";
    saveFile << hashpower << "\n";
    for (int i = 0; i < 5; i++)
    {
        saveFile << hardware[i] << "\n";
    }
    saveFile.close();
}
