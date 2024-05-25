#ifndef DISPLAY_RGB565FRAME_H
#define DISPLAY_RGB565FRAME_H

#include "../pqoi/pqoi.h"
#include "../common/cyclint.h"

#define MAX_FRAME_TIME_US 200000

template<size_t WIDTH>
class OutputBuffer {
public:
  int rownum;
  POAtomic<bool> done;
  POLYHOLE;
  PQOI::PqoiOutputChunk<WIDTH> chunk;
  POLYHOLE;
  
  template<size_t N>
  void fill(PQOI::PqoiStreamingDecoder* pqoi, CircularBuffer<uint8_t, N>* input_buffer, int left, int pixels) {
    TRACE2(RGB565_DATA, "fill", chunk.size());
    pqoi->set_input(input_buffer->data(), input_buffer->data() + input_buffer->continuous_data());
    chunk.fill(pqoi, left, pixels);
    TRACE2(RGB565_DATA, "fill pop=", pqoi->in - input_buffer->data());
    input_buffer->pop(pqoi->in - input_buffer->data());
  }
};

class EffectDetector {
public:
  EffectDetector(BladeEffectType effect) : effect_(effect) {}
  BladeEffect* Find() {
    BladeEffect* effects;
    size_t n = SaberBase::GetEffects(&effects);
    for (size_t i = 0; i < n; i++) {
      if (effect_ == effects[i].type) {
	return effects + i;
      }
    }
    return nullptr;
  }
  bool Detect() {
    BladeEffect* e = Find();
    if (!e) return false;
    if (e->start_micros == last_detected_) return false;
    last_detected_ = e->start_micros;
    return true;
  }

private:
  BladeEffectType effect_;
  uint32_t last_detected_;
};

template<size_t WIDTH, size_t HEIGHT>
class PQOILayer : public BufferedFileReader, public LayerControl {
public:
  const uint32_t W = WIDTH;
  const uint32_t H = HEIGHT;

  PQOILayer() : clash_detector_(EFFECT_CLASH), stab_detector_(EFFECT_STAB) {
    state_machine_.stop();
  }

  struct CommonPQVHeader {
    uint32_t magic;
    uint32_t length;
    uint32_t data1;
    uint32_t data2;
  };

  static constexpr uint32_t onecc(char c, int shift) {
    return ((uint32_t)c) << shift;
  }
  // Native byte order.
  static constexpr uint32_t fourcc(char const p[5] ) {
    return onecc(p[0], 0) + onecc(p[1], 8) + onecc(p[2], 16) + onecc(p[3], 24);
  }
  bool checklabel(uint32_t label) {
    switch (label) {
      case 0: return true;
      case fourcc("ison"): return SaberBase::IsOn();
      case fourcc("lock"): return SaberBase::Lockup() == SaberBase::LOCKUP_NORMAL;
      case fourcc("drag"): return SaberBase::Lockup() == SaberBase::LOCKUP_DRAG;
      case fourcc("melt"): return SaberBase::Lockup() == SaberBase::LOCKUP_MELT;
      case fourcc("clsh"): return clash_detector_.Detect();
      case fourcc("stab"): return stab_detector_.Detect();
    }
    // Labels of the form: A<60
    // A can be A, B, C, D, E or R (R is random)
    // < can be < > or =
    // 60 can be 00-99
    int p = 0;
    int variable = label & 0xff;
    if (variable >= 'A' && variable <= 'E') {
      if (variables[variable - 'A']) {
	p = variables[variable - 'A']->percent();
      }
    } else if (variable  == 'R') {
      p = random(100);
    }
    int N = (label >> 16) - '0' * 10 + (label >> 24) - '0';
    switch ((label >> 8) & 0xff) {
      case '<': return p < N;
      case '>': return p > N;
      case '=': return p == N;
    }
    return false;
  }

  void stop() {
    state_machine_.stop();
    play_ = false;
    next_header_ = 0;
  }

