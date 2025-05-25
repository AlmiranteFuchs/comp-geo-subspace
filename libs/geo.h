#ifndef __GEO__
#define __GEO__

typedef struct {
  int x, y;
} Point;

typedef struct {
  Point orig;
  Point dest;
} Segmento;

void print_segment(Segmento seg);

typedef struct {
  Segmento *segments;
  int seg_size;
} Face;

void print_face(Face f);

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

#endif // __GEO__
