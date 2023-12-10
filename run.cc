#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <cmath>
#include "Data.hh"

#include <fstream>

// generic function comparing two values of some type T used later for int and
// double
template <class T>
bool testEqual(const std::string& name, T expected, T real) {
  if (expected != real) {
    std::cout << "(" << name << ": <" << expected << ">"
              << ", " << real << ") ";
    return false;
  }
  return true;
}

bool testReadingSize() {
  std::cout << "testReadingSize...";
  Data datA("testA");
  return testEqual<int>("size", 1, datA.size());
}

bool testReadingMeasurement() {
  std::cout << "testReadingMeasurement...";
  Data datA("testA");
  return testEqual("measurement", 10., datA.measurement(0));
}

bool testReadingBinEdges() {
  std::cout << "testReadingBinEdges...";
  Data datA("testA");
  return testEqual("bin low", 0., datA.binLow(0)) &&
         testEqual("bin high", 1., datA.binHigh(0));
}

bool testReadingErrors() {
  std::cout << "testReadingErrors...";
  Data datA("testA");
  return testEqual("error", 2., datA.error(0));
}

bool testCopyConstructor() {
  std::cout << "testCopyConstructor...";
  Data a("testA");
  Data b(a);
  Data c = a;  // equivalent to Data c(a)
  return testEqual("measurement", 10., b.measurement(0)) &&
         testEqual("measurement", 10., c.measurement(0));
}

void runTests() {
  std::cout << "running tests...\n";
  std::vector<std::function<bool()> > tests(
      {testReadingSize, testReadingMeasurement, testReadingBinEdges,
       testReadingErrors, testCopyConstructor});
  for (auto test : tests)
    std::cout << (test() ? " ok" : " FAILED!") << std::endl;
}

int main() {
  using namespace std;

  cout << "******************************************************" << endl;
  runTests();
  cout << "******************************************************" << endl;
  // create an object which holds data of experiment A
  Data datA("exp_A");
  Data datB("exp_B");
  Data datC("exp_C");
  Data datD("exp_D");
  std::vector<Data> DataList;
  for(auto s: {datA, datB, datC, datD}){ // more elegant!!
    DataList.push_back(s);
  }

  std::vector<std::string> DataList_name;
  for(auto s: {"expA", "expB", "expC", "expD"}){ // more elegant!!
    DataList_name.push_back(s);
  }


  // DataList.push_back(datA);
  // DataList.push_back(datB);
  // DataList.push_back(datC);
  // DataList.push_back(datD);

  // here is the data from expA ~ expD 
  for(int i=0; i<DataList.size(); i++){
    cout << "bin 27: from " << DataList[i].binLow(27) << " to " << DataList[i].binHigh(27)
        << endl;
    cout << "measurement of " << DataList_name[i] << " in bin 27: " << DataList[i].measurement(27)
        << endl;
  }

  int n_sigma = 2; // 2 sigma
  cout << "======================================================" << endl;
  cout << "compatibility check with n = " << n_sigma << endl; 
  cout << "expected #(disagreed data points) ~ " << round(10000 * std::erfc(n_sigma/sqrt(2)))/100 << " % * data.size() = " << DataList[0].size() * std::erfc(n_sigma/sqrt(2)) << endl;
  
  for(int i=0; i<DataList.size(); i++){
    for (int j=i+1; j<DataList.size(); j++){
      cout << DataList_name[i] << " v.s. " << DataList_name[j] << ", #(mismatch) = " << DataList[i].checkCompatibility(DataList[j], n_sigma) << endl;
    }
  }
  cout << "======================================================" << endl;

  n_sigma = 3; // 3 sigma
  cout << "======================================================" << endl;
  cout << "compatibility check with n = " << n_sigma << endl; 
  cout << "expected #(disagreed data points) ~ " << round(10000 * std::erfc(n_sigma/sqrt(2)))/100 << " % * data.size() = " << DataList[0].size() * std::erfc(n_sigma/sqrt(2)) << endl;
  
  for(int i=0; i<DataList.size(); i++){
    for (int j=i+1; j<DataList.size(); j++){
      cout << DataList_name[i] << " v.s. " << DataList_name[j] << ", #(mismatch) = " << DataList[i].checkCompatibility(DataList[j], n_sigma) << endl;
    }
  }
  cout << "======================================================" << endl;

  // // combine data test
  // // cout << "combine: " << (datA + datB).size() << endl;
  // Data datAB = datA + datB;
  // for(int i=0; i<3; i++){
  //   cout << datAB.data(i) << " " << datA.data(i) << " " << datB.data(i) << endl;
  //   cout << datAB.error(i) << " " << datA.error(i) << " " << datB.data(i) << endl;
  // }


  // null hypothesis = "background only signal"
  // individual null hypothesis test
  for(int i=0; i<DataList.size(); i++){
    cout << DataList_name[i] << ", chi2/ndf = " << DataList[i].chi2_normalized() << endl;
  }
  
  // merged mull hypothesis test
  Data datABCD = datA + datB + datC + datD;
  cout << "expA + expB + expC + expD, chi2/ndf = " << datABCD.chi2_normalized() << endl;

  ofstream exp_merge_HypothesisTest("exp_merge_HypothesisTest");

  for(int i=0; i<datA.size(); i++){
    exp_merge_HypothesisTest << datA.binCenter(i) << " " << datA.measurement(i) << " " << background_func(datA.binCenter(i), ALPHA, BETA, GAMMA, DELTA) << endl;
  }

  exp_merge_HypothesisTest.close();

  // Remark:
  // individually the datasets are exhibits chi2/ndf close to 1, yet the merged one gives a value larger than any dataset alone
  // Averaging should in general suppress noise. With a behavior deviatinng farther away from the background model after averaging, 
  // it's reasonale to say that the data makes the null hypothesis weaker
  // plot with
  // gnuplot> plot "./background_plot" using 1:2, "./background_plot" using 1:3 smooth freq with lines
  return 0;
}
