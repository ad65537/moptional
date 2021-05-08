# moptional

![cmake tests](https://github.com/ad65537/moptional/actions/workflows/cmake.yml/badge.svg)

This header-only C++ library provides an alternative to `std::optional` which provides a better monadic interface for functional-style iteration and error handling. The `mopt::moptional` (monadic optional) is a wrapper around `std::optional` which adds STL-compatible iterators, so that STL algorithms such as `std::accumulate` or `std::transform` can be executed over the data structure. If the moptional contains a `std::nullopt`, the iterators will simulate an empty container. Furthermore, if the type paramter `T` of the optional is container-like, iterators of the moptional will wrap `T::iterator_type`.

# Example Code

Monadic error handling with `std::optional`:
```
std::optional<std::vector<int>> PossiblyFailingFunction() {
  ...
}

std::optional<std::vector<int>> TryTransformData() {

  auto opt_vec = PossiblyFailingFunction();

  if(opt_vec) {
    std::transform(
      opt_vec->begin(),
      opt_vec->end(),
      opt_vec->begin(), //re-write to opt_vec
      [](int _val) {
         return _val*2;
      }
    );
  } else {
    return opt_vec;
  }
}
```

Versus monadic error handling with `mopt::moptional`:

```
mopt::moptional<std::vector<int>> PossibleFailingFunction() {
  ...
}

mopt::moptional<std::vector<int>> TryTransformData() {

  auto opt_vec = PossiblyFailingFunction();

  //If opt_vec has a nullopt, begin() == end() so this is safe
  std::transform(
    opt_vec.begin(), //no indirection, the iterator will handle null cases if needed and trill down
    opt_vec.end(),
    opt_vec.begin(),
    [](int _val) {
      return _val*2;
    }
  );
  
  return opt_vec;
}
```

Similar behaviour is provided for `T` parameters which are not container-like or iterable i.e. the optional will simulate a container of size `0` or `1` depending if the underlying optional containts `std::nullopt`:

```
mopt::moptional<int> PossibleFailingFunction() {
  ...
}

mopt::moptional<int> TryTransformData() {

  auto opt_int = PossiblyFailingFunction();

  std::transform(
    opt_vec.begin(), //no indirection, the iterator will handle if needed
    opt_vec.end(),
    opt_vec.begin(),
    [](int _val) {
      return _val*2;
    }
  );
  
  return opt_int;
}
```


# Reasoning

Optional error handling is common among functional programming langauges and frameworks. For instance, a similar scenario to the above in Scala:

```
val optNoneList : Option[List[Int]] = None
val alsoNone = optNoneList.map(x => x*2)

val optSomeList : Option[List[Int]] = Some(1)
val doubledInt = optSomeInt.map(x => x*2) //Some(2)
```

Unfortuntely, the `std::optional` design does not allow for such error handling i.e. being able to chain subsequent computations together without checking the success of the previous computation.

There has been a previous [proposal](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0798r4.html) and [implementation](https://github.com/TartanLlama/optional) for such an improvement to `std::optional` through methods imitating global STL functions but implementing the monadic behaviour. The difference in this library is that safe error handling is provided entirely by iterators, which allows this to be used by any existing STL or other algorithms accepting standard iterator interfaces.
