#ifndef _RAPID_XMM_TOOLS_H_
#define _RAPID_XMM_TOOLS_H_

// this works !
#ifndef EXTERNAL_JSONCPP_PATH
// #define EXTERNAL_JSONCPP_PATH "../../../../json/json.h" // relative to xmmJson.h
#define EXTERNAL_JSONCPP_PATH "json.h"
#endif /* EXTERNAL_JSONCPP_PATH */

#include "xmm.h"

// forward declaration
namespace rapidmix { class trainingData; }

// original defined in xmmModelConfiguration.hpp
enum xmmRegressionEstimator {
  xmmLikeliestRegression,
  xmmMixtureRegression
};

// original defined in xmmGaussianDistribution.hpp
enum xmmCovarianceMode {
  xmmFullCovariance,
  xmmDiagonalCovariance
};

// original defined in xmmHmmParameters.hpp
enum xmmHmmTransitionMode {
  xmmHmmLeftRightTransition,
  xmmHmmErgodicTransition
};

// original defined in xmmHmmParameters.hpp
enum xmmHmmRegressionEstimator {
  xmmHmmFullRegression,
  xmmHmmWindowedRegression,
  xmmHmmLikeliestRegression
};

// this is the optional argument of machineLearning<xmmWhateverTool>'s constructors
struct xmmToolConfig {
  xmmToolConfig() :
  gaussians(1),
  relativeRegularization(0.01),
  absoluteRegularization(0.01),
  regressionEstimator(xmmMixtureRegression),
  covarianceMode(xmmFullCovariance),
  states(5),
  hierarchical(true),
  hmmTransitionMode(xmmHmmErgodicTransition),
  hmmRegressionEstimator(xmmHmmFullRegression),
  likelihoodWindow(5) {}

  // general parameters :
  uint32_t gaussians;
  float relativeRegularization;
  float absoluteRegularization;
  xmmRegressionEstimator regressionEstimator;
  xmmCovarianceMode covarianceMode;

  // hmm specific :
  uint32_t states;
  bool hierarchical;
  xmmHmmTransitionMode hmmTransitionMode;
  xmmHmmRegressionEstimator hmmRegressionEstimator;

  // run-time parameter :
  uint32_t likelihoodWindow;
};

//========================== template base class =============================//

template <class SingleClassModel, class Model>
class xmmTool {
protected:
  xmmTool(bool bimodal) {
    model = Model(bimodal);
    model.configuration.multithreading = xmm::MultithreadingMode::Sequential;
    model.configuration.changed = true;

    set = xmm::TrainingSet(xmm::MemoryMode::OwnMemory,
      bimodal
      ? xmm::Multimodality::Bimodal
      : xmm::Multimodality::Unimodal
    );
  }

  virtual void preProcess(const std::vector<double> &inputVector) {
    std::vector<float> fv(inputVector.begin(), inputVector.end());
    model.filter(fv);
  }

public:
  virtual ~xmmTool() {}

  virtual bool train(const rapidmix::trainingData &newTrainingData);

  virtual bool reset() {
    model.reset();
    return true;
  }

  /** Get a JSON representation of the model in the form of a styled string */
  virtual std::string getJSON();
  /** Write a JSON model description to specified file path */
  virtual void writeJSON(const std::string &filepath);
  /** configure empty model with string. See getJSON() */
  virtual bool putJSON(const std::string &jsonMessage);
  /** read a JSON file at file path and build a modelSet from it */
  virtual bool readJSON(const std::string &filepath);

protected:
  Model model;
  xmm::TrainingSet set;

  Json::Value toJSON();
  bool fromJSON(Json::Value &jm);
};

//======================= base class for GMM models ==========================//

