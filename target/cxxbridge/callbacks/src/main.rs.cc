#include "callbacks/src/cb.h"
#include "callbacks/src/cb2.h"
#include <cstddef>
#include <memory>
#include <new>
#include <string>
#include <type_traits>
#include <utility>

namespace rust {
inline namespace cxxbridge1 {
// #include "rust/cxx.h"

#ifndef CXXBRIDGE1_RUST_BOX
#define CXXBRIDGE1_RUST_BOX
template <typename T>
class Box final {
public:
  using element_type = T;
  using const_pointer =
      typename std::add_pointer<typename std::add_const<T>::type>::type;
  using pointer = typename std::add_pointer<T>::type;

  Box() = delete;
  Box(Box &&) noexcept;
  ~Box() noexcept;

  explicit Box(const T &);
  explicit Box(T &&);

  Box &operator=(Box &&) &noexcept;

  const T *operator->() const noexcept;
  const T &operator*() const noexcept;
  T *operator->() noexcept;
  T &operator*() noexcept;

  template <typename... Fields>
  static Box in_place(Fields &&...);

  void swap(Box &) noexcept;

  static Box from_raw(T *) noexcept;

  T *into_raw() noexcept;

  /* Deprecated */ using value_type = element_type;

private:
  class uninit;
  class allocation;
  Box(uninit) noexcept;
  void drop() noexcept;

  friend void swap(Box &lhs, Box &rhs) noexcept { lhs.swap(rhs); }

  T *ptr;
};

template <typename T>
class Box<T>::uninit {};

template <typename T>
class Box<T>::allocation {
  static T *alloc() noexcept;
  static void dealloc(T *) noexcept;

public:
  allocation() noexcept : ptr(alloc()) {}
  ~allocation() noexcept {
    if (this->ptr) {
      dealloc(this->ptr);
    }
  }
  T *ptr;
};

template <typename T>
Box<T>::Box(Box &&other) noexcept : ptr(other.ptr) {
  other.ptr = nullptr;
}

template <typename T>
Box<T>::Box(const T &val) {
  allocation alloc;
  ::new (alloc.ptr) T(val);
  this->ptr = alloc.ptr;
  alloc.ptr = nullptr;
}

template <typename T>
Box<T>::Box(T &&val) {
  allocation alloc;
  ::new (alloc.ptr) T(std::move(val));
  this->ptr = alloc.ptr;
  alloc.ptr = nullptr;
}

template <typename T>
Box<T>::~Box() noexcept {
  if (this->ptr) {
    this->drop();
  }
}

template <typename T>
Box<T> &Box<T>::operator=(Box &&other) &noexcept {
  if (this->ptr) {
    this->drop();
  }
  this->ptr = other.ptr;
  other.ptr = nullptr;
  return *this;
}

template <typename T>
const T *Box<T>::operator->() const noexcept {
  return this->ptr;
}

template <typename T>
const T &Box<T>::operator*() const noexcept {
  return *this->ptr;
}

template <typename T>
T *Box<T>::operator->() noexcept {
  return this->ptr;
}

template <typename T>
T &Box<T>::operator*() noexcept {
  return *this->ptr;
}

template <typename T>
template <typename... Fields>
Box<T> Box<T>::in_place(Fields &&...fields) {
  allocation alloc;
  auto ptr = alloc.ptr;
  ::new (ptr) T{std::forward<Fields>(fields)...};
  alloc.ptr = nullptr;
  return from_raw(ptr);
}

template <typename T>
void Box<T>::swap(Box &rhs) noexcept {
  using std::swap;
  swap(this->ptr, rhs.ptr);
}

template <typename T>
Box<T> Box<T>::from_raw(T *raw) noexcept {
  Box box = uninit{};
  box.ptr = raw;
  return box;
}

template <typename T>
T *Box<T>::into_raw() noexcept {
  T *raw = this->ptr;
  this->ptr = nullptr;
  return raw;
}

template <typename T>
Box<T>::Box(uninit) noexcept {}
#endif // CXXBRIDGE1_RUST_BOX

#ifndef CXXBRIDGE1_RUST_OPAQUE
#define CXXBRIDGE1_RUST_OPAQUE
class Opaque {
public:
  Opaque() = delete;
  Opaque(const Opaque &) = delete;
  ~Opaque() = delete;
};
#endif // CXXBRIDGE1_RUST_OPAQUE

#ifndef CXXBRIDGE1_IS_COMPLETE
#define CXXBRIDGE1_IS_COMPLETE
namespace detail {
namespace {
template <typename T, typename = std::size_t>
struct is_complete : std::false_type {};
template <typename T>
struct is_complete<T, decltype(sizeof(T))> : std::true_type {};
} // namespace
} // namespace detail
#endif // CXXBRIDGE1_IS_COMPLETE

#ifndef CXXBRIDGE1_LAYOUT
#define CXXBRIDGE1_LAYOUT
class layout {
  template <typename T>
  friend std::size_t size_of();
  template <typename T>
  friend std::size_t align_of();
  template <typename T>
  static typename std::enable_if<std::is_base_of<Opaque, T>::value,
                                 std::size_t>::type
  do_size_of() {
    return T::layout::size();
  }
  template <typename T>
  static typename std::enable_if<!std::is_base_of<Opaque, T>::value,
                                 std::size_t>::type
  do_size_of() {
    return sizeof(T);
  }
  template <typename T>
  static
      typename std::enable_if<detail::is_complete<T>::value, std::size_t>::type
      size_of() {
    return do_size_of<T>();
  }
  template <typename T>
  static typename std::enable_if<std::is_base_of<Opaque, T>::value,
                                 std::size_t>::type
  do_align_of() {
    return T::layout::align();
  }
  template <typename T>
  static typename std::enable_if<!std::is_base_of<Opaque, T>::value,
                                 std::size_t>::type
  do_align_of() {
    return alignof(T);
  }
  template <typename T>
  static
      typename std::enable_if<detail::is_complete<T>::value, std::size_t>::type
      align_of() {
    return do_align_of<T>();
  }
};

template <typename T>
std::size_t size_of() {
  return layout::size_of<T>();
}

template <typename T>
std::size_t align_of() {
  return layout::align_of<T>();
}
#endif // CXXBRIDGE1_LAYOUT

namespace {
template <bool> struct deleter_if {
  template <typename T> void operator()(T *) {}
};

template <> struct deleter_if<true> {
  template <typename T> void operator()(T *ptr) { ptr->~T(); }
};
} // namespace
} // namespace cxxbridge1
} // namespace rust

