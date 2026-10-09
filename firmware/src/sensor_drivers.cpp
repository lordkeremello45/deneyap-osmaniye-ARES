#include "sensor_drivers.h"
#include <Wire.h>
#if defined(ARES_ENABLE_LEPTON) && defined(ARES_LEPTON_CS_PIN) && defined(ARES_I2C_SDA_PIN) && defined(ARES_I2C_SCL_PIN)
#include <LeptonFLiR.h>
namespace { LeptonFLiR lepton(static_cast<byte>(ARES_LEPTON_CS_PIN)); bool lepton_ready = false; }
#endif
#if defined(ARES_ENABLE_LIDAR) && defined(ARES_I2C_SDA_PIN) && defined(ARES_I2C_SCL_PIN)
#include <LIDARLite.h>
namespace { LIDARLite lidar; bool lidar_ready = false; }
#endif
#if defined(ARES_ENABLE_GEOSPACE_ADC) && defined(ARES_GEOSPACE_ADC_PIN)
namespace { constexpr uint8_t kGeospaceAdcPin = ARES_GEOSPACE_ADC_PIN; }
#endif
namespace ares::sensors {
namespace {
Reading makeReading(Status status, uint32_t now, int32_t value = 0, bool valid = false) { return Reading{status, now, value, valid}; }
}
const char* statusName(Status status) {
  switch (status) {
    case Status::Disabled: return "disabled";
    case Status::Ready: return "ready";
    case Status::SampleReady: return "sample_ready";
    case Status::NoFrame: return "no_frame";
    case Status::ReadError: return "read_error";
    case Status::NotIntegrated: return "not_integrated";
    case Status::InvalidData: return "invalid_data";
  }
  return "unknown";
}
void begin() {
#if defined(ARES_I2C_SDA_PIN) && defined(ARES_I2C_SCL_PIN)
  Wire.begin(ARES_I2C_SDA_PIN, ARES_I2C_SCL_PIN);
  Wire.setClock(400000);
#endif
#if defined(ARES_ENABLE_LEPTON) && defined(ARES_LEPTON_CS_PIN) && defined(ARES_I2C_SDA_PIN) && defined(ARES_I2C_SCL_PIN)
  lepton.init(LeptonFLiR_CameraType_Lepton3_5, LeptonFLiR_TemperatureMode_Celsius);
  lepton_ready = true;
#endif
#if defined(ARES_ENABLE_LIDAR) && defined(ARES_I2C_SDA_PIN) && defined(ARES_I2C_SCL_PIN)
  lidar.begin(0, true);
  lidar.configure(0);
  lidar_ready = true;
#endif
#if defined(ARES_ENABLE_GEOSPACE_ADC) && defined(ARES_GEOSPACE_ADC_PIN)
  pinMode(kGeospaceAdcPin, INPUT);
#endif
}
void poll(Snapshot& out) {
  const uint32_t now = millis();
#if defined(ARES_ENABLE_LEPTON) && defined(ARES_LEPTON_CS_PIN) && defined(ARES_I2C_SDA_PIN) && defined(ARES_I2C_SCL_PIN)
  if (!lepton_ready) out.lepton_tlinear_raw = makeReading(Status::ReadError, now);
  else if (!lepton.tryReadNextFrame()) out.lepton_tlinear_raw = makeReading(Status::NoFrame, now);
  else if (!lepton.isImageDataAvailable() || !lepton.getTLinearEnabled()) {
    // Never mislabel AGC/grayscale pixels as temperature.
    out.lepton_tlinear_raw = makeReading(Status::InvalidData, now);
  } else {
    const int row = lepton.getImageHeight() / 2;
    const int col = lepton.getImageWidth() / 2;
    const LeptonFLiR_PixelData pixel = lepton.getImagePixelData(row, col);
    out.lepton_tlinear_raw = makeReading(Status::SampleReady, millis(), static_cast<int32_t>(pixel.tlinear.value), true);
  }
#else
  out.lepton_tlinear_raw = makeReading(Status::Disabled, now);
#endif
#if defined(ARES_ENABLE_LIDAR) && defined(ARES_I2C_SDA_PIN) && defined(ARES_I2C_SCL_PIN)
  if (!lidar_ready) out.lidar_mm = makeReading(Status::ReadError, now);
  else {
    const int distance_cm = lidar.distance(false);
    if (distance_cm <= 0) out.lidar_mm = makeReading(Status::InvalidData, millis());
    else out.lidar_mm = makeReading(Status::SampleReady, millis(), distance_cm * 10, true);
  }
#else
  out.lidar_mm = makeReading(Status::Disabled, now);
#endif
#if defined(ARES_ENABLE_GEOSPACE_ADC) && defined(ARES_GEOSPACE_ADC_PIN)
  out.geospace_adc_raw = makeReading(Status::SampleReady, now, analogRead(kGeospaceAdcPin), true);
#else
  // Raw ADC only; not a calibrated vibration measurement.
  out.geospace_adc_raw = makeReading(Status::Disabled, now);
#endif
  // Do not fabricate UWB ranges before the DW3xxx port and device ID are verified.
  out.dwm3000_mm = makeReading(Status::NotIntegrated, now);
  // XVF3800 USB/I2S capture requires the confirmed assembly and interface/clock configuration.
  out.xvf3800_audio_rms_raw = makeReading(Status::NotIntegrated, now);
}
}  // namespace ares::sensors
