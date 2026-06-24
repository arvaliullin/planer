#pragma once

#include <QList>

template <typename T> struct Paginated {
  QList<T> items;
  int total = 0;
};
