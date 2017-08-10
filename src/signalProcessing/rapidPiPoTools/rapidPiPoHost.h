#ifndef _RAPID_PIPO_HOST_H_
#define _RAPID_PIPO_HOST_H_

#include "PiPo.h"
#include "PiPoHost.h"
#include "PiPoCollection.h"

//#ifdef EXTERNAL_JSONCPP_PATH
#define EXTERNAL_JSONCPP_PATH "json.h"
#include EXTERNAL_JSONCPP_PATH
//#endif /* EXTERNAL_JSONCPP_PATH */

#define MIN_PIPO_SAMPLERATE (1.0 / 31536000000.0) /* once a year */
#define MAX_PIPO_SAMPLERATE (96000000000.0)

#define PIPO_OUT_RING_SIZE 2

struct pipoStreamAttributes {
  pipoStreamAttributes() : // default parameters suited for audio
  hasTimeTags(false),
  rate(MIN_PIPO_SAMPLERATE),
  offset(0),
  width(1),
  height(1),
  labels({ "" }),
  hasVarSize(false),
  domain(0),
  maxFrames(1) {}

  bool hasTimeTags;
  double rate;
  double offset;
  unsigned int width;
  unsigned int height;
  std::vector<std::string> labels;
  bool hasVarSize;
  double domain;
  unsigned int maxFrames;
};


//class PiPoObserver;
class PiPoOut;

//================================ H O S T ===================================//

class PiPoHost : public PiPo::Parent {
    friend class PiPoOut;
    
private:
    PiPo *graph;
    std::string graphName;
    PiPoOut *out;
    // PiPoObserver *obs;
    PiPoStreamAttributes inputStreamAttrs;
    PiPoStreamAttributes outputStreamAttrs;
    
public:
    // PiPoHost(PiPoObserver *obs);
    PiPoHost();
    ~PiPoHost();
    
    // PiPoObserver *getObserver();

    virtual bool setGraph(std::string name);
    virtual void clearGraph();

    // override this method when inheriting !!!
    virtual void onNewFrameOut(double time, std::vector<PiPoValue> &frame);
    virtual std::vector<PiPoValue> getLastFrameOut();
    
    virtual int setInputStreamAttributes(pipoStreamAttributes &sa, bool propagate = true);
    virtual pipoStreamAttributes getOutputStreamAttributes();

    virtual int frames(double time, double weight, PiPoValue *values, unsigned int size,
                       unsigned int num);

    // virtual bool setAttr(const std::string &attrName, bool value);
    // virtual bool setAttr(const std::string &attrName, int value);
    virtual bool setAttr(const std::string &attrName, double value);
    virtual bool setAttr(const std::string &attrName, const std::vector<double> &values);
    virtual bool setAttr(const std::string &attrName, const std::string &value); // for enums

    // virtual const std::vector<std::string>& getAttrNames();
    // virtual bool isBoolAttr(const std::string &attrName);
    // virtual bool isEnumAttr(const std::string &attrName);
    // virtual bool isIntAttr(const std::string &attrName);
    // virtual bool isIntArrayAttr(const std::string &attrName);
    // virtual bool isFloatAttr(const std::string &attrName);
    // virtual bool isFloatArrayAttr(const std::string &attrName);
    // virtual bool isStringAttr(const std::string &attrName);
    
    virtual std::vector<std::string> getAttrNames();

    virtual double getDoubleAttr(const std::string &attrName);
    virtual std::vector<double> getDoubleArrayAttr(const std::string &attrName);
    virtual std::string getEnumAttr(const std::string &attrName);

    /** Get a JSON representation of the model in the form of a styled string */
    virtual std::string getJSON();
    /** Write a JSON model description to specified file path */
    virtual void writeJSON(const std::string &filepath);
    /** configure empty model with string. See getJSON() */
    virtual bool putJSON(const std::string &jsonMessage);
    /** read a JSON file at file path and build a modelSet from it */
    virtual bool readJSON(const std::string &filepath);


    // int streamAttributes(bool hasTimeTags, double rate, double offset,
    //                      unsigned int width, unsigned int height,
    //                      const std::vector<std::string> &labels,
    //                      bool hasVarSize, double domain, unsigned int maxFrames,
    //                      bool propagate = true);

    // void propagateInputAttributes();
    

