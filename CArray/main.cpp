#include "CArray.h"

#include <stdlib.h>
#include <time.h>

#include <algorithm>
#include <iostream>


std::string generateWord()
{
  std::string result;
  for (int i = 0; i < (rand() % 15 + 1); ++ i)
    result.push_back(97 + rand() % 26);
  return result;
}

int main(int argc, char** argv)
{
  srand (time(NULL));

  CArray<int> intList;

  std::cout << "Push back" << std::endl;
  for (int i = 0; i < 20; ++i) intList.push_back(rand() % 101);
  for (const auto& value: intList) std::cout << value << " ";

  std::cout << std::endl << std::endl << "Sort" << std::endl;
  std::sort(intList.begin(), intList.end());
  for (const auto& value: intList) std::cout << value << " ";

  std::cout << std::endl << std::endl << "Erase" << std::endl;
  for (int i = 0; i < 10; ++i) intList.erase(i);
  for (const auto& value: intList) std::cout << value << " ";

  std::cout << std::endl << std::endl << "Insert" << std::endl;
  for (int i = 0; i < 10; ++i) {
    intList.insert(rand() % (intList.size() + 1), rand() % 101);
  }
  for (const auto& value: intList) std::cout << value << " ";

  std::cout << std::endl << std::endl << "Clear" << std::endl;
  intList.clear();
  for (const auto& value: intList) std::cout << value << " ";

  std::cout << std::endl << std::endl;


  CArray<std::string> stringList;

  std::cout << "Push back" << std::endl;
  for (int i = 0; i < 15; ++i) stringList.push_back(generateWord());
  for (const auto& value: stringList) std::cout << value.c_str() << " ";

  std::cout << std::endl << std::endl << "Sort" << std::endl;
  std::sort(stringList.begin(), stringList.end());
  for (const auto& value: stringList) std::cout << value.c_str() << " ";

  std::string markers("abcde");
  std::cout << std::endl << std::endl << "Erase" << std::endl;
  for (int i = 0; i < stringList.size(); ++i)
  {
    for (char ch: stringList[i])
    {
      if (markers.find(ch) != std::string::npos)
      {
        stringList.erase(i);
        --i;
        break;
      }
    }
  }
  for (const auto& value: stringList) std::cout << value.c_str() << " ";

  std::cout << std::endl << std::endl << "Insert" << std::endl;
  for (int i = 0; i < 3; ++i) {
    stringList.insert(rand() % (stringList.size() + 1), generateWord());
  }
  for (const auto& value: stringList) std::cout << value.c_str() << " ";

  return 0;
}
