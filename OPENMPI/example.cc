/// This is an example of Ray C++ application. Please visit
/// `https://docs.ray.io/en/master/ray-core/walkthrough.html#installation`
/// for more details.

/// including the `<ray/api.h>` header
#include <ray/api.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <typeinfo>

/// common function
int Plus(int x, int y) { return x + y; }
/// Declare remote function
RAY_REMOTE(Plus);

/// class
class Counter {
 public:
  int count;

  Counter(int init) { count = init; }
  /// static factory method
  static Counter *FactoryCreate(int init) { return new Counter(init); }

  /// non static function
  int Add(int x) {
    count += x;
    return count;
  }

  auto test_put(int num){
    auto result = ray::Put(num);
    std::cout << "Test 3:" << typeid(result).name() << std::endl;
    return result;
  }

};

auto test_send(std::vector<std::string>& buf){
    auto object_test = ray::Put(buf);
    std::cout << "Test 1:" << typeid(object_test).name() << std::endl;
    return object_test;
}

/// Declare remote function
RAY_REMOTE(Counter::FactoryCreate, &Counter::Add, &Counter::test_put);

int main(int argc, char **argv) {
  /// initialization
  ray::Init();

  /// put and get object
  auto object = ray::Put(100);
  auto put_get_result = *(ray::Get(object));
  std::cout << "put_get_result = " << put_get_result << std::endl;

  // std::unordered_map<std::string, ray::ObjectRef<int>> testmap2;
  // auto object = ray::Put(100);
  // testmap2["Alice"] = object;
  // std::cout << "Alice: " << testmap2["Alice"] << std::endl;

  std::cout << "Checkpoint 0" << std::endl;
  std::vector<std::string> stringToSend = {"Hello","I'm","Zicheng Ma","Nice to meet you!"};
  auto object_test = ray::Put(stringToSend);
  std::cout << "Checkpoint 1" << std::endl;
  std::cout << "type of object_test:" << typeid(object_test).name() << std::endl;

  auto temp_res = test_send(stringToSend);
  std::cout << "Test 2:" << typeid(temp_res).name() << std::endl;

  std::vector<ray::ObjectRef<std::vector<std::string>>> objectList;
  objectList.push_back(object_test);

  std::cout << "Checkpoint 2" << std::endl;
  for(const auto& obj_ref : objectList) {
    auto value = *(ray::Get(obj_ref));
    for(const auto& str : value) {
      std::cout << str << std::endl;
    }
  }

  /// common task
  auto task_object = ray::Task(Plus).Remote(1, 2);
  int task_result = *(ray::Get(task_object));
  std::cout << "task_result = " << task_result << std::endl;

  /// actor
  ray::ActorHandle<Counter> actor = ray::Actor(Counter::FactoryCreate).Remote(0);
  /// actor task
  auto actor_object = actor.Task(&Counter::Add).Remote(3);




  auto test_obj = actor.Task(&Counter::test_put).Remote(3);
  std::cout << "Test 4:" << typeid(test_obj).name() << std::endl;




  int actor_task_result = *(ray::Get(actor_object));
  std::cout << "actor_task_result = " << actor_task_result << std::endl;
  /// actor task with reference argument
  auto actor_object2 = actor.Task(&Counter::Add).Remote(task_object);
  int actor_task_result2 = *(ray::Get(actor_object2));
  std::cout << "actor_task_result2 = " << actor_task_result2 << std::endl;

  /// shutdown
  ray::Shutdown();
  return 0;
}
