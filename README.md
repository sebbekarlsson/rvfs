# rvfs
> Read-only virtual file system

## Purpose
> The purpose of this is to be able to ship single executables / libraries where all the
> external assets & files can be bundled into the binary, and also to provide a comfortable interface  
> to work with these files inside the application / library.

### Example Use cases
* VST Plugins that requires external files to function
* Games that depends on external shader files
* Any other application that depends on external files in some way

## usage

``` C

// Populates a RVFSFile struct with recursive content of directory,
// including files in it.
void rvfs_create_from(RVFSFile *f, const char *filepath, const char *name);

// Write a rvfs archive
void rvfs_write(RVFSFile *f, const char *filepath);

// Read a rvfs archive from disk
void rvfs_read(RVFSFile *f, const char *filepath);

// Extract the contents of an rvfs archive to disk
void rvfs_extract(RVFSFile *f, const char *filepath);

// Grab a file from an archive.
RVFSFile* rvfs_get_file(RVFSFile* f, const char* filepath);

// free & deallocate RVFSFile struct.
void rvfs_free(RVFSFile *f);

```

> Basic example:

``` C
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
  RVFSFile* myfile = rvfs_get_file(&rf, "assets/hello.txt");

  if (myfile) {
    printf("Found it %s\n", myfile->name);
  }

  // extract archive to disk
  rvfs_extract(&rf, "./extract");

  // free RVFSFile struct
  rvfs_free(&rf);

  return 0;
}

```


## Linking a .rvfs file into your library or executable
> First convert the .rvfs file to a .o file
``` bash
objcopy -B i386 -I binary -O elf32-i386 <your-file>.rvfs <your-file>.o
```
> Then you can link it to your lib/executable and access the data like this:

``` C
extern uint8_t foo_data[]      asm("_binary_foo_data_bin_start");
extern uint8_t foo_data_size[] asm("_binary_foo_data_bin_size");
extern uint8_t foo_data_end[]  asm("_binary_foo_data_bin_end");


int main(int argc, char* argv[]) {
    size_t foo_size = (size_t)((void *)foo_data_size);
    void  *foo_copy = malloc(foo_size);
    assert(foo_copy);
    memcpy(foo_copy, foo_data, foo_size);
    
    // foo_copy should now be an instance of `RVFSFile`.
    RVFSFile* rvfs_file = (RVFSFile*)foo_copy;
}
```
> "foo" in this case would be what you named your .o file.

## .rvfs file format specification
> The file contains the following fields:

``` C
typedef struct RVFS_FILE_STRUCT {
  uint32_t filepath_length;         // length of filepath in bytes
  char *filepath;                   // path to where the file was loaded from
  uint32_t name_length;             // length of name in bytes
  char *name;                       // name of file without path
  uint8_t is_directory;             // is it a directory or not? (1 / 0)
  uint32_t size;                    // length of the `bytes` field following
  uint8_t *bytes;                   // file contents
  uint32_t children_length;         // how many children this node has
  struct RVFS_FILE_STRUCT *children // list of children;
} RVFSFile;
```
