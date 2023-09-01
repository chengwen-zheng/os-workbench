#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>
#include "image-data.c"

#define SIDE 16

static int w, h; // Screen size

#define KEYNAME(key) \
  [AM_KEY_##key] = #key,
static const char *key_names[] = {AM_KEYS(KEYNAME)};

static inline void puts(const char *s)
{
  for (; *s; s++)
    putch(*s);
}

// void print_uint32(uint32_t value)
// {
//   char buffer[20];
//   int i = 0;
//   for (; value != 0; i++)
//   {
//     buffer[i] = value % 10 + '0';
//     value /= 10;
//   }

//   buffer[i] = '\0';

//   for (int j = i - 1; j >= 0; j--)
//   {
//     putch(buffer[j]);
//   }
// }

void print_int(int value)
{

  if (value == 0)
  {
    putch('0');
    return;
  }

  int sign = value >= 0 ? 1 : -1;
  value *= sign;

  char buffer[20];
  int i = 0;

  while (value)
  {
    buffer[i++] = value % 10 + '0';
    value /= 10;
  }

  if (sign == -1)
    buffer[i++] = '-';

  for (int j = i - 1; j >= 0; j--)
  {
    putch(buffer[j]);
  }
}

void print_key()
{
  AM_INPUT_KEYBRD_T event = {.keycode = AM_KEY_NONE};
  ioe_read(AM_INPUT_KEYBRD, &event);
  if (event.keycode != AM_KEY_NONE && event.keydown)
  {
    puts("Key pressed: ");
    puts(key_names[event.keycode]);
    puts("\n");
  }

  if (event.keycode == AM_KEY_ESCAPE && event.keydown)
  {
    halt(1);
  }
}

static void draw_pixel(int x, int y, char color)
{
  AM_GPU_FBDRAW_T event = {
      .x = x,
      .y = y,
      .w = 1,
      .h = 1,
      .sync = 1,
      .pixels = &color,
  };

  ioe_write(AM_GPU_FBDRAW, &event);
}

static void draw_image(const char *pixels, int width, int height)
{
  // 以宽度为定值截取高度
  int x_ratio = w / width; // 横向缩放比例
  // int y_ratio = h / height; // 纵向缩放比例
  int newHeight = height * x_ratio;
  puts("width:");
  print_int(width);
  puts("\n");
  puts("height");
  print_int(height);
  puts("\n");
  puts("start draw image");
  for (int y = 0; y < newHeight; y++)
  {
    for (int x = 0; x < w; x++)
    {
      int src_x = x * width / w;
      int src_y = y * height / newHeight;

      char color;
      // 根据坐标在image_data数组中取颜色值
      // 检查坐标是否超出边界
      if (src_x < 0 || src_x >= width || src_y < 0 || src_y >= height)
      {
        puts("The array is out of bounds.");
        color = 0; // 黑色
      }
      else
      {

        color = pixels[src_y * width + src_x];
        // puts("color: ");
        // putch(color);
        // puts("\n");
      }

      draw_pixel(x, y, color);
   
    }
  }
}

void splash()
{
  AM_GPU_CONFIG_T info = {0};
  ioe_read(AM_GPU_CONFIG, &info);
  w = info.width;
  h = info.height;

  draw_image(pixels, 289, 174); // 绘制图片
}

// Operating system is a C program!
int main(const char *args)
{
  ioe_init();

  puts("mainargs = \"");
  puts(args); // make run mainargs=xxx
  puts("\"\n");

  splash();

  puts("Press any key to see its key code...\n");
  while (1)
  {
    print_key();
  }
  return 0;
}
