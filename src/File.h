//
// Created by neverchanje on 11/28/15.
//

#ifndef LEXER_FILE_H
#define LEXER_FILE_H

#include <boost/noncopyable.hpp>
#include <cstdio>
#include <vector>

//TODO: handling exception.

namespace lexer {

/**
 * Buffer read from input_ file and will be write to the output file.
 */
struct Buffer {

  static const size_t DEFAULT_BUFFER_SIZE = (1 << 16);

  static const char END_OF_BUFFER_CHAR = '\0';

  typedef std::vector<char>::iterator Iterator;

  std::vector<char> buf;

  Iterator pos;

  unsigned int lineno, column;

  Buffer(size_t size) :
      buf(size),
      pos(buf.begin()),
      lineno(0),
      column(0) {
    buf[0] = END_OF_BUFFER_CHAR;
  }
};

/**
 * File encapsulates the FILE pointer in RAII.
 */
class File: boost::noncopyable {
 public:

  ~File() {
    fclose(file_);
  }

  const FILE *GetFilePtr() {
    return file_;
  }

 protected:
  FILE *file_;
};

class FileInput: public File {

 public:

  // default input_: stdin
  FileInput() :
      buf_(Buffer::DEFAULT_BUFFER_SIZE) {
    file_ = stdin; // this assignment cannot be place in initializer list.
  }

  // Read the content in filename and initialize the buffer size.
  FileInput(const char *filename, size_t size) :
      buf_(size) {
    file_ = fopen(filename, "r");
  }

  size_t Read(size_t size) {
    return fread(buf_.buf.data(), 1, size, file_);
  }

  Buffer &GetBufferLval() {
    return buf_;
  }

  const Buffer &GetBuffer() const {
    return buf_;
  }

 private:
  Buffer buf_;
};

class FileOutput: public File {

 public:

  // default output: stdout
  FileOutput() {
    file_ = stdout;
  }

  FileOutput(const char *filename) {
    fopen(filename, "w");
  }

  size_t Write(Buffer &buf, size_t size) {
    return fwrite(buf.buf.data(), 1, size, file_);
  }
};

} // namespace lexer

#endif //LEXER_FILE_H