#ifndef FUNCTIONS_WAV_TIME_H
#define FUNCTIONS_WAV_TIME_H

#include "wav_effect.h"
#include "int.h"
#include "../blades/blade_base.h"

// Usage: WavLen<EFFECT_NAME>  
// EFFECT_NAMENUM : any off the available effect form the style edittor
// Returns length of wav file returned as const int in [ms]
// returned value: INTEGER
//
template<BladeEffectType EFFECT>
class WavLen {
 public:
  void run(BladeBase* blade) {
    bool found_ = false;
	eff_name_ = wav_effect.GetEffectName(EFFECT);//find correspondig SoundEffectName for BladeEffectType
	for (size_t i = 0; i < NELEM(wav_players); i++) {
	    if(strstr(eff_name_, wav_players[i].effectname()) && wav_players[i].isPlaying()) {found_ = true;idx=(int)i;}
		if(strcmp(eff_name_,"pstoff")==0) {if(strstr(eff_name_, wav_players[i].effect2ndname()) && wav_players[i].isPlaying()) {found_ = true;idx=(int)i;}}
			if (found_) { value_ = clampi32(wav_players[idx].length()*1000, 0, 32768);}
			if (found_ && !shown_){
				STDOUT << "WavLen: eff_name:'" << eff_name_  << "', effectname: '";
				if(strcmp(eff_name_,"pstoff")==0) { STDOUT << wav_players[i].effect2ndname();} else{ STDOUT << wav_players[i].effectname();}
   				STDOUT << "' wav_player:" << i 
				       << ", filename=" << wav_players[i].filename() 
					   << ", length: " << wav_players[i].length() << "[s]," 
				       << " converted = " << value_ << "[ms]\n";
			shown_ = true;
			}
	}
	if (!found_){
		if(shown_) {STDOUT << "WavLen stopped for effect '" << eff_name_ <<"', value retained is: " << value_ << "\n";}
		//should we retain value after the effect has gone? Only reset it when new effect is selected?
        //value_ = 32767;
        shown_ = false;
	}
  }
  int getInteger(int led) { return value_; }
  int value_ = 32767;
private:
  int idx;
  const char* eff_name_;
  bool shown_;  
  WavEffect wav_effect;
  
};


// Usage: WavPos<EFFECT_NAME>  
// EFFECT_NAME : any off the available effect form the style edittor
// Returns current position [0..32768] of wav file returned as const int in [ms]
// returned value: INTEGER
//
template<BladeEffectType EFFECT>
class WavPos {
 public:
  void run(BladeBase* blade) {
    m = millis();
	bool found_ = false;
	eff_name_ = wav_effect.GetEffectName(EFFECT);
	for (size_t i = 0; i < NELEM(wav_players); i++) {
	    if(strstr(eff_name_, wav_players[i].effectname()) && wav_players[i].isPlaying()) {found_ = true; shown_=false;idx=(int)i;};
		if(strcmp(eff_name_,"pstoff")==0) {if(strstr(eff_name_, wav_players[i].effect2ndname()) && wav_players[i].isPlaying()) {found_ = true; shown_=false;idx=(int)i;}}
			if(found_) { value_ = clampi32(wav_players[idx].pos()/wav_players[idx].length()*32768, 0, 32768);}
			if (value_ == 0) value_ = 32768; //to prevent glitch at end of file
			if (found_ && !shown_ && ((m % 100)==0)){
				STDOUT << "WavPos: eff_name:'" << eff_name_  << "', effectname: ";
				if(strcmp(eff_name_,"pstoff")==0) { STDOUT << wav_players[i].effect2ndname();} else{ STDOUT << wav_players[i].effectname();}
   				STDOUT << "' wav_player:" << i 
				       << ", filename=" << wav_players[i].filename() 
					   << ", length: " << wav_players[i].length() << "[s]," 
				       << " current pos = " << value_ << "[ms]\n";
			shown_ = true;
			}
	}
	if (!found_){
		if(shown_) {STDOUT << "WavPos stopped for effect '" << eff_name_ <<"', value retained is: " << value_ << "\n";}
		//value_ = 0; // make it 0 so that a pos does start with 0 when sound is not playing.
        shown_ = false;
	}
  }
  int getInteger(int led) { return value_; }
  int value_ = 0;
private:
  int idx;
  const char* eff_name_;
  bool shown_;  
  WavEffect wav_effect;
  uint32_t m;
 
};


// Usage: WavRem<EFFECT_NAME>  
// EFFECT_NAME : any off the available effect form the style edittor
// Returns remaining time [32768..0] of wav file returned as const int in [ms]
// returned value: INTEGER
//
template<BladeEffectType EFFECT>
class WavRem {
 public:
  void run(BladeBase* blade) {
    m = millis();
    bool found_ = false;
	eff_name_ = wav_effect.GetEffectName(EFFECT);
	for (size_t i = 0; i < NELEM(wav_players); i++) {
	    if(strstr(eff_name_, wav_players[i].effectname()) && wav_players[i].isPlaying()) {found_ = true; shown_ = false;idx=(int)i;} ;
		if(strcmp(eff_name_,"pstoff")==0) {if(strstr(eff_name_, wav_players[i].effect2ndname()) && wav_players[i].isPlaying()) {found_ = true; shown_=false;idx=(int)i;}}
			if (found_) { value_ = clampi32(((wav_players[idx].length()-wav_players[idx].pos()))/wav_players[idx].length()*32768, 0, 32768);}
			if (value_ == 32768) value_ = 0; //to prevent glitch at end of file
			if (found_ && !shown_ && ((m % 100)==0)){
				STDOUT << "WavRem: eff_name:'" << eff_name_  << ", effectname: ";
				if(strcmp(eff_name_,"pstoff")==0) { STDOUT << wav_players[i].effect2ndname();} else{ STDOUT << wav_players[i].effectname();}
   				STDOUT << "' wav_player:" << i 
				       << ", filename=" << wav_players[i].filename() 
					   << ", length: " << wav_players[i].length() << "[s]," 
				       << " remaining time = " << value_ << "[ms]\n";
			shown_ = true;
			}
	}
	if (!found_){
		if(shown_) {STDOUT << "WavRem stopped for effect '" << eff_name_ <<"', value retained is: " << value_ << "\n";}
		//value_ = 32767; // make it large so that a InOutHelper does not start with 0 and instantly ignites
        shown_ = false;
	}
  }
  int getInteger(int led) { return value_; }
  int value_ = 32767;
private:
  int idx;
  const char* eff_name_;
  bool shown_;  
  WavEffect wav_effect;
  uint32_t m;
  
};

#endif
