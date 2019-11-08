#include <iostream>
#include "al/app/al_App.hpp"

using namespace al;

// class === struct
struct MyApp : App {
  Mesh verts;
  Mesh verts2{Mesh::LINES};

  double timeAccum{0};

  void onSound(AudioIOData& io) {
    io();
    float f = 1;
    io.out(0) = f;
    io.out(1) = f;
    while (io()) {
      float f = 0;
      io.out(0) = f;
      io.out(1) = f;
    }
  }

  void onCreate() {
    verts.primitive(Mesh::LINE_STRIP);
    // Create a sine wave
    int N = 128;
    for (int i = 0; i < N; ++i) {
      float f = float(i) / (N - 1);
      float phase = float(M_2PI) * f;
      verts.vertex(2.0f * f - 1.0f, 0.5f * sinf(phase));
    }
  }

  void onAnimate(double dt) {
    int N = 256;
    // We update drawing according to current width and height
    float w = float(width());
    float h = float(height());
    float s = float(al_steady_time());
    verts2.reset();
    for (int i = 0; i < N; i += 1) {
      float t = float(i) / (N - 1);
      float phase = float(M_2PI) * t + s;
      verts2.vertex(w * t, h * (0.5f + 0.5f * cosf(phase)));
      verts2.color(t, 1 - t, 0);
    }
  }

  void onDraw(Graphics& g) {
    g.clear(0, 0, 0);

    g.camera(Viewpoint::IDENTITY);  // Ortho [-1:1] x [-1:1]
    g.color(1, 1, 1);
    g.draw(verts);

    g.camera(Viewpoint::ORTHO_FOR_2D);  // Ortho [0:width] x [0:height]
    g.meshColor();
    g.draw(verts2);
  }
};

int main() {
  MyApp app;
  app.configureAudio();
  app.start();
}
