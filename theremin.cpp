#include <iostream>
#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"
#include "al/ui/al_ControlGUI.hpp"
#include "al/ui/al_Parameter.hpp"
#include "al/ui/al_ParameterServer.hpp"

using namespace al;
using namespace std;

int SAMPLE_RATE = 44100;

struct Phasor {
  float phase = 0;
  float increment = 0;

  void frequency(float hertz) {  //
    increment = hertz / SAMPLE_RATE;
  }

  void period(float seconds) {  //
    frequency(1 / seconds);
  }

  // makes Phasor a "functor"
  // implement the "call operator"
  float operator()() {
    float value = phase;  // capture the actual current phase

    // change the world for next time
    //
    phase += increment;
    if (phase > 1)  //
      phase -= 1;

    // return the captured phase
    return value;
  }
};

struct Timer : Phasor {
  bool operator()() {
    phase += increment;
    if (phase > 1) {
      phase -= 1;
      return true;
    }
    return false;
  }
};

struct Edge {
  float history{0};
  bool operator()(float value) {
    bool condition = value < history;
    history = value;
    return condition;
  }
};

struct OnePole {
  float b0 = 1, a1 = 0, yn1 = 0;

  void frequency(float hertz) {
    a1 = exp(-2.0f * 3.14159265358979323846f * hertz / SAMPLE_RATE);
    b0 = 1.0f - a1;
  }

  void period(float seconds) { frequency(1 / seconds); }

  float operator()(float xn) {  //
    return yn1 = b0 * xn + a1 * yn1;
  }
};

inline float mtof(float m) { return 8.175799f * powf(2.0f, m / 12.0f); }

struct MyApp : App {
  Phasor foo;

  Edge edge;
  Phasor bar;
  OnePole onePole;

  Color color;

  ControlGUI gui;

  Parameter frequency{"frequency", "", 0.0, "", -30.0, 60.0};

  void onCreate() {
    gui << frequency;
    gui.init();

    bar.period(2.0);
    onePole.frequency(10);
  }
  float value;
  void onSound(AudioIOData& io) {
    while (io()) {
      // foo.frequency(mtof(frequency));

      if (edge(bar())) {
        // change color
        color = HSV(al::rnd::uniform(), 1.0, 1.0);
      }

      float f = onePole(value);

      foo.frequency(mtof(f * 127));

      // float v = 2 * foo() - 1;
      float v = sin(foo() * M_2PI);

      io.out(0) = v;
      io.out(1) = f;
    }
  }

  bool onMouseMove(const Mouse& m) {  //
    // cout << m.x() << ", " << m.y() << endl;
    float x = m.x();
    float y = m.y();
    x /= width();
    y /= height();
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x > 1) x = 1;
    if (y > 1) y = 1;
    // XXX do we have a clamp function? limit? constrain?

    value = x;

    cout << x << ", " << y << endl;
  }

  void onDraw(Graphics& g) {
    // what are the mouse coodinates?

    // Color c = color;
    // c *= foo.phase;
    // g.clear(c);
    g.clear(color);
    gui.draw(g);
    //
  }
};

int main() {
  MyApp app;
  app.configureAudio();
  app.start();
}
