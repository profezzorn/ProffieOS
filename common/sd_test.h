#ifndef COMMON_SD_TEST_H
#define COMMON_SD_TEST_H

struct SDTestHistogram {
  SDTestHistogram() {
    for (size_t col = 0; col < NELEM(counts); col++) counts[col] = 0;
  }
  void count(uint32_t m) {
    // 10 ticks = 1 ms = > 1 tick == 100 us
    samples++;
    sum_micros+=m;
    counts[clampi32(m/50, 0, NELEM(counts) - 1)]++;
  }
  void print(int rows) {
    uint32_t max = 0;
    for (size_t col = 0; col < NELEM(counts); col++) max = std::max(max, counts[col]);
    float mul = rows / (float)max;
    for (int row = rows; row >=0 ; row--) {
      for (uint32_t col = 0; col < NELEM(counts); col++) {
        float m = counts[col] * mul - row;
	if (m > 0.5) STDOUT << ':';
	else if (m > 0.0) STDOUT << '.';
	else STDOUT << ' ';
      }
      STDOUT << "\n";
    }
    STDOUT << "x100us              1                   2                   3                   4                   5\n";
    STDOUT << "0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0\n";
  }
  float average_us() { return sum_micros / (float)samples; }
  float average_s() { return average_us() / 1000000; }
  float average_kb_per_s() { return 0.5 / average_s(); }
  float average_streams() { return average_kb_per_s() / 88.2; }
  void print_averages() {
    // STDOUT << "Sum micros: " << sum_micros << " samples: " << samples << "\n";
    STDOUT << "Average time: " << average_us() << " us\n";
  }
  void print_speed() {
    // STDOUT << "Sum micros: " << sum_micros << " samples: " << samples << "\n";
    STDOUT << "Average speed: " << average_kb_per_s() << " kb/s, " << average_streams() << " simultaneous audio streams.\n";
  }
  uint32_t samples = 0;
  uint32_t sum_micros = 0;
  uint32_t counts[100];
};

class SDTestHelper {
public:
  SDTestHelper() {
    LOCK_SD(true);
  }
  ~SDTestHelper() {
    LOCK_SD(false);
  }

  void Print() {
    STDOUT << "Time to open files: ";
    open_histogram.print_averages();
    open_histogram.print(10);
    STDOUT << "Time to read blocks: ";
    histogram.print_speed();
    histogram.print(10);
  }

  void TestFile(const char* filename) {
    uint8_t block[512];
    uint32_t start_open = micros();
    File f = LSFS::Open(filename);
    if (!f) {
      STDOUT << "Failed to open!";
    }
    open_histogram.count(micros() - start_open);

    int cnt = 0;
    uint32_t block_start = micros();
    while (f.available()) {
      uint32_t start = micros();
      f.read(block, 512);
      uint32_t end = micros();
      histogram.count(end - start);
      if (++cnt == 128) {
        cnt = 0;
        uint32_t block_time = micros() - block_start;
	int streams = (int)((64.0 / 88.2) / (block_time * 0.000001));
	if (streams < 10) STDOUT << (char)('0' + streams);
	else if (streams < 36) STDOUT << (char)('A' + streams - 10);
	else STDOUT << '!';
	block_start = micros();
        cnt = 0;
      }
    }
    STDOUT << '\n';
    f.close();
  }

  void TestFont() {
    for (Effect* effect = all_effects; effect; effect = effect->next_) {
      for (uint32_t alt = 0; alt < effect->number_of_alternatives(); alt++) {
	for (uint32_t i = 0; i < effect->files_found(); i++) {
	  for (uint32_t subid = 0; subid < effect->number_of_subfiles(); subid++) {
	    Effect::FileID file_id(effect, i, subid, alt);
	    effect->GetName(filename_, &file_id);
	    TestFile(filename_);
	  }
	}
      }
    }
    Print();
  }

  void TestIterator(LSFS::Iterator& iter) {
    STDOUT << "Testing " << filename_ << "\n";
    char* fend = filename_;
    int flen = strlen(filename_);
    fend += flen;
    if (flen && fend[-1] != '/') {
      *fend = '/';
      fend++;
    }
    for (; iter; ++iter) {
      if (!strcmp(iter.name(), ".")) continue;
      if (!strcmp(iter.name(), "..")) continue;
      strcpy(fend, iter.name());
      if (iter.isdir()) {
	LSFS::Iterator i2(iter);
	TestIterator(i2);
      } else {
	STDOUT << "Reading " << filename_ << ": ";
        TestFile(filename_);
      }
    }
    *fend = 0;
  }

  void TestDir(const char* dirname) {
    if (*dirname && !LSFS::Exists(dirname)) {
      STDOUT << "Directory " << dirname << " does not exist.\n";
      return;
    }
    LSFS::Iterator iter(dirname);
    strcpy(filename_, dirname);
    TestIterator(iter);
    Print();
  }

private:
  char filename_[256];
  SDTestHistogram histogram;
  SDTestHistogram open_histogram;
};

#endif
