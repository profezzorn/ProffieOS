#ifndef SOUND_SMOOTH_SWING_CFX_CONFIG_H
#define SOUND_SMOOTH_SWING_CFX_CONFIG_H

class SmoothSwingCFXConfigFile : public ConfigFile {
public:
  void SetVariable(const char* variable, float v) override {
    CONFIG_VARIABLE(smooth_mode, 1);
    CONFIG_VARIABLE(smooth_sens, 450.0f);
    CONFIG_VARIABLE(smooth_dampen, 75.0f);
    CONFIG_VARIABLE(smooth_sharp, 1.75f);
    CONFIG_VARIABLE(smooth_gate, 20.0f);
    CONFIG_VARIABLE(smooth_width1, 45.0f);
    CONFIG_VARIABLE(smooth_width2, 160.0f);
    CONFIG_VARIABLE(smooth_gain, 100.0f);
    CONFIG_VARIABLE(hswing, 450.0f);
  };

  int smooth_mode;
  float smooth_sens;
  float smooth_dampen;
  float smooth_sharp;
  float smooth_gate;
  float smooth_width1;
  float smooth_width2;
  float smooth_gain;
  float hswing;
};

SmoothSwingCFXConfigFile smooth_swing_cfx_config;

#endif
