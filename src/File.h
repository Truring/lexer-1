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
 * Buffer read from input file and will be write to the output file.
 */
struct Buffer {

  static const size_t DEFAULT_BUFFER_SIZE = (1 << 16);

  static const char END_OF_BUFFER_CHAR = '\0';

  // The foolish compiler warns me if I use const rather than constexpr.
  static constexpr float BUFFER_GROW_FACTOR = 1.5f;

  /* static const size_t MAX_BUFFER_SIZE = std::numeric_limit<int>::max(); */

  // The internal buffer array where the data is stored.
  std::vector<char> buf;

  // The current position in the buffer.
  // pos is always in the range of [0, buf.length()]
  size_t pos;

  // The line number and column number of current position.
  unsigned int lineno, column;

  // std::vector always sets size() to capacity() when we specify its initial
  // size. It's odd and therefore we maintain the real size `bufsz`.
  // `bufsz` is one greater than the index of the last valid byte in the buffer.
  size_t bufsz;

  Buffer(size_t size);
  Buffer() :
      Buffer(DEFAULT_BUFFER_SIZE) {
  }

  void Init();
};

/**
 * File encapsulates the FILE pointer in RAII.
 */
class File: boost::noncopyable {
 public:

  File();
  ~File();

  virtual void Reset(const char *filename) = 0;
  void Reset(FILE *file);

  const FILE *GetFilePtr() { return file_; }

 protected:
  FILE *file_;
};

// FileInput is a buffered file reader
class FileInput: public File {

 public:

  FileInput() = default;

  // Read the content in filename and initialize the buffer size.
  FileInput(const char *filename, size_t size);

  FileInput(const char *filename) :
      FileInput(filename, Buffer::DEFAULT_BUFFER_SIZE) {
  }

  void Reset(const char *filename);

  // Read one character from the file.
  char Read();

  Buffer &GetBufferLval() { return buf_; }
  const Buffer &GetBuffer() const { return buf_; }

 private:
  Buffer buf_;
};

class FileOutput: public File {

 public:

  FileOutput() = default;

  FileOutput(const char *filename);

  void Reset(const char *filename);

  // Return the total number of bytes write from the buffer.
  size_t Write(Buffer &buf, size_t size);
};

} // namespace lexer

#endif //LEXER_FILE_H