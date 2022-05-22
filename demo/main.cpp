#include <example.hpp>

#include "picosha2.h"
#include "hash.hpp"
#include <atomic>
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <thread>


    // Правильно говорить "Салам-алейкум"
    // Правильно отвечать "Алейкум-асалам"

    //флаги
    std::atomic<bool> continueProcess = true;
std::atomic<bool> addToJson = false;

// обработка ctrl + c
void stopProcess(int param) {
  if (param == SIGINT) {
    continueProcess = false;
  }
}

//генератор хэш фукнции + логирования
void hashGenerator(JsonFiler& j) {
  while (continueProcess) {
    std::string genstring = std::to_string(std::rand());
    // генерация 16-ричной строки хэша
    std::string hash = picosha2::hash256_hex_string(genstring);
    // фиксация времени
    std::time_t timestamp(std::time(nullptr));
    // возвращение 4 посление цифры хэша для дальнейшей проверки
    std::string lastChar = hash.substr(hash.size() - NumZeroes);

    //Проверяем этот хэш с 4-мя нулями в конце для дальнейшей его записи или пропуска
    if (lastChar == Ending) {
      // если нашли нужный хэш, то записываем его, если нет, то скипаем
      // нужный хэш должен быть с 0 нулями в конце
      BOOST_LOG_TRIVIAL(info)
          << "Necessary ending found in hash |" << hash
          << "| generarted from string |" << genstring << "|";
      // создаем json файл, в который потом будем записывать хэши
      if (addToJson) {
        j.NewElement(genstring, hash, timestamp);
      }
    } else {
      // ставим уровень логирования трэйс и записываем логи в файл и выводим в терминал
      BOOST_LOG_TRIVIAL(trace)
          << "Hash |" << hash << "| generated from string|"
          << genstring << "|";
    }
  }
}
void StartProcess(const int& argc, char **argv){
  //указываем колличестко потоков логирования
  //тут будет 3 кейса, 2 будет запускаться если 1 выдал ошибку, так же с 3
  // если первый 1 выдает ошибку, 2 выводит сообщение об ошибке и продолжает работать
  // если 2 выдал ошибку, то 3 выводит финальное сообщение и останавливает процесс логирования
  unsigned int numberOfThreads (1);
  std::string json_path;
  std::srand(time(nullptr));
  switch (argc) {
    case 1:
      numberOfThreads = std::thread::hardware_concurrency();
      break;
    case 2:
      numberOfThreads = std::atoi(argv[1]);
      if (numberOfThreads == 0 ||
          numberOfThreads > std::thread::hardware_concurrency()) {
        throw std::out_of_range(" Invalid number of threads!!!");
      }
      break;
    case 3:
      numberOfThreads = std::atoi(argv[1]);
      if (numberOfThreads == 0 ||
          numberOfThreads > std::thread::hardware_concurrency()) {
        throw std::out_of_range(" Invalid number of threads!!!");
      }
      json_path = argv[2];
      addToJson = true;
      break;
    default:
      throw std::out_of_range("Invalid number of arguments!!!");
  }
  //запускаем процесс логирования
  SetUpLogging();
  //ведем журнал логирования(тобишь та херня, которая в терминал выйдет)
  boost::log::add_common_attributes();
  // инициализваия потоково логирования
  std::vector<std::thread> threads;
  // создаем json файл для логирования
  JsonFiler json_obj;
  // обработка ctrl+c
  threads.reserve(numberOfThreads);

  std::signal(SIGINT, stopProcess);

  for (size_t i = 0; i < numberOfThreads; ++i) {
    // передаем в потоки хэши
    threads.emplace_back(hashGenerator, std::ref(json_obj));
  }
  for (auto& thread : threads) {
    //ожидаем завершения каждого потока
    thread.join();
  }
  // добавляет найденые хэши в json-файл
  if (addToJson) {
    std::ofstream fout{json_path};
    fout << json_obj;
  }
}

int main(int argc, char* argv[]) {
  StartProcess(argc,argv);
  return 0;
}