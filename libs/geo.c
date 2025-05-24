#include "geo.h"
#include <stdio.h>

void print_segment(Segmento seg) {
  printf("Segment from (%d, %d) to (%d, %d)\n", seg.orig.x, seg.orig.y,
         seg.dest.x, seg.dest.y);
}

void print_face(Face f) {
    for (int i = 0; i < f.seg_size; i++) {
        printf("  Segment %d: ", i);
        print_segment(f.segments[i]);
    }
}
