// Copyright by Enigma

#include "hash.hpp"
//запуск процесса логирования
// у нас есть процессы с разным уровнем логироания типо trace и тд
// тут будет производиться логирование всех этих процессов
// суть одна и та же
// есть лог, записываем его в файл и выводим в терминал

void SetUpLogging() {
  // обработка trace фалага
  auto TraceFileLogger = boost::log::add_file_log(
      boost::log::keywords::file_name =
          "/home/enigma/twinLabs/c++ developer/lab6/logs/"
          "TraceLog_%N.log",
      boost::log::keywords::rotation_size = 10 * 1024 * 1024,
      boost::log::keywords::format =
          "[%TimeStamp%][%Severity%][%ThreadID%]: %Message%");
  //обработка info флага
  auto InfoFileLogger = boost::log::add_file_log(
      boost::log::keywords::file_name =
          "/home/enigma/twinLabs/c++ developer/lab6/logs/"
          "TraceLog_%N.log",
      boost::log::keywords::format =
          "[%TimeStamp%][%Severity%][%ThreadID%]: %Message%");
  // событие вывода в консоль логов
  auto consoleLogger = boost::log::add_console_log(
      std::cout, boost::log::keywords::format =
                     "[%TimeStamp%][%Severity%][%ThreadID%]: %Message%");
  //устновка фильтров для отсеивания логов каждого типа
  TraceFileLogger->set_filter(boost::log::trivial::severity >=
                              boost::log::trivial::trace);
  InfoFileLogger->set_filter(boost::log::trivial::severity ==
                             boost::log::trivial::info);
  consoleLogger->set_filter(boost::log::trivial::severity ==
                            boost::log::trivial::info);
}
// добавление нового элемента в json файл
void JsonFiler::NewElement(const std::string& randString,
                           const std::string& hash,
                           std::time_t timestamp) {
  // многопоточный вывод
  std::scoped_lock<std::mutex> lock(mut);

  std::stringstream HexString;
  HexString << std::hex << std::uppercase << std::stoi(randString);
  json JsonObj;
  JsonObj["randString"] = HexString.str();
  JsonObj["hash"] = hash;
  JsonObj["timestamp"] = timestamp;
  JsonArray.push_back(JsonObj);
}
// вывод всех элементов в терминал
std::ostream& operator<<(std::ostream& out, const JsonFiler& j) {
  std::scoped_lock<std::mutex> lock(j.mut);
  out << j.JsonArray.dump(4);
  return out;
}
