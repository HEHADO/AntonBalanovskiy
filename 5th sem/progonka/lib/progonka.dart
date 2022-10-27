List<double> progonka(
  int n,
  List<double> A,
  List<double> B,
  List<double> C,
  List<double> F,
  double x1,
  double x2,
  double m1,
  double m2, {
  bool printOutput = false,
}) {
  int i = -1;
  List<double> a = [], b = [], y = [];
  a.add(x1);
  b.add(m1);
  while (++i < n - 1) {
    a.add(B[i] / (C[i] - A[i] * a[i]));
    b.add((F[i] + A[i] * b[i]) / (C[i] - A[i] * a[i]));
  }
  y.add((m2 + x2 * b[n - 1]) / (1 - x2 * a[n - 1]));
  while (i >= 0) {
    y.add(a[i] * y[n - 1 - i] + b[i--]);
  }
  y = y.reversed.toList();
  if (printOutput) print("y = $y");
  return y;
}

void main(List<String> args) {
  progonka(
      5,
      [86.83, 15.77, 35.93, 92.86],
      [21.49, 27.62, 59.9, 26.63],
      [109.32, 44.39, 96.83, 120.49],
      [258.234, 1624.84, -1420.36, 329.345],
      0,
      0,
      26.4,
      23.62,
      printOutput: true);
}