struct my_cb;
using simple_cb = ::simple_cb;

#ifndef CXXBRIDGE1_STRUCT_my_cb
#define CXXBRIDGE1_STRUCT_my_cb
struct my_cb final : public ::rust::Opaque {
  void call_my_cb_ok(const ::std::string &a, const ::std::string &b) noexcept;
  void call_my_cb_err(const ::std::string &a) noexcept;
  void call_my_cb_drop() noexcept;
  ~my_cb() = delete;

private:
  friend ::rust::layout;
  struct layout {
    static ::std::size_t size() noexcept;
    static ::std::size_t align() noexcept;
  };
};
#endif // CXXBRIDGE1_STRUCT_my_cb

extern "C" {
::std::size_t cxxbridge1$my_cb$operator$sizeof() noexcept;
::std::size_t cxxbridge1$my_cb$operator$alignof() noexcept;

::my_cb *cxxbridge1$create_my_cb(::std::size_t ok_, ::std::size_t err_, ::std::size_t drop_, ::std::size_t data_) noexcept;

void cxxbridge1$my_cb$call_my_cb_ok(::my_cb &self, const ::std::string &a, const ::std::string &b) noexcept;

void cxxbridge1$my_cb$call_my_cb_err(::my_cb &self, const ::std::string &a) noexcept;

void cxxbridge1$my_cb$call_my_cb_drop(::my_cb &self) noexcept;

::simple_cb *cxxbridge1$create_simple(::std::size_t ok_, ::std::size_t err_, ::std::size_t drop_, ::std::size_t data_) noexcept {
  ::std::unique_ptr<::simple_cb> (*create_simple$)(::std::size_t, ::std::size_t, ::std::size_t, ::std::size_t) = ::create_simple;
  return create_simple$(ok_, err_, drop_, data_).release();
}

void cxxbridge1$call_ok(::simple_cb &cb, const ::std::string &a, const ::std::string &b) noexcept {
  void (*call_ok$)(::simple_cb &, const ::std::string &, const ::std::string &) = ::call_ok;
  call_ok$(cb, a, b);
}

void cxxbridge1$call_err(::simple_cb &cb, const ::std::string &a) noexcept {
  void (*call_err$)(::simple_cb &, const ::std::string &) = ::call_err;
  call_err$(cb, a);
}

void cxxbridge1$call_drop(::simple_cb &cb) noexcept {
  void (*call_drop$)(::simple_cb &) = ::call_drop;
  call_drop$(cb);
}

void cxxbridge1$tst() noexcept {
  void (*tst$)() = ::tst;
  tst$();
}
} // extern "C"

