#include <dirent.h>
#include <rvfs/rvfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static unsigned int f_exists(const char *filepath) {
  return access(filepath, F_OK) == 0;
}

static unsigned int is_directory(const char *filepath) {
  struct stat statbuf;
  if (stat(filepath, &statbuf) != 0)
    return 0;
  return S_ISDIR(statbuf.st_mode);
}

void rvfs_create_from(RVFSFile *f, const char *filepath, const char *name) {
  if (!filepath)
    return;
  f->filepath_length = strlen(filepath);
  f->filepath = strdup(filepath);
  f->bytes = 0;
  f->name = name ? strdup(name) : 0;
  f->name_length = name ? strlen(name) : 0;
  if (!f_exists(filepath)) {
    f->size = 0;
    return;
  }
  f->is_directory = is_directory(filepath);
  f->children_length = 0;

  if (f->is_directory) {
    DIR *pdir = opendir(f->filepath);

    struct dirent *d = 0;
    while ((d = readdir(pdir))) {
      if (d->d_name[0] == '.')
        continue;
      char *buff = (char *)calloc(strlen(filepath) + strlen(d->d_name) + 2,
                                  sizeof(char));
      sprintf(buff, "%s/%s", filepath, d->d_name);
      f->children_length += 1;
      f->children = (RVFSFile *)realloc(f->children,
                                        f->children_length * sizeof(RVFSFile));
      RVFSFile child = {};
      rvfs_create_from(&child, buff, d->d_name);
      memcpy(&f->children[f->children_length - 1], &child, sizeof(child));

      free(buff);
    }

    if (pdir)
      closedir(pdir);
    return;
  }

  uint32_t read_bytes = 0;
  const uint32_t bufflen = 1;
  uint8_t tmp[bufflen];

  FILE *fp = fopen(filepath, "rb");

  while ((read_bytes = fread(tmp, bufflen, sizeof(uint8_t), fp)) != 0) {
    f->size += read_bytes;
    f->bytes = (uint8_t *)realloc(f->bytes, f->size * sizeof(uint8_t));
    memcpy(&f->bytes[f->size - bufflen], tmp, read_bytes * sizeof(uint8_t));
  }

  printf("Packaged %d bytes.\n", f->size);

  fclose(fp);
}

void _rvfs_write(RVFSFile *f, FILE *fp) {
  fwrite(&f->filepath_length, sizeof(uint32_t), 1, fp);
  fwrite(&*f->filepath, f->filepath_length, sizeof(char), fp);
  fwrite(&f->name_length, sizeof(uint32_t), 1, fp);
  fwrite(&*f->name, sizeof(char), f->name_length, fp);
  fwrite(&f->is_directory, sizeof(uint8_t), 1, fp);
  fwrite(&f->size, sizeof(uint32_t), 1, fp);
  fwrite(&f->bytes[0], sizeof(uint8_t), f->size, fp);
  fwrite(&f->children_length, sizeof(uint32_t), 1, fp);

  for (uint32_t i = 0; i < f->children_length; i++) {
    _rvfs_write(&f->children[i], fp);
  }
}

void rvfs_write(RVFSFile *f, const char *filepath) {
  FILE *fp = fopen(filepath, "wb");
  _rvfs_write(f, fp);
  fclose(fp);
}

void _rvfs_read(RVFSFile *f, FILE *fp) {
  fread(&f->filepath_length, sizeof(uint32_t), 1, fp);
  f->filepath = (char *)calloc(f->filepath_length + 1, sizeof(char));
  fread(&*f->filepath, sizeof(char), f->filepath_length, fp);
  fread(&f->name_length, sizeof(uint32_t), 1, fp);
  f->name = (char *)calloc(f->name_length + 1, sizeof(char));
  fread(&*f->name, sizeof(char), f->name_length, fp);
  fread(&f->is_directory, sizeof(uint8_t), 1, fp);
  fread(&f->size, sizeof(uint32_t), 1, fp);

  f->bytes = (uint8_t *)calloc(f->size, sizeof(uint8_t));
  fread(&f->bytes[0], sizeof(uint8_t), f->size, fp);

  fread(&f->children_length, sizeof(uint32_t), 1, fp);

  f->children = (RVFSFile *)calloc(f->children_length, sizeof(RVFSFile));

  for (uint32_t i = 0; i < f->children_length; i++) {
    _rvfs_read(&f->children[i], fp);
  }
}

