#ifndef RVFS_H
#define RVFS_H
#include <stdint.h>
typedef struct RVFS_FILE_STRUCT {
  uint32_t filepath_length;
  char *filepath;
  uint32_t name_length;
  char *name;
  uint8_t is_directory;
  uint32_t size;
  uint8_t *bytes;
  uint32_t children_length;
  struct RVFS_FILE_STRUCT *children;
} RVFSFile;

void rvfs_create_from(RVFSFile *f, const char *filepath, const char *name);

void rvfs_write(RVFSFile *f, const char *filepath);

void rvfs_read(RVFSFile *f, const char *filepath);

void rvfs_extract(RVFSFile *f, const char *filepath);

void rvfs_free(RVFSFile *f);

#endif
