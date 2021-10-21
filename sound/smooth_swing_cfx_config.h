#ifndef SOUND_SMOOTH_SWING_CFX_CONFIG_H
#define SOUND_SMOOTH_SWING_CFX_CONFIG_H

class SmoothSwingCFXConfigFile : public ConfigFile {
public:
  void iterateVariables(VariableOP *op) override {
    CONFIG_VARIABLE2(smooth_mode, 1);
    CONFIG_VARIABLE2(smooth_sens, 450.0f);
    CONFIG_VARIABLE2(smooth_dampen, 75.0f);
    CONFIG_VARIABLE2(smooth_sharp, 1.75f);
    CONFIG_VARIABLE2(smooth_gate, 20.0f);
    CONFIG_VARIABLE2(smooth_width1, 45.0f);
    CONFIG_VARIABLE2(smooth_width2, 160.0f);
    CONFIG_VARIABLE2(smooth_gain, 100.0f);
    CONFIG_VARIABLE2(hswing, 450.0f);
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
