# Chow-Chow

Chow-Chow<sup>[1]</sup> is an FM synth<sup>[2]</sup> library with
an emphasis on flexibility and ease-of-use. It supports an
arbitrary number of operators as well as arbitrarily complex FM
algorithms. Current focus is on the library, but once the library
is mature, standalone and plugin graphical frontends are on the
roadmap as well.

1. Named in honor of John Chowning, FM synthesis pioneer, as well
   as a cute breed of dog.
1. Technically it's a phase modulation synth, like most FM synths
   in practice.

## Contributing

Thanks for your interest!

### Getting started

If you want to add a new feature or make any other significant
changes to the program's behavior, please make a new issue first
and describe what you have in mind there, making it clear that
you're willing to write the implementation. This includes any
modifications to the public API. That way we can talk it over
first before you spend a lot of time writing code.

If you want to submit a bugfix or optimization, you can just go
ahead and create a pull request if you like. If you're unsure if
what you're seeing is a bug, or your planned fix has some
downsides, feel free to make a new issue for it and then we can
discuss. Optimization pull requests should be accompanied with
test results, a detailed description of the environment(s) the
tests were run in (including hardware), and ideally a precise
description of why and in what sense your change improves
performance.

There is a simple end-to-end speed test available under
`build/tests/perf` (with the default build settings) that you may
find convenient. It outputs timing results for each sine
algorithm using a test routine and the stdlib timing facilities.

### Style

Try to stick to the coding style of [_The C++ Programming
Language_](https://www.stroustrup.com/4th.html) and [_A Tour of
C++_](https://www.stroustrup.com/tour2.html) and the like. I'm
not ultra-strict about syntax; I appreciate that C++ is very
flexible in that regard, so if you have an unusual construction
that reads best written in an unusual way, that might be okay
with me. That said, it's nice to keep things looking consistent
if all else is equal, so please only deviate if you really have a
good reason.

Talking in semantic terms, I definitely think the
recommendations in those books are worth taking, and am likely to
push back against code that strays from them much. In particular,
please use the standard library facilities whenever you don't
have a good reason not to, and lean on the type system as much as
you reasonably can. C++17 is the current target.

### Conduct

Hopefully this will never come up, but I will block anyone who
purposely mistreats people based on intrinsic demographic
categories (gender expression, sexuality, ethnicity, race, nation
of origin, age, etc.). If you do something like this without
meaning to and are willing to change your behavior once you hear
about it, that's okay provided you follow through. If someone is
treating you this way on this project and it's escaped my notice,
ping me right away.

Less drastically, I deeply appreciate politeness and courtesy.
Approaching this project with a genuine desire to get along with
people and help others will endear me to you (and probably to
others as well). If you feel like I'm being too short with you,
it's okay with me if you say so.
