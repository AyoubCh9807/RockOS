#pragma once

#include "../../boot/graphics.hpp"
#include "../../boot/multiboot2.hpp"
#include "../storage/layout.hpp"
#include "../utils/string_utils.hpp"

class Debugger {
public:
  enum class DebugType {
    FS,
    GRAPHICS,
    PROCESS,
    KERNEL,
    GLOBAL,
  };

private:
  static constexpr bool FS_DEBUG = false;
  static constexpr bool GRAPHICS_DEBUG = false;
  static constexpr bool PROCESS_DEBUG = false;
  static constexpr bool KERNEL_DEBUG = false;
  static constexpr bool GLOBAL_DEBUG = false;

  static constexpr u32 MARGIN = 8;
  static constexpr u32 LINE_HEIGHT = 10;

  static constexpr u32 MAX_MESSAGES = 64;
  static constexpr u32 MAX_MESSAGE_LENGTH = 256;

  struct Message {
    DebugType type;
    char text[MAX_MESSAGE_LENGTH];
  };

  static inline Message messages[MAX_MESSAGES];

  // Index of the oldest message in the ring buffer.
  static inline u32 first_message = 0;

  // Number of valid messages currently stored.
  static inline u32 message_count = 0;

  static bool is_type_active(DebugType type) {
    switch (type) {
    case DebugType::FS:
      return FS_DEBUG;

    case DebugType::GRAPHICS:
      return GRAPHICS_DEBUG;

    case DebugType::PROCESS:
      return PROCESS_DEBUG;

    case DebugType::KERNEL:
      return KERNEL_DEBUG;

    case DebugType::GLOBAL:
      return GLOBAL_DEBUG;
    }

    return false;
  }

  static u32 screen_width() {
    return Multiboot2::framebuffer.width;
  }

  static u32 screen_height() {
    return Multiboot2::framebuffer.height;
  }

  static u32 text_width(const char *str) {
    if (!str)
      return 0;

    u32 width = 0;

    while (*str) {
      if (*str != '\n' && *str != '\r')
        width++;

      str++;
    }

    return width * Graphics::CHARACTER_WIDTH;
  }

  static void store_message(DebugType type, const char *str) {
    if (!str)
      return;

    u32 index;

    if (message_count < MAX_MESSAGES) {
      index = (first_message + message_count) % MAX_MESSAGES;
      message_count++;
    } else {
      index = first_message;
      first_message = (first_message + 1) % MAX_MESSAGES;
    }

    messages[index].type = type;

    u32 i = 0;

    while (str[i] != '\0' && i < MAX_MESSAGE_LENGTH - 1) {
      messages[index].text[i] = str[i];
      i++;
    }

    messages[index].text[i] = '\0';
  }

  static void store_text(DebugType type, const char *str) {
    if (!str)
      return;

    const char *start = str;

    while (*start) {
      const char *end = start;

      while (*end && *end != '\n')
        end++;

      char line_buffer[MAX_MESSAGE_LENGTH];

      u32 length = static_cast<u32>(end - start);

      if (length >= MAX_MESSAGE_LENGTH)
        length = MAX_MESSAGE_LENGTH - 1;

      for (u32 i = 0; i < length; i++)
        line_buffer[i] = start[i];

      line_buffer[length] = '\0';

      if (length > 0)
        store_message(type, line_buffer);

      if (*end == '\n')
        start = end + 1;
      else
        break;
    }
  }

  static void draw_message(const Message &message, u32 line) {
    const u32 width = text_width(message.text);
    const u32 screen = screen_width();

    u32 x = MARGIN;

    if (width + MARGIN <= screen)
      x = screen - MARGIN - width;

    const u32 y = MARGIN + line * LINE_HEIGHT;

    Graphics::draw_string(
        message.text,
        x,
        y,
        0xFFFFFFFF
    );
  }

public:
  static void reset() {
    first_message = 0;
    message_count = 0;
  }

  static void log(const char *str) {
    log(DebugType::GLOBAL, str);
  }

  static void log(DebugType type, const char *str) {
    if (!is_type_active(type) || !str)
      return;

    store_text(type, str);
  }

  static void logf(const char *fmt, ...) {
    if (!GLOBAL_DEBUG || !fmt)
      return;

    char buf[256];

    va_list args;
    va_start(args, fmt);

    StringUtils::vsnprintf(
        buf,
        sizeof(buf),
        fmt,
        args
    );

    va_end(args);

    log(DebugType::GLOBAL, buf);
  }

  static void logf(DebugType type, const char *fmt, ...) {
    if (!is_type_active(type) || !fmt)
      return;

    char buf[256];

    va_list args;
    va_start(args, fmt);

    StringUtils::vsnprintf(
        buf,
        sizeof(buf),
        fmt,
        args
    );

    va_end(args);

    log(type, buf);
  }

  static void log_number(int n) {
    log_number(DebugType::GLOBAL, n);
  }

  static void log_number(DebugType type, int n) {
    if (!is_type_active(type))
      return;

    char buf[16];

    StringUtils::snprintf(
        buf,
        sizeof(buf),
        "%d",
        n
    );

    log(type, buf);
  }

  static void render() {
    if (!Multiboot2::framebuffer.valid)
      return;

    if (message_count == 0)
      return;

    const u32 height = screen_height();

    if (height <= MARGIN * 2)
      return;

    const u32 max_lines =
        (height - MARGIN * 2) / LINE_HEIGHT;

    u32 visible_count = message_count;

    if (visible_count > max_lines)
      visible_count = max_lines;

    const u32 start =
        (first_message + message_count - visible_count) %
        MAX_MESSAGES;

    for (u32 i = 0; i < visible_count; i++) {
      const u32 index =
          (start + i) % MAX_MESSAGES;

      draw_message(messages[index], i);
    }
  }
};
