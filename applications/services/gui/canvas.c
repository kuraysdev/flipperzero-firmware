#include "canvas_i.h"
#include "icon_animation_i.h"

#include <furi.h>
#include <furi_hal.h>
#include <stdint.h>
#include <u8g2_glue.h>

const CanvasFontParameters canvas_font_params[FontTotalNumber] = {
    [FontPrimary] = {.leading_default = 12, .leading_min = 11, .height = 8, .descender = 2},
    [FontSecondary] = {.leading_default = 11, .leading_min = 9, .height = 7, .descender = 2},
    [FontKeyboard] = {.leading_default = 11, .leading_min = 9, .height = 7, .descender = 2},
    [FontBigNumbers] = {.leading_default = 18, .leading_min = 16, .height = 15, .descender = 0},
};

Canvas* canvas_init(void) {
    Canvas* canvas = malloc(sizeof(Canvas));
    canvas->compress_icon = compress_icon_alloc(ICON_DECOMPRESSOR_BUFFER_SIZE);

    // Initialize mutex
    canvas->mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    // Initialize callback array
    CanvasCallbackPairArray_init(canvas->canvas_callback_pair);

    // Setup u8g2
    u8g2_Setup_st756x_flipper(&canvas->fb, U8G2_R0, u8x8_hw_spi_stm32, u8g2_gpio_and_delay_stm32);
    canvas->orientation = CanvasOrientationHorizontal;
    // Initialize display
    u8g2_InitDisplay(&canvas->fb);
    // Wake up display
    u8g2_SetPowerSave(&canvas->fb, 0);

    // Clear buffer and send to device
    canvas_clear(canvas);
    canvas_commit(canvas);

    return canvas;
}

void canvas_free(Canvas* canvas) {
    furi_check(canvas);
    compress_icon_free(canvas->compress_icon);
    CanvasCallbackPairArray_clear(canvas->canvas_callback_pair);
    furi_mutex_free(canvas->mutex);
    free(canvas);
}

static void canvas_lock(Canvas* canvas) {
    furi_assert(canvas);
    furi_check(furi_mutex_acquire(canvas->mutex, FuriWaitForever) == FuriStatusOk);
}

static void canvas_unlock(Canvas* canvas) {
    furi_assert(canvas);
    furi_check(furi_mutex_release(canvas->mutex) == FuriStatusOk);
}

void canvas_reset(Canvas* canvas) {
    furi_check(canvas);

    canvas_clear(canvas);

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    canvas_set_font_direction(canvas, CanvasDirectionLeftToRight);
}

void canvas_commit(Canvas* canvas) {
    furi_check(canvas);
    u8g2_SendBuffer(&canvas->fb);

    // Iterate over callbacks
    canvas_lock(canvas);
    for
        M_EACH(p, canvas->canvas_callback_pair, CanvasCallbackPairArray_t) {
            p->callback(
                canvas_get_buffer(canvas),
                canvas_get_buffer_size(canvas),
                canvas_get_orientation(canvas),
                p->context);
        }
    canvas_unlock(canvas);
}

uint8_t* canvas_get_buffer(Canvas* canvas) {
    furi_check(canvas);
    return u8g2_GetBufferPtr(&canvas->fb);
}

size_t canvas_get_buffer_size(const Canvas* canvas) {
    furi_check(canvas);
    return u8g2_GetBufferTileWidth(&canvas->fb) * u8g2_GetBufferTileHeight(&canvas->fb) * 8;
}

void canvas_frame_set(
    Canvas* canvas,
    int32_t offset_x,
    int32_t offset_y,
    size_t width,
    size_t height) {
    furi_check(canvas);
    canvas->offset_x = offset_x;
    canvas->offset_y = offset_y;
    canvas->width = width;
    canvas->height = height;
}

size_t canvas_width(const Canvas* canvas) {
    furi_check(canvas);
    return canvas->width;
}

size_t canvas_height(const Canvas* canvas) {
    furi_check(canvas);
    return canvas->height;
}

size_t canvas_current_font_height(const Canvas* canvas) {
    furi_check(canvas);
    size_t font_height = u8g2_GetMaxCharHeight(&canvas->fb);

    if(canvas->fb.font == u8g2_font_haxrcorp4089_tr) {
        font_height += 1;
    }

    return font_height;
}

