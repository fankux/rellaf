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
// 2. RELLAF_MODEL_DCL宏申明这个类是一个Model反射类
// 3. RELLAF_MODEL_DEF_${type}宏用来定义各个类型的数据字段名和默认值

class Object : public Model {
RELLAF_MODEL_DCL(Object)
RELLAF_MODEL_DEF_int(id, -222);
};
```
2. 在源文件中定义反射类
```c++
// 4. 在类定义外，用宏RELLAF_MODEL_DEF定义Model反射类
RELLAF_MODEL_DEF(Object);
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
| uint32 | uint32_t |   
| uint64 | uint64_t |   
| bool | bool |   
| float | float |    
| double | double |   
| str | std::string |   

当然，成员可以加多个。更加重要的是，**支持嵌套！支持数组！** 我们来看相对复杂的例子，两个基本类型，一个对象成员，一个数组。
```c++
class SubObject : public Model {
RELLAF_MODEL_DCL(SubObject)
RELLAF_MODEL_DEF_uint16(port, 18765);
};
RELLAF_MODEL_DEF(SubObject)

class List : public Model {
RELLAF_MODEL_DCL(List)
RELLAF_MODEL_DEF_float(ratio, 11.8);
};
RELLAF_MODEL_DEF(List)

class Object : public Model {
RELLAF_MODEL_DCL(Object)
RELLAF_MODEL_DEF_int(id, -111);
RELLAF_MODEL_DEF_str(name, "aaa");

// 定义对象，字段名，类型名（必须是Model类）
RELLAF_MODEL_DEF_object(sub, SubObject);

// 定义数组，字段名，类型名（必须是Model类）
RELLAF_MODEL_DEF_list(list, List);
};
RELLAF_MODEL_DEF(Object)
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


##  编译
```shell
mkdir build && cd build
cmake .. && make
```

## 功能扩展
`src/json`   
包含了使用Jsoncpp实现的**Rellaf**对象Json相互转换。（递归解析DOING……）

`src/mysql`  
包含一个简单的Mysql连接池实现（比较low，轻喷），这个模块为了对接SQL生成后的执行过程。

**如何实现？**  
C++11特性，成员变量就地初始化，可变参模板。静态注册套路。另外，宏, 大量的宏, 对, 多到令人发指的宏。虽然，宏在课堂里或者大部分编程规范里，是badcase，然而我们不得不这么做，至少C++11的语法范围，不得不用宏。  
TODO 实现细节

**为什么C++11？**  
因为公司早已全面推广GCC4.8.2，C++11完全支持，14，17甚至20，虽然能更加轻易实现，但兼容性还不敢想。国内大部分厂估计也是这种情况。

**'Rellaf'单词什么意思？**  
relief（得到解脱），reflection（反射），relax（放松），3个单词组合一下，得到'**Rellaf**'。

## TODO
* Json递归转换
* C++ SQL Dao
* 枚举类

## 最后
本人能力有限，精力也有限，存在很多不足，欢迎交流指出。
fankux@gmail.com（经常看邮件）