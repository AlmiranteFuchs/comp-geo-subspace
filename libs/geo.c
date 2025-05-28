#include "geo.h"
#include "types.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

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

int counter_clock_wise(Point a, Point b, Point c) {
  return (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
}

int on_segment(Point a, Point b, Point c) {
  return MIN(a.x, b.x) <= c.x && c.x <= MAX(a.x, b.x) && MIN(a.y, b.y) <= c.y &&
         c.y <= MAX(a.y, b.y);
}

int is_endpoint_only(Segmento seg, Point pt) {
  return (seg.orig.x == pt.x && seg.orig.y == pt.y) ||
         (seg.dest.x == pt.x && seg.dest.y == pt.y);
}

int intersect(Segmento a, Segmento b) {
  int d1 = counter_clock_wise(a.orig, a.dest, b.orig);
  int d2 = counter_clock_wise(a.orig, a.dest, b.dest);
  int d3 = counter_clock_wise(b.orig, b.dest, a.orig);
  int d4 = counter_clock_wise(b.orig, b.dest, a.dest);

  // Proper intersection: segments cross each other
  if (d1 * d2 < 0 && d3 * d4 < 0)
    return 1;

  // Exclude cases where intersection is a single endpoint
  if (d1 == 0 && on_segment(a.orig, a.dest, b.orig))
    return is_endpoint_only(a, b.orig) ? 0 : 1;
  if (d2 == 0 && on_segment(a.orig, a.dest, b.dest))
    return is_endpoint_only(a, b.dest) ? 0 : 1;
  if (d3 == 0 && on_segment(b.orig, b.dest, a.orig))
    return is_endpoint_only(b, a.orig) ? 0 : 1;
  if (d4 == 0 && on_segment(b.orig, b.dest, a.dest))
    return is_endpoint_only(b, a.dest) ? 0 : 1;

  return 0;
}

int validate_well_defined_topology(Face *faces, int n_faces, EdgeMap *map) {

  // #################################################
  // # "não subdivisão planar" case, duplicate edges #
  // #################################################

  for (int i = 0; i < n_faces; i++) {
    for (int j = 0; j < faces[i].seg_size; j++) {
      //   print_segment(faces[i].segments[j]);
      Segmento this_seg = faces[i].segments[j];

      // Check if this segment exists on the hashtable
      // If it does already --> não subdivisão planar

      int face;
      if (edge_map_get(map, this_seg, &face, NULL)) {

        return TOPOLOGY_INVALID_DUPLICATE;
      } else {
        // Bad code, but let's use this step to pré build de DCEL
        Vertex *origin = malloc(sizeof(Vertex));
        HalfEdge *edge = malloc(sizeof(HalfEdge));

        origin->coords = this_seg.orig;
        origin->incident_edge = edge;

        edge->e_orig = origin;

        edge_map_insert(map, this_seg, i, edge);
      }
    }
  }

  // #############################################################
  // # Now we check for "aberta" case, isolated faces on the map #
  // #############################################################

  for (int i = 0; i < n_faces; i++) {
    int n_segs = faces[i].seg_size;
    int valid_segs = 0;

    for (int j = 0; j < n_segs; j++) {
      Segmento this_seg = faces[i].segments[j];
      Segmento inverted_seg = {{this_seg.dest.x, this_seg.dest.y},
                               {this_seg.orig.x, this_seg.orig.y}};

      // We need to check if all segments of all faces are covered by it's
      // respective inverted from another face

      int face;
      if (edge_map_get(map, inverted_seg, &face, NULL) && face != i) {
        // printf("Neighboors / Same on face %d and face %d: \n", i, face);
        // print_segment(this_seg);
        // print_segment(inverted_seg);
        // printf("\n");
        valid_segs++;
      }
    }

    if (n_segs != valid_segs) {
      // printf("Face %d is invalid\n", i);
      // printf("%d segments exists, but shared %d\n", n_segs, valid_segs);
      return TOPOLOGY_INVALID_OPEN;
    }
  }

  // ############################################################
  // # Now we check for “superposta” case, when edges intersect #
  // ############################################################

  for (int i = 0; i < n_faces; i++) {
    for (int j = 0; j < faces[i].seg_size; j++) {
      //   print_segment(faces[i].segments[j]);
      Segmento seg_a = faces[i].segments[j];
      Segmento seg_a_inverse = {{seg_a.dest.x, seg_a.dest.y},
                                {seg_a.orig.x, seg_a.orig.y}};

      for (int k = i + 1; k < n_faces; k++) {
        for (int l = 0; l < faces[k].seg_size; l++) {
          Segmento seg_b = faces[k].segments[l];

          if (same_segmento(seg_a_inverse, seg_b)) {
            continue;
          }

          if (intersect(seg_a, seg_b)) {
            return TOPOLOGY_INVALID_OVERRIDE;
          }
        }
      }
    }
  }

  return TOPOLOGY_VALID;
}

DCEL *generate_DCEL(EdgeMap *map, Face *faces, int n_faces) {
  DCEL *dcel = malloc(sizeof(DCEL));
  dcel->faces = malloc(n_faces * (sizeof(Dcel_Face)));

  for (int i = 0; i < 1; i++) {
    Face thisFace = faces[i];
    for (int x = 0; x < 1; x++) {
      Segmento thisSeg = thisFace.segments[x];
      HalfEdge *test;
      edge_map_get(map, faces[i].segments[x], NULL, &test);

      print_segment(thisSeg);
      printf("%d %d\n", test->e_orig->coords.x, test->e_orig->coords.y);

      // Find inverted, asign as twin, fix ids idk... 
    }
  }

  return dcel;
}
