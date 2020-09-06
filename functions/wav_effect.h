#ifndef FUNCTIONS_WAV_EFFECT_H
#define FUNCTIONS_WAV_EFFECT_H

#include "../blades/blade_base.h"


class WavEffect{
public:	

  const char* GetEffectName(BladeEffectType type) {
	  switch (type){
	   case EFFECT_CLASH:      	return "clash,clsh";
	   case EFFECT_BLAST:      	return "blaster,blst";
	   case EFFECT_FORCE:      	return "force";
	   case EFFECT_STAB:       	return "stab";
	   case EFFECT_BOOT:	   	return "boot";
	   case EFFECT_LOCKUP_BEGIN:return "bgnlock";
	   case EFFECT_LOCKUP_END: 	return "endlock";
	   case EFFECT_DRAG_BEGIN:	return "bgndrag";
	   case EFFECT_DRAG_END:	return "enddrag";
	   case EFFECT_PREON:      	return "preon";
	   case EFFECT_PSTOFF:     	return "pstoff";
	   case EFFECT_IGNITION:   	return "poweron,out,poweronf";//including force poweron
	   case EFFECT_RETRACTION: 	return "poweroff,in";
	   case EFFECT_CHANGE:	   	return "ccbegin,ccchange,ccend";
	   case EFFECT_NEWFONT:    	return "font";
	   
	   case EFFECT_STUN:	   	return "stun";
	   case EFFECT_FIRE:	   	return "none";//no effect defined
	   case EFFECT_CLIP_IN:	   	return "clipin";
	   case EFFECT_CLIP_OUT:   	return "clipout";
	   case EFFECT_RELOAD:	   	return "reload";
	   case EFFECT_MODE:	   	return "mode";
	   case EFFECT_RANGE:	   	return "range";
	   case EFFECT_EMPTY:	   	return "empty";
	   case EFFECT_FULL:	   	return "full";
	   case EFFECT_JAM:		   	return "jam";
	   case EFFECT_UNJAM:	   	return "unjam";
	   case EFFECT_PLI_ON:	   	return "plion";
	   case EFFECT_PLI_OFF:	   	return "plioff";
	   case EFFECT_NONE:	   	return "none";
	  } 
	 return "none"; 
	  
  }

};

const char* pstoff_ = "pstoff";

#endif
