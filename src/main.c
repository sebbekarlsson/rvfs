#include <rvfs/rvfs.h>
#include <stdio.h>

int main(int argc, char *argv[]) {

  RVFSFile f = {};
  rvfs_create_from(&f, argv[1], 0);
  rvfs_write(&f, "test.rvfs");

  rvfs_free(&f);
  printf("Wrote test.rvfs.\n");

  RVFSFile rf = {};
  rvfs_read(&rf, "test.rvfs");

  rvfs_extract(&rf, "./extract");

  rvfs_free(&rf);

  return 0;
}
