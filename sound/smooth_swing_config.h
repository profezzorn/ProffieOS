#ifndef SOUND_SMOOTH_SWING_CONFIG_H
#define SOUND_SMOOTH_SWING_CONFIG_H

class SmoothSwingConfigFile : public ConfigFile {
public:
  void iterateVariables(VariableOP *op) override {
    CONFIG_VARIABLE2(Version, 2);
    CONFIG_VARIABLE2(SwingSensitivity, 450.0f);
    CONFIG_VARIABLE2(MaximumHumDucking, 75.0f);
    CONFIG_VARIABLE2(SwingSharpness, 1.75f);
    CONFIG_VARIABLE2(SwingStrengthThreshold, 20.0f);
    CONFIG_VARIABLE2(Transition1Degrees, 45.0f);
    CONFIG_VARIABLE2(Transition2Degrees, 160.0f);
    CONFIG_VARIABLE2(MaxSwingVolume, 3.0f);
    CONFIG_VARIABLE2(AccentSwingSpeedThreshold, 0.0f);
    CONFIG_VARIABLE2(AccentSlashAccelerationThreshold, 260.0f);
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
