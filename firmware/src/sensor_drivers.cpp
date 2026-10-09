#include "sensor_drivers.h"
#include <Wire.h>
#if defined(ARES_ENABLE_LEPTON) && defined(ARES_LEPTON_CS_PIN) && defined(ARES_I2C_SDA_PIN) && defined(ARES_I2C_SCL_PIN)
#include <LeptonFLiR.h>
namespace { LeptonFLiR lepton(static_cast<byte>(ARES_LEPTON_CS_PIN)); bool leptonReady = false; }
#endif
#if defined(ARES_ENABLE_LIDAR) && defined(ARES_I2C_SDA_PIN) && defined(ARES_I2C_SCL_PIN)
#include <LIDARLite.h>
namespace { LIDARLite lidar; bool lidarReady = false; }
#endif
#if defined(ARES_ENABLE_GEOSPACE_ADC) && defined(ARES_GEOSPACE_ADC_PIN)
namespace { const uint8_t geospaceAdcPin = ARES_GEOSPACE_ADC_PIN; }
#endif
namespace ares { namespace sensors {
namespace { Reading reading(Status s, uint32_t t, int32_t v = 0, bool ok = false) { return Reading(s,t,v,ok); } }
const char* statusName(Status s) {
  switch(s) { case Status::Disabled:return "disabled"; case Status::SampleReady:return "sample_ready"; case Status::NoFrame:return "no_frame"; case Status::ReadError:return "read_error"; case Status::NotIntegrated:return "not_integrated"; case Status::InvalidData:return "invalid_data"; } return "unknown";
}
void begin() {
#if defined(ARES_I2C_SDA_PIN) && defined(ARES_I2C_SCL_PIN)
  Wire.begin(ARES_I2C_SDA_PIN, ARES_I2C_SCL_PIN); Wire.setClock(400000);
#endif
#if defined(ARES_ENABLE_LEPTON) && defined(ARES_LEPTON_CS_PIN) && defined(ARES_I2C_SDA_PIN) && defined(ARES_I2C_SCL_PIN)
  lepton.init(LeptonFLiR_CameraType_Lepton3_5, LeptonFLiR_TemperatureMode_Celsius); leptonReady = true;
#endif
#if defined(ARES_ENABLE_LIDAR) && defined(ARES_I2C_SDA_PIN) && defined(ARES_I2C_SCL_PIN)
  lidar.begin(0,true); lidar.configure(0); lidarReady = true;
#endif
#if defined(ARES_ENABLE_GEOSPACE_ADC) && defined(ARES_GEOSPACE_ADC_PIN)
  pinMode(geospaceAdcPin, INPUT);
#endif
}
void poll(Snapshot& o) {
  uint32_t now = millis();
#if defined(ARES_ENABLE_LEPTON) && defined(ARES_LEPTON_CS_PIN) && defined(ARES_I2C_SDA_PIN) && defined(ARES_I2C_SCL_PIN)
  if (!leptonReady) o.lepton_tlinear_raw=reading(Status::ReadError,now);
  else if (!lepton.tryReadNextFrame()) o.lepton_tlinear_raw=reading(Status::NoFrame,now);
  else if (!lepton.isImageDataAvailable() || !lepton.getTLinearEnabled()) o.lepton_tlinear_raw=reading(Status::InvalidData,now);
  else { LeptonFLiR_PixelData p=lepton.getImagePixelData(lepton.getImageHeight()/2,lepton.getImageWidth()/2); o.lepton_tlinear_raw=reading(Status::SampleReady,millis(),static_cast<int32_t>(p.tlinear.value),true); }
#else
  o.lepton_tlinear_raw=reading(Status::Disabled,now);
#endif
#if defined(ARES_ENABLE_LIDAR) && defined(ARES_I2C_SDA_PIN) && defined(ARES_I2C_SCL_PIN)
  if (!lidarReady) o.lidar_mm=reading(Status::ReadError,now);
  else { int cm=lidar.distance(false); o.lidar_mm=(cm>0)?reading(Status::SampleReady,millis(),cm*10,true):reading(Status::InvalidData,millis()); }
#else
  o.lidar_mm=reading(Status::Disabled,now);
#endif
#if defined(ARES_ENABLE_GEOSPACE_ADC) && defined(ARES_GEOSPACE_ADC_PIN)
  o.geospace_adc_raw=reading(Status::SampleReady,now,analogRead(geospaceAdcPin),true);
#else
  o.geospace_adc_raw=reading(Status::Disabled,now);
#endif
  // No fabricated ranges/audio: these require a validated DW3xxx port and XMOS host/interface implementation.
  o.dwm3000_mm=reading(Status::NotIntegrated,now);
  o.xvf3800_audio_rms_raw=reading(Status::NotIntegrated,now);
}
} }