::std::size_t my_cb::layout::size() noexcept {
  return cxxbridge1$my_cb$operator$sizeof();
}

::std::size_t my_cb::layout::align() noexcept {
  return cxxbridge1$my_cb$operator$alignof();
}

::rust::Box<::my_cb> create_my_cb(::std::size_t ok_, ::std::size_t err_, ::std::size_t drop_, ::std::size_t data_) noexcept {
  return ::rust::Box<::my_cb>::from_raw(cxxbridge1$create_my_cb(ok_, err_, drop_, data_));
}

void my_cb::call_my_cb_ok(const ::std::string &a, const ::std::string &b) noexcept {
  cxxbridge1$my_cb$call_my_cb_ok(*this, a, b);
}

void my_cb::call_my_cb_err(const ::std::string &a) noexcept {
  cxxbridge1$my_cb$call_my_cb_err(*this, a);
}

void my_cb::call_my_cb_drop() noexcept {
  cxxbridge1$my_cb$call_my_cb_drop(*this);
}

extern "C" {
::my_cb *cxxbridge1$box$my_cb$alloc() noexcept;
void cxxbridge1$box$my_cb$dealloc(::my_cb *) noexcept;
void cxxbridge1$box$my_cb$drop(::rust::Box<::my_cb> *ptr) noexcept;

static_assert(::rust::detail::is_complete<::simple_cb>::value, "definition of simple_cb is required");
static_assert(sizeof(::std::unique_ptr<::simple_cb>) == sizeof(void *), "");
static_assert(alignof(::std::unique_ptr<::simple_cb>) == alignof(void *), "");
void cxxbridge1$unique_ptr$simple_cb$null(::std::unique_ptr<::simple_cb> *ptr) noexcept {
  ::new (ptr) ::std::unique_ptr<::simple_cb>();
}
void cxxbridge1$unique_ptr$simple_cb$raw(::std::unique_ptr<::simple_cb> *ptr, ::simple_cb *raw) noexcept {
  ::new (ptr) ::std::unique_ptr<::simple_cb>(raw);
}
const ::simple_cb *cxxbridge1$unique_ptr$simple_cb$get(const ::std::unique_ptr<::simple_cb>& ptr) noexcept {
  return ptr.get();
}
::simple_cb *cxxbridge1$unique_ptr$simple_cb$release(::std::unique_ptr<::simple_cb>& ptr) noexcept {
  return ptr.release();
}
void cxxbridge1$unique_ptr$simple_cb$drop(::std::unique_ptr<::simple_cb> *ptr) noexcept {
  ::rust::deleter_if<::rust::detail::is_complete<::simple_cb>::value>{}(ptr);
}
} // extern "C"

namespace rust {
inline namespace cxxbridge1 {
template <>
::my_cb *Box<::my_cb>::allocation::alloc() noexcept {
  return cxxbridge1$box$my_cb$alloc();
}
template <>
void Box<::my_cb>::allocation::dealloc(::my_cb *ptr) noexcept {
  cxxbridge1$box$my_cb$dealloc(ptr);
}
template <>
void Box<::my_cb>::drop() noexcept {
  cxxbridge1$box$my_cb$drop(this);
}
} // namespace cxxbridge1
} // namespace rust