const CanvasFontParameters* canvas_get_font_params(const Canvas* canvas, Font font) {
    furi_check(canvas);
    furi_check(font < FontTotalNumber);
    return &canvas_font_params[font];
}

void canvas_clear(Canvas* canvas) {
    furi_check(canvas);
    u8g2_ClearBuffer(&canvas->fb);
}

void canvas_set_color(Canvas* canvas, Color color) {
    furi_check(canvas);
    u8g2_SetDrawColor(&canvas->fb, color);
}

void canvas_set_font_direction(Canvas* canvas, CanvasDirection dir) {
    furi_check(canvas);
    u8g2_SetFontDirection(&canvas->fb, dir);
}

void canvas_invert_color(Canvas* canvas) {
    canvas->fb.draw_color = !canvas->fb.draw_color;
}

void canvas_set_font(Canvas* canvas, Font font) {
    furi_check(canvas);
    u8g2_SetFontMode(&canvas->fb, 1);
    //Надо найти шрифты нормальные под каждое
    if(font == FontPrimary) {
        //u8g2_SetFont(&canvas->fb, u8g2_font_helvB08_tr);
        u8g2_SetFont(&canvas->fb, u8g2_font_6x12_t_cyrillic);
    } else if(font == FontSecondary) {
        //u8g2_SetFont(&canvas->fb, u8g2_font_haxrcorp4089_tr);
        u8g2_SetFont(&canvas->fb, u8g2_font_6x12_t_cyrillic);
    } else if(font == FontKeyboard) {
        //u8g2_SetFont(&canvas->fb, u8g2_font_profont11_mr);
        u8g2_SetFont(&canvas->fb, u8g2_font_6x12_t_cyrillic);
    } else if(font == FontBigNumbers) {
        //u8g2_SetFont(&canvas->fb, u8g2_font_profont22_tn);
        u8g2_SetFont(&canvas->fb, u8g2_font_6x12_t_cyrillic);
    } else {
        furi_crash();
    }
}

void canvas_set_custom_u8g2_font(Canvas* canvas, const uint8_t* font) {
    furi_check(canvas);
    u8g2_SetFontMode(&canvas->fb, 1);
    u8g2_SetFont(&canvas->fb, font);
}

// Ориг функции комментнул возможно будут проблемы с мерджем новых версий
// void canvas_draw_str(Canvas* canvas, int32_t x, int32_t y, const char* str) {
//     furi_check(canvas);
//     if(!str) return;
//     x += canvas->offset_x;
//     y += canvas->offset_y;
//     u8g2_DrawUTF8(&canvas->fb, x, y, str);
// }

// void canvas_draw_str_aligned(
//     Canvas* canvas,
//     int32_t x,
//     int32_t y,
//     Align horizontal,
//     Align vertical,
//     const char* str) {
//     furi_check(canvas);
//     if(!str) return;
//     x += canvas->offset_x;
//     y += canvas->offset_y;

//     switch(horizontal) {
//     case AlignLeft:
//         break;
//     case AlignRight:
//         x -= u8g2_GetUTF8Width(&canvas->fb, str);
//         break;
//     case AlignCenter:
//         x -= (u8g2_GetUTF8Width(&canvas->fb, str) / 2);
//         break;
//     default:
//         furi_crash();
//         break;
//     }

//     switch(vertical) {
//     case AlignTop:
//         y += u8g2_GetAscent(&canvas->fb);
//         break;
//     case AlignBottom:
//         break;
//     case AlignCenter:
//         y += (u8g2_GetAscent(&canvas->fb) / 2);
//         break;
//     default:
//         furi_crash();
//         break;
//     }

//     u8g2_DrawUTF8(&canvas->fb, x, y, str);
// }

void canvas_draw_str(Canvas* canvas, int32_t x, int32_t y, const char* str) {
    if(canvas->fb.font == u8g2_font_6x12_t_cyrillic) {
        canvas_draw_utf8_str(canvas, x, y, str);
    } else {
        u8g2_DrawUTF8(&canvas->fb, x + canvas->offset_x, y + canvas->offset_y, str);
    }
}

