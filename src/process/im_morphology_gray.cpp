/** \file
 * \brief Morphology Operations for Gray Images
 *
 * See Copyright Notice in im_lib.h
 */


#include <im.h>
#include <im_util.h>
#include <im_convert.h>

#include "im_process_counter.h"
#include "im_process_loc.h"
#include "im_process_pnt.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <math.h>


template <class T, class DT> 
static int DoGrayMorphConvolve(T *map, T* new_map, int width, int height, const imImage* kernel, int counter, int ismax, DT)
{
  int kw = kernel->width;
  int kh2 = kernel->height/2;
  int kw2 = kernel->width/2;

  DT* kernel_data = (DT*)kernel->data[0];

  IM_INT_PROCESSING;

#ifdef _OPENMP
#pragma omp parallel for if (IM_OMP_MINHEIGHT(height))
#endif
  for(int j = 0; j < height; j++)
  {
#ifdef _OPENMP
#pragma omp flush (processing)
#endif
    IM_BEGIN_PROCESSING;

    int new_offset = j * width;

    for(int i = 0; i < width; i++)
    {
      DT value, max = 0, min = 0;
      int init = 0;
    
      for(int y = -kh2; y <= kh2; y++)
      {
        int line_offset;
        DT* kernel_line = kernel_data + (y+kh2)*kw;

        if ((j + y < 0) ||          // pass the bottom border
            (j + y >= height))      // pass the top border
          continue;
        else
          line_offset = (j + y) * width;

        for(int x = -kw2; x <= kw2; x++)
        {
          if (kernel_line[x+kw2] != -1)
          {
            if ((i + x < 0) ||      // pass the left border
                (i + x >= width))   // pass the right border
              continue;
            else
              value = kernel_line[x+kw2] + map[line_offset + (i + x)];

            if (init == 0)  // first time here for each pass
            {
              if (ismax)
                max = value;
              else
                min = value;

              init = 1;
            }
            else
            {
              if (ismax && value > max)
                max = value;

              if (!ismax && value < min)
                min = value;
            }
          }
        }
      }
      
      int size_of = sizeof(imbyte);
      if (sizeof(T) == size_of)
      {
        if (ismax)
          new_map[new_offset + i] = (T)IM_BYTECROP(max);
        else
          new_map[new_offset + i] = (T)IM_BYTECROP(min);
      }
      else
      {
        if (ismax)
          new_map[new_offset + i] = (T)max;
        else
          new_map[new_offset + i] = (T)min;
      }
    }    

    IM_COUNT_PROCESSING;
#ifdef _OPENMP
#pragma omp flush (processing)
#endif
    IM_END_PROCESSING;
  }

  return processing;
}

int imProcessGrayMorphConvolve(const imImage* src_image, imImage* dst_image, const imImage *kernel, int ismax)
{
  int ret = 0;

  int counter = imProcessCounterBegin("GrayMorphConvolve");
  imCounterTotal(counter, src_image->depth*src_image->height, "Processing...");

  imImage* fkernel = NULL;
  if ((src_image->data_type == IM_FLOAT || src_image->data_type == IM_DOUBLE) && 
       kernel->data_type != src_image->data_type)
  {
    fkernel = imImageCreate(kernel->width, kernel->height, IM_GRAY, src_image->data_type);
    imProcessConvertDataType(kernel, fkernel, 0, 0, 0, IM_CAST_DIRECT);
    kernel = fkernel;
  }

  for (int i = 0; i < src_image->depth; i++)
  {
    switch(src_image->data_type)
    {
    case IM_BYTE:
      ret = DoGrayMorphConvolve((imbyte*)src_image->data[i], (imbyte*)dst_image->data[i], src_image->width, src_image->height, kernel, counter, ismax, (int)0);
      break;                                                                                
    case IM_SHORT:
      ret = DoGrayMorphConvolve((short*)src_image->data[i], (short*)dst_image->data[i], src_image->width, src_image->height, kernel, counter, ismax, (int)0);
      break;                                                                                
    case IM_USHORT:
      ret = DoGrayMorphConvolve((imushort*)src_image->data[i], (imushort*)dst_image->data[i], src_image->width, src_image->height, kernel, counter, ismax, (int)0);
      break;                                                                                
    case IM_INT:                                                                           
      ret = DoGrayMorphConvolve((int*)src_image->data[i], (int*)dst_image->data[i], src_image->width, src_image->height, kernel, counter, ismax, (int)0);
      break;                                                                                
    case IM_FLOAT:
      ret = DoGrayMorphConvolve((float*)src_image->data[i], (float*)dst_image->data[i], src_image->width, src_image->height, kernel, counter, ismax, (float)0);
      break;                                                                                
    case IM_DOUBLE:
      ret = DoGrayMorphConvolve((double*)src_image->data[i], (double*)dst_image->data[i], src_image->width, src_image->height, kernel, counter, ismax, (double)0);
      break;
    }
    
    if (!ret) 
      break;
  }

  if (fkernel) imImageDestroy(fkernel);

  imProcessCounterEnd(counter);

  return ret;
}

