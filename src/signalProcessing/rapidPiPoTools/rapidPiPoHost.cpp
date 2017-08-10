#include "rapidPiPoHost.h"
#include <iostream>
#include <fstream>

//=========================== H O S T === U T I L S ==========================//

/*
static const unsigned int maxWordLen = 256;

static bool getPiPoInstanceAndAttrName(const char *attrName,
                                       char *instanceName,
                                       char *pipoAttrName)
{
  const char *dot = std::strrchr(attrName, '.');

  if (dot != NULL)
  {
    long pipoAttrNameLen = dot - attrName;
    std::strcpy(pipoAttrName, dot + 1);

    if (pipoAttrNameLen > maxWordLen)
    {
      pipoAttrNameLen = maxWordLen;
    }

    std::strncpy(instanceName, attrName, pipoAttrNameLen);
    instanceName[pipoAttrNameLen] = '\0';

    return true;
  }

  return false;
}
//*/


static void fromPiPoStreamAttributes(PiPoStreamAttributes &src,
                                     pipoStreamAttributes &dst)
{
  unsigned int numCols = src.dims[0];
  unsigned int numLabels = src.numLabels;
  
  if (numLabels > PIPO_MAX_LABELS) {
    numLabels = PIPO_MAX_LABELS;
  }
  
  if (numLabels > numCols) {
    numLabels = numCols;
  }

  dst.hasTimeTags = src.hasTimeTags;

  if (src.rate <= MIN_PIPO_SAMPLERATE) {
    dst.rate = MIN_PIPO_SAMPLERATE;
  } else if (src.rate >= MAX_PIPO_SAMPLERATE) {
    dst.rate = MAX_PIPO_SAMPLERATE;
  } else {
    dst.rate = src.rate;
  }

  dst.rate = src.rate;
  dst.offset = src.offset;
  dst.width = src.dims[0];
  dst.height = src.dims[1];

  dst.labels = std::vector<std::string>();

  for (unsigned int i = 0; i < numLabels; ++i)
  {
    dst.labels.push_back(std::string(src.labels[i]));
  }

  dst.hasVarSize = src.hasVarSize;
  dst.domain = src.domain;
  dst.maxFrames = src.maxFrames;
}


static void toPiPoStreamAttributes(pipoStreamAttributes &src,
                                   PiPoStreamAttributes &dst)
{
  const char *labs[src.labels.size()];

  for (unsigned int i = 0; i < src.labels.size(); ++i)
  {
    labs[i] = src.labels[i].c_str();
  }

  dst = PiPoStreamAttributes(
    src.hasTimeTags,
    src.rate,
    src.offset,
    src.width,
    src.height,
    &labs[0],
    src.hasVarSize,
    src.domain,
    src.maxFrames
  );
}

//========================= H O S T === M E T H O D S ========================//

PiPoHost::PiPoHost() :
inputStreamAttrs(PIPO_MAX_LABELS),
outputStreamAttrs(PIPO_MAX_LABELS)
{
  PiPoCollection::init();
  this->out = new PiPoOut(this);
  this->graph = nullptr;
}

PiPoHost::~PiPoHost()
{
  if (this->graph != nullptr)
  {
    delete this->graph;
  }

  delete this->out;
}

bool
PiPoHost::setGraph(std::string name)
{
  if (this->graph != nullptr)
  {
    delete this->graph;
  }

  this->graph = PiPoCollection::create(name);

  if (this->graph != NULL)
  {
    this->graphName = name;
    this->graph->setReceiver((PiPo *)this->out);
    return true;
  }

  this->graph = nullptr;
  this->graphName = "undefined";
  return false;
}

void
PiPoHost::clearGraph()
{
  if (this->graph != nullptr)
  {
    delete this->graph;
    this->graph = nullptr;
  }
}


void
PiPoHost::onNewFrameOut(double time, std::vector<PiPoValue> &frame)
{
  std::cout << time << std::endl;
  std::cout << "please override this method" << std::endl;
}

std::vector<PiPoValue>
PiPoHost::getLastFrameOut()
{
  return this->out->getLastFrame();
}



