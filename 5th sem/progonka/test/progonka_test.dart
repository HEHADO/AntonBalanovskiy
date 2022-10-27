import 'package:progonka/progonka.dart';
import 'package:test/test.dart';
import 'package:collection/collection.dart';

void main() {
  test(
    'calculate',
    () {
      var epsilon = 0.0001;
      var expected = [26.4, 36.72, 68.11, 29.67, 30.82, 23.62];
      var result = progonka(
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
      expect(result.length, expected.length);
      for (final pairs in IterableZip([expected, result])) {
        expect(pairs[1], closeTo(pairs[0], epsilon));
      }
    },
  );
}
