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
};
/// Declare remote function
RAY_REMOTE(Counter::FactoryCreate, &Counter::Add);

int main(int argc, char **argv) {
  /// initialization
  ray::Init();

  // std::unordered_map<std::string, int> testmap1;
  // testmap1["Bob"] = 25;
  // std::cout << "Bob: " << testmap1["Bob"] << std::endl;

  /// put and get object
  // std::unordered_map<std::string, ray::ObjectRef> testmap2;
  std::cout << "The type of 1 is: " << typeid(1).name() << std::endl;
  std::cout << "The type of 'Bob' is: " << typeid("Bob").name() << std::endl;
  auto object = ray::Put(100);
  std::cout << "The type of ObjectRef is: " << typeid(object).name() << std::endl;
  // testmap2["Alice"] = object;
  // std::cout << "Alice: " << testmap2["Alice"] << std::endl;

  auto put_get_result = *(ray::Get(object));
  std::cout << "put_get_result = " << put_get_result << std::endl;

  /// common task
  auto task_object = ray::Task(Plus).Remote(1, 2);
  int task_result = *(ray::Get(task_object));
  std::cout << "task_result = " << task_result << std::endl;

  /// actor
  ray::ActorHandle<Counter> actor = ray::Actor(Counter::FactoryCreate).Remote(0);
  /// actor task
  auto actor_object = actor.Task(&Counter::Add).Remote(3);
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