int
PiPoHost::setInputStreamAttributes(pipoStreamAttributes &sa, bool propagate)
{
  toPiPoStreamAttributes(sa, inputStreamAttrs);

  if (propagate)
  {
    return this->propagateInputStreamAttributes();
  }
    
  return 0;
}

pipoStreamAttributes
PiPoHost::getOutputStreamAttributes()
{
  pipoStreamAttributes sa;
  fromPiPoStreamAttributes(this->outputStreamAttrs, sa);
  return sa;
}

int
PiPoHost::frames(double time, double weight, PiPoValue *values, unsigned int size,
                 unsigned int num)
{
  return this->graph->frames(time, weight, values, size, num);
}



// bool
// setAttr(const std::string &attrName, bool value)
// {

// }

// bool
// setAttr(const std::string &attrName, int value)
// {

// }

bool
PiPoHost::setAttr(const std::string &attrName, double value)
{
  PiPo::Attr *attr = this->graph->getAttr(attrName.c_str());
  
  if (attr != NULL)
  {
    int iAttr = attr->getIndex();
    return this->graph->setAttr(iAttr, value);
  }

  return false;
}

bool
PiPoHost::setAttr(const std::string &attrName, const std::vector<double> &values)
{
  PiPo::Attr *attr = this->graph->getAttr(attrName.c_str());

  if (attr != NULL)
  {
    int iAttr = attr->getIndex();
    double vals[values.size()];
    unsigned int i = 0;

    for (auto &value : values)
    {
      vals[i] = value;
      i++;
    }

    return this->graph->setAttr(iAttr, &vals[0], static_cast<unsigned int>(values.size()));
  }

  return false;
}

bool
PiPoHost::setAttr(const std::string &attrName, const std::string &value) // for enums
{
  PiPo::Attr *attr = this->graph->getAttr(attrName.c_str());
  
  if (attr != NULL)
  {
    // int iAttr = attr->getIndex();
    PiPo::Type type = attr->getType();
  
    if (type == PiPo::Type::Enum)
    {
      std::vector<const char *> *list = attr->getEnumList();
      
      for (int i = 0; i < list->size(); i++)
      {
        if (strcmp(list->at(i), value.c_str()) == 0)
        {
          attr->set(0, i);
          return true;
        }
      }
    }
  }

  return false;
}

std::vector<std::string>
PiPoHost::getAttrNames()
{
  std::vector<std::string> res;

  for (unsigned int i = 0; i < this->graph->getNumAttrs(); ++i)
  {
    res.push_back(this->graph->getAttr(i)->getName());
  }

  return res;
}

double
PiPoHost::getDoubleAttr(const std::string &attrName)
{
    PiPo::Attr *attr = this->graph->getAttr(attrName.c_str());
    
    if (attr != NULL) {
        // int iAttr = attr->getIndex();
        PiPo::Type type = attr->getType();
        
        if (type == PiPo::Type::Double) {
            return attr->getDbl(0);
        }
    }
    
    return 0;
}

std::vector<double>
PiPoHost::getDoubleArrayAttr(const std::string &attrName)
{
    std::vector<double> res;
    PiPo::Attr *attr = this->graph->getAttr(attrName.c_str());
    
    if (attr != NULL) {
        // int iAttr = attr->getIndex();
        PiPo::Type type = attr->getType();
        
        if (type == PiPo::Type::Double) {
            for (int i = 0; i < attr->getSize(); ++i) {
                res.push_back(attr->getDbl(i));
            }
        }
    }
    
    return res;
}

std::string
PiPoHost::getEnumAttr(const std::string &attrName)
{
    PiPo::Attr *attr = this->graph->getAttr(attrName.c_str());
    
    if (attr != NULL) {
        // int iAttr = attr->getIndex();
        PiPo::Type type = attr->getType();
        
        if (type == PiPo::Type::Enum) {
            return attr->getStr(0);
        }
    }

    return "";
}

//============================= JSON FORMATTING ==============================//

std::string
PiPoHost::getJSON()
{
  Json::Value result = toJSON();
  return result.toStyledString();
}

