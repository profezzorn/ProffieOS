#ifndef SOUND_SMOOTH_SWING_CONFIG_H
#define SOUND_SMOOTH_SWING_CONFIG_H

class SmoothSwingConfigFile : public ConfigFile {
public:
  void SetVariable(const char* variable, float v) override {
    CONFIG_VARIABLE(Version, 2);
    CONFIG_VARIABLE(SwingSensitivity, 450.0f);
    CONFIG_VARIABLE(MaximumHumDucking, 75.0f);
    CONFIG_VARIABLE(SwingSharpness, 1.75f);
    CONFIG_VARIABLE(SwingStrengthThreshold, 20.0f);
    CONFIG_VARIABLE(Transition1Degrees, 45.0f);
    CONFIG_VARIABLE(Transition2Degrees, 160.0f);
    CONFIG_VARIABLE(MaxSwingVolume, 3.0f);
    CONFIG_VARIABLE(AccentSwingSpeedThreshold, 0.0f);
    CONFIG_VARIABLE(AccentSlashAccelerationThreshold, 260.0f);
  };

  int  Version;
  float SwingSensitivity;
  float MaximumHumDucking;
  float SwingSharpness;
  float SwingStrengthThreshold;
  float Transition1Degrees;
  float Transition2Degrees;
  float MaxSwingVolume;
  float AccentSwingSpeedThreshold;
  float AccentSlashAccelerationThreshold;
};

SmoothSwingConfigFile smooth_swing_config;

#endif
