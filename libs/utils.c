#include "utils.h"
#include "geo.h"
#include "types.h"
#include <stdlib.h>

void edge_map_init(EdgeMap *map) {
  for (int i = 0; i < TABLE_SIZE; i++) {
    map->table[i].used = 0;
  }
}

unsigned int point_hash(Point p) {
  return ((unsigned int)p.x * 73856093u) ^ ((unsigned int)p.y * 19349663u);
}

unsigned int segmento_hash(Segmento s) {
  return point_hash(s.orig) ^ (point_hash(s.dest) << 1);
}

int same_point(Point a, Point b) { return a.x == b.x && a.y == b.y; }

int same_segmento(Segmento a, Segmento b) {
  return same_point(a.orig, b.orig) && same_point(a.dest, b.dest);
}

int edge_map_insert(EdgeMap *map, Segmento s, int face_id, HalfEdge *edge) {
  unsigned int idx = segmento_hash(s) % TABLE_SIZE;

  for (int i = 0; i < TABLE_SIZE; i++) {
    int pos = (idx + i) % TABLE_SIZE;
    if (!map->table[pos].used) {
      map->table[pos].key = s;
      map->table[pos].face_id = face_id;
      map->table[pos].used = 1;
      map->table[pos].half_edge = edge;
      return 1; // Inserido com sucesso
    } else if (same_segmento(map->table[pos].key, s)) {
      map->table[pos].half_edge = edge; // <<< sobrescreve
      return 1; // atualizado com sucesso
    }
  }
  return -1; // Mapa cheio
}

int edge_map_get(EdgeMap *map, Segmento s, int *face_id, HalfEdge **edge) {
  unsigned int idx = segmento_hash(s) % TABLE_SIZE;

  for (int i = 0; i < TABLE_SIZE; i++) {
    int pos = (idx + i) % TABLE_SIZE;
    if (!map->table[pos].used)
      return 0;
    if (same_segmento(map->table[pos].key, s)) {
        if (face_id) *face_id = map->table[pos].face_id;
        if (edge) *edge = map->table[pos].half_edge;
      return 1;
    }
  }
  return 0; // Não encontrado
}

void free_faces(Face *faces, int f_faces) {
  if (!faces) return;
  for (int i = 0; i < f_faces; i++) {
    if (faces[i].segments)
      free(faces[i].segments);
  }
  free(faces);
}

void free_dcel(DCEL *dcel) {
  if (!dcel) return;

  // Free DCEL face array
  free(dcel->faces);
  free(dcel);
}