int imProcessGrayMorphErode(const imImage* src_image, imImage* dst_image, int kernel_size)
{
  imImage* kernel = imImageCreate(kernel_size, kernel_size, IM_GRAY, IM_INT);
  imImageSetAttribute(kernel, "Description", IM_BYTE, -1, (void*)"Erode");
  // Kernel is all zeros
  int ret = imProcessGrayMorphConvolve(src_image, dst_image, kernel, 0);
  imImageDestroy(kernel);
  return ret;
}

int imProcessGrayMorphDilate(const imImage* src_image, imImage* dst_image, int kernel_size)
{
  imImage* kernel = imImageCreate(kernel_size, kernel_size, IM_GRAY, IM_INT);
  imImageSetAttribute(kernel, "Description", IM_BYTE, -1, (void*)"Dilate");
  // Kernel is all zeros
  int ret = imProcessGrayMorphConvolve(src_image, dst_image, kernel, 1);
  imImageDestroy(kernel);
  return ret;
}

int imProcessGrayMorphOpen(const imImage* src_image, imImage* dst_image, int kernel_size)
{
  imImage*temp = imImageClone(src_image);
  if (!temp)
    return 0;

  if (!imProcessGrayMorphErode(src_image, temp, kernel_size)) 
    {imImageDestroy(temp); return 0;}
  if (!imProcessGrayMorphDilate(temp, dst_image, kernel_size)) 
    {imImageDestroy(temp); return 0;}

  imImageDestroy(temp);
  return 1;
}

int imProcessGrayMorphClose(const imImage* src_image, imImage* dst_image, int kernel_size)
{
  imImage*temp = imImageClone(src_image);
  if (!temp)
    return 0;

  if (!imProcessGrayMorphDilate(src_image, temp, kernel_size)) 
    {imImageDestroy(temp); return 0;}
  if (!imProcessGrayMorphErode(temp, dst_image, kernel_size)) 
    {imImageDestroy(temp); return 0;}

  imImageDestroy(temp);
  return 1;
}

int imProcessGrayMorphTopHat(const imImage* src_image, imImage* dst_image, int kernel_size)
{
  if (!imProcessGrayMorphOpen(src_image, dst_image, kernel_size)) 
    return 0;
  imProcessArithmeticOp(src_image, dst_image, dst_image, IM_BIN_DIFF);
  return 1;
}

int imProcessGrayMorphWell(const imImage* src_image, imImage* dst_image, int kernel_size)
{
  if (!imProcessGrayMorphClose(src_image, dst_image, kernel_size)) 
    return 0;
  imProcessArithmeticOp(src_image, dst_image, dst_image, IM_BIN_DIFF);
  return 1;
}

int imProcessGrayMorphGradient(const imImage* src_image, imImage* dst_image, int kernel_size)
{
  imImage*temp = imImageClone(src_image);
  if (!temp)
    return 0;

  if (!imProcessGrayMorphDilate(src_image, temp, kernel_size)) 
    {imImageDestroy(temp); return 0;}
  if (!imProcessGrayMorphErode(src_image, dst_image, kernel_size)) 
    {imImageDestroy(temp); return 0;}

  imProcessArithmeticOp(temp, dst_image, dst_image, IM_BIN_DIFF);

  imImageDestroy(temp);
  return 1;
}

