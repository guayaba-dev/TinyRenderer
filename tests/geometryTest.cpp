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

void testCofacMatrix() {
  Matrix A(4, 4);

  A(0, 0) = 1;
  A(1, 0) = 2;
  A(2, 0) = 3;
  A(3, 0) = 4;

  A(0, 1) = 5;
  A(1, 1) = 6;
  A(2, 1) = 7;
  A(3, 1) = 8;

  A(0, 2) = 9;
  A(1, 2) = 10;
  A(2, 2) = 11;
  A(3, 2) = 12;

  A(0, 3) = 13;
  A(1, 3) = 14;
  A(2, 3) = 15;
  A(3, 3) = 16;

  Matrix cofac(3, 3);
  Matrix expected(3, 3);

  expected(0, 0) = 6;
  expected(1, 0) = 7;
  expected(2, 0) = 8;

  expected(0, 1) = 10;
  expected(1, 1) = 11;
  expected(2, 1) = 12;

  expected(0, 2) = 14;
  expected(1, 2) = 15;
  expected(2, 2) = 16;

  A.getCofac(0, 0, cofac);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      assert(cofac(i, j) == expected(i, j));
    }
  }

  std::cout << "✅ testCofacMatrix passed!\n";
}

void testgetDeterminant() {
  // Matriz 1: 2x2
  Matrix A(2, 2);
  A(0, 0) = 1;
  A(0, 1) = 2;
  A(1, 0) = 3;
  A(1, 1) = 4;

  assert(A.getDeterminant(2) == -2);

  // Matriz 2: 2x2
  Matrix B(2, 2);
  B(0, 0) = 2;
  B(0, 1) = 5;
  B(1, 0) = 1;
  B(1, 1) = 3;
  assert(B.getDeterminant(2) == 1);

  // Matriz 3: 3x3 (getDeterminante = 0)
  Matrix C(3, 3);
  C(0, 0) = 1;
  C(0, 1) = 2;
  C(0, 2) = 3;
  C(1, 0) = 4;
  C(1, 1) = 5;
  C(1, 2) = 6;
  C(2, 0) = 7;
  C(2, 1) = 8;
  C(2, 2) = 9;
  assert(C.getDeterminant(3) == 0);

  // Matriz 4: 3x3
  Matrix D(3, 3);
  D(0, 0) = 3;
  D(0, 1) = 2;
  D(0, 2) = -1;
  D(1, 0) = 2;
  D(1, 1) = -3;
  D(1, 2) = 1;
  D(2, 0) = 1;
  D(2, 1) = 2;
  D(2, 2) = 1;
  assert(D.getDeterminant(3) == -24);

  // Matriz 5: 4x4
  Matrix E(4, 4);
  E(0, 0) = 2;
  E(0, 1) = 0;
  E(0, 2) = 1;
  E(0, 3) = 3;
  E(1, 0) = 1;
  E(1, 1) = 2;
  E(1, 2) = 3;
  E(1, 3) = 1;
  E(2, 0) = 0;
  E(2, 1) = 1;
  E(2, 2) = 2;
  E(2, 3) = 1;
  E(3, 0) = 3;
  E(3, 1) = 1;
  E(3, 2) = 1;
  E(3, 3) = 2;
  assert(E.getDeterminant(4) == 2);
  std::cout << "✅ Todos los tests de getDeterminantes pasaron!\n";
}
