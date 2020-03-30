#ifndef COMMON_PROFILING_H
#define COMMON_PROFILING_H

// #define ENABLE_PROFILING

#ifdef ENABLE_PROFILING

class ProfileLocation;
ProfileLocation* profile_locations_ = nullptr;
struct ProfileLocation {
ProfileLocation(const char* location, const char *func) : location_(location), func_(func) {
    next_ = profile_locations_;
    profile_locations_ = this;
  }
  void Dump(uint64_t total) {
    STDOUT << (cycles_ * 100.0 / total) << " @ "<< location_ << ":";
    for (int i = 0; i < 256; i++) {
      if (!func_[i]) break;
      STDOUT.print(func_[i]);
    }
    STDOUT.println("");
    cycles_ = 0;
  }
  const char* location_;
  const char* func_;
  ProfileLocation* next_;
  uint64_t cycles_ = 0;

};
void DumpProfileLocations(uint64_t total) {
  for (ProfileLocation* p = profile_locations_; p; p = p->next_) p->Dump(total);
}
uint64_t CountProfileCycles() {
  uint64_t ret = 0;
  for (ProfileLocation* p = profile_locations_; p; p = p->next_) ret += p->cycles_;
  return ret;
}
#define SCOPED_PROFILER() \
  static ProfileLocation trace_location_(__FILE__ ":" TOSTRING(__LINE__), __PRETTY_FUNCTION__); \
  ScopedCycleCounter cycle_counter_(trace_location_.cycles_)

#else

#define SCOPED_PROFILER() do { } while(0)
#define DumpProfileLocations(X) do { } while(0)
#define CountProfileCycles() 0

#endif

#endif
