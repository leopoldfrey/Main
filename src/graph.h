#ifndef graph_h
#define graph_h

#include "leapPoint.h"
#include "ofApp.h"

#define GRAPH_P_X 0
#define GRAPH_P_Y 1
#define GRAPH_P_Z 2
#define GRAPH_I_X 3
#define GRAPH_I_Y 4
#define GRAPH_I_Z 5
#define GRAPH_P_N 6
#define GRAPH_I_N 7


typedef struct _timePoint {
    int time;
    leapPoint point;
    float normP;
    float normI;
} timePoint;

class Graph {
public:
    string name;
    timePoint minV;
    timePoint maxV;
    timePoint meanV;
    //float meanNormP;
    //float meanNormI;
    timePoint medianV;
    int maxTime;
    vector<timePoint> histo;
    bool hasData;
    
    void clear() {
        histo.clear();
        maxTime = 0;
        hasData = false;
    }
    
    void setName(string n) {
        name = n;
    }
    
    void setDuration(int d) {
        maxTime = d;
    }
    
    void add(int t, leapPoint p) {
        float nP = p.norm(false);
        float nI = p.norm(true);
        timePoint val = {t, p, nP, nI};
        histo.push_back(val);
        maxV = minV = val;
    }
    
    void calc() {
        
        leapPoint p;
        for(int i = 0 ; i < histo.size() ; i++)
        {
            timePoint p = histo[i];
            
            minV.point.palm.x = min(minV.point.palm.x, p.point.palm.x);
            minV.point.palm.y = min(minV.point.palm.y, p.point.palm.y);
            minV.point.palm.z = min(minV.point.palm.z, p.point.palm.z);
            minV.point.index.x = min(minV.point.index.x, p.point.index.x);
            minV.point.index.y = min(minV.point.index.y, p.point.index.y);
            minV.point.index.z = min(minV.point.index.z, p.point.index.z);
            minV.normP = min(minV.normP, p.normP);
            minV.normI = min(minV.normI, p.normI);

            maxV.point.palm.x = max(maxV.point.palm.x, p.point.palm.x);
            maxV.point.palm.y = max(maxV.point.palm.y, p.point.palm.y);
            maxV.point.palm.z = max(maxV.point.palm.z, p.point.palm.z);
            maxV.point.index.x = max(maxV.point.index.x, p.point.index.x);
            maxV.point.index.y = max(maxV.point.index.y, p.point.index.y);
            maxV.point.index.z = max(maxV.point.index.z, p.point.index.z);
            maxV.normP = max(maxV.normP, p.normP);
            maxV.normI = max(maxV.normI, p.normI);
        }
        
        hasData = (minV.point != leapZero() || maxV.point != leapZero());
        mean();
        //median();
        ofLog() << "Calc graph " << name << " data(" << hasData << ")";
        ofLog() << "min(" << toString(minV, 2) << ")";
        ofLog() << "max(" << toString(maxV, 2) << ")";
        ofLog() << "mean(" << toString(meanV,2) << ")";
        //ofLog() << "median(" << toString(medianV,2) << ")";
    }
    
    void draw(int x, int y, int w, int h, float start, float end, int mode, ofColor c, ofTrueTypeFont f) {
        if(!hasData)
            return;
        
        float min, max;
        /*
         switch(dV) {
            case GRAPH_P_X:
                min = -50;//minV.palm.x;
                max = 50;//maxV.palm.x;
                break;
            case GRAPH_P_Y:
                min = -40;//minV.palm.y;
                max = 40;//maxV.palm.y;
                break;
            case GRAPH_P_Z:
                min = -30;//minV.palm.z;
                max = 30;//maxV.palm.z;
                break;
            case GRAPH_I_X:
                min = -50;//minV.index.x;
                max = 50;//maxV.index.x;
                break;
            case GRAPH_I_Y:
                min = -40;//minV.index.y;
                max = 40;//maxV.index.y;
                break;
            case GRAPH_I_Z:
                min = -50;//minV.index.z;
                max = 50;//maxV.index.z;
                break;
        }//*/
        
        min = mode == GRAPH_P_N || mode == GRAPH_I_N ? 0 : -100.;
        max = 100.;
        
        // AXES
        
        ofSetColor(255,255,255,255);
        ofFill();
        ofDrawRectangle(x, y, w, h);
        
        ofSetColor(50,50,50,255);
        
        glBegin(GL_LINES);
        glVertex2f(x, ofMap(0., min, max, y+h, y));
        glVertex2f(x+w, ofMap(0., min, max, y+h, y));
        glEnd();//*/
        
        // HISTO
        
        ofSetColor(c);
        f.drawString(getModeName(mode), x+10, y+20);
        f.drawString("min:"+ofToString(getValue(minV,mode),2), x+120, y+20);
        f.drawString("max:"+ofToString(getValue(maxV,mode),2), x+220, y+20);
        f.drawString("mean:"+ofToString(getValue(meanV,mode),2), x+320, y+20);
        //f.drawString("median:"+ofToString(getValue(medianV,mode),2), x+420, y+20);
        
        float v, curV, prevV;
        int t, curT, prevT;
        leapPoint p;
        timePoint val;

        ofEnableSmoothing();
        glBegin(GL_LINE_STRIP);
        
        prevT = x;
        prevV = ofMap(0, min, max, y+h, y);
        
        bool first = true;
        for(int i = 0 ; i < histo.size() ; i++) {
            t = histo[i].time;
            v = getValue(histo[i], mode);
            
            curT = ofMap(t, 0, maxTime, x, x+w);
            curV = ofMap(v, min, max, y+h, y);
            
            if(curV != prevV)//glVertex2f(prevT, prevV);
            {
                if(first)
                    first = false;
                else
                    glVertex2f(curT, curV);
                
            }
            prevV = curV;
            prevT = curT;
        }
        
        glEnd();
        
        
        ofFill();
        
    }
    