  void run() {
    TRACE(RGB565, "layer::run()");
    STATE_MACHINE_BEGIN();
    TRACE(RGB565, "layer::run::begin");

    if (frame_position_in_file_ && micros_ < next_frame_micros_) {
      SEEK(frame_position_in_file_);
      PVLOG_VERBOSE << "Repeat last frame.\n";
      goto frame_found;
    } else {
    read_header:
      if (next_header_ && TELL() != next_header_) {
	STDERR << "Length mismatch, pos = " << TELL() << " expected = " << next_header_ << "\n";
	stop();
	return;
      }
      next_header_ = 0;
      TRACE2(RGB565, "layer::run::read header", TELL());
      read_pos_ = (char*)&header_;
      read_end_ = read_pos_ + sizeof(header_);
      while (read_pos_ < read_end_) {
	TRACE(RGB565, "layer::run::read_header2");
	while (!input_buffer_.size()) {
	  // TRACE(RGB565, "layer::run::PREEOF");
	  if (ATEOF()) {
	    TRACE(RGB565, "layer::run::EOF");
	    stop();
	    return;
	  }
	  // TRACE(RGB565, "layer::run::fill");
	  scheduleFillBuffer();
	  YIELD();
	}
	TRACE(RGB565, "layer::run::copy");
	size_t to_copy = min(read_end_ - read_pos_, input_buffer_.continuous_data());
	PROFFIEOS_ASSERT(to_copy <= 16);
	memcpy(read_pos_, input_buffer_.data(), to_copy);
	input_buffer_.pop(to_copy);
	read_pos_ += to_copy;
      }
      
      TRACE2(RGB565, "layer::run::got header data1=", header_.data1);
      TRACE2(RGB565, "layer::run::got header data2=", header_.data2);

      PVLOG_VERBOSE << "MAGIC: "
	       << (char)(header_.magic & 0xff)
	       << (char)((header_.magic >> 8) & 0xff)
	       << (char)((header_.magic >> 16) & 0xff)
	       << (char)((header_.magic >> 24) & 0xff)
	       << " @ " << (TELL() - 16)
	       << " len = " << header_.length << "\n";
      
      next_header_ = TELL() + header_.length - 8;
      if (header_.magic == fourcc("PQVF")) {
	TRACE(RGB565, "layer::run::PQVF");
	fps_ = header_.data1 / (float)header_.data2;
	micros_per_frame_ = 1000000 / fps_;
	goto read_header;
      } else if (header_.magic == fourcc("GOTO")) {
	TRACE(RGB565, "layer::run::GOTO");
	if (checklabel(header_.data1)) {
	  SEEK(header_.data2);
	  next_header_ = header_.data2;
	}
	goto read_header;
      } else if (header_.magic == fourcc("PQOI")) {
	TRACE(RGB565, "layer::run::PQOI");
	transparent_ = false;
	width_ = header_.data1;
	height_ = header_.data2;
      } else if (header_.magic == fourcc("PQOA")) {
	transparent_ = true;
	if (layer_ == 0) {
	  STDERR << "Transparent image on base layer, giving up...\n";
	  stop();
	  return;
	}
	TRACE(RGB565, "layer::run::PQOA");
	width_ = header_.data1;
	height_ = header_.data2;
      } else if (header_.magic & 0x80808080u || header_.length > 100000000) {
	TRACE(RGB565, "invalid header, end");
	STDERR << "Invalid header = " << header_.magic << " pos = " << TELL() << " FILE = " << file_.GetFilename() << "\n";
	stop();
	return;
      } else {
	TRACE(RGB565, "unknown header, skip");
	STDERR << "Unknown header = " << header_.magic << " pos = " << TELL() << " FILE = " << file_.GetFilename() << "\n";
	SEEK(next_header_);
	goto read_header;
      }
    }

    if (height_ > HEIGHT) {
      STDERR << "PQ file is too tall " << height_ << " > " << HEIGHT << "\n";
      stop();
      return;
    }

    if (width_ > WIDTH) {
      STDERR << "PQ file is too wide " << width_ << " > " << WIDTH << "\n";
      stop();
      return;
    }
    top_margin_ = (HEIGHT - height_) / 2;
    left_margin_ = (WIDTH - width_) / 2;

    TRACE(RGB565, "layer::run::MPF");
    if (micros_per_frame_ == 0 || ATEOF()) goto frame_found;
    TRACE(RGB565, "layer::run::MPF2");
    if (first_frame_) {
      next_frame_micros_ = micros_ + micros_per_frame_;
      first_frame_ = false;
      goto frame_found;
    } else {
      next_frame_micros_ += micros_per_frame_;
      if (next_frame_micros_ > micros_) goto frame_found;
    }
    TRACE(RGB565, "layer::run::FF");
    
    // Skip frame
    SEEK(TELL() + header_.length - 8);
    dropped_frame_counter_.Update(1);
    goto read_header;

  frame_found:
    dropped_frame_counter_.Update(0);
    TRACE(RGB565, "layer::run::frame found");

    frame_position_in_file_ = TELL();
    frame_selected_ = true;

    STATE_MACHINE_END();
  }

