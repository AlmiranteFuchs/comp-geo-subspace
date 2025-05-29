#ifndef __UTILS_H__
#define __UTILS_H__

#include "geo.h"
#include "types.h"
#define TABLE_SIZE 1024

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// Functions
void edge_map_init(EdgeMap *map);
int edge_map_insert(EdgeMap *map, Segmento s, int face_id, HalfEdge *edge);
int edge_map_get(EdgeMap *map, Segmento s, int *face_id, HalfEdge **edge);

int same_segmento(Segmento a, Segmento b);

void free_faces(Face *faces, int f_faces);
void free_dcel(DCEL *dcel);

#endif // __UTILS_H__