void canvas_draw_str_aligned(
    Canvas* canvas,
    int32_t x,
    int32_t y,
    Align horizontal,
    Align vertical,
    const char* str) {
    furi_check(canvas);
    if(!str) return;
    x += canvas->offset_x;
    y += canvas->offset_y;

    switch(horizontal) {
    case AlignLeft:
        break;
    case AlignRight:
        x -= canvas_string_width(canvas, str);
        break;
    case AlignCenter:
        x -= (canvas_string_width(canvas, str) / 2);
        break;
    default:
        furi_crash();
        break;
    }

    switch(vertical) {
    case AlignTop:
        y += canvas_current_font_height(canvas);
        break;
    case AlignBottom:
        break;
    case AlignCenter:
        y += (canvas_current_font_height(canvas) / 2);
        break;
    default:
        furi_crash();
        break;
    }

    if(canvas->fb.font == u8g2_font_6x12_t_cyrillic) {
        canvas_draw_utf8_str(canvas, x, y, str);
    } else {
        u8g2_DrawUTF8(&canvas->fb, x + canvas->offset_x, y + canvas->offset_y, str);
    }
}

void canvas_draw_utf8_str(Canvas* canvas, int32_t x, int32_t y, const char* str) {
    FuriStringUTF8State state = FuriStringUTF8StateStarting;
    FuriStringUnicodeValue value = 0;

    for(; *str; str++) {
        furi_string_utf8_decode(*str, &state, &value);
        if(state == FuriStringUTF8StateError) furi_crash(NULL);

        if(state == FuriStringUTF8StateStarting) {
            //ну что вы ещё ожидали
            switch(value) {
                case 'A': value = 0x0410; break; // А
                case 'B': value = 0x0411; break; // Б
                case 'C': value = 0x0421; break; // С
                case 'D': value = 0x0414; break; // Д
                case 'E': value = 0x0415; break; // Е
                case 'F': value = 0x0424; break; // Ф
                case 'G': value = 0x0413; break; // Г
                case 'H': value = 0x0425; break; // Х
                case 'I': value = 0x0418; break; // И
                case 'J': value = 0x0419; break; // Й
                case 'K': value = 0x041A; break; // К
                case 'L': value = 0x041B; break; // Л
                case 'M': value = 0x041C; break; // М
                case 'N': value = 0x041D; break; // Н
                case 'O': value = 0x041E; break; // О
                case 'P': value = 0x041F; break; // П
                case 'Q': value = 0x042F; break; // Я
                case 'R': value = 0x0420; break; // Р
                case 'S': value = 0x0421; break; // С
                case 'T': value = 0x0422; break; // Т
                case 'U': value = 0x0423; break; // У
                case 'V': value = 0x0412; break; // В
                case 'W': value = 0x0428; break; // Ш
                case 'X': value = 0x0425; break; // Х
                case 'Y': value = 0x042B; break; // Ы
                case 'Z': value = 0x0417; break; // З
                case 'a': value = 0x0430; break; // а
                case 'b': value = 0x0431; break; // б
                case 'c': value = 0x0441; break; // с
                case 'd': value = 0x0434; break; // д
                case 'e': value = 0x0435; break; // е
                case 'f': value = 0x0444; break; // ф
                case 'g': value = 0x0433; break; // г
                case 'h': value = 0x0445; break; // х
                case 'i': value = 0x0438; break; // и
                case 'j': value = 0x0439; break; // й
                case 'k': value = 0x043A; break; // к
                case 'l': value = 0x043B; break; // л
                case 'm': value = 0x043C; break; // м
                case 'n': value = 0x043D; break; // н
                case 'o': value = 0x043E; break; // о
                case 'p': value = 0x043F; break; // п
                case 'q': value = 0x044F; break; // я
                case 'r': value = 0x0440; break; // р
                case 's': value = 0x0441; break; // с
                case 't': value = 0x0442; break; // т
                case 'u': value = 0x0443; break; // у
                case 'v': value = 0x0432; break; // в
                case 'w': value = 0x0448; break; // ш
                case 'x': value = 0x0445; break; // х
                case 'y': value = 0x044B; break; // ы
                case 'z': value = 0x0437; break; // з
            }
            canvas_draw_glyph(canvas, x, y, value);

            // Only one-byte glyphs supported by canvas_glyph_width
            x += value <= 0xFF ? canvas_glyph_width(canvas, value) :
                                 canvas_glyph_width(canvas, ' ');
        }
    }
}

uint16_t canvas_string_width(Canvas* canvas, const char* str) {
    furi_check(canvas);
    if(!str) return 0;
    return u8g2_GetUTF8Width(&canvas->fb, str);
}