    // void streamAttributesChanged(PiPo *pipo, PiPo::Attr *attr);
    // void signalError(PiPo *pipo, std::string errorMsg);
    // void signalWarning(PiPo *pipo, std::string warningMsg);
    
    /*
    void setInputHasTimeTags(bool hasTimeTags, bool propagate = true);
    void setInputFrameRate(double rate, bool propagate = true);
    void setInputFrameOffset(double offset, bool propagate = true);
    void setInputDims(int width, int height, bool propagate = true);
    void setInputLabels(const std::vector<std::string> &labels, bool propagate = true);
    void setInputHasVarSize(bool hasVarSize, bool propagate = true);
    void setInputDomain(double domain, bool propagate = true);
    void setInputMaxFrames(int maxFrames, bool propagate = true);
    
    bool getInputHasTimeTags();
    double getInputFrameRate();
    double getInputFrameOffset();
    void getInputDims(int &width, int &height);
    void getInputLabels(std::vector<std::string> &labels);
    bool getInputHasVarSize();
    double getInputDomain();
    int getInputMaxFrames();
    
    bool getOutputHasTimeTags();
    double getOutputFrameRate();
    double getOutputFrameOffset();
    void getOutputDims(int &width, int &height);
    void getOutputLabels(std::vector<std::string> &labels);
    bool getOutputHasVarSize();
    double getOutputDomain();
    int getOutputMaxFrames();

    // void setPiPoParam(PiPoParam *param);
    //*/
protected:
  Json::Value toJSON();
  bool fromJSON(Json::Value &jv);

private:
    int propagateInputStreamAttributes();
    void setOutputAttributes(bool hasTimeTags, double rate, double offset,
                             unsigned int width, unsigned int height,
                             const char **labels, bool hasVarSize,
                             double domain, unsigned int maxFrames);
    
};

//================================= PiPoOut ==================================//

 class PiPoOut : public PiPo {
private:
    PiPoHost *host;
    std::atomic<int> writeIndex, readIndex;
    std::vector<std::vector<PiPoValue>> ringBuffer;
    // std::function<void(std::vector<PiPoValue>, PiPoObserver *rpo)> frameCallback;
    // std::function<void(std::vector<PiPoValue>)> simpleFrameCallback;
    
public:
    PiPoOut(PiPoHost *host) :
    PiPo((PiPo::Parent *)host) {
        this->host = host;
        writeIndex = 0;
        readIndex = 0;
        ringBuffer.resize(PIPO_OUT_RING_SIZE);
    }
    
    ~PiPoOut() {}
    
    int streamAttributes(bool hasTimeTags,
                         double rate, double offset,
                         unsigned int width, unsigned int height,
                         const char **labels, bool hasVarSize,
                         double domain, unsigned int maxFrames) {
        
        this->host->setOutputAttributes(hasTimeTags, rate, offset, width, height,
                                        labels, hasVarSize, domain, maxFrames);
        
        for (int i = 0; i < PIPO_OUT_RING_SIZE; ++i) {
            ringBuffer[i].resize(width * height);
        }
        
        return 0;
    }
    
    int frames(double time, double weight, float *values,
               unsigned int size, unsigned int num) {
        
        if (num > 0) {
            for (int i = 0; i < num; ++i) {
                
                for (int j = 0; j < size; ++j) {
                    ringBuffer[writeIndex][j] = values[i * size + j];
                }
                
                // atomic swap ?
                writeIndex = 1 - writeIndex;
                readIndex = 1 - writeIndex;
                
                this->host->onNewFrameOut(time, ringBuffer[readIndex]);
                
                if (writeIndex + 1 == PIPO_OUT_RING_SIZE) {
                    writeIndex = 0;
                } else {
                    writeIndex++;
                }
            }
        }
        
        return 0;
    }
    
    //void setFrameCallback(std::function<void(std::vector<PiPoValue>,
    //                                         PiPoObserver *obs)> f) {
    //    frameCallback = f;
    //}
    
    // void setSimpleFrameCallback(std::function<void(std::vector<PiPoValue>)> f) {
    //     simpleFrameCallback = f;
    // }
    
    std::vector<PiPoValue> getLastFrame() {
        std::vector<PiPoValue> f;
        
        if (readIndex > -1) {
            f = ringBuffer[readIndex];
        }
        
        return f;
    }
};

#endif /* _RAPID_PIPO_HOST_H_ */