void rvfs_read(RVFSFile *f, const char *filepath) {
  FILE *fp = fopen(filepath, "rb");
  _rvfs_read(f, fp);
  fclose(fp);
}

void _rvfs_from_bytes(RVFSFile* f, uint8_t* raw_bytes, uint32_t len) {
  unsigned long int i = 0;
  memcpy(&f->filepath_length, &raw_bytes[i], sizeof(uint32_t) * 1);
  i += sizeof(uint32_t);
  memcpy(&*f->filepath, &raw_bytes[i], sizeof(char) * f->filepath_length);
  i += f->filepath_length * sizeof(char);
  memcpy(&f->name_length, &raw_bytes[i], sizeof(uint32_t) * 1);
  i += sizeof(uint32_t);
  memcpy(&*f->name, &raw_bytes[i], sizeof(char) * f->name_length);
  i += f->name_length * sizeof(char);
  memcpy(&f->is_directory, &raw_bytes[i], sizeof(uint8_t) * 1);
  i += sizeof(uint8_t);
  memcpy(&f->size, &raw_bytes[i], sizeof(uint32_t) * 1);
  i += sizeof(uint32_t);
  memcpy(&f->bytes[0], &raw_bytes[i], sizeof(uint8_t) * f->size);
  i += f->size * sizeof(uint8_t);
  memcpy(&f->children_length, &raw_bytes[i], sizeof(uint32_t) * 1);
  i += sizeof(uint32_t);

  f->children = (RVFSFile *)calloc(f->children_length, sizeof(RVFSFile));

  for (uint32_t i = 0; i < f->children_length; i++) {
    _rvfs_from_bytes(&f->children[i], raw_bytes+i, len);
  }
}

void rvfs_from_bytes(RVFSFile* f, uint8_t* raw_bytes, uint32_t len) {
  _rvfs_from_bytes(f, raw_bytes, len);
  // TODO: implement
}

void rvfs_extract(RVFSFile *f, const char *filepath) {
  if (!filepath)
    return;
  if (f->size && f->bytes) {
    FILE *fp = fopen(filepath, "wb");
    fwrite(&f->bytes[0], sizeof(uint8_t), f->size, fp);
    fclose(fp);
    printf("Extract: wrote %d bytes for %s\n", f->size, filepath);
  }

  for (uint32_t i = 0; i < f->children_length; i++) {
    RVFSFile child = f->children[i];
    if (!child.name)
      continue;
    char *buff =
        (char *)calloc(strlen(filepath) + child.name_length + 16, sizeof(char));
    if (!buff) {
      continue;
    }
    sprintf(buff, "%s/%s", filepath, child.name);
    rvfs_extract(&f->children[i], buff);
    free(buff);
  }
}

RVFSFile *_rvfs_get_file(RVFSFile *f, char *tok) {
  if (strcmp(f->name, tok) == 0) {
    return f;
  }

  if (f->children && f->children_length) {
    tok = strtok(0, "/");

    for (uint32_t i = 0; i < f->children_length; i++) {
      RVFSFile *child = &f->children[i];
      RVFSFile *found = _rvfs_get_file(child, tok);

      if (found)
        return found;
    }
  }

  return 0;
}

RVFSFile *rvfs_get_file(RVFSFile *f, const char *filepath) {
  char *copypath = strdup(filepath);
  char *tok = strtok(copypath, "/");

  return _rvfs_get_file(f, tok);

  return 0;
}

void rvfs_free(RVFSFile *f) {
  if (!f)
    return;
  if (f->filepath)
    free(f->filepath);
  if (f->name)
    free(f->name);
  if (f->bytes)
    free(f->bytes);

  if (f->children && f->children_length) {
    for (uint32_t i = 0; i < f->children_length; i++) {
      RVFSFile child = f->children[i];
      rvfs_free(&child);
    }
  }

  if (f->children) {
    free(f->children);
  }
}