void
PiPoHost::writeJSON(const std::string &filepath)
{
  Json::Value root = toJSON();
  std::ofstream jsonOut;
  jsonOut.open (filepath);
  Json::StyledStreamWriter writer;
  writer.write(jsonOut, root);
  jsonOut.close();
}

bool
PiPoHost::putJSON(const std::string &jsonMessage)
{
  Json::Value parsedFromString;
  Json::Reader reader;
  bool parsingSuccessful = reader.parse(jsonMessage, parsedFromString);
  return (parsingSuccessful && fromJSON(parsedFromString));
}

bool
PiPoHost::readJSON(const std::string &filepath)
{
  Json::Value root;
  std::ifstream file(filepath);
  file >> root;
  return fromJSON(root);
}

//======================== PROTECTED HOST METHODS ============================//

Json::Value
PiPoHost::toJSON() 
{
  Json::Value root;
  Json::Value metadata;
  Json::Value pipodata;

  metadata["creator"] = "RAPID-MIX API";// C++";
  metadata["version"] = "v0.1.1"; //TODO: This should be a macro someplace
  metadata["family"] = "pipo";
    
  root["metadata"] = metadata;

  pipodata["description"] = this->graphName;
    
  Json::Value inputStream;
  inputStream["hasTimeTags"] = inputStreamAttrs.hasTimeTags;
  inputStream["rate"] = inputStreamAttrs.rate;
  inputStream["offset"] = inputStreamAttrs.offset;
  inputStream["width"] = inputStreamAttrs.dims[0];
  inputStream["height"] = inputStreamAttrs.dims[1];
  inputStream["labels"] = inputStreamAttrs.labels;
  inputStream["hasVarSize"] = inputStreamAttrs.hasVarSize;
  inputStream["domain"] = inputStreamAttrs.domain;
  inputStream["maxFrames"] = inputStreamAttrs.maxFrames;
  
  Json::Value outputStream;
  outputStream["hasTimeTags"] = outputStreamAttrs.hasTimeTags;
  outputStream["rate"] = outputStreamAttrs.rate;
  outputStream["offset"] = outputStreamAttrs.offset;
  outputStream["width"] = outputStreamAttrs.dims[0];
  outputStream["height"] = outputStreamAttrs.dims[1];
  outputStream["labels"] = outputStreamAttrs.labels;
  outputStream["hasVarSize"] = outputStreamAttrs.hasVarSize;
  outputStream["domain"] = outputStreamAttrs.domain;
  outputStream["maxFrames"] = outputStreamAttrs.maxFrames;

  Json::Value streams;
  streams["input"] =  inputStream;
  streams["output"] =  outputStream;
  
  pipodata["streamAttributes"] = streams;
    
  Json::Value params;
  int n = this->graph->getNumAttrs();
  params.resize(static_cast<Json::ArrayIndex>(n));

  for (unsigned int i = 0; i < n; ++i)
  {
    Json::Value param;
    PiPo::Attr *a = this->graph->getAttr(i);
    param["name"] = a->getName();
    param["value"] = a->getDbl(0);
    params[i] = param;
  }
    
  pipodata["parameters"] = params;

  root["pipodata"] = pipodata;

  return root;
}

bool
PiPoHost::fromJSON(Json::Value &jv)
{
  if (jv["metadata"]["family"].asString().compare("pipo") == 0 &&
    jv["pipodata"].size() > 0) {

    this->setGraph(jv["pipodata"]["description"].asString());

    Json::Value inputStream = jv["pipodata"]["streamAttributes"]["input"];
    // setInputStreamAttributes(
    //   inputStream["hasTimeTags"].getDbl()//,
    //   //...
    // );
    return true;
  }
  
  return false;
}

//========================= PRIVATE HOST METHODS =============================//

int
PiPoHost::propagateInputStreamAttributes()
{
  if (this->graph != nullptr)
  {
    return this->graph->streamAttributes(this->inputStreamAttrs.hasTimeTags,
                                         this->inputStreamAttrs.rate,
                                         this->inputStreamAttrs.offset,
                                         this->inputStreamAttrs.dims[0],
                                         this->inputStreamAttrs.dims[1],
                                         this->inputStreamAttrs.labels,
                                         this->inputStreamAttrs.hasVarSize,
                                         this->inputStreamAttrs.domain,
                                         this->inputStreamAttrs.maxFrames);
  }
    
  return 0;
}

