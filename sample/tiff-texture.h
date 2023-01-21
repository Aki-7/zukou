#pragma once

#include <zukou.h>

#include <bits/types/FILE.h>
#include <stdio.h>
//
#include <jpeglib.h>

class TiffTexture final : public zukou::GlTexture
{
 public:
  DISABLE_MOVE_AND_COPY(TiffTexture);
  TiffTexture(zukou::System *system);
  ~TiffTexture();

  bool Init();

  bool Load(const char *filename);

 private:
  bool loaded_ = false;

  int fd_ = 0;
  zukou::ShmPool pool_;
  zukou::Buffer texture_buffer_;
};
