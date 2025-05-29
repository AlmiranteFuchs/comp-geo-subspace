#include "libs/geo.h"
#include "libs/types.h"
#include "libs/utils.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  // Read input
  int n_ver, f_faces;
  scanf("%d %d", &n_ver, &f_faces);

  // Builds the points array ( array of struct )
  Point *verts = malloc(n_ver * sizeof(Point));
  for (int i = 0; i < n_ver; i++) {
    scanf("%d %d", &verts[i].x, &verts[i].y);
  }

  // Builds the faces
  Face *faces = malloc(f_faces * sizeof(Face));

  for (int i = 0; i < f_faces; i++) {
    // Initializes
    // On each line
    Point point_buffer[100];
    int v_index_ref = 0;
    int point_count = 0;

    // Reads points to point_buffer
    while (scanf("%d", &v_index_ref) == 1) {
      Point point = verts[v_index_ref - 1];
      point_buffer[point_count] = point;

      point_count++;

      // Ends reading
      char c = getchar();
      if (c == '\n')
        break;
      if (c != ' ')
        break;
    }

    if (point_count <= 2) {
      printf("aberto");
      exit(1); // TODO: Free
    }

    // Creates faces segments and basic structure
    faces[i].segments = malloc(point_count * sizeof(Segmento));
    faces[i].seg_size = point_count;

    for (int j = 1; j < point_count; j++) {
      Segmento seg;
      seg.orig = point_buffer[j - 1];
      seg.dest = point_buffer[j];

      faces[i].segments[j - 1] = seg;
    }

    Segmento seg;
    seg.orig = point_buffer[point_count - 1];
    seg.dest = point_buffer[0];

    faces[i].segments[point_count - 1] = seg;
  }

  // Initializes the hash table that will help us check segments
  EdgeMap map;
  edge_map_init(&map);

  // Validation
  int topology_case = validate_well_defined_topology(faces, f_faces, &map);
  switch (topology_case) {
  case TOPOLOGY_INVALID_DUPLICATE:
    printf("não subdivisão planar\n");
    break;

  case TOPOLOGY_INVALID_OPEN:
    printf("aberto");
    break;

  case TOPOLOGY_INVALID_OVERRIDE:
    printf("superposta");
    break;

  case TOPOLOGY_VALID:
    DCEL *dcel = generate_DCEL(&map, faces, f_faces);
    //  vertices, arestas, e faces
    int n_arr = 0;
    for (int i = 0; i < f_faces; i++) {
      n_arr += faces[i].seg_size;
    }

    // Printf n m f
    printf("%d %d %d\n", n_ver, n_arr / 2, f_faces);

    // Print xi e yi de cada v´ertice e o ´ındice de uma semi-aresta
    for (int i = 0; i < n_ver; i++) {
      Vertex *ver = find_vertex_by_point(dcel, f_faces, verts[i]);
      printf("%d %d %d\n", verts[i].x, verts[i].y, ver->incident_edge->e_id);
    }

    // Print face and first half edge of face
    for (int i = 0; i < f_faces; i++) {
      HalfEdge *start = dcel->faces[i].outer_component;
      printf("%d\n", start->e_id);
    }

    // Print all half edges
    print_all_half_edges();

    break;
  }
}