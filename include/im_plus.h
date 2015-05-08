/** \file
 * \brief C++ Wrapper for File Access
 *
 * See Copyright Notice in im_lib.h
 */

#ifndef __IM_PLUS_H
#define __IM_PLUS_H

#include "im.h"
#include "im_raw.h"


/** \brief Image File Wrapper Class
 *
 * \par
 * Usage is just like the C API. Open and New are replaced by equivalent constructors. \n
 * Close is replaced by the destructor. Error checking is done by the Error() member. \n
 * Open and New errors are cheked using the Failed() member.
 *  \ingroup file */
class imImageFile
{
protected:
  imFile* ifile;
  int error;

  imImageFile() {};

public:

  imImageFile(const char* file_name)
    { this->ifile = imFileOpen(file_name, &this->error); }

  imImageFile(const char* file_name, const char* format)
    { this->ifile = imFileNew(file_name, format, &this->error); }

  virtual ~imImageFile()
    { if (this->ifile) imFileClose(this->ifile); }

  int Failed() 
    { return this->ifile == 0; }

  int Error()
    { return this->error; }

  void SetAttribute(const char* attrib, int data_type, int count, const void* data)
    { imFileSetAttribute(this->ifile, attrib, data_type, count, data); }
    
  void SetAttribInteger(const char* attrib, int data_type, int value)
    { imFileSetAttribInteger(this->ifile, attrib, data_type, value); }

  void SetAttribReal(const char* attrib, int data_type, double value)
    { imFileSetAttribReal(this->ifile, attrib, data_type, value); }

  void SetAttribString(const char* attrib, const char* value)
    { imFileSetAttribString(this->ifile, attrib, value); }

  const void* GetAttribute(const char* attrib, int *data_type, int *count)
    { return imFileGetAttribute(this->ifile, attrib, data_type, count); }
    
  const int GetAttribInteger(const char* attrib, int index)
    { return imFileGetAttribInteger(this->ifile, attrib, index); }

  const double GetAttribReal(const char* attrib, int index)
    { return imFileGetAttribReal(this->ifile, attrib, index); }

  const char* GetAttribString(const char* attrib)
    { return imFileGetAttribString(this->ifile, attrib); }

  void GetInfo(char* format, char* compression, int *image_count)
    { imFileGetInfo(this->ifile, format, compression, image_count); }

  void ReadImageInfo(int index, int *width, int *height, int *color_mode, int *data_type)
    { this->error = imFileReadImageInfo(this->ifile, index, width, height, color_mode, data_type); }

  void GetPalette(long* palette, int *palette_count)
    { imFileGetPalette(this->ifile, palette, palette_count); }

  void ReadImageData(void* data, int convert2bitmap, int color_mode_flags)
    { this->error = imFileReadImageData(this->ifile, data, convert2bitmap, color_mode_flags); }

  void SetInfo(const char* compression)
    { imFileSetInfo(this->ifile, compression); }

  void SetPalette(long* palette, int palette_count)
    { imFileSetPalette(this->ifile, palette, palette_count); }

  void WriteImageInfo(int width, int height, int color_mode, int data_type)
    { this->error = imFileWriteImageInfo(this->ifile, width, height, color_mode, data_type); }

  void WriteImageData(void* data)
    { this->error = imFileWriteImageData(this->ifile, data); }
};


/** \brief Image File Raw Wrapper Class
*
*  \ingroup file */
class imImageFileRaw : public imImageFile
{
  imImageFileRaw() {};

public:

  imImageFileRaw(const char* file_name, bool new_file): imImageFile()
    { if (new_file) this->ifile = imFileNewRaw(file_name, &this->error);
      else this->ifile = imFileOpenRaw(file_name, &this->error); }
};


#endif
