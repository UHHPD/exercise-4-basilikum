#ifndef DATA_HH
#define DATA_HH

#include <vector>
#include <string>

// constants for background model
const double ALPHA = 0.005;
const double BETA = -0.00001;
const double GAMMA = 0.08;
const double DELTA = 0.015;

double background_func(double x, double alpha, double beta, double gamma, double delta);

class Data {
 public:
  Data(const std::string& filename);

  unsigned int size() const { return m_data.size(); }
  double measurement(int i) const { return m_data[i]; }
  double binCenter(int i) const { return (m_bins[i]+m_bins[i+1])/2; }
  double binLow(int i) const { return m_bins[i]; }
  double binHigh(int i) const { return m_bins[i+1]; }
  double error(int i) const { return m_error[i]; }
  
  int checkCompatibility(const Data & in, int n);
  // Data sum(Data Dat1, Data Dat2); // fail, WHY?
  Data sumwith(const Data Dat2);
  Data operator+ (const Data& dat2);
  double chi2_normalized();

 private:
  Data() {}  // disallow empty data set
  void assertSizes();
  std::vector<double> m_data;
  std::vector<double> m_bins;
  std::vector<double> m_error;
};

#endif
