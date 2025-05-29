#ifndef __GEO_H__
#define __GEO_H__

#include "types.h"
#define TOPOLOGY_INVALID_DUPLICATE 1
#define TOPOLOGY_INVALID_OPEN 2
#define TOPOLOGY_INVALID_OVERRIDE 3
#define TOPOLOGY_VALID 0

void print_segment(Segmento seg);

typedef struct {
  Segmento *segments;
  int seg_size;
} Face;

void print_face(Face f);

int validate_well_defined_topology(Face *faces, int n_faces, EdgeMap *map);

DCEL *generate_DCEL(EdgeMap *map, Face *faces, int n_faces);
void print_all_half_edges();
#endif // __GEO_H__
