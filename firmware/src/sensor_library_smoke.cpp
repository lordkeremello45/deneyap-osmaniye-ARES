// Compile-time smoke check for the real, pinned sensor libraries.
// No GPIO pins are initialized here: the exact breakout and Deneyap pin map
// must be verified before any sensor is activated.
#include <LeptonFLiR.h>
#include <LIDARLite.h>

void aresSensorLibraryCompileCheck() {
  (void)sizeof(LeptonFLiR);
  (void)sizeof(LIDARLite);
}