void
PiPoHost::setOutputAttributes(bool hasTimeTags, double rate, double offset,
                              unsigned int width, unsigned int height,
                              const char **labels, bool hasVarSize,
                              double domain, unsigned int maxFrames) {
  if (labels != NULL) {
    int numLabels = width;
    
    if (numLabels > PIPO_MAX_LABELS) {
      numLabels = PIPO_MAX_LABELS;
    }
    
    for (unsigned int i = 0; i < numLabels; i++) {
      try {
        this->outputStreamAttrs.labels[i] = labels[i];        
      } catch(std::exception e) {
        this->outputStreamAttrs.labels[i] = "unnamed";
      }
    }
    
    this->outputStreamAttrs.numLabels = numLabels;
  } else {
    this->outputStreamAttrs.numLabels = 0;
  }
  
  this->outputStreamAttrs.hasTimeTags = hasTimeTags;
  this->outputStreamAttrs.rate = rate;
  this->outputStreamAttrs.offset = offset;
  this->outputStreamAttrs.dims[0] = width;
  this->outputStreamAttrs.dims[1] = height;
  this->outputStreamAttrs.hasVarSize = hasVarSize;
  this->outputStreamAttrs.domain = domain;
  this->outputStreamAttrs.maxFrames = maxFrames; 
}



// void
// PiPoHost::streamAttributesChanged(PiPo *pipo, PiPo::Attr *attr) {
//   this->propagateInputAttributes();
// }

// void
// PiPoHost::signalError(PiPo *pipo, std::string errorMsg) {
//   // todo
// }

// void
// PiPoHost::signalWarning(PiPo *pipo, std::string warningMsg) {
//   // todo
// }

//--------------------- INPUT STREAM ATTRIBUTES SETTERS ----------------------//

/*
void
PiPoHost::setInputHasTimeTags(bool hasTimeTags, bool propagate) {
  this->inputStreamAttrs.hasTimeTags = hasTimeTags;

  if (propagate) {
    this->propagateInputAttributes();
  }
}

#define MIN_PIPO_SAMPLERATE (1.0 / 31536000000.0) // once a year
#define MAX_PIPO_SAMPLERATE (96000000000.0)

void
PiPoHost::setInputFrameRate(double rate, bool propagate) {
  if (rate <= MIN_PIPO_SAMPLERATE) {
    this->inputStreamAttrs.rate = MIN_PIPO_SAMPLERATE;
  } else if (rate >= MAX_PIPO_SAMPLERATE) {
    this->inputStreamAttrs.rate = MAX_PIPO_SAMPLERATE;
  } else {
    this->inputStreamAttrs.rate = rate;
  }

  if (propagate) {
    this->propagateInputAttributes();
  }
}

void
PiPoHost::setInputFrameOffset(double offset, bool propagate) {
  this->inputStreamAttrs.offset = offset;

  if (propagate) {
    this->propagateInputAttributes();
  }
}

void
PiPoHost::setInputDims(int width, int height, bool propagate) {
  this->inputStreamAttrs.dims[0] = width;
  this->inputStreamAttrs.dims[1] = height;
  
  if (propagate) {
    this->propagateInputAttributes();
  }
}

void
PiPoHost::setInputLabels(const std::vector<std::string> &labels, bool propagate) {
  const char *labs[labels.size()];

  for (unsigned int i = 0; i < labels.size(); ++i) {
    labs[i] = labels[i].c_str();
  }

  this->inputStreamAttrs.labels = &labs[0];

  if (propagate) {
    this->propagateInputAttributes();
  }
}

void
PiPoHost::setInputHasVarSize(bool hasVarSize, bool propagate) {
  this->inputStreamAttrs.hasVarSize = hasVarSize;

  if (propagate) {
    this->propagateInputAttributes();
  }
}

void
PiPoHost::setInputDomain(double domain, bool propagate) {
  this->inputStreamAttrs.domain = domain;

  if (propagate) {
    this->propagateInputAttributes();
  }
}

void
PiPoHost::setInputMaxFrames(int maxFrames, bool propagate) {
  this->inputStreamAttrs.maxFrames = maxFrames;

  if (propagate) {
    this->propagateInputAttributes();
  }
}
//*/