    void setValue(timePoint p, int mode, float v) {
        switch(mode) {
            default:
            case GRAPH_P_X:
                p.point.palm.x = v;
            case GRAPH_P_Y:
                p.point.palm.y = v;
            case GRAPH_P_Z:
                p.point.palm.z = v;
            case GRAPH_I_X:
                p.point.index.x = v;
            case GRAPH_I_Y:
                p.point.index.y = v;
            case GRAPH_I_Z:
                p.point.index.z = v;
            case GRAPH_P_N:
                p.normP = v;
            case GRAPH_I_N:
                p.normI = v;
        }
    }
    
    float getValue(timePoint tp, int mode) {
        switch(mode) {
            default:
            case GRAPH_P_X:
                return tp.point.palm.x;
            case GRAPH_P_Y:
                return tp.point.palm.y;
            case GRAPH_P_Z:
                return tp.point.palm.z;
            case GRAPH_I_X:
                return tp.point.index.x;
            case GRAPH_I_Y:
                return tp.point.index.y;
            case GRAPH_I_Z:
                return tp.point.index.z;
            case GRAPH_P_N:
                return tp.normP;
            case GRAPH_I_N:
                return tp.normI;
        }
    }
    
    /*float getValue(leapPoint p, int mode) {
        switch(mode) {
            default:
            case GRAPH_P_X:
                return p.palm.x;
            case GRAPH_P_Y:
                return p.palm.y;
            case GRAPH_P_Z:
                return p.palm.z;
            case GRAPH_I_X:
                return p.index.x;
            case GRAPH_I_Y:
                return p.index.y;
            case GRAPH_I_Z:
                return p.index.z;
            case GRAPH_P_N:
                return 0;
            case GRAPH_I_N:
                return 0;
        }
    }//*/
    
    string getModeName(int mode) {
        switch(mode) {
            case GRAPH_P_X:
                return "Palm X";
            case GRAPH_P_Y:
                return "Palm Y";
            case GRAPH_P_Z:
                return "Palm Z";
            case GRAPH_I_X:
                return "Index X";
            case GRAPH_I_Y:
                return "Index Y";
            case GRAPH_I_Z:
                return "Index Z";
            case GRAPH_P_N:
                return "Palm Norm";
            case GRAPH_I_N:
                return "Index Norm";
        }
    }
    
    /*
    void median()
    {
        size_t size = histo.size();
        if (size == 0)
        {
            meanV.point = leapZero();
            return;
        }
        
        for(int mode = GRAPH_P_X ; mode <= GRAPH_I_N ; mode++)
        {
            vector<float> vals;
            for(int i = 0 ; i < size ; i++)
                vals.push_back(getValue(histo[i],mode));
            
            setValue(medianV, mode, Median(vals.begin(),vals.end()));
        }
    }//*/
    
    /*
    template <typename It>
    typename std::iterator_traits<It>::value_type Median(It begin, It end)
    {
        auto size = std::distance(begin, end);
        std::nth_element(begin, begin + size / 2, end);
        return *std::next(begin, size / 2);
    }//*/
    
    /*float median(vector<float> &v, size_t size)
    {
        double median;
        
        sort(v.begin(), v.end());
            
        if (size  % 2 == 0)
        {
            median = (v[size / 2 - 1] + v[size / 2]) / 2;
        } else  {
            median = v[size / 2];
        }
            
        return median;
    }//*/
    
    void mean() {
        typedef vector<int>::size_type vec_sz;
        
        vec_sz size = histo.size();
        if (size == 0)
        {
            meanV.point = leapZero();
            return;
        }
        leapPoint sum;
        float normPsum, normIsum;
        for(int i = 0 ; i < size ; i++)
        {
            sum += histo[i].point;
            normPsum += histo[i].normP;
            normIsum += histo[i].normI;
        }
        
        meanV.point = sum / size;
        meanV.normP = normPsum / size;
        meanV.normI = normIsum / size;
    }
    
    string toString(timePoint tp, int p) {
        return tp.point.toString(p) + ", " + ofToString(tp.normP, p) + ", " + ofToString(tp.normI, p);
    }
};

#endif /* graph_h */
