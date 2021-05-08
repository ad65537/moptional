#ifndef MOPTIONAL_
#define MOPTIONAL_

#include <optional>
#include <type_traits>

namespace mopt {

namespace details {

// To allow ADL with custom begin/end
using std::begin;
using std::end;

/* SFINAE using comma operator. The int overload is preferred and hence
   checked first
   Based on
   https://stackoverflow.com/questions/13830158/check-if-a-variable-type-is-iterable
*/
template <typename T>
auto IsIterable(int)
    -> decltype(begin(std::declval<T&>()) != end(std::declval<T&>()), void(),
                ++std::declval<decltype(begin(std::declval<T&>()))&>(),
                void(*begin(std::declval<T&>())), std::true_type{});

template <typename T>
std::false_type IsIterable(...);

template <typename T>
constexpr static bool IsContainerLike() {
  return decltype(details::IsIterable<T>(0))::value;
}

// A type tag to use where IsContainerLike<T> is true
template <typename T,
          std::enable_if_t<details::IsContainerLike<T>(), bool> = true>
struct IteratorType {
  using Type = std::decay_t<decltype(std::declval<T>().begin())>;
};

}  // namespace details

template <typename T, typename PointerOrIterator>
class MoptIterator {
  /* Based on
   https://stackoverflow.com/questions/22426974/implementing-custom-MoptIteratorators-in-c11
   This iterator is used when T is not optional


   TODO allow for const iterators
   TODO constexpr memebers where appropriate

  */

 protected:
  // PointerOrIterator should be a pointer when T is not container-like and an
  // iterator type otherwise.
  PointerOrIterator underlying_;

 public:
  // Will grab the iterator_traits of either a T*
  using Traits = std::iterator_traits<PointerOrIterator>;
  using dereference_type =
      typename std::decay_t<decltype(*std::declval<PointerOrIterator>())>;

  // Used by STL algorithms
  using iterator_type = MoptIterator<T, PointerOrIterator>;
  using iterator_category = typename Traits::iterator_category;
  using value_type = typename Traits::value_type;
  using difference_type = typename Traits::difference_type;
  using reference = typename Traits::reference;
  using pointer = typename Traits::pointer;

  // These should be the case whether using T* or an iterator
  static_assert(std::is_same_v<reference, dereference_type&> || std::is_same_v<reference, const dereference_type&>);
  static_assert(std::is_same_v<pointer, dereference_type*> || std::is_same_v<pointer, const dereference_type*>);

  /* TODO This will only work for container-like T when T's iterators are
   default constructible right now? We could have an optional around this,
  */
  MoptIterator() : underlying_() {}

  explicit MoptIterator(PointerOrIterator _underlying)
      : underlying_(_underlying) {}

  // Forward MoptIteratorator requirements
  reference operator*() const { return *underlying_; }

  pointer operator->() const {
    if constexpr (details::IsContainerLike<T>()) {
      return underlying_->operator->();
    } else {
      return underlying_;
    }
  }

  MoptIterator& operator++() {
    ++underlying_;
    return *this;
  }

  MoptIterator operator++(int) { return MoptIterator(underlying_++); }

  MoptIterator& operator--() {
    --underlying_;
    return *this;
  }

  MoptIterator operator--(int) { return MoptIterator(underlying_--); }

  // TODO Implement Random Access iterator methods where appropriate on T

  bool operator==(const MoptIterator& _other) const {
    return underlying_ == _other.underlying_;
  }

  bool operator!=(const MoptIterator& _other) const {
    return !(*this == _other);
  }
};

template <class T>
class Moptional {
 public:
  /*
   * begin() and end() will create compile-time branches depending if T is
   * container-like, and do run-time checks to determine if there is anything to
   * iterate over
   * */

  decltype(auto) begin() {
    if constexpr (details::IsContainerLike<T>()) {
      if (internal_) {
        return MoptIterator<T, typename details::IteratorType<T>::Type>(
            internal_->begin());
      } else {
        return MoptIterator<T, typename details::IteratorType<T>::Type>();
      }
    } else {
      if (internal_) {
        return MoptIterator<T, T*>(&*internal_);
      } else {
        return MoptIterator<T, T*>();
      }
    }
  }

  decltype(auto) end() {
    if constexpr (details::IsContainerLike<T>()) {
      if (internal_) {
        return MoptIterator<T, typename details::IteratorType<T>::Type>(
            internal_->end());
      } else {
        return MoptIterator<T, typename details::IteratorType<T>::Type>();
      }
    } else {
      // Doesn't matter if we are nullopt or not!
      if (internal_) {
        // Give a pointer past the memory
        // owned by the internal optional
        return MoptIterator<T, T*>(
            (&*internal_) + 1);
      } else {
        return MoptIterator<T, T*>();
      }
    }
  }

  /*
   * The followng part is boring - simply wrap the std::optional interface. No
   * inheritance because STL optional has no virtual destructor (like usual for
   * STL)
   */

  template <typename... Args>
  Moptional(Args&&... _args) : internal_(std::forward<Args>(_args)...) {}

  constexpr const T* operator->() const { return internal_.operator->(); }

  constexpr T* operator->() { return internal_.operator->(); }

  constexpr const T& operator*() const& { return *internal_; }

  constexpr T& operator*() & { return *internal_; }

  constexpr const T&& operator*() const&& { return *internal_; }

  constexpr T&& operator*() && { return *internal_; }

  constexpr explicit operator bool() const noexcept { return (bool)internal_; }

  constexpr bool has_value() const noexcept { return internal_.has_value(); }

  constexpr T& value() & { return internal_.value(); }

  constexpr const T& value() const& { return internal_.value(); }

  constexpr T&& value() && { return internal_.value(); }

  constexpr const T&& value() const&& { return internal_.value(); }

  void reset() noexcept { return internal_.reset(); }

  template <class... Args>
  T& emplace(Args&&... _args) {
    return internal_.emplace(std::forward<Args>(_args)...);
  }

  template <class U, class... Args>
  T& emplace(std::initializer_list<U> _ilist, Args&&... _args) {
    // Moving the init list
    return internal_.emplace(std::move(_ilist), std::forward<Args>(_args)...);
  }

  void swap(Moptional& _other) noexcept(
      std::is_nothrow_move_constructible_v<T>&&
          std::is_nothrow_swappable_v<T>) {
    internal_.swap(_other.internal_);
  }

  // TODO - comparison operators. If decide to just support C++20, can just use
  // <=>

  // TODO make_moptional, (std::swap?)

 private:
  std::optional<T> internal_;
};

}  // namespace mopt

#endif
