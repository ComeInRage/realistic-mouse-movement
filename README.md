# realistic-mouse-movement

**realistic-mouse-movement** is a lightweight library (maybe header-only in the future) which allows you to emulate the user's work with the computer mouse. At the moment the library works ***only on Windows***, but adding Linux-version is in the plans for the future.

> The library was inspired by [Wind Mouse](https://ben.land/post/2021/04/25/windmouse-human-mouse-movement/) and has a similar, but not the same algorithm.
---
> I'm just a student, so there might be (for sure) bugs in the project. If you find such or you have suggestions how to improve the code, then welcome to the issues and pull requests.
---
> README and Documentation will be improved over time.
---
## Why?

Sometimes it is required to emulate the user's work at the computer, including his mouse movements. However, if we simply moves the mouse from point A to point B, we will have a perfect straight line, which doesn't look like a user's movements, because real user's trajectory is a curved line.

This library allows you to emulate user's trajectory. For example, try to find out which trajectories are drawn by computer:

![image](https://user-images.githubusercontent.com/74464036/209230309-77df8079-fe52-4138-a2c3-3b4700dd03c6.png)

The answer: second (2) and last (5) lines. And yes, line numbers were drawn in Paint. Don't ask me why.

> You could say, we already know all this, it’s better to tell why we need another library. There is already [Wind Mouse](https://ben.land/post/2021/04/25/windmouse-human-mouse-movement/) or [AntoinePassemiers's mouse](https://github.com/AntoinePassemiers/Realistic-Mouse). [SLAUC91's c++ mouse](https://github.com/SLAUC91/RealisticMouse) eventually. First of all, most of same libraries are written in another programming languages. ~~Secondly, check ***Features*** chapter~~ I wanted - I did.

## Features

### Human-like movements

**RealisticMove** member function will move cursor to given coords with human-like behavior. For example:

![image](https://user-images.githubusercontent.com/74464036/209235824-cb842775-8457-4886-85c0-6ed39006644a.png)

### Fluent interface

**Mouse** object provides all necessary tools not only for emulate the user's work with the mouse, but also for mouse controlling in general. Library provides convenient fluent interface, instead of WinAPI-ways to control mouse:

```cpp
namespace rm = real_mouse;
// ....
auto &mouse = rm::Mouse::Init();
mouse.SetPosition(x, y)
     .PushDown()
     .Move(x2, y2)
     .PushUp()
     .WaitForMove();
```

### Multithreading

Library is planned as multithreaded. At the moment, there are only several such methods (check documentation below).

Most of multithreaded methods are non-blocking asynchronous functions. For example, if you calls **RealisticMove** in the code below, mouse will move at the same time with output of cursor coordinates:

```cpp
auto &mouse = rm::Mouse::Init();
mouse.RealisticMove(x, y, movingVelocity);
while(mouse.IsMoving())
{
    auto [currX, currY] = mouse.GetPosition();
    std::cout << currX << " " << currY << std::endl;
}
```

Every asynchronous function call doesn't create new std::thread. Also functions are executed in the order they are called. It is achieved using Thread Pool pattern:

```cpp
auto &mouse = rm::Mouse::Init();
mouse.RealisticMove(x, y)
     .Move(x2, y2)
     .RealisticMove(x3, y3)
     .WaitForMove();
```

In the code above functions executed successively, but asynchronously (in distinct thread).

There is different thread pools to different operations. For example, **Click** methods will be executed in parallel with **Move** operations.

### Documentation

Static member functions:

1. **Init** - returns non-const reference to **Mouse** singleton.
1. **GetPosition** - returns std::pair with current coordinates of mouse cursor.

Asynchronous methods:

1. **Move** - non-blocking asynchronous. Mouse moves in distinct thread, control is returned to caller.
1. **RealisticMove** - non-blocking asynchronous. Same as **Move**.
1. **Click** - non-blocking asynchronous. Clicking time calculates in distinct thread, control is returned to caller.

Since the methods above are non-blocking (*it means that internal logic executes in distinct thread and control returns to caller, once thread is created*), there are some synchronizing methods:

1. **WaitForMove** - blocks calling thread, until all **Move** operations become finished. Yes, "all operations" means including even **RealisticMove** or similar.
1. **WaitForClick** - blocks calling thread, until all **Click** operations become finished.

Other member functions:

1. **SetPosition** - immediately sets mouse cursor position to given.
1. **PushDown** - pushes down given mouse button.
1. **PushUp** - pushes up given mouse button.

Observing methods:

1. **IsClicking** - return true if any thread processing click operation (here is underlying operations counter). False otherwise.
1. **IsMoving** -  return true if any thread processing moving operation (include **RealisticMove** and similar). False otherwise.
