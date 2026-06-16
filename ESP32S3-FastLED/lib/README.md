
此目录用于存放项目专用的私有库。
PlatformIO 会将它们编译为静态库，并链接到可执行文件中。

每个库的源代码应放在单独的目录中
（"lib/你的库名/[代码文件]"）。

例如，以下示例库 `Foo` 和 `Bar` 的结构：

|--lib
|  |
|  |--Bar
|  |  |--docs
|  |  |--examples
|  |  |--src
|  |     |- Bar.c
|  |     |- Bar.h
|  |  |- library.json（可选，用于自定义编译选项等）https://docs.platformio.org/page/librarymanager/config.html
|  |
|  |--Foo
|  |  |- Foo.c
|  |  |- Foo.h
|  |
|  |--README

|  |  |- Foo.c
|  |  |- Foo.h
|  |
|  |- README --> THIS FILE
|
|- platformio.ini
|--src
   |- main.c

Example contents of `src/main.c` using Foo and Bar:
```
#include <Foo.h>
#include <Bar.h>

int main (void)
{
  ...
}

```

The PlatformIO Library Dependency Finder will find automatically dependent
libraries by scanning project source files.

More information about PlatformIO Library Dependency Finder
- https://docs.platformio.org/page/librarymanager/ldf.html
