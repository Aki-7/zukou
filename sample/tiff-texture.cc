#include "tiff-texture.h"

#include <sys/mman.h>
#include <tiffio.h>
#include <unistd.h>

TiffTexture::TiffTexture(zukou::System *system)
    : zukou::GlTexture(system), pool_(system)
{}

TiffTexture::~TiffTexture()
{
  if (fd_ != 0) {
    close(fd_);
  }
}

bool
TiffTexture::Init()
{
  return zukou::GlTexture::Init();
}

bool
TiffTexture::Load(const char *texture_path)
{
  TIFF *tiff;
  uint32_t length = 0;
  uint32_t width = 0;
  uint32_t bits_per_sample = 0;
  uint32_t channel = 0;
  ssize_t size = 0;
  bool result = false;

  tiff = TIFFOpen(texture_path, "r");
  if (tiff == NULL) {
    fprintf(stderr, "Failed to open tiff file");
    return false;
  }

  if (!TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &length)) {
    fprintf(stderr, "Failed to get image length");
    goto out;
  }

  if (!TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width)) {
    fprintf(stderr, "Failed to get image width");
    goto out;
  }

  if (!TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &bits_per_sample)) {
    fprintf(stderr, "Failed to get bits per sample");
    goto out;
  }

  if (!TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &channel)) {
    fprintf(stderr, "Failed to get samples per pixel");
    goto out;
  }

  size = bits_per_sample * channel * width * length / __CHAR_BIT__;
  fd_ = zukou::Util::CreateAnonymousFile(size);
  if (fd_ <= 0) goto out;
  if (!pool_.Init(fd_, size)) goto out_fd;
  if (!texture_buffer_.Init(&pool_, 0, size)) goto out_fd;

  {
    unsigned char *rgba = NULL;
    unsigned char *cursor = NULL;
    rgba = (unsigned char *)mmap(nullptr, size, PROT_WRITE, MAP_SHARED, fd_, 0);
    cursor = rgba;

    for (uint32_t row = 0; row < length; row++) {
      TIFFReadScanline(tiff, cursor, row);
      cursor += TIFFScanlineSize(tiff);
    }

    munmap(rgba, size);
  }

  if (channel == 3 && bits_per_sample == 8) {
    Image2D(GL_TEXTURE_2D, 0, GL_RGB8, width, length, 0, GL_RGB,
        GL_UNSIGNED_BYTE, &texture_buffer_);
  } else if (channel == 1 && bits_per_sample == 32) {
    Image2D(GL_TEXTURE_2D, 0, GL_R32F, width, length, 0, GL_RED, GL_FLOAT,
        &texture_buffer_);
  } else if (channel == 1 && bits_per_sample == 16) {
    Image2D(GL_TEXTURE_2D, 0, GL_R16UI, width, length, 0, GL_RED_INTEGER,
        GL_UNSIGNED_SHORT, &texture_buffer_);
  } else {
    fprintf(stderr, "Unsupported format channel = %d bits_per_sample = %d",
        channel, bits_per_sample);
    goto out_fd;
  }

  loaded_ = true;
  result = true;

out_fd:
  close(fd_);
  fd_ = 0;

out:
  TIFFClose(tiff);

  return result;
}