//--------------------- INPUT STREAM ATTRIBUTES GETTERS ----------------------//

/*
bool
PiPoHost::getInputHasTimeTags() {
  return this->inputStreamAttrs.hasTimeTags;
}

double
PiPoHost::getInputFrameRate() {
  return this->inputStreamAttrs.rate;
}

double
PiPoHost::getInputFrameOffset() {
  return this->inputStreamAttrs.offset;
}

void
PiPoHost::getInputDims(int &width, int &height) {
  width = this->inputStreamAttrs.dims[0];
  height = this->inputStreamAttrs.dims[1];
}

void
PiPoHost::getInputLabels(std::vector<std::string> &labels) {
  //for (unsigned int i = 0; i < )
}

bool
PiPoHost::getInputHasVarSize() {
  return this->inputStreamAttrs.hasVarSize;
}

double
PiPoHost::getInputDomain() {
  return this->inputStreamAttrs.domain;
}

int
PiPoHost::getInputMaxFrames() {
  return this->inputStreamAttrs.maxFrames;
}
//*/

//--------------------- OUTPUT STREAM ATTRIBUTES GETTERS ---------------------//

// void
// PiPoHost::setOutputAttributes(bool hasTimeTags, double rate, double offset,
//                               unsigned int width, unsigned int height,
//                               const char **labels, bool hasVarSize,
//                               double domain, unsigned int maxFrames) {
//   if (labels != NULL) {
//     int numLabels = width;
    
//     if (numLabels > PIPO_MAX_LABELS) {
//       numLabels = PIPO_MAX_LABELS;
//     }
    
//     for (unsigned int i = 0; i < numLabels; i++) {
//       try {
//         this->outputStreamAttrs.labels[i] = labels[i];        
//       } catch(std::exception e) {
//         this->outputStreamAttrs.labels[i] = "unnamed";
//       }
//     }
    
//     this->outputStreamAttrs.numLabels = numLabels;
//   } else {
//     this->outputStreamAttrs.numLabels = 0;
//   }
  
//   this->outputStreamAttrs.hasTimeTags = hasTimeTags;
//   this->outputStreamAttrs.rate = rate;
//   this->outputStreamAttrs.offset = offset;
//   this->outputStreamAttrs.dims[0] = width;
//   this->outputStreamAttrs.dims[1] = height;
//   this->outputStreamAttrs.hasVarSize = hasVarSize;
//   this->outputStreamAttrs.domain = domain;
//   this->outputStreamAttrs.maxFrames = maxFrames; 
// }

/*
bool
PiPoHost::getOutputHasTimeTags() {
  return this->outputStreamAttrs.hasTimeTags;
}

double
PiPoHost::getOutputFrameRate() {
  return this->outputStreamAttrs.rate;
}

double
PiPoHost::getOutputFrameOffset() {
  return this->outputStreamAttrs.offset;
}

void
PiPoHost::getOutputDims(int &width, int &height) {
  width = this->outputStreamAttrs.dims[0];
  height = this->outputStreamAttrs.dims[1];
}

void
PiPoHost::getOutputLabels(std::vector<std::string> &labels) {
  labels.clear();

  for (unsigned int i = 0; this->outputStreamAttrs.numLabels; ++i) {

    if (this->outputStreamAttrs.labels[i] != NULL) {
      labels.push_back(std::string(this->outputStreamAttrs.labels[i]));
    } else {
      labels.push_back("unnamed");
    }
  }
}

bool
PiPoHost::getOutputHasVarSize() {
  return this->outputStreamAttrs.hasVarSize;
}

double
PiPoHost::getOutputDomain() {
  return this->outputStreamAttrs.domain;
}

int
PiPoHost::getOutputMaxFrames() {
  return this->outputStreamAttrs.maxFrames;
}
//*/
