import 'package:sky/src/fn3.dart';
import 'package:test/test.dart';

import '../engine/mock_events.dart';
import '../fn3/widget_tester.dart';

class Inside extends StatefulComponent {
  InsideState createState() => new InsideState();
}

class InsideState extends State<Inside> {
  Widget build(BuildContext context) {
    return new Listener(
      onPointerDown: _handlePointerDown,
      child: new Text('INSIDE')
    );
  }

  void _handlePointerDown(_) {
    setState(() { });
  }
}

class Middle extends StatefulComponent {
  Middle({ this.child });

  final Inside child;

  MiddleState createState() => new MiddleState();
}

class MiddleState extends State<Middle> {
  Widget build(BuildContext context) {
    return new Listener(
      onPointerDown: _handlePointerDown,
      child: config.child
    );
  }

  void _handlePointerDown(_) {
    setState(() { });
  }
}

class Outside extends StatefulComponent {
  OutsideState createState() => new OutsideState();
}

class OutsideState extends State<Outside> {
  Widget build(BuildContext context) {
    return new Middle(child: new Inside());
  }
}

void main() {
  test('setState() smoke test', () {
    WidgetTester tester = new WidgetTester();
    tester.pumpFrame(new Outside());
    TestPointer pointer = new TestPointer(1);
    Point location = tester.getCenter(tester.findText('INSIDE'));
    tester.dispatchEvent(pointer.down(location), location);
    tester.pumpFrameWithoutChange();
    tester.dispatchEvent(pointer.up(), location);
    tester.pumpFrameWithoutChange();
  });
}
