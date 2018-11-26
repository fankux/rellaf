# Rellaf

## 介绍
**Rellaf**是一个C++反射库，得益于C++11的语法，让我们可以做到一些神奇的效果——程序在运行时可以遍历对象自身的所有成员字段名字和值，并且可以通过对象名字（字符串）获得该成员的值。 

**这么做意义何在？**  
现今服务端程序两大"刚需"：Json序列化和拼SQL。写过Java的同学可能不以为然，写一个和Json对象成员对应的Model类，Gson，Jackson双向"一键直达"；拼SQL？Mybatis的SQL模板中条件预留好字段名称，例如`WHERE field=#{成员名}`，调用时传递对象，同样"一键直达"，Mybatis能够自动根据名称拿到对象的成员值。

**以上两个场景都需要反射。**

传统C++怎么做？（摘自真实线上代码，已"打码"）
```c++
json["name"] = _field_name;
///////////////////
_count_big = threshold.get("count_big", 10000).asInt();
```
同样的字段名我们需要写两遍，变量写错了还好，编译不过；Json取值的索引字符串错了只能运行时等报错。

```c++
std::string sql = "INSERT INTO XXX_OOO(product, stream, profile, environment, "
        "plat_product, uniq_stream, plat_profile) VALUES('" + iden.product + "', '" +
        iden.stream + "', '" + iden.profile + "', '" + iden.environment + "', '" + pdb +
        "', '" + uniq_stream + "', '" + platform + "') ON DUPLICATE KEY UPDATE plat_product='" +
        pdb + "', uniq_stream='" + uniq_stream + "', plat_profile='" + platform + "'";
```
如果说上面处理Json还能接受，这个简直是折磨，不言而喻……你可以用sprintf这类，但是%xxoo和后面变量检查对应同样很痛苦，没有本质区别。


**Rellaf**致力于解放C++程序员这块的生产力，也做到了和Java类似的十分爽快的体验，大大提高生产力，减少低级错误。

## 我们来QUICK START！
首先，我们演示一个最简单的对象，设置一个`int`型字段名为`id`。  
1. 包含头文件`rellaf.h`，在头文件申明反射类：
```c++
// 1.申明对象Object 继承 rellaf::Model
// 2. rellaf_model_dcl宏申明这个类是一个Model反射类
// 3. rellaf_model_def_${type}宏用来定义各个类型的数据字段名和默认值

class Object : public Model {
rellaf_model_dcl(Object)
rellaf_model_def_int(id, -222);
};
```
2. 在源文件中定义反射类
```c++
// 4. 在类定义外，用宏rellaf_model_def定义Model反射类
rellaf_model_def(Object);
```
3. 好了，可以使用了：
```c++
Object object;
int val = object.id();         // 初始化时返回默认值 -222

object.set_id(233);            // 修改
val = object.id();             // 字段名方法调用, 返回 233
val = object.get_int("id");    // 通过字段名字符串索引取值, 返回 233

// 通过静态方法`${type}_names`返回某一个类型`<字段，默认值>`集合
// 包含 `<"id", -222>`
const std::map<std::string, int>& int_names = Object::int_names();

// 通过成员方法`${type}s`返回某一个类型`<字段，值>`集合
// 包含 `<"id", 233>`
const std::map<std::string, int>& int_names = Object::ints();
```
当前支持9种基本类型: 

| ${type} | C++类型 |   
| ---- | ------- |  
| int | int |   
| int64 | int64_t |   
| uint16 | uint16_t |   
| uint32 | uint32_t |   
| uint64 | uint64_t |   
| bool | bool |   
| float | float |    
| double | double |   
| str | std::string |   

当然，成员可以加多个。更加重要的是，**支持嵌套！支持数组！** 我们来看相对复杂的例子，两个基本类型，一个对象成员，一个数组。
```c++
class SubObject : public Model {
rellaf_model_dcl(SubObject)
rellaf_model_def_uint16(port, 18765);
};
rellaf_model_def(SubObject);

class List : public Model {
rellaf_model_dcl(List)
rellaf_model_def_float(ratio, 11.8);
};
rellaf_model_def(List);

class Object : public Model {
rellaf_model_dcl(Object)
rellaf_model_def_int(id, -111);
rellaf_model_def_str(name, "aaa");

// 定义对象，字段名，类型名（必须是Model类）
rellaf_model_def_object(sub, SubObject);

// 定义数组，字段名，类型名（必须是Model类）
rellaf_model_def_list(list, List);
};
rellaf_model_def(Object);
```
定义好了，对象操作
```c++
Object object;
// 常规操作
object.set_id(123);
object.set_name("fankux");

// 初始情况下是nullptr
SubObject* ptr = object.sub();    

// 赋值一个对象, 会进行内存复制，生命周期由Model管理。
SubObject sub;
object.set_sub(&sub);

// 给对象成员的成员赋值(注意，对象成员方法返回的是指针)
object.sub()->set_port(8121);
// 通过字段名字符串索引, 若不存在则返回nullptr
ptr = object->get_object("sub");
ptr->set_port(8121);

// 取值, 返回 8121
uint16_t port = object.sub()->port();
port = object.sub()->get_uint16("port");
```
数组操作
```c++
Object object;
// 数组通过ModuleList实现，模块size()==0
ModuleList& list = object.list();
// 与STL一致的接口
size_t size = list.size();      // 返回 0
bool is_empty = list.empty();   // 返回 true

// 插入成员，会进行内存复制，生命周期由Model管理。
List item;
list.push_front(&item);
list.push_back(&item);
size = list.size();             // 返回 2
is_empty = list.empty();        // 返回 false

// 索引数组成员，注意返回的是 Model*, 转换成具体类型即可
List* ptr = (List*)list.front();
ptr = (List*)list.back();
ptr = (List*)list[0];
ptr = (List*)list[1];

// 遍历
for (Model* item : list) {
    // do something
}
for (auto i = list.begin(); i != list.end(); ++i) {
    // do something
}

// 修改
List item_to_mod;
list.set(0, &item_to_mod);
list.set(1, &item_to_mod);

// 弹出
list.pop_front();
list.pop_back();

// 清空
list.clear();
```
详细的API见：参考手册。

