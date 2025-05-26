// types.h
#ifndef __TYPES_H__
#define __TYPES_H__

typedef struct {
  int x, y;
} Point;

typedef struct {
  Point orig;
  Point dest;
} Segmento;

typedef struct {
    Segmento key;
    int face_id;   // Mapped value
    int used;      // 0 = empty, 1 = used
} EdgeEntry;

typedef struct {
    EdgeEntry table[1024];
} EdgeMap;

#endif