template <class SingleClassModel, class Model>
class xmmStaticTool : public xmmTool<SingleClassModel, Model> {
protected:
  xmmStaticTool(xmmToolConfig cfg, bool bimodal) :
  xmmTool<SingleClassModel, Model>(bimodal) {
    xmm::Configuration<SingleClassModel>& mCfg = this->model.configuration;

    mCfg.gaussians.set(cfg.gaussians);
    mCfg.relative_regularization.set(cfg.relativeRegularization);
    mCfg.absolute_regularization.set(cfg.absoluteRegularization);
    
    xmm::MultiClassRegressionEstimator mcre;
    switch (cfg.regressionEstimator) {
      case xmmLikeliestRegression:
        mcre = xmm::MultiClassRegressionEstimator::Likeliest;
      case xmmMixtureRegression:
      default:
        mcre = xmm::MultiClassRegressionEstimator::Mixture;
        break;
    }
    mCfg.multiClass_regression_estimator = mcre;

    xmm::GaussianDistribution::CovarianceMode gdcm;
    switch (cfg.covarianceMode) {
      case xmmFullCovariance:
        gdcm = xmm::GaussianDistribution::CovarianceMode::Full;
        break;
      case xmmDiagonalCovariance:
      default:
        gdcm = xmm::GaussianDistribution::CovarianceMode::Diagonal;
        break;
    }
    mCfg.covariance_mode.set(gdcm);

    mCfg.changed = true;

    this->model.shared_parameters->likelihood_window.set(cfg.likelihoodWindow);
  }

public:
  virtual ~xmmStaticTool() {}
};

//======================= base class for HMM models ==========================//

template <class SingleClassModel, class Model>
class xmmTemporalTool : public xmmStaticTool<SingleClassModel, Model> {
protected:
  xmmTemporalTool(xmmToolConfig cfg, bool bimodal) :
  xmmStaticTool<SingleClassModel, Model>(cfg, bimodal) {
    xmm::Configuration<SingleClassModel>& mCfg = this->model.configuration;

    mCfg.states.set(cfg.states);
    mCfg.hierarchical.set(cfg.hierarchical);

    xmm::HMM::TransitionMode htm;
    switch (cfg.hmmTransitionMode) {
      case xmmHmmLeftRightTransition:
        htm = xmm::HMM::TransitionMode::LeftRight;
        break;
      case xmmHmmErgodicTransition:
      default:
        htm = xmm::HMM::TransitionMode::Ergodic;
        break;
    }
    mCfg.transition_mode.set(htm);

    xmm::HMM::RegressionEstimator hre;
    switch (cfg.hmmRegressionEstimator) {
      case xmmHmmFullRegression:
        hre = xmm::HMM::RegressionEstimator::Full;
        break;
      case xmmHmmWindowedRegression:
        hre = xmm::HMM::RegressionEstimator::Windowed;
        break;
      case xmmHmmLikeliestRegression:
      default:
        hre = xmm::HMM::RegressionEstimator::Likeliest;
        break;
    }
    mCfg.regression_estimator.set(hre);

    mCfg.changed = true;
  }

public:
  virtual ~xmmTemporalTool() {}
};

//================== actual classes used in machineLearning.h ================//

class xmmGmmTool : public xmmStaticTool<xmm::GMM, xmm::GMM> {
public:
  xmmGmmTool(xmmToolConfig cfg = xmmToolConfig()) :
  xmmStaticTool<xmm::GMM, xmm::GMM>(cfg, false) {}
  ~xmmGmmTool() {}

  std::vector<double> run(const std::vector<double>& inputVector);
};

class xmmGmrTool : public xmmStaticTool<xmm::GMM, xmm::GMM> {
public:
  xmmGmrTool(xmmToolConfig cfg = xmmToolConfig()) :
  xmmStaticTool<xmm::GMM, xmm::GMM>(cfg, true) {}
  ~xmmGmrTool() {}

  std::vector<double> run(const std::vector<double>& inputVector);
};

class xmmHmmTool : public xmmTemporalTool<xmm::HMM, xmm::HierarchicalHMM> {
public:
  xmmHmmTool(xmmToolConfig cfg = xmmToolConfig()) :
  xmmTemporalTool<xmm::HMM, xmm::HierarchicalHMM>(cfg, false) {}
  ~xmmHmmTool() {}

  std::vector<double> run(const std::vector<double>& inputVector);
};

class xmmHmrTool : public xmmTemporalTool<xmm::HMM, xmm::HierarchicalHMM> {
public:
  xmmHmrTool(xmmToolConfig cfg = xmmToolConfig()) :
  xmmTemporalTool<xmm::HMM, xmm::HierarchicalHMM>(cfg, true) {}
  ~xmmHmrTool() {}

  std::vector<double> run(const std::vector<double>& inputVector);
};

#endif /* _RAPID_XMM_TOOLS_H_ */