  bool SelectFrame(Cyclint<uint32_t> micros) {
    SCOPED_PROFILER();
    TRACE2(RGB565, "SelectFrame", TELL());
    if (micros_ != micros) {
      // New frame.
      micros_ = micros;
      if (delayed_open_) {
	delayed_open_ = false;
	file_.do_open();
	SEEK(0);
	play_ = true;
	first_frame_ = true;
	next_header_ = 0;
	start_time_millis_ = millis();
      }
      if (!play_) return true;
      frame_selected_ = false;
      TRACE(RGB565, "SelectFrame::reset");
      state_machine_.reset_state_machine();
      pqoi.reset();
      if (time_ && play_time() > time_) {
	TRACE(RGB565, "timeout");
	time_ = 0;
	play_ = false;
	return true;
      }
    }
    if (!play_) return true;
    run();
    return state_machine_.done();
  }

  bool Fill(OutputBuffer<WIDTH>* output_buffer) {
    SCOPED_PROFILER();
    TRACE2(RGB565_DATA, "Fill pos=", TELL());
    TRACE2(RGB565_DATA, "Fill row=", output_buffer->rownum);
    if (output_buffer->rownum < top_margin_ || output_buffer->rownum >= height_ + top_margin_) {
      TRACE(RGB565_DATA, "Fill high row");
      output_buffer->chunk.zero(left_margin_, width_);
      return true;
    }
    if (!frame_selected_) {
      TRACE(RGB565_DATA, "Fill@eof");
      // TODO: fill chunk with zeroes?? or wait until data is available?
      ///memset(output_buffer->chunk.pixels.begin(), 0, WIDTH*2);
      return true;
    }
#if 0
    if (!input_buffer_.size()) scheduleFillBuffer();
    output_buffer->fill(&pqoi, &input_buffer_, left_margin_, width_);
    return output_buffer->chunk.full(left_margin_, width_);
#else
    while (!output_buffer->chunk.full(left_margin_, width_)) {
      if (!input_buffer_.size()) scheduleFillBuffer();
      if (!input_buffer_.size()) return false;
      output_buffer->fill(&pqoi, &input_buffer_, left_margin_, width_);
    }
    return true;
#endif    
  }


  // Returns true when done.
  bool Apply(OutputBuffer<WIDTH>* output_buffer, uint16_t* &out) {
    TRACE2(RGB565_DATA, "Apply", TELL());
    if (!play_) return true;
    if (!transparent_) {
      if (!out) output_buffer->chunk.init();
      return Fill(output_buffer);
    }
    if (!frame_selected_) return true;
    if (output_buffer->rownum < top_margin_ || output_buffer->rownum >= height_ + top_margin_) {
      return true;
    }
    if (!out) out = output_buffer->chunk.begin() + left_margin_;
    uint16_t *end = output_buffer->chunk.begin() + left_margin_ + width_;
#if 0
    
    if (!input_buffer_.size()) scheduleFillBuffer();
    pqoi.set_input(input_buffer_.data(), input_buffer_.data() + input_buffer_.continuous_data());
    out = pqoi.Apply(out, end);
    input_buffer_.pop(pqoi.in - input_buffer_.data());
    if (out > end) {
      STDERR << "Rows do not line up: " << (out-end) << " rownum="<< output_buffer->rownum  <<"!\n";
      stop();
    }
    return out >= end;
#else
    while (out < end) {
      if (!input_buffer_.size()) scheduleFillBuffer();
      if (!input_buffer_.size()) return false;
      pqoi.set_input(input_buffer_.data(), input_buffer_.data() + input_buffer_.continuous_data());
      out = pqoi.Apply(out, end);
      input_buffer_.pop(pqoi.in - input_buffer_.data());
    }
    if (out > end) {
      STDERR << "Rows do not line up: " << (out-end) << " rownum="<< output_buffer->rownum  <<"!\n";
      stop();
    }
    return true;
#endif    
  }

  Cyclint<uint32_t> next_frame_time() {
    TRACE(RGB565, "next_frame_time");
    if (micros_per_frame_) return next_frame_micros_;
    return micros_ + MAX_FRAME_TIME_US;
  }


  void LC_setVariable(int variable, VariableSource* variable_source) override {
    TRACE(RGB565, "setVariable");
    if (variable < 0 || variable >= NELEM(variables)) {
      STDERR << "Illegal variable!\n";
      return;
    }
    variables[variable] = variable_source;
  }
  const char* LC_get_filename() override {
    return file_.GetFilename();
  }
  const void LC_restart() override {
    TRACE(RGB565, "LC_restart");
    SEEK(0);
  }
  void LC_play(const char* filename) override {
    TRACE(RGB565, "LC_play");
    delayed_open_ = true;
    file_.PlayInternal(filename);
  }

