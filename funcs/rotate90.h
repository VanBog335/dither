#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Поворачивает изображение RGBA на 90 градусов по часовой стрелке.
// in     - указатель на входный буфер (size = width * height * 4)
// out    - указатель на выходный буфер; если NULL, будет выделен внутри (требуется free)
// width  - ширина входного изображения
// height - высота входного изображения
// Возвращает указатель на повёрнутый буфер (нужно free), или NULL при ошибке.
uint8_t *rotate_rgba90_cw(const uint8_t *in, uint8_t *out, size_t width, size_t height) {
    if (!in || width == 0 || height == 0) return NULL;

    size_t in_pixels = width * height;
    size_t bytes = in_pixels * 4;

    // Если внешний буфер не передан, выделяем.
    if (!out) {
        out = (uint8_t *)malloc(bytes);
        if (!out) return NULL;
    }

    // Новый размер: width' = height, height' = width
    // Для каждой входной позиции (x, y) (0 <= x < width, 0 <= y < height)
    // новая позиция при повороте на 90° CW:
    //   x' = height - 1 - y
    //   y' = x
    // индекс в буфере = (y * width + x) * 4
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            size_t in_idx = (y * width + x) * 4;
            size_t x_p = height - 1 - y;
            size_t y_p = x;
            size_t out_idx = (y_p * height + x_p) * 4; // note: out width = height

            // Копируем 4 байта RGBA
            out[out_idx + 0] = in[in_idx + 0];
            out[out_idx + 1] = in[in_idx + 1];
            out[out_idx + 2] = in[in_idx + 2];
            out[out_idx + 3] = in[in_idx + 3];
        }
    }

    return out;
}

