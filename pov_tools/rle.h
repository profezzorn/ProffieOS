/*
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

void rle_encode(const unsigned char *input,
                int input_length,
                unsigned char *output,
                int *output_length) {
  int i, l, e;
  int olen = 0;
  unsigned char *cnt = 0;
#define VALID_CNT (cnt && *cnt < 255 && cnt+*cnt+2==output+olen)
  assert(*output_length >= input_length + 2 + (input_length >> 7));

  for (i = 0; i < input_length; ) {
    int best = -1;
    int best_offset = -1;
    int l, p;
    for (p = 0; p < 256; p++) {
      int q;
      if (i - p - 1 < 0) break;
      for (l = 0; l < 256 && i+l < input_length; l++) {
        if (input[i+l] != input[i-1-p+l]) break;
      }
      if (l > best) {
        best = l;
        best_offset = p;
      }
    }

    for (l = 1; l < input_length - i; l++) {
      if (input[i] != input[i+l]) {
        break;
      }
    }
    if (best > l+1 && best > (VALID_CNT ? 3 : 2)) {
      output[olen++] = 255;
      output[olen++] = best_offset;
      output[olen++] = best;
      i += best;
    } else if (l >= (VALID_CNT ? 3 : 2)) {
      if (l > 127) {
        l = 127;
      }
      output[olen++] = 128 + (l - 2);
      output[olen++] = input[i];
      i += l;
    } else {
      if (VALID_CNT) {
        (*cnt)++;
      } else {
        cnt = output+olen;
        output[olen++] = 0;
      }
      output[olen++] = input[i];
      i++;
    }
  }
  assert(olen <= *output_length);
  *output_length = olen;
}
