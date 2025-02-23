#include <cassert>
#include <iostream>

#include "../src/geometry.h"  // Asegúrate de incluir tu clase Matrix

void testIdentityMatrix() {
  Matrix I = Matrix::identity(3);
  assert(I(0, 0) == 1.0f);
  assert(I(1, 1) == 1.0f);
  assert(I(2, 2) == 1.0f);
  assert(I(0, 1) == 0.0f);
  assert(I(1, 0) == 0.0f);
  std::cout << "✅ testIdentityMatrix passed!\n";
}

void testMultiplication() {
  Matrix A(2, 2);
  A(0, 0) = 1;
  A(0, 1) = 2;
  A(1, 0) = 3;
  A(1, 1) = 4;

  Matrix B(2, 2);
  B(0, 0) = 2;
  B(0, 1) = 0;
  B(1, 0) = 1;
  B(1, 1) = 2;

  Matrix C = A * B;

  assert(C(0, 0) == 4);   // 1*2 + 2*1 = 4
  assert(C(0, 1) == 4);   // 1*0 + 2*2 = 4
  assert(C(1, 0) == 10);  // 3*2 + 4*1 = 10
  assert(C(1, 1) == 8);   // 3*0 + 4*2 = 8

  std::cout << "✅ testMultiplication passed!\n";
}

int main() {
  testIdentityMatrix();
  testMultiplication();
  std::cout << "All tests passed!\n";
  return 0;
}
