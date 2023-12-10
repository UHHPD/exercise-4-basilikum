#include "Data.hh"

#include <iostream>
#include <fstream>
#include <cassert>
#include <stdexcept>
#include <cmath>

using namespace std;

double background_func(double x, double alpha, double beta, double gamma, double delta){
  return alpha + beta * x + gamma * exp(- delta * x);
}

Data::Data(const std::string& filename) {
  ifstream file(filename);

  // check if file opened correctly
  if (!file.is_open()) {
    throw std::runtime_error("Error could not open file with name: " +
                             filename);
  }

  int size = -1;
  file >> size;

  // read in data from file: m_siz+1 bin edges
  for (int i = 0; i < size + 1; ++i) {
    double edge;
    file >> edge;
    m_bins.push_back(edge);
  }
  // read in data from file: m_siz bin contents
  for (int i = 0; i < size; ++i) {
    double entries;
    file >> entries;
    m_data.push_back(entries);
  }
  
  // read in data from file: m_siz error contents
  for (int i = 0; i < size; ++i) {
    double entries;
    file >> entries;
    m_error.push_back(entries);
  }

  // done! close the file
  file.close();

  assertSizes();
};

int Data::checkCompatibility(const Data& in, int n){
  int count = 0;
  for(int i=0; i<m_bins.size(); i++){
    double diff_err = sqrt(in.m_error[i] * in.m_error[i] + m_error[i] * m_error[i]);
    if(std::abs(in.m_data[i] - m_data[i]) > diff_err * n){ // note: clearer to specify std::abs, so that it take care of the data type (float)
      count ++;
    }
  }
  return count;
}

// Data Data::sum(const Data Dat1, const Data Dat2){ // why does this fail??
Data Data::sumwith(Data Dat2){
  Data Dat12(*this); // copy the first data set first for initialization

  // check compatibility before combining
  if(this->checkCompatibility(Dat2, 2)){
    cout << "compatibility check passed while combining" << endl;
  }
  else{
    cout << "compatibility check FAILED while combining" << endl;
  }

  // combining
  for(int i=0; i<this->m_data.size(); i++){
    double error12 = 0;
    error12 = 1/sqrt(1/this->error(i)/this->error(i) + 1/Dat2.error(i)/Dat2.error(i));
    Dat12.m_error[i] = error12;
    Dat12.m_data[i] = (this->m_data[i] / (this->m_error[i] * this->m_error[i]) + Dat2.m_data[i] / (Dat2.m_error[i] * Dat2.m_error[i])) * error12 * error12;
  }

  return Dat12;
  // return 0;
}

Data Data::operator+ (const Data& Dat2){
  Data Dat12(*this);
    
  // // check compatibility before combining
  // if(this->checkCompatibility(Dat2, 2) > ???){
  //   cout << "compatibility check passed while combining" << endl;
  // }
  // else{
  //   cout << "compatibility check FAILED while combining" << endl;
  // }

  // combining
  for(int i=0; i<this->m_data.size(); i++){
    double error12 = 0;
    error12 = 1/sqrt(1/this->error(i)/this->error(i) + 1/Dat2.error(i)/Dat2.error(i));
    Dat12.m_error[i] = error12;
    Dat12.m_data[i] = (this->m_data[i] / (this->m_error[i] * this->m_error[i]) + Dat2.m_data[i] / (Dat2.m_error[i] * Dat2.m_error[i])) * error12 * error12;
  }

  return Dat12;
}
    

void Data::assertSizes() { assert(m_data.size() + 1 == m_bins.size()); }

double Data::chi2_normalized(){
  double res = 0;
  int ndf = 52;

  for(int i=0; i<this->size(); i++){
    res += (this->measurement(i) - background_func(this->binCenter(i), ALPHA, BETA, GAMMA, DELTA))
          * (this->measurement(i) - background_func(this->binCenter(i), ALPHA, BETA, GAMMA, DELTA))
          /this->error(i)/this->error(i);
  }
  return res / ndf;
}