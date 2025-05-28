// types.h
#ifndef __TYPES_H__
#define __TYPES_H__

struct Vertex;
struct HalfEdge;
struct Dcel_Face;

typedef struct {
  int x, y;
} Point;

typedef struct {
  Point orig;
  Point dest;
} Segmento;

typedef struct {
  Segmento key;
  int face_id;                // Mapped value
  int used;                   // 0 = empty, 1 = used
  struct HalfEdge *half_edge; // For mapping
} EdgeEntry;

typedef struct {
  EdgeEntry table[1024];
} EdgeMap;

// ##################
// # DCEL STRUCTURE #
// ##################

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

  int e_id;
} HalfEdge;

typedef struct Dcel_Face {
  HalfEdge *outer_component;

  int face_id;
} Dcel_Face;

typedef struct DCEL {
  Dcel_Face *faces;
} DCEL;

#endif
