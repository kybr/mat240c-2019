#include <iostream>
#include "al/app/al_App.hpp"
#include "al/ui/al_ControlGUI.hpp"
#include "al/ui/al_Parameter.hpp"
#include "al/ui/al_ParameterServer.hpp"

using namespace al;

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
    phase += increment;
    if (phase > 1)  //
      phase -= 1;
    return phase;
  }
};

inline float mtof(float m) { return 8.175799f * powf(2.0f, m / 12.0f); }

// class === struct
struct MyApp : App {
  Phasor foo;

  ControlGUI gui;

  Parameter frequency{"frequency", "", 0.0, "", -30.0, 60.0};

  void onCreate() {
    gui << frequency;
    gui.init();
  }

  void onSound(AudioIOData& io) {
    while (io()) {
      foo.frequency(mtof(frequency));
      float v = 2 * foo() - 1;

      io.out(0) = v;
      io.out(1) = v;
    }
  }

  void onDraw(Graphics& g) {
    g.clear(foo.phase);
    gui.draw(g);
    //
  }
};

int main() {
  MyApp app;
  app.configureAudio();
  app.start();
}
