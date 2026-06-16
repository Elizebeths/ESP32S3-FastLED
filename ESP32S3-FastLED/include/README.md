
此目录用于存放项目头文件。

头文件是包含 C 语言声明和宏定义的文件，供项目中的多个源文件共享使用。
你可以在 `src` 文件夹中的源文件（C、C++ 等）里，通过 C 预处理指令 `#include` 来引用头文件。

```src/main.c

#include "header.h"

int main (void)
{
 ...
}
```

引用头文件的效果等同于将该头文件的内容复制到每个需要它的源文件中。
但手动复制会非常耗时且容易出错，使用 #include 则可以避免这些问题。

and error-prone. With a header file, the related declarations appear
in only one place. If they need to be changed, they can be changed in one
place, and programs that include the header file will automatically use the
new version when next recompiled. The header file eliminates the labor of
finding and changing all the copies as well as the risk that a failure to
find one copy will result in inconsistencies within a program.

In C, the convention is to give header files names that end with `.h'.

Read more about using header files in official GCC documentation:

* Include Syntax
* Include Operation
* Once-Only Headers
* Computed Includes

https://gcc.gnu.org/onlinedocs/cpp/Header-Files.html