size_t canvas_glyph_width(Canvas* canvas, uint16_t symbol) {
    furi_check(canvas);
    return u8g2_GetGlyphWidth(&canvas->fb, symbol);
}

void canvas_draw_bitmap(
    Canvas* canvas,
    int32_t x,
    int32_t y,
    size_t width,
    size_t height,
    const uint8_t* compressed_bitmap_data) {
    furi_check(canvas);

    x += canvas->offset_x;
    y += canvas->offset_y;
    uint8_t* bitmap_data = NULL;
    compress_icon_decode(canvas->compress_icon, compressed_bitmap_data, &bitmap_data);
    canvas_draw_u8g2_bitmap(&canvas->fb, x, y, width, height, bitmap_data, IconRotation0);
}

void canvas_draw_icon_animation(
    Canvas* canvas,
    int32_t x,
    int32_t y,
    IconAnimation* icon_animation) {
    furi_check(canvas);
    furi_check(icon_animation);

    x += canvas->offset_x;
    y += canvas->offset_y;
    uint8_t* icon_data = NULL;
    compress_icon_decode(
        canvas->compress_icon, icon_animation_get_data(icon_animation), &icon_data);
    canvas_draw_u8g2_bitmap(
        &canvas->fb,
        x,
        y,
        icon_animation_get_width(icon_animation),
        icon_animation_get_height(icon_animation),
        icon_data,
        IconRotation0);
}

static void canvas_draw_u8g2_bitmap_int(
    u8g2_t* u8g2,
    u8g2_uint_t x,
    u8g2_uint_t y,
    u8g2_uint_t w,
    u8g2_uint_t h,
    bool mirror,
    bool rotation,
    const uint8_t* bitmap) {
    u8g2_uint_t blen;
    blen = w;
    blen += 7;
    blen >>= 3;

    if(rotation && !mirror) {
        x += w + 1;
    } else if(mirror && !rotation) {
        y += h - 1;
    }

    while(h > 0) {
        const uint8_t* b = bitmap;
        uint16_t len = w;
        uint16_t x0 = x;
        uint16_t y0 = y;
        uint8_t mask;
        uint8_t color = u8g2->draw_color;
        uint8_t ncolor = (color == 0 ? 1 : 0);
        mask = 1;

        while(len > 0) {
            if(u8x8_pgm_read(b) & mask) {
                u8g2->draw_color = color;
                u8g2_DrawHVLine(u8g2, x0, y0, 1, 0);
            } else if(u8g2->bitmap_transparency == 0) {
                u8g2->draw_color = ncolor;
                u8g2_DrawHVLine(u8g2, x0, y0, 1, 0);
            }

            if(rotation) {
                y0++;
            } else {
                x0++;
            }

            mask <<= 1;
            if(mask == 0) {
                mask = 1;
                b++;
            }
            len--;
        }

        u8g2->draw_color = color;
        bitmap += blen;

        if(mirror) {
            if(rotation) {
                x++;
            } else {
                y--;
            }
        } else {
            if(rotation) {
                x--;
            } else {
                y++;
            }
        }
        h--;
    }
}

void canvas_draw_u8g2_bitmap(
    u8g2_t* u8g2,
    int32_t x,
    int32_t y,
    size_t width,
    size_t height,
    const uint8_t* bitmap,
    IconRotation rotation) {
#ifdef U8G2_WITH_INTERSECTION
    if(u8g2_IsIntersection(u8g2, x, y, x + width, y + height) == 0) return;
#endif /* U8G2_WITH_INTERSECTION */

    switch(rotation) {
    case IconRotation0:
        canvas_draw_u8g2_bitmap_int(u8g2, x, y, width, height, 0, 0, bitmap);
        break;
    case IconRotation90:
        canvas_draw_u8g2_bitmap_int(u8g2, x, y, width, height, 0, 1, bitmap);
        break;
    case IconRotation180:
        canvas_draw_u8g2_bitmap_int(u8g2, x, y, width, height, 1, 0, bitmap);
        break;
    case IconRotation270:
        canvas_draw_u8g2_bitmap_int(u8g2, x, y, width, height, 1, 1, bitmap);
        break;
    default:
        break;
    }
}

