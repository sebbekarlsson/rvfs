#include <rvfs/rvfs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static int extract(const char* inputfile, const char* destdir) {
  RVFSFile rf = {};
  rvfs_read(&rf, inputfile);
  rvfs_extract(&rf, destdir);
  rvfs_free(&rf);
  return 0;
}

static int package(const char* inputdir, const char* destfile) {
  RVFSFile f = {};
  rvfs_create_from(&f, inputdir, 0);
  rvfs_write(&f, destfile);
  rvfs_free(&f);

  return 0;
}

/*
char* get_indent(uint32_t n) {
  char* buff = (char*)calloc(n+1, sizeof(char));
  memset(buff, '\t', n);
  return buff;
}*/

static int _show(RVFSFile* f, char* filepath, int indent) {
  if (f == 0) return 1;
    if (f->children && f->children_length) {
      for (uint32_t i = 0; i < f->children_length; i++) {
        RVFSFile child = f->children[i];
        char* newpath = (char*)calloc((filepath ? strlen(filepath) : 0) + strlen(child.name) + 16, sizeof(char));

        if (filepath) {
          strcat(newpath, filepath);
          strcat(newpath, "/");
        }
        strcat(newpath, child.name);
        printf("%d\t%d\t%s\n", child.is_directory, child.size, newpath);
        _show(&child, newpath, indent+1);
        free(newpath);
      }
    }

    return 0;
}

static int show(const char* inputfile) {
  RVFSFile rf = {};
  rvfs_read(&rf, inputfile);
  printf("is_dir\tsize\tname\n");
  _show(&rf, rf.name ? rf.name : rf.filepath, 0);
  rvfs_free(&rf);
  return 1;
}

static int print_help() {
  printf(
    "Usage:\n"
    "\t rvfs <command>\n\n"
    "Commands:\n"
    "\t extract <inputfile> <destdir>\n"
    "\t package <inputdir> <destfile>\n"
    "\t show <inputfile>\n"
  );

  return 1;
}

int main(int argc, char *argv[]) {

  const char* cmd = argv[1];

  if (argc < 2) return print_help();

  if (strcmp(cmd, "extract") == 0) {
    if (argc < 3) return print_help();
    return extract(argv[2], argv[3]);
  }

  if (strcmp(cmd, "package") == 0) {
    if (argc < 3) return print_help();
    return package(argv[2], argv[3]);
  }

  if (strcmp(cmd, "show") == 0) {
    if (argc < 2) return print_help();
    return show(argv[2]);
  }

  return print_help();
}
