#ifndef __GEO_H__
#define __GEO_H__

#include "types.h"
#define TOPOLOGY_INVALID_DUPLICATE 1
#define TOPOLOGY_INVALID_OPEN      2
#define TOPOLOGY_INVALID_OVERRIDE  3
#define TOPOLOGY_VALID             0

void print_segment(Segmento seg);

typedef struct {
  Segmento *segments;
  int seg_size;
} Face;

void print_face(Face f);

int validate_well_defined_topology(Face *faces, int n_faces, EdgeMap map);

// ##################
// # DCEL STRUCTURE #
// ##################

struct Vertex;
struct HalfEdge;
struct Dcel_Face;

typedef struct Vertex {
  Point coords;
  struct HalfEdge *incident_edge; // half-edges that starts from this vertex
} Vertex;

typedef struct HalfEdge {
  Vertex *e_orig;
  struct HalfEdge *e_twin;
  struct HalfEdge *e_next;
  struct HalfEdge *e_prev;

  struct Dcel_Face *e_incident_face; // face that lies to the left
} HalfEdge;

typedef struct Dcel_Face {
  HalfEdge *outer_face;
  HalfEdge **inner_faces;
  int n_inner_faces;
} Dcel_Face;

#endif // __GEO_H__
