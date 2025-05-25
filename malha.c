#include "libs/geo.h"
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
    // printf("%d %d\n", verts[i].x, verts[i].y);
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
      printf("Aberta");
      exit(1); // TODO: Free
    }

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
    // printf("Face %d \n", i);
    // print_face(faces[i]);
    // printf("\n ");
  }
}