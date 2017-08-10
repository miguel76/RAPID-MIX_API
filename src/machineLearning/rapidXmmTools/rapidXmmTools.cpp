#include "rapidXmmTools.h"
#include "trainingData.h"
#include "machineLearning.h"

static bool trainingData2xmmTrainingSet(const rapidmix::trainingData& data, xmm::TrainingSet& set) {
  if (data.trainingSet.size() <= 1) {
    // no recorded phrase (only default one)
    return false;
  }

  if (data.trainingSet.size() > 1 && data.trainingSet[1].elements.size() == 0) {
    // empty recorded phrase
    return false;
  }

  rapidmix::trainingData::element el = data.trainingSet[1].elements[0];
  int dimIn = static_cast<int>(el.input.size());
  int dimOut = static_cast<int>(el.output.size());

  // translate and return true if data and set are compatible
  // don't translate and return false otherwise

  if (dimOut > 0 != set.bimodal()) {
    return false;
  }

  xmm::Phrase xp;

  if (set.bimodal()) {
    set.dimension.set(dimIn + dimOut);
    set.dimension_input.set(dimIn);
    xp = xmm::Phrase(xmm::MemoryMode::OwnMemory, xmm::Multimodality::Bimodal);
    xp.dimension.set(dimIn + dimOut);
    xp.dimension_input.set(dimIn);
  } else {
    set.dimension.set(dimIn);
    set.dimension_input.set(0);
    xp = xmm::Phrase(xmm::MemoryMode::OwnMemory, xmm::Multimodality::Unimodal);
    xp.dimension.set(dimIn);
    xp.dimension_input.set(0);
  }

  set.clear();

  //for (auto &phrase : data.trainingSet) {
  // changed to look starting from index 1
  // because phrase at index 0 is for unordered elements
  for (int i = 1; i < data.trainingSet.size(); ++i) {
    const rapidmix::trainingData::phrase &phrase = data.trainingSet[i];
    xp.clear();
    xp.label.set(phrase.label);

    for (auto &element : phrase.elements) {
      std::vector<float> obsIn(element.input.begin(), element.input.end());
      std::vector<float> obsOut(element.output.begin(), element.output.end());
      std::vector<float> obs;
      obs.insert(obs.end(), obsIn.begin(), obsIn.end());
      obs.insert(obs.end(), obsOut.begin(), obsOut.end());
      xp.record(obs);
    }

    set.addPhrase(static_cast<int>(set.size()), xp);
  }

  return true;
}

//=============================== xmmTool ====================================//

template <class SingleClassModel, class Model>
bool xmmTool<SingleClassModel, Model>::train(const rapidmix::trainingData& newTrainingData) {
  if (trainingData2xmmTrainingSet(newTrainingData, set)) {
    model.train(&set);
    model.reset();
    return true;
  }

  return false;
}

////////// private JSON data manipulation methods :

//TODO: add a type field (gmm/gmr/hmm/hmr) in metadata when family is xmm
template <class SingleClassModel, class Model>
Json::Value xmmTool<SingleClassModel, Model>::toJSON(/*std::string modelType*/) {
  Json::Value root;
  Json::Value metadata;
  Json::Value modelSet;

  metadata["creator"] = "Rapid API C++";
  metadata["version"] = "v0.1.1"; //TODO: This should be a macro someplace
  metadata["family"] = "xmm";
  root["metadata"] = metadata;

  modelSet.append(model.toJson());
  root["modelSet"] = modelSet;

  return root;
}

template <class SingleClassModel, class Model>
bool xmmTool<SingleClassModel, Model>::fromJSON(Json::Value &jm) {
  if (jm["metadata"]["family"].asString().compare("xmm") == 0 &&
      jm["modelSet"].size() > 0) {
    model.fromJson(jm["modelSet"][0]);
    model.reset();
    return true;
  }

  return false;
}

////////// public JSON file manipulation interface :

template <class SingleClassModel, class Model>
std::string xmmTool<SingleClassModel, Model>::getJSON() {
  Json::Value result = toJSON();
  return result.toStyledString();
}

template <class SingleClassModel, class Model>
void xmmTool<SingleClassModel, Model>::writeJSON(const std::string &filepath) {
  Json::Value root = toJSON();
  std::ofstream jsonOut;
  jsonOut.open (filepath);
  Json::StyledStreamWriter writer;
  writer.write(jsonOut, root);
  jsonOut.close();
}

template <class SingleClassModel, class Model>
bool xmmTool<SingleClassModel, Model>::putJSON(const std::string &jsonMessage) {
  Json::Value parsedFromString;
  Json::Reader reader;
  bool parsingSuccessful = reader.parse(jsonMessage, parsedFromString);
  return (parsingSuccessful && fromJSON(parsedFromString));
}

template <class SingleClassModel, class Model>
bool xmmTool<SingleClassModel, Model>::readJSON(const std::string &filepath) {
  Json::Value root;
  std::ifstream file(filepath);
  file >> root;
  return fromJSON(root);
}

//============================== xmmGmmTool ==================================//

std::vector<double> xmmGmmTool::run(const std::vector<double>& inputVector) {
  xmmTool::preProcess(inputVector);
  return model.results.smoothed_normalized_likelihoods;
}

//============================== xmmGmrTool ==================================//

std::vector<double> xmmGmrTool::run(const std::vector<double>& inputVector) {
  xmmTool::preProcess(inputVector);
  std::vector<float> *res = &model.results.output_values;
  std::vector<double> dRes(res->begin(), res->end());
  return dRes;
}

//============================== xmmHmmTool ==================================//

std::vector<double> xmmHmmTool::run(const std::vector<double>& inputVector) {
  xmmTool::preProcess(inputVector);
  std::vector<double> res;

  int i(0);
  for (auto &m : model.models) {
    res.push_back(model.results.smoothed_normalized_likelihoods[i]);
    res.push_back(m.second.results.progress);
    i++;
  }

  return res;
}

//============================== xmmHmrTool ==================================//

std::vector<double> xmmHmrTool::run(const std::vector<double>& inputVector) {
  xmmTool::preProcess(inputVector);
  std::vector<float> *res = &model.results.output_values;
  std::vector<double> dRes(res->begin(), res->end());
  return dRes;
}

///////////////////////////////////////////////////////////////////////////
///// generic train method and forward declaration of specialized templates 
///////////////////////////////////////////////////////////////////////////

template <class MachineLearningModule>
bool rapidmix::machineLearning<MachineLearningModule>::train(const trainingData &newTrainingData) {
    return MachineLearningModule::train(newTrainingData);
}

template class rapidmix::machineLearning<xmmGmmTool>;
template class rapidmix::machineLearning<xmmGmrTool>;
template class rapidmix::machineLearning<xmmHmmTool>;
template class rapidmix::machineLearning<xmmHmrTool>;

