#ifndef COMMON_SABER_BASE_PASSTHROUGH_H
#define COMMON_SABER_BASE_PASSTHROUGH_H

class SaberBasePassThrough : public SaberBase {
public:
  SaberBasePassThrough() : SaberBase(NOLINK) {}
protected:
  void SetDelegate(SaberBase* delegate) {
    Unlink(this);
    if (delegate_) {
      SaberBase::Link(delegate_);
    }
    delegate_ = delegate;
    if (delegate_) {
      SaberBase::Unlink(delegate_);
      SaberBase::Link(this);
    }
  }
#define SABERFUN(NAME, EFFECT, TYPED_ARGS, ARGS)	\
  void SB_##NAME TYPED_ARGS override {          \
    delegate_->SB_##NAME ARGS;                  \
  }

  SABERBASEFUNCTIONS();
#undef SABERFUN

  SaberBase* delegate_ = NULL;
};

#endif 