  void LC_set_time(uint32_t millis) override {
    time_ = millis;
  }

  bool is_playing() const { return play_ || delayed_open_; }

  uint32_t play_time() const {
    return millis() - start_time_millis_;
  }

  void dumpstate() {
    STDOUT << (is_playing() ? "PLAY" : "STOP")
	   << " File: " << file_.GetFilename()
	   << "\nplay time: " << play_time() << "ms"
	   << " POS: " << TELL()
	   << " EOF: " << ATEOF()
	   << " KBPS: " << kbps()
	   << " Bufsize: " << input_buffer_.size()
	   << " next state: " << state_machine_.next_state_ << "\n";
    STDOUT.print(" frame drop fps: ");
    dropped_frame_counter_.Print();
    STDOUT.println("");
  }

  // Video size
  uint16_t width_;
  uint16_t height_;
  uint16_t left_margin_;
  uint16_t top_margin_;

  uint8_t layer_;
  bool transparent_;
protected:
  LoopCounter dropped_frame_counter_;
  uint32_t start_time_millis_;
  bool play_ = false;
  bool frame_selected_ = false;
  
  POLYHOLE;
  uint32_t time_ = 0;
  char* read_pos_;
  char* read_end_;
  CommonPQVHeader header_;
  char tmp_[16];
  Cyclint<uint32_t> micros_;  // time of current frame
  Cyclint<uint32_t> next_frame_micros_;
  uint32_t micros_per_frame_;
  float fps_;
  
  uint32_t frame_position_in_file_;
  uint32_t next_header_= 0;
  
  bool first_frame_ = true;
  bool delayed_open_ = false;

  VariableSource *variables[5];

  PQOI::StreamingAlphaDecoder pqoi;
  EffectDetector clash_detector_;
  EffectDetector stab_detector_;
private:
  POLYHOLE;
  StateMachineState state_machine_;
  POLYHOLE;
};


template<size_t WIDTH, size_t HEIGHT, size_t LAYERS>
class RGB565Frame : public SizedLayeredScreenControl<WIDTH, HEIGHT> {
public:
  static const size_t W = WIDTH;
  static const size_t H = HEIGHT;
  static const uint32_t slice_micros = 2000; // 2ms

  POLYHOLE;
  PQOILayer<WIDTH, HEIGHT> layers[LAYERS];
  POLYHOLE;

  RGB565Frame() {
    for (size_t i = 0; i < LAYERS; i++) {
      layers[i].layer_ = i;
    }
  }

  // Calling initDisplay should disable loop() calls until display is ready.
  virtual void initDisplay() = 0;
  virtual void enableDisplay() = 0;
  virtual void enableBacklight() = 0;
  virtual void disableDisplay() = 0;
  virtual void startTransfer() = 0;
  virtual void fixByteOrder() = 0;
  virtual void swapBuffers() = 0;

  OutputBuffer<WIDTH>* getOutputBuffer() {
    if (output_buffers_.space_available() == 0) return nullptr;
    OutputBuffer<WIDTH>* ret = output_buffers_.space();
    ret->done.set(false);
    output_buffers_.push();
    return ret;
  }

