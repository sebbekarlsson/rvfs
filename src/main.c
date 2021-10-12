#include <rvfs/rvfs.h>
#include <stdio.h>

int main(int argc, char *argv[]) {

  // create an archive in-memory from specified path to directory
  RVFSFile f = {};
  rvfs_create_from(&f, argv[1], 0);
  rvfs_free(&f);

  // read an archive from disk into memory
  RVFSFile rf = {};
  rvfs_read(&rf, "test.rvfs");

  // find a file from an archive in memory
  RVFSFile *myfile = rvfs_get_file(&rf, "assets/hello.txt");

  if (myfile) {
    printf("Found it %s\n", myfile->name);
  }

  // extract archive to disk
  rvfs_extract(&rf, "./extract");

  // free RVFSFile struct
  rvfs_free(&rf);

  return 0;
}
