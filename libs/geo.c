#include "geo.h"
#include "types.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTICES 4096
typedef struct {
  Point pt;
  int v_id;
  Vertex *ref; // This is the shared pointer we will reuse
} VertexRegistry;

VertexRegistry visited_vertices[MAX_VERTICES];
int num_visited_vertices = 0;
int next_vertex_id = 1;

Vertex *find_or_create_vertex(Point pt) {
  for (int i = 0; i < num_visited_vertices; i++) {
    if (visited_vertices[i].pt.x == pt.x && visited_vertices[i].pt.y == pt.y) {
      return visited_vertices[i].ref; // Return existing Vertex*
    }
  }

  // Create new vertex
  Vertex *new_vertex = malloc(sizeof(Vertex));
  new_vertex->coords = pt;
  new_vertex->incident_edge = NULL;
  new_vertex->v_id = next_vertex_id;

  // Register the vertex
  visited_vertices[num_visited_vertices].pt = pt;
  visited_vertices[num_visited_vertices].v_id = next_vertex_id;
  visited_vertices[num_visited_vertices].ref = new_vertex;

  num_visited_vertices++;
  next_vertex_id++;

  return new_vertex;
}

#define MAX_HALF_EDGES 4096

HalfEdge *all_half_edges[MAX_HALF_EDGES];
int half_edge_count = 0;

void add_half_edge(HalfEdge *e) {
    if (half_edge_count >= MAX_HALF_EDGES) {
        fprintf(stderr, "Too many half edges!\n");
        exit(1);
    }
    all_half_edges[half_edge_count++] = e;
}

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
        HalfEdge *edge = malloc(sizeof(HalfEdge));

        Vertex *origin = find_or_create_vertex(this_seg.orig);
        if (origin->incident_edge == NULL) {
          origin->incident_edge = edge;
        }
        edge->e_orig = origin;

        add_half_edge(edge);

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

  int edge_count = 1;

  for (int i = 0; i < n_faces; i++) {
    Face curr_face = faces[i];

    // Create the dcel face that holds all half edges
    dcel->faces[i].face_id = i + 1;

    // Previous edge ref
    HalfEdge *prev_seg_edge = NULL;
    HalfEdge *first = NULL;
    HalfEdge *last = NULL;

    for (int x = 0; x < curr_face.seg_size; x++) {
      Segmento curr_seg = curr_face.segments[x];
      Segmento curr_seg_inverted = {{curr_seg.dest.x, curr_seg.dest.y},
                                    {curr_seg.orig.x, curr_seg.orig.y}};

      // All segments already have their original half edge linked to them via
      // map.
      // Now we need to find the twin of that segment and assign each other
      HalfEdge *curr_seg_edge;
      edge_map_get(map, curr_seg, NULL, &curr_seg_edge);

      if (curr_seg_edge == NULL) {
        printf("Could not find edge of this segment, major error :C");
        exit(0);
      }

      HalfEdge *curr_seg_edge_twin;
      edge_map_get(map, curr_seg_inverted, NULL, &curr_seg_edge_twin);

      if (curr_seg_edge_twin == NULL) {
        printf("Could not find twin edge of this segment, major error :C");
        exit(0);
      }

      // Assign twin pointers
      curr_seg_edge->e_twin = curr_seg_edge_twin;
      curr_seg_edge_twin->e_twin = curr_seg_edge;

      // Assign face pointer to this edge
      curr_seg_edge->e_incident_face = &dcel->faces[i];
      curr_seg_edge->e_id = edge_count;
      edge_count++;

      // Fix vertex info

      // Assign previous half edge as previous and next half edge on this edge
      if (x > 0 && x < curr_face.seg_size) {
        curr_seg_edge->e_prev = prev_seg_edge;
        prev_seg_edge->e_next = curr_seg_edge;
      }

      if (x == 0) {
        first = curr_seg_edge; // Save pointer to first
        dcel->faces[i].outer_component = curr_seg_edge;
      }

      if (x == curr_face.seg_size - 1) {
        last = curr_seg_edge; // Save pointer to last
      }

      prev_seg_edge = curr_seg_edge;
    }

    last->e_next = first;
    first->e_prev = last;
  }

  return dcel;
}

void print_all_half_edges() {
  for (int i = 0; i < half_edge_count; i++) {
      HalfEdge *e = all_half_edges[i];
      printf("%d %d %d %d %d\n",
             e->e_orig ? e->e_orig->v_id : -1,
             e->e_twin ? e->e_twin->e_id : -1,
             e->e_incident_face ? e->e_incident_face->face_id : -1,
             e->e_next ? e->e_next->e_id : -1,
             e->e_prev ? e->e_prev->e_id : -1);
  }
}

Vertex *find_vertex_by_point(DCEL *dcel, int n_faces, Point pt) {
  for (int i = 0; i < n_faces; i++) {
    Dcel_Face face = dcel->faces[i];
    HalfEdge *start = face.outer_component;
    if (!start)
      continue;

    HalfEdge *curr = start;
    do {
      if (curr->e_orig && curr->e_orig->coords.x == pt.x &&
          curr->e_orig->coords.y == pt.y) {
        return curr->e_orig;
      }
      curr = curr->e_next;
    } while (curr && curr != start);
  }

  return NULL; // Not found
}