## 枚举
C/C++枚举（`enum`）能力很有限，只是一个数字，没有从字符串获得枚举的能力，也不能判断一个枚举是否存在。  
`Rellaf`实现了灵活的枚举类，使用同样非常简单。
1. 包含头文件`"enum.h"`，申明枚举类
```shell
class DemoEnum : public IEnum {
rellaf_enum_dcl(DemoEnum);

//  按照 code(int)，name(std::string) 定义，都保证唯一。
rellaf_enum_item_def(0, A);
rellaf_enum_item_def(1, B);
rellaf_enum_item_def(2, C);
};
```
2. 源文件定义枚举类
```shell
rellaf_enum_def(DemoEnum);
```
3. 可以使用了, 枚举类的成员类型都是`rellaf::EnumItem`
```shell
// 枚举都是单例类，通过单例方法或者 rellaf_enum宏 访问
std::string name = DemoEnum::e().A.name;                    // 返回 "A"
int code = rellaf_enum(DemoEnum).B.code;                    // 返回 1

// 静态数字，可用于switch case (仅C++17以上)
code = DemoEnum::A_code;                                    // 返回 1
code = DemoEnum::B_code;                                    // 返回 2
code = DemoEnum::C_code;                                    // 返回 3

// 比较
if (DemoEnum::e().B != DemoEnum::e().C) {
    // B not equal C
}

// 判断是否存在
DemoEnum::e().exist(2);                                     // 返回 true
DemoEnum::e().exist("D");                                   // 返回 false

// 取值
EnumItem name = DemoEnum::e().get("B");
if (name.available()) {
    // exist, do something
}
EnumItem code  = DemoEnum::e().get(1);
if (code.available()) {
    // exist, do something
}

// 获得取值范围
const std::map<std::string, int>& names = DemoEnum::e().names();
const std::map<int, std::string>& codes = DemoEnum::e().codes();

```

## Dao
这是一个`Java Mybatis like`的SQL语句生成器。并不是说使用方式和语法与`Mybatis`一样，我们强调写代码体验，`Rellaf`做到的是在写Dao的体验上，尽可能靠近`Mybatis`，简单灵活而'自动化'。用户简单配置一个SQL模板，然后`Rellaf`将生成Dao执行方法，运行过程中'自动'将`Model`填入SQL模板，生成可执行SQL语句，*执行SQL*（需要实现Mysql数据传递接口）后，将返回值'自动'转换为Model对象返回用户。

demo TODO。。

## 功能扩展
`src/json`   
包含了使用Jsoncpp实现的**Rellaf**对象Json相互转换。（递归解析DOING……）

`src/mysql`  
包含一个简单的Mysql连接池实现（比较low，轻喷），这个模块为了对接SQL生成后的执行过程。

##  编译
不开启扩展的话，不依赖第三方组件。开启不同扩展依赖不同第三方库。  
编译选项：

| 选项 | 默认 | 说明 | 依赖 |   
| ------- | ----- | ------- | --------- | 
| WITH_JSON | ON | json扩展 | jsoncpp |  
| WITH_MYSQL | ON | 简单mysql连接池 |  mysqlclient |  
| WITH_TEST | ON | 单元测试 | gtest |   

安装依赖（可选）：  
**ubuntu**
```shell
sudo apt-get install libjsoncpp-dev libmysqlclient-dev libgtest-dev
# 注意，libgtest-dev这个源安装是源码，需要进入目录/usr/src/gtest(也可能是/usr/src/googletest/googletest)
# 执行 sudo mkdir build && cd build && sudo cmake .. && sudo make && sudo make install
```

**centos**
```shell
sudo yum install libjsoncpp-devel libmysqlclient-devel gtest-devel 
```

**macOS**
```shell
brew install jsoncpp mysql-connector-c
# 注意，gtest需要从源码安装，见 https://github.com/google/googletest
``` 

最小化编译：  
```shell
mkdir build && cd build
cmake —DWITH_JSON=OFF -DWITH_MYSQL=OFF -DWITH_TEST=OFF .. && make
```


**如何实现？**  
C++11特性，成员变量就地初始化，可变参模板。静态注册套路。另外，宏, 大量的宏, 对, 多到令人发指的宏。虽然，宏在课堂里或者大部分编程规范里，是badcase，然而我们不得不这么做，至少C++11的语法范围，不得不用宏。  
TODO 实现细节

**为什么C++11？**  
因为公司早已全面推广GCC4.8.2，C++11完全支持，14，17甚至20，虽然能更加轻易实现，但兼容性还不敢想。国内大部分厂估计也是这种情况。

**'Rellaf'单词什么意思？**  
relief（得到解脱），reflection（反射），relax（放松），3个单词组合一下，得到'**Rellaf**'。

## TODO
* C++ SQL Dao
* brpc接口映射

## 最后
本人能力有限，精力也有限，存在很多不足，欢迎交流指出。
fankux@gmail.com（经常看邮件）