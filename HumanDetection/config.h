#ifndef _MC_CONFIG
#define _MC_CONFIG 1

#ifdef _WIN32
#include <windows.h>
#endif

#include <map>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>

#include "Detector.h"
#include "DefaultDetector.h"
#include "TestDetector.h"
#include "PartBasedDetector.h"

#include "Tracker.h"
#include "DefaultTracker.h"
#include "MeanshiftTracker.h"
#include "CmtTracker.h"

using namespace std;

struct Config {
	static const string VIDEO;
	static const string WIDTH;
	static const string HEIGHT;
	static const string DETECTOR;
	static const string TRACKER;
	static const string STEP;
	static const string FPS;
	static const string OUTPUT;
	static const string EVALUATE;
		
	map<string, string> config;
	int width = 640;
	int height = 480;
	int step = 10;
	int fps = 25;
	string video = "0";	
	string detector = "DEFAULT_DETECTOR";
	string tracker = "DEFAULT_TRACKER";
	string output = "NONE";
	string evaluate = "NONE";
	
	bool checkConfig(string key){
		return config.find(key) != config.end();
	}
	
	void init(){
		if (checkConfig(VIDEO))
			video = config[VIDEO];
		
		if (checkConfig(WIDTH))
			width = atoi(config[WIDTH].c_str());
		
		if (checkConfig(HEIGHT))
			height = atoi(config[HEIGHT].c_str());
		
		if (checkConfig(DETECTOR))
			detector = config[DETECTOR];
		
		if (checkConfig(TRACKER))
			tracker = config[TRACKER];
		
		if (checkConfig(STEP))
			step = atoi(config[STEP].c_str());
		
		if (checkConfig(FPS))
			fps = atoi(config[FPS].c_str());

		if (checkConfig(OUTPUT))
			output = config[OUTPUT];

		if (checkConfig(EVALUATE))
			evaluate = config[EVALUATE];
	}



	Config(string name = "config.txt") {
		ifstream fi(name);
		string line;
		while (getline(fi, line)){
			stringstream ss(line);
			string key, value;
			getline(ss, key, '=');
			getline(ss, value);
			config[key] = value;
		}
		init();
		//cout << video << " " << width << " " << height << endl;
	}

	void parseArgument(string s){
		s = s.substr(2);
		for (auto & c: s) c = toupper(c);
		stringstream ss(s);
		string key, value;
		getline(ss, key, '=');
		getline(ss, value);
		config[key] = value;
	}

	void debug(){
		for(auto p: config)
			cerr << p.first << " " << p.second << endl;
	}

	int getWidth(){
		return width;		
	}
	
	int getHeight(){
		return height;		
	}
	
	int getStep(){
		return step;
	}
	
	int getFps(){
		return fps;
	}
	
	string getVideo(){
		return video;
	}
	
	string getOutput(){
		return output;
	}

	string getEvaluate() {
		return evaluate;
	}

	bool isEvaluated() {
		return evaluate != "NONE";
	}
	
	bool isOutput(){
		return output != "NONE";
	}
	
	Detector* getDetector(){
		if (detector == "DEFAULT")
			return new DefaultDetector;
		
		if (detector == "TEST")
			return new TestDetector;
		
		if (detector == "DPM")
			return new PartBasedDetector("Models/inriaperson.xml");

		return new DefaultDetector;
	}
	
	Tracker* getTracker(){
		if (tracker == "DEFAULT")
			return new DefaultTracker;
		
		if (tracker == "MEANSHIFT")
			return new MeanshiftTracker;

		if (tracker == "CMT")
			return new CmtTracker;
			
		return new DefaultTracker;
	}
	
};

const string Config::VIDEO = "VIDEO";
const string Config::WIDTH = "WIDTH";
const string Config::HEIGHT = "HEIGHT";
const string Config::DETECTOR = "DETECTOR";
const string Config::TRACKER = "TRACKER";
const string Config::STEP = "STEP";
const string Config::FPS = "FPS";
const string Config::OUTPUT = "OUTPUT";
const string Config::EVALUATE = "EVALUATE";

#endif