  void frame_loop() {
    SCOPED_PROFILER();
    uint32_t slice_start = micros();
    STATE_MACHINE_BEGIN();

    initDisplay();

    current_output_buffer_ = getOutputBuffer();

    while(true) {
      while (!layers[0].is_playing()) YIELD();

      TRACE(RGB565, "enableDisplay");

      enableDisplay();
      YIELD();  // This makes sure that any pending commands are sent.

      // Clear Screen
      current_output_buffer_->chunk.init(layers[0].left_margin_);
      for (rownum_ = 0; rownum_ < HEIGHT; rownum_++) {
	current_output_buffer_->rownum = rownum_;
	current_output_buffer_->chunk.zero();
	
	// After this, it will be ok to make modifications to the current output buffer.
	while (!(next_output_buffer_ = getOutputBuffer())) YIELD();
	next_output_buffer_->chunk.init_next_chunk(&current_output_buffer_->chunk, 0, 0);
	fixByteOrder();
	current_output_buffer_->done.set(true);
	startTransfer();
	current_output_buffer_ = next_output_buffer_;
	next_output_buffer_ = nullptr;
      }
      swapBuffers();

      // Guess frame update time
      SLEEP_MICROS(1000000/60);
      enableBacklight();
    
      // TODO: deal with screen on/off
      while (layers[0].is_playing()) {
	YIELD();
	TRACE(RGB565, "loop");
	MountSDCard();
	frame_start_ = Cyclint<uint32_t>(micros());

	for (layer = 0; layer < LAYERS; layer++) {
	  while (!layers[layer].SelectFrame(frame_start_)) YIELD();
	}

	if (!layers[0].is_playing()) break;

	for (layer = LAYERS-1; layer >= 0; layer--) {
	  if (layers[layer].is_playing()) {
	    top_layer_ = layer;
	    break;
	  }
	}

	TRACE(RGB565, "loop2");

	current_output_buffer_->chunk.init(layers[0].left_margin_);

	for (rownum_ = 0; rownum_ < HEIGHT; rownum_++) {
	  if (micros() - slice_start > slice_micros) YIELD();
	  
	  TRACE2(RGB565_DATA, "inner loop row=", rownum_);
	  current_output_buffer_->rownum = rownum_;

	  // Base layer
	  while (!layers[0].Fill(current_output_buffer_)) YIELD();

	  // After this, it will be ok to make modifications to the current output buffer.
	  while (!(next_output_buffer_ = getOutputBuffer())) YIELD();
	  next_output_buffer_->chunk.init_next_chunk(&current_output_buffer_->chunk,
						     layers[0].left_margin_,
						     layers[0].width_);
	  
	  current_output_buffer_->chunk.zero_margins(layers[0].left_margin_,
						     layers[0].width_);

	  // Transparent layer(s)
	  for (layer = 1; layer <= top_layer_; layer++) {
	    out = nullptr;
	    while (!layers[layer].Apply(current_output_buffer_, out)) {
	      YIELD();
	    }
	  }

	  fixByteOrder();
	  current_output_buffer_->done.set(true);
	  startTransfer();

	  current_output_buffer_ = next_output_buffer_;
	  next_output_buffer_ = nullptr;
	}
	frame_num_ ++;
	TRACE(RGB565, "loop3");
	swapBuffers(); // may do nothing
	loop_counter_.Update();
	
	next_frame_time_ = frame_start_ + MAX_FRAME_TIME_US;
	for (int l = 0; l < LAYERS; l++) {
	  if (layers[l].is_playing()) {
	    next_frame_time_ = std::min(next_frame_time_, layers[l].next_frame_time());
	  }
	}

	// Frame transferred. Wait for next frame.
	frame_end = Cyclint<uint32_t>(micros());
	if (frame_end < next_frame_time_) {
	  SLEEP_MICROS(next_frame_time_ - frame_end);
	}
      }

      disableDisplay();
      YIELD();  // This makes sure that any pending commands are sent.
    }
    STATE_MACHINE_END();
  }

  LayerControl* getLayer(int layer) override {
    if (layer < 0 || layer >= LAYERS) return nullptr;
    return layers + layer;
  }

  void SB_Top() override {
    STDOUT.print("display fps: ");
    loop_counter_.Print();
    STDOUT.println("");
  }
  
  void dumpstate() {
    STDOUT << "frame = "<< frame_num_
	   << " rownum_ = " << rownum_
	   << " next state: " << state_machine_.next_state_
	   << " buffered rows: " << output_buffers_.size()
	   << "\n";
    for (size_t l = 0; l < LAYERS; l++) {
      STDOUT << "*** Layer " << l << " ***\n";
      layers[l].dumpstate();
    }
#ifdef PQOI_HISTOGRAM    
    uint64_t total = 0;
    for (int b = 0; b < 256; b++) {
      total += PQOI::PQHIST[b];
    }
    for (int b = 0; b < 256; b++) {
      STDOUT << b << " : "
	     << (PQOI::PQHIST[b] * 100.0d / total)
	     <<"% n=" << (double)(PQOI::PQHIST[b]) << "\n";
    }
#endif
  }


protected:
  POLYHOLE;
  uint16_t* out;
  Cyclint<uint32_t> frame_start_;
  uint32_t frame_time_;
  uint8_t* send_pos_;
  // uint8_t cmd;
  
  OutputBuffer<WIDTH>* current_output_buffer_;
  OutputBuffer<WIDTH>* next_output_buffer_;

  uint32_t last_frame_;
  // uint32_t micros_per_frame_;
  Cyclint<uint32_t> next_frame_time_;
  Cyclint<uint32_t> frame_end;
  // Current row
  uint32_t rownum_;
  int layer;
  int top_layer_;
  uint32_t frame_num_ = 0;
  
  CircularBuffer<OutputBuffer<WIDTH>, 32> output_buffers_;
  LoopCounter loop_counter_;
private:
  POLYHOLE;
  StateMachineState state_machine_;
  POLYHOLE;
};

#endif
