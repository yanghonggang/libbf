#include <iostream>
#include <fstream>

#include <bf/bloom_filter/basic.hpp>

#include "cmdline.h"

int training_bf(bf::basic_bloom_filter& bf, std::ifstream& filter)
{
  std::string obj;
  while (getline(filter, obj)) {
    bf.add(obj.c_str());
  }

  return 0;
}

inline std::string get_prefix(const std::string& obj)
{
  // FIXME
  return obj;
}

int filter_objs(bf::basic_bloom_filter& bf,
                std::ifstream& t,
                std::ofstream& t2,
                std::ofstream& t3)
{
  std::string obj, prefix;
  while (getline(t, obj)) {
    prefix = get_prefix(obj);
    if (bf.lookup(prefix.c_str()) == 1) { // maybe in bf
      t3 << obj << std::endl;
    } else { // not in bf
      t2 << obj << std::endl;
    }
  }

  return 0;
}
                
int main(int argc, char** argv)
{
  std::string prefix_file; // prefix should be not be deleted
  std::string tail_file; // whole set of tailf objects
  std::string t2_file; // objs not in bf
  std::string t3_file; // objs maybe in bf

  cmdline::parser cmd;
  cmd.add<std::string>("prefix", 'p', "prefix file", true, "");
  cmd.add<std::string>("tail", 't', "tail objs file", true, "");
  cmd.add<std::string>("t2", 'n', "objs not in bf", true, "");
  cmd.add<std::string>("t3", 'i', "objs maybe in bf", true, "");
  cmd.parse_check(argc, argv);

  prefix_file = cmd.get<std::string>("prefix");
  tail_file = cmd.get<std::string>("tail");
  t2_file = cmd.get<std::string>("t2");
  t3_file = cmd.get<std::string>("t3");

  // open files
  std::ifstream p(prefix_file.c_str());
  std::ifstream t(tail_file.c_str());
  std::ofstream t2(t2_file.c_str());
  std::ofstream t3(t3_file.c_str());

  bf::basic_bloom_filter bf(0.8, 100);

  // training bf with prefix file's content
  int ret = training_bf(bf, p);
  if (ret < 0) {
    std::cerr << "training_bf failed: " << ret << std::endl;
    return ret;
  }

  // check tail objects
  ret = filter_objs(bf, t, t2, t3);
  if (ret < 0) {
    std::cerr << "filter_objs failed: " << ret << std::endl;
  }

  // flush
  t2.close();
  t3.close();

  return 0;
}
