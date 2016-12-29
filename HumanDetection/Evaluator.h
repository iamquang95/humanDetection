#ifndef _MC_EVALUATOR_H
#define _MC_EVALUATOR_H 1

#include "Matching.h"
#include <opencv2/opencv.hpp>

using namespace std;

class Evaluator {
	private:
		int truePositive, falsePositive, falseNegative;
	public:
	
		Evaluator(){
			truePositive = falsePositive = falseNegative = 0;
		}
	
		int getTruePositive(){
			return truePositive;
		}
		
		int getFalsePositive(){
			return falsePositive;
		}
		
		int getFalseNegative(){
			return falseNegative;
		}
		
		double getValue(int up, int down){
			if (down == 0)
				return 1;
			
			return 1.0*up / down;
		}
		
		double getPrecision(){
			int up = truePositive;
			int down = truePositive + falsePositive;
			return getValue(up, down);
		}
		
		double getRecall(){
			int up = truePositive;
			int down = truePositive + falseNegative;
			return getValue(up, down);
		}
		
		double getF1(){
			int up = truePositive * 2;
			int down = truePositive * 2 + falsePositive + falseNegative;
			return getValue(up, down);
		}
		
		void debug(){
			cout << getPrecision() << " " << getRecall() << " " << getF1() << endl;
		}
		
		void update(const vector<Rect>& predicted, const vector<Rect>& groundTruth){
			VVI a;
			VI mr(predicted.size()), mc(groundTruth.size());
			
			for(const Rect& pre: predicted){
				VI aa;
				for(const Rect& truth: groundTruth){
					double overlap = (pre & truth).area();
					double full = (double)(pre.area() + truth.area()) - overlap;
					if (overlap/full > 0.5)
						aa.push_back(1);
					else
						aa.push_back(0);
					
					cout << pre.tl() << " " << pre.width << " " << pre.height << endl;
					cout << truth.tl() << " " << truth.width << " " << truth.height << endl;
					cout << overlap << " " << full << endl;
					cout << "-------------" << endl;
					
				}
				a.push_back(aa);
				
			}
						
			int numMatch = BipartiteMatching(a, mr, mc);						
			truePositive += numMatch;
			falsePositive += (int)predicted.size() - numMatch;
			falseNegative += (int)groundTruth.size() - numMatch;
		}
		
	
};

#endif