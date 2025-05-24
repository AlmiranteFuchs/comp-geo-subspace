#ifndef __GEO__
#define __GEO__

typedef struct {
  int x, y;
} Vertice;

typedef struct {
  Vertice orig;
  Vertice dest;
} Segmento;

void print_segment(Segmento seg);

typedef struct {
  Segmento *segments;
  int seg_size;
} Face;

void print_face(Face f);

#endif // __GEO__
