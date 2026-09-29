#pragma once

enum class EasingType {
  Linear,

  EaseIn,
  EaseOut,
  EaseInOut,

  EaseInQuad,
  EaseOutQuad,
  EaseInOutQuad,

  EaseInCubic,
  EaseOutCubic,
  EaseInOutCubic,

  EaseInQuart,
  EaseOutQuart,
  EaseInOutQuart,

  EaseInQuint,
  EaseOutQuint,
  EaseInOutQuint
};

enum class AnimationType {
  None,

  FadeIn,
  FadeOut,

  SlideLeft,
  SlideRight,
  SlideUp,
  SlideDown,

  ScaleIn,

  FadeSlideLeft,
  FadeSlideRight,
  FadeSlideUp,
  FadeSlideDown,

  FadeScale
};

namespace Easing {

inline float linear(float t) {
  return t;
}

inline float ease_in_quad(float t) {
  return t * t;
}

inline float ease_out_quad(float t) {
  return 1.0f - (1.0f - t) * (1.0f - t);
}

inline float ease_in_out_quad(float t) {
  if (t < 0.5f)
    return 2.0f * t * t;

  const float x = -2.0f * t + 2.0f;
  return 1.0f - (x * x) / 2.0f;
}

inline float ease_in_cubic(float t) {
  return t * t * t;
}

inline float ease_out_cubic(float t) {
  const float x = t - 1.0f;
  return x * x * x + 1.0f;
}

inline float ease_in_out_cubic(float t) {
  if (t < 0.5f)
    return 4.0f * t * t * t;

  const float x = -2.0f * t + 2.0f;
  return 1.0f - (x * x * x) / 2.0f;
}

inline float ease_in_quart(float t) {
  return t * t * t * t;
}

inline float ease_out_quart(float t) {
  const float x = t - 1.0f;
  return 1.0f - x * x * x * x;
}

inline float ease_in_out_quart(float t) {
  if (t < 0.5f)
    return 8.0f * t * t * t * t;

  const float x = -2.0f * t + 2.0f;
  return 1.0f - (x * x * x * x) / 2.0f;
}

inline float ease_in_quint(float t) {
  return t * t * t * t * t;
}

inline float ease_out_quint(float t) {
  const float x = t - 1.0f;
  return x * x * x * x * x + 1.0f;
}

inline float ease_in_out_quint(float t) {
  if (t < 0.5f)
    return 16.0f * t * t * t * t * t;

  const float x = -2.0f * t + 2.0f;
  return 1.0f - (x * x * x * x * x) / 2.0f;
}

inline float apply(float t, EasingType type) {
  if (t < 0.0f)
    t = 0.0f;

  if (t > 1.0f)
    t = 1.0f;

  switch (type) {
  case EasingType::Linear:
    return linear(t);

  case EasingType::EaseIn:
  case EasingType::EaseInQuad:
    return ease_in_quad(t);

  case EasingType::EaseOut:
  case EasingType::EaseOutQuad:
    return ease_out_quad(t);

  case EasingType::EaseInOut:
  case EasingType::EaseInOutQuad:
    return ease_in_out_quad(t);

  case EasingType::EaseInCubic:
    return ease_in_cubic(t);

  case EasingType::EaseOutCubic:
    return ease_out_cubic(t);

  case EasingType::EaseInOutCubic:
    return ease_in_out_cubic(t);

  case EasingType::EaseInQuart:
    return ease_in_quart(t);

  case EasingType::EaseOutQuart:
    return ease_out_quart(t);

  case EasingType::EaseInOutQuart:
    return ease_in_out_quart(t);

  case EasingType::EaseInQuint:
    return ease_in_quint(t);

  case EasingType::EaseOutQuint:
    return ease_out_quint(t);

  case EasingType::EaseInOutQuint:
    return ease_in_out_quint(t);
  }

  return t;
}

} // namespace Easing