void canvas_draw_icon_ex(
    Canvas* canvas,
    int32_t x,
    int32_t y,
    const Icon* icon,
    IconRotation rotation) {
    furi_check(canvas);
    furi_check(icon);

    x += canvas->offset_x;
    y += canvas->offset_y;
    uint8_t* icon_data = NULL;
    compress_icon_decode(canvas->compress_icon, icon_get_frame_data(icon, 0), &icon_data);
    canvas_draw_u8g2_bitmap(
        &canvas->fb, x, y, icon_get_width(icon), icon_get_height(icon), icon_data, rotation);
}

void canvas_draw_icon(Canvas* canvas, int32_t x, int32_t y, const Icon* icon) {
    furi_check(canvas);
    furi_check(icon);

    x += canvas->offset_x;
    y += canvas->offset_y;
    uint8_t* icon_data = NULL;
    compress_icon_decode(canvas->compress_icon, icon_get_frame_data(icon, 0), &icon_data);
    canvas_draw_u8g2_bitmap(
        &canvas->fb, x, y, icon_get_width(icon), icon_get_height(icon), icon_data, IconRotation0);
}

void canvas_draw_dot(Canvas* canvas, int32_t x, int32_t y) {
    furi_check(canvas);
    x += canvas->offset_x;
    y += canvas->offset_y;
    u8g2_DrawPixel(&canvas->fb, x, y);
}

void canvas_draw_box(Canvas* canvas, int32_t x, int32_t y, size_t width, size_t height) {
    furi_check(canvas);
    x += canvas->offset_x;
    y += canvas->offset_y;
    u8g2_DrawBox(&canvas->fb, x, y, width, height);
}

void canvas_draw_rbox(
    Canvas* canvas,
    int32_t x,
    int32_t y,
    size_t width,
    size_t height,
    size_t radius) {
    furi_check(canvas);
    x += canvas->offset_x;
    y += canvas->offset_y;
    u8g2_DrawRBox(&canvas->fb, x, y, width, height, radius);
}

void canvas_draw_frame(Canvas* canvas, int32_t x, int32_t y, size_t width, size_t height) {
    furi_check(canvas);
    x += canvas->offset_x;
    y += canvas->offset_y;
    u8g2_DrawFrame(&canvas->fb, x, y, width, height);
}

void canvas_draw_rframe(
    Canvas* canvas,
    int32_t x,
    int32_t y,
    size_t width,
    size_t height,
    size_t radius) {
    furi_check(canvas);
    x += canvas->offset_x;
    y += canvas->offset_y;
    u8g2_DrawRFrame(&canvas->fb, x, y, width, height, radius);
}

void canvas_draw_line(Canvas* canvas, int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
    furi_check(canvas);
    x1 += canvas->offset_x;
    y1 += canvas->offset_y;
    x2 += canvas->offset_x;
    y2 += canvas->offset_y;
    u8g2_DrawLine(&canvas->fb, x1, y1, x2, y2);
}

void canvas_draw_circle(Canvas* canvas, int32_t x, int32_t y, size_t radius) {
    furi_check(canvas);
    x += canvas->offset_x;
    y += canvas->offset_y;
    u8g2_DrawCircle(&canvas->fb, x, y, radius, U8G2_DRAW_ALL);
}

void canvas_draw_disc(Canvas* canvas, int32_t x, int32_t y, size_t radius) {
    furi_check(canvas);
    x += canvas->offset_x;
    y += canvas->offset_y;
    u8g2_DrawDisc(&canvas->fb, x, y, radius, U8G2_DRAW_ALL);
}

void canvas_draw_triangle(
    Canvas* canvas,
    int32_t x,
    int32_t y,
    size_t base,
    size_t height,
    CanvasDirection dir) {
    furi_check(canvas);
    if(dir == CanvasDirectionBottomToTop) {
        canvas_draw_line(canvas, x - base / 2, y, x + base / 2, y);
        canvas_draw_line(canvas, x - base / 2, y, x, y - height + 1);
        canvas_draw_line(canvas, x, y - height + 1, x + base / 2, y);
    } else if(dir == CanvasDirectionTopToBottom) {
        canvas_draw_line(canvas, x - base / 2, y, x + base / 2, y);
        canvas_draw_line(canvas, x - base / 2, y, x, y + height - 1);
        canvas_draw_line(canvas, x, y + height - 1, x + base / 2, y);
    } else if(dir == CanvasDirectionRightToLeft) {
        canvas_draw_line(canvas, x, y - base / 2, x, y + base / 2);
        canvas_draw_line(canvas, x, y - base / 2, x - height + 1, y);
        canvas_draw_line(canvas, x - height + 1, y, x, y + base / 2);
    } else if(dir == CanvasDirectionLeftToRight) {
        canvas_draw_line(canvas, x, y - base / 2, x, y + base / 2);
        canvas_draw_line(canvas, x, y - base / 2, x + height - 1, y);
        canvas_draw_line(canvas, x + height - 1, y, x, y + base / 2);
    }
}

