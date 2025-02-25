#include "geometryTest.cpp"

int main() {
  testIdentityMatrix();
  testMultiplication();
  testCofacMatrix();
  testgetDeterminant();
  std::cout << "All tests passed!\n";
  return 0;
}
