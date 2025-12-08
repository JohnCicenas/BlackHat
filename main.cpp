//all libraries (some may not be used was messing around with different libraries especially for timing and randomization)
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
#include <vector>
using namespace std;
/*create global stop flag to close thread whenever user presses next and target hackedflag to tell when target is hacked
and track screen - // 1 is home 2 is play 3 is pause 4 is stats 5 is credits 6 is help - atomic library allows for tracking between threads
*/
atomic<bool> stopFlag(false);
atomic<bool> targethacked(false);
atomic<int> screenType(1);

// global random number generator seeding engine - based off time for random seeding persisting through program
mt19937 randengine(static_cast<unsigned long>(
    chrono::high_resolution_clock::now().time_since_epoch().count()
));
//keep track of each thread - gives ablity to wait for thread to stop globally
HANDLE threadHandle;
//class declarations
//hannah
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
    //set up basic functions to get private values, add to values, save, and constructor
    player();
    double getMoney();
    void addToMoney(double addmoney);
    int getTargetsHacked();
    void addToTargetsHacked();
    double getHashpower();
    void addToHashpower(double addhashpower);
    int getHardware(int position);
    void addToHardware(int type);
    //john wrote
    void saveDataToMemory();
};

//function declarations and who wrote
//hannah - function to format large numbers to readable forms
string formatScientific(double value);
//john - function to generate a random password - used for target's password
string generatePassword();
//john - helper function to get random int
int generateRandInt(int min, int max);
//john - function to calculate the bits of entropy off of a password
float getBitsEntropy(string password);
//john - same as generateRandInt except for floats
float generateRandFloat(float min, float max);
//john - function to calculate time to crack (y) based off of bits and hashes/s
float computeTime(float bitStrength, double hashpower);
//john - function ran by the thread to update progress bar and %complete
void updateProgressBar(float progress);
//john - function to create the thread
void createThread(float timeTillComp);
//hannah - handles homescreen logic
void homeScreen(FEHImage imgHome);
//john - handles playscreen logic
void playScreen(FEHImage imgPlay, player& p);
//hannah - handles statsscreen logic
void statsScreen(FEHImage imgStats, player& p);
//hannah - handles helpscreen logic
void helpScreen(FEHImage imgHelp);
//hannah - handles creditssceen logic
void creditsScreen(FEHImage imgCredits);
//hannah - handles pausescreen logic
void pauseScreen(FEHImage imgPause, player& p);
//john - function ran on the thread (note this is using C threads (windows.h) - I think only runs on windows)
DWORD WINAPI clockThread(LPVOID lpparam);
player p1;
//main
int main(int argc, char const *argv[])
{

    //credits top left 9, 212 bottom 9, 230 77,212 77,230
    //play button 205 - 237 y 117 - 201 x
    //help 254 - 294 x 212 - 230 y
    //stats 259 - 311 x 8 - 26 y
    
    while(1){
        //screentype int to keep track of what screen is being displayed - different functions touch functions for each screen type
        //create all screen image objects
        FEHImage imghome, imgplay, imgstats, imghelp, imgcredits, imgpause;
        imghome.Open("homescreen.png");
        imgplay.Open("playscreen.png");
        imgstats.Open("statsScreen.png");
        imghelp.Open("help.png");
        imgcredits.Open("credits.png");
        imgpause.Open("pausescreen.png");
        //handle logic
        switch (screenType)
        {
        case 1:{
            homeScreen(imghome);
            break;
        }
        case 2:{
            playScreen(imgplay, p1);
            break;
        }
        case 3:{
            pauseScreen(imgpause, p1);
            break;
        }
        case 4:{
            statsScreen(imgstats, p1);
            break;
        }
        case 5:{
            creditsScreen(imgcredits);
            break;
        }
        case 6:{
            helpScreen(imghelp);
            break;
        }
        default:
            break;
        }
    }
}
//homescreen function
void homeScreen(FEHImage imgHome){
    //clear lcd then replace with homescreen lcd - get ready for touch logic
    LCD.Clear();
    float x, y;
    bool exit = false;
    imgHome.Draw(0,0);
    LCD.Update();
    //touch logic for each individual button - switch to certain screen using the atomic int screentype
    while(!exit){
        while(!LCD.Touch(&x, &y)){}
        while(LCD.Touch(&x, &y)){}
        // Credits
        if (x > 9 && x < 77 && y > 212 && y < 230){
            screenType = 5;
            exit = true;
        }
        // Play
        else if (x > 117 && x < 201 && y > 205 && y < 237){
            screenType = 2;
            exit = true;
        }
        // Help
        else if (x > 254 && x < 294 && y > 212 && y < 230){
            screenType = 6;
            exit = true;
        }
        // Stats
        else if (x > 259 && x < 311 && y > 8 && y < 26){
            screenType = 4; 
            exit = true;
        }
    }
}
void playScreen(FEHImage imgPlay, player& p){
    //next x 275 - 303 y 207 - 221
    //pause 287 34 300 47
    // 1 52 43 74 65
    // 2 47 83  74 95
    // 3 46, 116 76, 125
    // 4 50, 147 76, 165
    // 5 54, 181 72, 201
    //clear screen and draw image - also create vector of target objects so they can be created during runtime and iterated through
    LCD.Clear();
    vector<target> targets;
    int targetCounter = 0;
    float x, y;
    bool exit = false;
    imgPlay.Draw(0,0);
    LCD.Update();
    //handle constant updates to statistics
    while (!exit)
    {
        while(!LCD.Touch(&x, &y)){
                // Clear only the areas where text will be written
            LCD.SetFontColor(WHITE);  // Match background color
            LCD.FillRectangle(7, 216, 76, 10);  // Clear money area
            LCD.FillRectangle(54, 216, 30, 10);  // Clear hashpower area
            LCD.FillRectangle(17, 51, 20, 10);   // Clear h1
            LCD.FillRectangle(17, 85, 20, 10);   // Clear h2
            LCD.FillRectangle(17, 120, 20, 10);  // Clear h3
            LCD.FillRectangle(17, 152, 20, 10);  // Clear h4
            LCD.FillRectangle(17, 189, 20, 10);  // Clear h5

            LCD.SetFontColor(GREEN);
            LCD.SetFontScale(0.5);
            LCD.WriteAt("$", 7, 216);
            LCD.WriteAt(formatScientific(p.getMoney()), 14, 216);
            LCD.WriteAt("#", 49, 216);
            LCD.WriteAt(formatScientific(p.getHashpower()), 54, 216);
            LCD.SetFontColor(BLACK);
            LCD.WriteAt(p.getHardware(1), 17, 51);
            LCD.WriteAt(p.getHardware(2), 17, 85);
            LCD.WriteAt(p.getHardware(3), 17, 120);
            LCD.WriteAt(p.getHardware(4), 17, 152);
            LCD.WriteAt(p.getHardware(5), 17, 189);
            LCD.Update();
            if(targethacked){
                p.addToMoney(targets[targetCounter-1].getMoney());
                LCD.WriteAt("Password:", 166, 49);
                LCD.WriteAt(targets[targetCounter-1].getPass(), 166, 69);
                p1.addToTargetsHacked();
                targethacked = false;
            }
            Sleep(10);
        }
        while(LCD.Touch(&x, &y)){}
        // next
        //handle button logic
        if (x > 275 && x < 303 && y > 207 && y < 221){
            //randomized sprite for each target
            FEHImage i("playscreen-120x40-(162,41).png"), sprite1("sprite1.PNG"), sprite2("sprite2.PNG"), sprite3("sprite3.PNG");
            int spriteType = generateRandInt(1,3);
            imgPlay.Draw(0,0);
            switch (spriteType)
            {
            case 1:{
                sprite1.Draw(145,40);
                break;
            }
            case 2:{
                sprite2.Draw(145,40);
                break;
            }
            case 3:{
                sprite3.Draw(145,40);
                break;
            }
            default:
                break;
            }
            i.Draw(162,41);
            //create new target and add to vector of targets
            targetCounter++;
            targets.emplace_back();
            cout << targets[targetCounter-1].getBitStrength() << endl;
            cout << targets[targetCounter-1].getMoney() << endl;
            cout << targets[targetCounter-1].getPass()<< endl;
            //stop thread if running
            stopFlag = true;
            WaitForSingleObject(threadHandle, INFINITE);
            stopFlag = false;
            //create new thread with target time to crack as input
            createThread(computeTime(targets[targetCounter-1].getBitStrength(), p.getHashpower()));

        }
        // pause
        else if (x > 287 && x < 300 && y > 34 && y < 47){
            //bring up pause screen and stop thread from running
            screenType = 3;
            stopFlag = true;
            WaitForSingleObject(threadHandle, INFINITE);
            stopFlag = false;
            exit = true;
            
        }
        // h1
        //next logic is just purchasing hardware
        else if (x > 52 && x < 74 && y > 43 && y < 65){

            double cost = 1000;
            if(cost <= p.getMoney()){
                p.addToMoney(-1 * cost);
                p.addToHashpower(7500);
                p.addToHardware(1);
            }
        }
        // h2

        else if (x > 47 && x < 74 && y > 83 && y < 95){
            double cost = 5000;
            double hashpower = 3200000;
            if(cost <= p.getMoney()){
                p.addToMoney(-1 * cost);
                p.addToHashpower(hashpower);
                p.addToHardware(2);
            }
        }
        // h3 46, 116 76, 125
        else if (x > 46 && x < 76 && y > 116 && y < 125){
            double cost = 150000;
            double hashpower = 580000000;
            if(cost <= p.getMoney()){
                p.addToMoney(-1 * cost);
                p.addToHashpower(hashpower);
                p.addToHardware(3);
            }
        }
        // h4 50, 147 76, 165
        else if (x > 50 && x < 76 && y > 147 && y < 165){
            double cost = 8000000;
            double hashpower = 90000000000;
            if(cost <= p.getMoney()){
                p.addToMoney(-1 * cost);
                p.addToHashpower(hashpower);
                p.addToHardware(4);
            }
        }
        // h5 54, 181 72, 201
        else if (x > 54 && x < 72 && y > 181 && y < 201){
            double cost = 150000000;
            double hashpower = 1000000000000;
            if(cost <= p.getMoney()){
                p.addToMoney(-1 * cost);
                p.addToHashpower(hashpower);
                p.addToHardware(5);
            }
        }
    }
    
};
//statsScreen
void statsScreen(FEHImage imgStats, player& p){
    float x, y;
    bool exit = false;
    LCD.Clear();
    //draw statsScreen with playerstats as input
    imgStats.Draw(0, 0);
    LCD.SetFontColor(WHITE);
    LCD.WriteAt("#", 134, 82);
    LCD.WriteAt(formatScientific(p.getHashpower()), 144, 82);
    LCD.WriteAt("$", 132, 125);
    LCD.WriteAt(formatScientific(p.getMoney()), 144, 125);
    LCD.WriteAt(formatScientific(p.getTargetsHacked()), 144, 169);
    LCD.Update();
    //wait for input to return
    while(!exit){
        while(!LCD.Touch(&x, &y)){}
        while(LCD.Touch(&x, &y)){}
        //87 204 107 211
        if(87 < x && x < 138 && 204 < y && 213 > y){
            screenType = 1;
            exit = true;
        }
    }
    

};
//helpscreen
void helpScreen(FEHImage imgHelp){
    //draw helpscreen and wait for input to return
    float x, y;
    LCD.Clear();
    imgHelp.Draw(0,0);
    bool exit = false;
    LCD.Update();
    while(!exit){
        while(!LCD.Touch(&x, &y)){}
        while(LCD.Touch(&x, &y)){}
        //87 204 107 211
        if(87 < x && x < 138 && 204 < y && 213 > y){
            screenType = 1;
            exit = true;
        }
    }


};
//pauseScreen
void pauseScreen(FEHImage imgPause, player& p){
    float x, y;
    LCD.Clear();
    imgPause.Draw(0,0);
    bool exit = false;
    LCD.Update();
    //stop thread from running and handle input logic 
    while(!exit){
        while(!LCD.Touch(&x, &y)){}
        while(LCD.Touch(&x, &y)){}
        //87 204 107 211
        if(87 < x && x < 138 && 204 < y && 213 > y){
            //home
            screenType = 1;
            stopFlag = true;
            WaitForSingleObject(threadHandle, INFINITE);
            stopFlag = false;
            exit = true;
        }else if(134 < x && x < 184 && 92 < y && y < 104){
            //return
            screenType = 2;
            stopFlag = true;
            WaitForSingleObject(threadHandle, INFINITE);
            stopFlag = false;
            exit = true;
        }else if(142 < x && x < 175 && 123 < y && y < 135){
            //save
            stopFlag = true;
            WaitForSingleObject(threadHandle, INFINITE);
            stopFlag = false;
            p1.saveDataToMemory();
            LCD.SetFontColor(WHITE);
            LCD.WriteAt("Saved!", 129, 187);
            LCD.Update();
            Sleep(1.0);
            imgPause.Draw(0,0);
            LCD.Update();
            

        }
    }
};
//creditsScreen
void creditsScreen(FEHImage imgCredits){
    //display credits and return to main menu
    float x, y;
    LCD.Clear();
    imgCredits.Draw(0,0);
    bool exit = false;
    LCD.Update();
    while(!exit){
        while(!LCD.Touch(&x, &y)){}
        while(LCD.Touch(&x, &y)){}
        //87 204 107 211
        if(87 < x && x < 138 && 204 < y && 213 > y){
            screenType = 1;
            exit = true;
        }
    }
};
//function to format values to display in clean formatting
string formatScientific(double value) {
    //print to buffer the value divided by the amount in places + symbol (k, m, b, t)
    char buffer[50];
    if (value >= 1e12) {
        sprintf(buffer, "%.1fT", value / 1e12);
    } else if (value >= 1e9) {
        sprintf(buffer, "%.1fB", value / 1e9);
    } else if (value >= 1e6) {
        sprintf(buffer, "%.1fM", value / 1e6);
    } else if (value >= 1e3) {
        sprintf(buffer, "%.1fK", value / 1e3);
    } else {
        sprintf(buffer, "%.1f", value);
    }
    return string(buffer);
}
//function to generate a random int given a range of ints
int generateRandInt(int min, int max){
    //use seeding declared at top and distribution declared below
    uniform_int_distribution<int> distribution(min, max);
    int randomNum = distribution(randengine);
    return randomNum;

}
//function to generate a random float given a range of floats
float generateRandFloat(float min, float max){
    //same thing as generateRandInt except distribution is reals in range
    uniform_real_distribution<float> distribution(min, max);
    float randomNum = distribution(randengine);
    return randomNum;

}
//function to create a random password
string generatePassword() {
    string pass;
    string charArr = "abcdefghijklmnopqrstuvwxyz";
    int randCase = generateRandInt(1, 5);
    int pwdLength = generateRandInt(4, 20);
    //switch through each possible case for differing password strength
    switch (randCase) {
        case 1: { // common word
            //iterate through list of words and add to vector - then choose randomly from it
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
//function to update the progress bar based on time left - used by thread
void updateProgressBar(float progress){
    //for %progress
    LCD.SetFontColor(WHITE);
    LCD.FillRectangle(120, 60, 25, 10);
    int totalWidth = 206;
    int progresspixel = static_cast<int>(round(totalWidth * progress));
    int randomInt = generateRandInt(1, 2);
    switch (randomInt)
    {
    case 1:{
        LCD.SetFontColor(BLACK);
        break;
    }
    case 2:{
        LCD.SetFontColor(GREEN);
        break;
    }
    default:
        break;
    }

    LCD.WriteAt((int)(progress * 100), 120, 60);
    LCD.WriteAt("%", 138, 60);
    //for progressbar
    LCD.SetFontColor(GREEN);
    LCD.FillRectangle(103, 14, progresspixel, 9);
    LCD.Update();
}
//function running on thread
DWORD WINAPI clockThread(LPVOID param)
{
    LCD.SetFontColor(BLACK);
    LCD.FillRectangle(103, 14, 206, 9);
    LCD.Update();
    //set dynamically alloc param to non-dyn variable - delete instantly after for memory alloc
    float timeToCrack = *static_cast<float*>(param);
    delete static_cast<float*>(param);
    timeToCrack = timeToCrack * 60.0;
    //using chrono library for better timekeeping - updates at a much higher frequency
    auto start = chrono::steady_clock::now();
    auto end   = start + chrono::duration<float>(timeToCrack);

    while (true)
    {
        // If stopFlag is set, exit immediately
        if (stopFlag) {
            LCD.SetFontColor(BLACK);
            LCD.FillRectangle(103, 14, 206, 9);
            LCD.Update();
            return 0;
        }

        auto now = chrono::steady_clock::now();

        //compute progress in range as a decimal percent
        float progress = 
            chrono::duration<float>(now - start).count() / timeToCrack;
        //finishedcondition
        if (progress >= 1.0f){
            break;
        }
        updateProgressBar(progress);
        
        //sleep a bit so cpu doesnt get fried
        Sleep(10);
    }

    // Thread finished normally (time ran out)
    updateProgressBar(1.0f);
    targethacked = true;
    //always return something to guarantee thread closing
    return 0;
}

//function that creates thread - passing in ptr timeToCrack which is dynamically allocated
void createThread(float time){
    float *timeToCrack = new float(time);
    threadHandle = CreateThread(nullptr, 0, clockThread, timeToCrack, 0, nullptr);
    
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
    money = 500 * pow(bitStrength, 1.5) * generateRandFloat(0.8, 1.2);
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
        hashpower = 7000;
        targetsHacked = 0;
        hardware[0] = 1;
        for (int i = 1; i < 5; i++)
        {
            hardware[i] = 0;
        }
        
    }
}
//getter functions for player class 
double player::getMoney(){
    return money;
}
double player::getHashpower(){
    return hashpower;
}
int player::getTargetsHacked(){
    return targetsHacked;
}
int player::getHardware(int position){
    return hardware[position - 1];
}
//setter functions for player class
void player::addToMoney(double addMoney){
    money += addMoney;
}
void player::addToTargetsHacked(){
    targetsHacked++;
}
void player::addToHashpower(double addhashpower){
    hashpower += addhashpower;
}
void player::addToHardware(int type){
    hardware[type-1]++;
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