void canvas_draw_xbm(
    Canvas* canvas,
    int32_t x,
    int32_t y,
    size_t width,
    size_t height,
    const uint8_t* bitmap) {
    furi_check(canvas);
    canvas_draw_xbm_ex(canvas, x, y, width, height, IconRotation0, bitmap);
}

void canvas_draw_xbm_ex(
    Canvas* canvas,
    int32_t x,
    int32_t y,
    size_t width,
    size_t height,
    IconRotation rotation,
    const uint8_t* bitmap_data) {
    furi_check(canvas);
    x += canvas->offset_x;
    y += canvas->offset_y;
    canvas_draw_u8g2_bitmap(&canvas->fb, x, y, width, height, bitmap_data, rotation);
}

void canvas_draw_glyph(Canvas* canvas, int32_t x, int32_t y, uint16_t ch) {
    furi_check(canvas);
    x += canvas->offset_x;
    y += canvas->offset_y;
    u8g2_DrawGlyph(&canvas->fb, x, y, ch);
}

void canvas_set_bitmap_mode(Canvas* canvas, bool alpha) {
    u8g2_SetBitmapMode(&canvas->fb, alpha ? 1 : 0);
}

void canvas_set_orientation(Canvas* canvas, CanvasOrientation orientation) {
    furi_check(canvas);
    const u8g2_cb_t* rotate_cb = NULL;
    bool need_swap = false;
    if(canvas->orientation != orientation) {
        switch(orientation) {
        case CanvasOrientationHorizontal:
            need_swap = canvas->orientation == CanvasOrientationVertical ||
                        canvas->orientation == CanvasOrientationVerticalFlip;
            rotate_cb = U8G2_R0;
            break;
        case CanvasOrientationHorizontalFlip:
            need_swap = canvas->orientation == CanvasOrientationVertical ||
                        canvas->orientation == CanvasOrientationVerticalFlip;
            rotate_cb = U8G2_R2;
            break;
        case CanvasOrientationVertical:
            need_swap = canvas->orientation == CanvasOrientationHorizontal ||
                        canvas->orientation == CanvasOrientationHorizontalFlip;
            rotate_cb = U8G2_R3;
            break;
        case CanvasOrientationVerticalFlip:
            need_swap = canvas->orientation == CanvasOrientationHorizontal ||
                        canvas->orientation == CanvasOrientationHorizontalFlip;
            rotate_cb = U8G2_R1;
            break;
        default:
            furi_crash();
        }

        if(need_swap) FURI_SWAP(canvas->width, canvas->height);
        u8g2_SetDisplayRotation(&canvas->fb, rotate_cb);
        canvas->orientation = orientation;
    }
}

CanvasOrientation canvas_get_orientation(const Canvas* canvas) {
    return canvas->orientation;
}

void canvas_add_framebuffer_callback(Canvas* canvas, CanvasCommitCallback callback, void* context) {
    furi_check(canvas);

    const CanvasCallbackPair p = {callback, context};

    canvas_lock(canvas);
    furi_check(!CanvasCallbackPairArray_count(canvas->canvas_callback_pair, p));
    CanvasCallbackPairArray_push_back(canvas->canvas_callback_pair, p);
    canvas_unlock(canvas);
}

void canvas_remove_framebuffer_callback(
    Canvas* canvas,
    CanvasCommitCallback callback,
    void* context) {
    furi_check(canvas);

    const CanvasCallbackPair p = {callback, context};

    canvas_lock(canvas);
    furi_check(CanvasCallbackPairArray_count(canvas->canvas_callback_pair, p) == 1);
    CanvasCallbackPairArray_remove_val(canvas->canvas_callback_pair, p);
    canvas_unlock(canvas);
}
